#include "cAnimationClip.h"
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>
#include <new>
#include <External/Lua/Includes.h>
#include <iostream>
#include <Engine/Platform/Platform.h>
#include <Engine/Math/cQuaternion.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Graphics/VertexFormats.h>

void eae6320::Animation::cAnimationClip::AddKeyFrame(const sKeyFrame& i_keyframe)
{
	keyframes.push_back(i_keyframe);
}

eae6320::cResult eae6320::Animation::cAnimationClip::LoadFromFile(const std::string& i_path, cAnimationClip*& o_clip)
{
	auto result = eae6320::Results::Success;

	uint16_t vCount = 0;
	eae6320::Graphics::VertexFormats::sVertex_mesh* vData = nullptr;
	uint16_t iCount = 0;
	uint16_t* iData = nullptr;

	struct Transform
	{
		eae6320::Math::sVector translation;
		eae6320::Math::cQuaternion rotation;
		eae6320::Math::sVector scale;
	};

	Transform transform;


	Platform::sDataFromFile s_DataFile;
	std::string errMessage;

	if (!(result = Platform::LoadBinaryFile(i_path.c_str(), s_DataFile, &errMessage)))
	{
		EAE6320_ASSERTF(false, "Failed to LoadBinaryFile");
		Logging::OutputError("Failed to LoadBinaryFile %s", errMessage.c_str());
	}
	auto currentOffset = reinterpret_cast<uintptr_t>(s_DataFile.data);
	const auto finalOffset = currentOffset + s_DataFile.size;




	//vertexData

	memcpy(&vCount, reinterpret_cast<void*>(currentOffset), sizeof(vCount));
	currentOffset += sizeof(vCount);

	vData = new eae6320::Graphics::VertexFormats::sVertex_mesh[vCount];

	memcpy(vData, reinterpret_cast<void*>(currentOffset), sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh) * vCount);
	currentOffset += sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh) * vCount;

	//indexdata

	memcpy(&iCount, reinterpret_cast<void*>(currentOffset), sizeof(iCount));
	currentOffset += sizeof(iCount);

	iData = new uint16_t[iCount];

	memcpy(iData, reinterpret_cast<void*>(currentOffset), sizeof(uint16_t) * iCount);
	currentOffset += sizeof(uint16_t) * iCount;

	//transform data
	memcpy(&transform.translation, reinterpret_cast<void*>(currentOffset), sizeof(transform.translation));
	currentOffset += sizeof(transform.translation);
	memcpy(&transform.rotation, reinterpret_cast<void*>(currentOffset), sizeof(transform.rotation));
	currentOffset += sizeof(transform.rotation);
	memcpy(&transform.scale, reinterpret_cast<void*>(currentOffset), sizeof(transform.scale));
	currentOffset += sizeof(transform.scale);

	//store animation clip
	uint16_t numOfFrames = 0;
	memcpy(&numOfFrames, reinterpret_cast<void*>(currentOffset), sizeof(uint16_t));
	currentOffset += sizeof(uint16_t);

	auto newClip = new cAnimationClip();
	newClip->keyframes.reserve(numOfFrames);

	for(uint16_t i = 0; i < numOfFrames; i++)
	{
		Animation::sKeyFrame keyframe;
		memcpy(&keyframe.frame, reinterpret_cast<void*>(currentOffset), sizeof(uint16_t));
		currentOffset += sizeof(uint16_t);
		memcpy(&keyframe.translation, reinterpret_cast<void*>(currentOffset), sizeof(eae6320::Math::sVector));
		currentOffset += sizeof(eae6320::Math::sVector);
		memcpy(&keyframe.rotation, reinterpret_cast<void*>(currentOffset), sizeof(eae6320::Math::cQuaternion));
		currentOffset += sizeof(eae6320::Math::cQuaternion);
		newClip->AddKeyFrame(keyframe);
	}

	o_clip = newClip;

	return result;
}

void eae6320::Animation::cAnimationClip::clearKeyFrames()
{
	keyframes.clear();
}

float eae6320::Animation::cAnimationClip::GetDuration() const
{
	if(keyframes.empty())
	{
		return 0.0f;
	}
	return static_cast<float>(keyframes.back().frame) / frameRate;
}
