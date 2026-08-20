#define HR_RET(x, rv) do { HRESULT __hr = (x);                       \
if (FAILED(__hr)) {                                                \
printf("Error: %s (0x%08X)\n", #x, (unsigned)__hr); return (rv); \
} } while (0)


#include "Audio.h"

#include <algorithm>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <xaudio2.h>
#include <vector>
#include <string>
#include <atlbase.h> // CComPtr
#include <iostream>
#include <cmath>
#include <fstream>
#include "External/JSON/3.9.1/json.hpp"

#include "Engine/Math/sVector.h"

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "xaudio2.lib")

#define HR(x) if (FAILED(x)) { printf("Error: %s (0x%08X)\n", #x, (unsigned)(x)); return; }

constexpr float clamp(float &v, float lo, float hi)
{
    if (v < lo) v = lo;
    if (v > hi) v = hi;
    return v;
}

bool Audio::DecodeToPCM()
{
    if (pcmBuffer.wf) { CoTaskMemFree(pcmBuffer.wf); pcmBuffer.wf = nullptr; }
    pcmBuffer.data.clear();

    CComPtr<IMFSourceReader> reader;
    HR_RET(MFCreateSourceReaderFromURL(path.c_str(), nullptr, &reader), false);

    CComPtr<IMFMediaType> outType;
    HR_RET(MFCreateMediaType(&outType), false);
    HR_RET(outType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio), false);
    HR_RET(outType->SetGUID(MF_MT_SUBTYPE,  MFAudioFormat_PCM),   false);
    HR_RET(outType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16),   false);
    HR_RET(outType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS,    1),    false);

    HR_RET(reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, outType), false);

    CComPtr<IMFMediaType> actual;
    HR_RET(reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &actual), false);

    WAVEFORMATEX* wf = nullptr; UINT32 cb = 0;
    HR_RET(MFCreateWaveFormatExFromMFMediaType(actual, &wf, &cb), false);
    pcmBuffer.wf = wf;

    DWORD flags = 0;
    for (;;)
    {
        CComPtr<IMFSample> sample;
        HR_RET(reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample), false);

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
        if (!sample) continue;

        CComPtr<IMFMediaBuffer> buffer;
        HR_RET(sample->ConvertToContiguousBuffer(&buffer), false);

        BYTE*  p      = nullptr;
        DWORD  maxLen = 0;
        DWORD  curLen = 0;
        HR_RET(buffer->Lock(&p, &maxLen, &curLen), false);

        const size_t oldSize = pcmBuffer.data.size();
        pcmBuffer.data.resize(oldSize + curLen);
        std::memcpy(pcmBuffer.data.data() + oldSize, p, curLen);

        buffer->Unlock();
    }

    printf("Decoded: %u ch, %u bits, %u Hz, %zu bytes\n",
           pcmBuffer.wf->nChannels,
           pcmBuffer.wf->wBitsPerSample,
           pcmBuffer.wf->nSamplesPerSec,
           pcmBuffer.data.size());

    return true;
}


void Audio::SetSpeed(float speed)
{
    if (speed <= 0.0f)
    {
        std::puts("Speed must be > 0");
        return;
    }
    playbackSpeed = speed;
}

void Audio::SetVolume(float _volume)
{
    volume = _volume;
    if (currentVoice)
        currentVoice->SetVolume(volume);
}

void Audio::SetSpatial(eae6320::Math::sVector i_pos, float i_radius, float i_max, float i_min)
{
    isSpatial = true;
    spatial.position = i_pos;
    spatial.radius = i_radius;
    spatial.max = i_max;
    spatial.min = i_min;
}

void Audio::SetLoop(bool _isLoop)
{
    isLoop = _isLoop;   
}

inline float Lerp(float a, float b, float t)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return a + (b - a) * t;
}

void Audio::SpatialVolumeUpdate(eae6320::Math::sVector i_pos)
{
    if (isSpatial)
    {
        float _volume = Lerp(spatial.max, spatial.min,
			(i_pos - spatial.position).GetLength() / spatial.radius);
		SetVolume(_volume);
    }
	//print distance for testing
	char buffer[256];
	sprintf_s(buffer, "Distance: %f\n", (i_pos - spatial.position).GetLength());
	OutputDebugStringA(buffer);

    /*if (isSpatial)
    {
        float _volume = Lerp(spatial.max, spatial.min,
            (camera->rbState.position-spatial.position).GetLength()/spatial.radius);
        SetVolume(_volume);
        std::cout<<(camera->rbState.position-spatial.position).GetLength()<<std::endl;
    }*/
}

