#pragma once
#include "cAnimationPlayer.h"
#include <cmath>
#include <algorithm>

void eae6320::Animation::cAnimationPlayer::Play(const cAnimationClip* i_clip)
{
	m_currentClip = i_clip;
	m_currentTime = 0.0f;
	m_isPlaying = (i_clip != nullptr);
}

void eae6320::Animation::cAnimationPlayer::Stop()
{
	m_isPlaying = false;
	m_currentClip = nullptr;
}



void eae6320::Animation::cAnimationPlayer::Update(float i_deltaTime)
{
	if(!m_isPlaying||!m_currentClip)
	{
		return;
	}
	m_currentTime += i_deltaTime;
	const float duration = m_currentClip->GetDuration();
	if(m_currentTime>duration)
	{
		if(m_currentClip->looped)
		{
			m_currentTime = std::fmod(m_currentTime,duration);
		}
		else
		{
			m_isPlaying = false;
		}
	}
}

eae6320::Math::sVector eae6320::Animation::cAnimationPlayer::GetCurrentTranslation() const
{
	if (!m_currentClip || m_currentClip->keyframes.empty())
	{
		return Math::sVector(0.f, 0.f, 0.f);
	}

	const float currentFrame = m_currentTime * m_currentClip->frameRate;

	const auto& kf = m_currentClip->keyframes;
	size_t nextIndex = 1;
	while(nextIndex < kf.size() && kf[nextIndex].frame < currentFrame)
	{
		++nextIndex;
	}

	if(nextIndex == 0)
	{
		return kf[0].translation;
	}
	if(nextIndex >= kf.size())
	{
		return kf.back().translation;
	}

	const auto& prevKeyFrame = kf[nextIndex - 1];
	const auto& nextKeyFrame = kf[nextIndex];
	const float t = (currentFrame - prevKeyFrame.frame) / float(nextKeyFrame.frame - prevKeyFrame.frame);
	return(prevKeyFrame.translation + (nextKeyFrame.translation - prevKeyFrame.translation) * t);
	//return prevKeyFrame.translation * (1.0f - t) + nextKeyFrame.translation * t;
	//return kf[nextIndex].translation;
}

eae6320::Math::cQuaternion eae6320::Animation::cAnimationPlayer::GetCurrentRotation() const
{
	if( !m_currentClip || m_currentClip->keyframes.empty())
	{
		return Math::cQuaternion();
	}

	const float currentFrame = m_currentTime * m_currentClip->frameRate;

	const auto& kf = m_currentClip->keyframes;
	size_t nextIndex = 1;
	while (nextIndex < kf.size() && kf[nextIndex].frame < currentFrame)
	{
		++nextIndex;
	}

	if (nextIndex == 0)
	{
		return kf[0].rotation;
	}
	if (nextIndex >= kf.size())
	{
		return kf.back().rotation;
	}
	//const auto& prevKeyFrame = kf[nextIndex - 1];
	//const auto& nextKeyFrame = kf[nextIndex];
	//const float t = (currentFrame - prevKeyFrame.frame) / float(nextKeyFrame.frame - prevKeyFrame.frame);
	//return Math::cQuaternion::Slerp(prevKeyFrame.rotation, nextKeyFrame.rotation, t);
	return kf[nextIndex].rotation;
}
