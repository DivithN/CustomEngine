#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>


namespace eae6320
{
	namespace Graphics
	{
		struct sTransform
		{
			eae6320::Math::sVector translation;
			eae6320::Math::cQuaternion rotation;
			eae6320::Math::sVector scale;
		};
	}
}