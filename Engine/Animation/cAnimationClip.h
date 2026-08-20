#pragma once
#include <vector>
#include "sKeyFrame.h"
#include "Engine/Results/Results.h"
#include <string>

namespace eae6320
{
	namespace Animation
	{
		class cAnimationClip
		{
		public:
			// stores keyframes
			std::vector<sKeyFrame> keyframes;

			// frame rate of animation
			float frameRate = 24.0f;

			// whether the animation is looped
			bool looped = false;

			// adds a keyframe to the animation clip
			void AddKeyFrame(const sKeyFrame& i_keyframe);

			// loads an animation clip from a file
			static cResult LoadFromFile(const std::string& i_path, cAnimationClip*& o_clip);

			// clears all keyframes
			void clearKeyFrames();

			// gets duration of animation in seconds
			float GetDuration() const;
		private:
			cAnimationClip() = default;
		};
	}
}