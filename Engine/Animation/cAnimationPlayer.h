#pragma once
#include "cAnimationClip.h"
#include "Engine/Math/cQuaternion.h"
#include "Engine/Math/sVector.h"

namespace eae6320
{
	namespace Animation
	{
		class cAnimationPlayer
		{
		public:

			// Interface
			//==========

			//plays the given animation clip from the beginning
			void Play(const cAnimationClip* i_clip);

			//updates the animation player by the given delta time
			void Update(float i_deltaTime);

			//stops the current animation
			void Stop();

			//returns whether the animation is currently playing
			bool IsPlaying() const { return m_isPlaying; }

			//gets the current translation and rotation of the animation
			Math::sVector GetCurrentTranslation() const;
			Math::cQuaternion GetCurrentRotation() const;

		private:
			const cAnimationClip* m_currentClip = nullptr;
			float m_currentTime = 0.0f;
			bool m_isPlaying = false;

		};
	}
}