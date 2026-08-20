#include <array>
#include <string>
#include <vector>
#include <windows.h>
#include <xaudio2.h>

#include "Engine/Math/sVector.h"
//#include "MyGame_/MyGame/Camera.h"

class Audio
{
    struct PCMBuffer {
    WAVEFORMATEX* wf = nullptr;
    std::vector<BYTE> data;
    };
    std::wstring path;
    PCMBuffer pcmBuffer;
    IXAudio2* xa_ = nullptr;
    IXAudio2MasteringVoice* master_ = nullptr;
    std::vector<BYTE> playBuffer;
    IXAudio2SourceVoice* currentVoice = nullptr;
    float playbackSpeed = 1.0f;
    //Camera *camera;
    bool isLoop = false;
    float volume = 1.0f;
    bool isSpatial = false;
    
    struct Spatial
    {
        eae6320::Math::sVector position;
        float radius;
        float max;
        float min; 
    };
    Spatial spatial;
    
    bool DecodeToPCM();
  
public:
    Audio() = default;
    Audio(std::wstring i_path):path(i_path){}
    Audio(std::string i_path):path(i_path.begin(), i_path.end()){}
    //void Init(std::wstring i_path, Camera* i_camera);
    void Init(std::wstring i_path);
    void Play(bool isLoop);
    void Reverse();
    void SetSpeed(float speed);
    void SetVolume(float volume);
    void SetSpatial(eae6320::Math::sVector i_pos, float i_radius, float i_max, float i_min);
    void SetLoop(bool isLoop);
    void SpatialVolumeUpdate(eae6320::Math::sVector i_pos);
    void CleanUp();
    //static size_t CreateAudiosFromJson(const std::string& jsonPath, std::vector<Audio>& audios, Camera* camera);
    static size_t CreateAudiosFromJson(const std::string& jsonPath, std::vector<Audio>& audios);
};