void Audio::Init(std::wstring i_path)
{
    path = i_path;
    //camera = i_camera;

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    MFStartup(MF_VERSION);

    if (!xa_) {
        if (FAILED(XAudio2Create(&xa_, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
            throw std::runtime_error("XAudio2Create failed");
        }
    }
    if (!master_) {
        if (FAILED(xa_->CreateMasteringVoice(&master_))) {
            xa_->Release(); xa_ = nullptr;
            throw std::runtime_error("CreateMasteringVoice failed");
        }
    }

    if (!DecodeToPCM()) {
        std::puts("Failed to decode audio.");
        throw std::runtime_error("Decode failed");
    }
}

static std::vector<int16_t> TimeStretchWSOLA_Mono16(
    const int16_t* in, size_t inSamples, float tempo)
{
    if (tempo == 1.0f || inSamples < 2048) {
        return std::vector<int16_t>(in, in + inSamples);
    }

    const int win   = 1024;
    const int Ha    = win / 2;
    const int Hs    = max(1, int(std::round(float(Ha) / tempo)));
    const int searchRadius = win / 4;

    std::vector<float> w(win);
    for (int n = 0; n < win; ++n)
        w[n] = 0.5f * (1.0f - std::cos(2.0f * float(3.14f) * n / (win - 1)));

    const size_t estOut = size_t((double)inSamples / Ha * Hs + win + 1);
    std::vector<float> acc(estOut, 0.0f);
    std::vector<float> norm(estOut, 0.0f);

    size_t inPos  = 0;
    size_t outPos = 0;
    auto addFrame = [&](size_t srcPos, size_t dstPos) {
        if (srcPos + win > inSamples) return;
        if (dstPos + win > acc.size()) {
            acc.resize(dstPos + win + 1, 0.0f);
            norm.resize(dstPos + win + 1, 0.0f);
        }
        for (int n = 0; n < win; ++n) {
            float s = in[srcPos + n] / 32768.0f;
            acc [dstPos + n] += s * w[n];
            norm[dstPos + n] += w[n];
        }
    };

    addFrame(inPos, outPos);
    inPos  += Ha;
    outPos += Hs;

    while (inPos + win <= inSamples)
    {
        const size_t refStart = (outPos >= static_cast<size_t>(Hs)) ? (outPos - static_cast<size_t>(Hs)) : 0;
        const int    overlap  = min(Hs, win);

        if (overlap <= 16) {
            addFrame(inPos, outPos);
            inPos  += Ha;
            outPos += Hs;
            continue;
        }

        int bestOffset = 0;
        double bestCorr = -1e30;

        const int start = std::max<int>(int(inPos) - searchRadius, 0);
        const int stop  = std::min<int>(int(inPos) + searchRadius, int(inSamples) - win);

        std::vector<float> ref(overlap);
        for (int n = 0; n < overlap; ++n) {
            float v = 0.0f;
            if (refStart + (win - overlap) + n < acc.size())
                v = acc[refStart + (win - overlap) + n];
            ref[n] = v;
        }

        double refEnergy = 1e-9;
        for (int n = 0; n < overlap; ++n) refEnergy += ref[n] * ref[n];

        for (int k = start; k <= stop; ++k)
        {
            double dot = 0.0, candEnergy = 1e-9;
            for (int n = 0; n < overlap; ++n) {
                float s = in[k + n] / 32768.0f;
                dot += s * ref[n];
                candEnergy += s * s;
            }
            double ncc = dot / std::sqrt(refEnergy * candEnergy);
            if (ncc > bestCorr) { bestCorr = ncc; bestOffset = k - int(inPos); }
        }

        const size_t src = size_t(int(inPos) + bestOffset);
        addFrame(src, outPos);

        inPos  += Ha;
        outPos += Hs;
    }

    size_t valid = min(acc.size(), outPos + win);
    std::vector<int16_t> out(valid);
    for (size_t i = 0; i < valid; ++i) {
        float v = (norm[i] > 1e-6f) ? (acc[i] / norm[i]) : 0.0f;
        out[i] = (int16_t)std::lrintf(v * 32767.0f);
    }
    return out;
}

void Audio::Play(bool isLoop)
{
    if (!pcmBuffer.wf || pcmBuffer.data.empty()) {
        std::puts("Play: no PCM data");
        return;
    }

    playBuffer.clear();

    const WORD bits = pcmBuffer.wf->wBitsPerSample;
    const WORD ch   = pcmBuffer.wf->nChannels;

    if (bits == 16 && ch == 1 && playbackSpeed != 1.0f) {
        const int16_t* in = reinterpret_cast<const int16_t*>(pcmBuffer.data.data());
        const size_t inSamples = pcmBuffer.data.size() / sizeof(int16_t);
        auto stretched = TimeStretchWSOLA_Mono16(in, inSamples, playbackSpeed);
        playBuffer.resize(stretched.size() * sizeof(int16_t));
        memcpy(playBuffer.data(), stretched.data(), playBuffer.size());
    } else {
        playBuffer = pcmBuffer.data;
    }

    if (currentVoice) {
        currentVoice->DestroyVoice();
        currentVoice = nullptr;
    }

    if (FAILED(xa_->CreateSourceVoice(&currentVoice, pcmBuffer.wf))) {
        std::puts("CreateSourceVoice failed");
        return;
    }

    XAUDIO2_BUFFER buf{};
    buf.pAudioData = playBuffer.data();
    buf.AudioBytes = (UINT32)playBuffer.size();
    buf.Flags = isLoop ? 0 : XAUDIO2_END_OF_STREAM;

    if (isLoop) {
        buf.LoopBegin = 0;
        buf.LoopLength = 0;
        buf.LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    currentVoice->SubmitSourceBuffer(&buf);
    currentVoice->SetVolume(volume);
    currentVoice->Start(0);
}



void Audio::Reverse()
{
    if (!pcmBuffer.wf || pcmBuffer.data.empty())
    {
        std::puts("Reverse: no PCM data loaded.");
        return;
    }

    const int bytesPerSample = pcmBuffer.wf->wBitsPerSample / 8;
    const int frameBytes     = bytesPerSample * pcmBuffer.wf->nChannels;

    if (frameBytes <= 0 || pcmBuffer.data.size() % frameBytes != 0)
    {
        std::puts("Reverse: invalid frame size.");
        return;
    }

    std::vector<BYTE> tmp(frameBytes);
    size_t i = 0;
    size_t j = pcmBuffer.data.size() / frameBytes - 1;

    while (i < j)
    {
        BYTE* a = pcmBuffer.data.data() + i * frameBytes;
        BYTE* b = pcmBuffer.data.data() + j * frameBytes;

        memcpy(tmp.data(), a, frameBytes);
        memcpy(a, b, frameBytes);
        memcpy(b, tmp.data(), frameBytes);

        ++i; --j;
    }

    std::puts("Audio reversed successfully.");
}

void Audio::CleanUp()
{
    if (master_) { master_->DestroyVoice(); master_ = nullptr; }
    if (xa_)     { xa_->Release(); xa_ = nullptr; }
    MFShutdown();
    CoUninitialize();
}

size_t Audio::CreateAudiosFromJson(const std::string& jsonPath, std::vector<Audio>& m_audio)
{
    using nlohmann::json;
    
    std::ifstream in(jsonPath);
    if (!in.is_open()) {
        std::cerr << "[Audio] Cannot open JSON: " << jsonPath << "\n";
        return 0;
    }

    json root;
    try { in >> root; }
    catch (const std::exception& e) {
        std::cerr << "[Audio] JSON parse error: " << e.what() << "\n";
        return 0;
    }

    if (!root.contains("audios") || !root["audios"].is_array()) {
        std::cerr << "[Audio] JSON missing 'audios' array\n";
        return 0;
    }

    m_audio.clear();

    for (const auto& a : root["audios"]) {
        const std::string filePath = a.value("file_path", "");
        const float playbackSpeed  = a.value("playback_speed", 1.0f);
        const bool  isSpatial      = a.value("isSpatial", false);
        
        float range = 0.0f;
        if (a.contains("range") && !a["range"].is_null())
            range = a["range"].get<float>();

        float volMin = 0.0f, volMax = 1.0f;
        if (a.contains("volume") && a["volume"].is_object()) {
            volMin = a["volume"].value("min", 0.0f);
            volMax = a["volume"].value("max", 1.0f);
        }

        Audio audio;

        std::wstring wpath(filePath.begin(), filePath.end());
        audio.Init(wpath);
        audio.SetSpeed(playbackSpeed);

        if (isSpatial) {
            eae6320::Math::sVector pos{0,0,0};
            audio.SetSpatial(pos, range, volMax, volMin);
        } else {
            audio.SetVolume(volMax);
        }

        m_audio.push_back(std::move(audio));
    }

    std::cout << "[Audio] Loaded " << m_audio.size() << " audios from " << jsonPath << "\n";
    return m_audio.size();
}
