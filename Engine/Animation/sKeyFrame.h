#pragma once


#include "Engine/Math/sVector.h"
#include "Engine/Math/cQuaternion.h"
#include <cstdint>

namespace eae6320
{
	namespace Animation
	{
		struct sKeyFrame
		{
			uint16_t frame;
			eae6320::Math::sVector translation;
			eae6320::Math::cQuaternion rotation;
		};
	}
}