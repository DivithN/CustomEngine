#pragma once
#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>

#include "cShader.h"
#include "cRenderState.h"

#if defined( EAE6320_PLATFORM_D3D)
struct ID3D11DeviceContext;
#endif

namespace eae6320
{
	namespace Graphics
	{
		class cEffect
		{
		public:

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect);


			void BindShadingData();

			static cResult Load(const std::string& vertexShaderLocation, const std::string& fragmentShaderLocation, cEffect*& o_effect);


		private:

			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			cEffect() = default;
			~cEffect();

			class cResult Initialize(const std::string& vertexShaderLocation, const std::string& fragmentShaderLocation);
			void CleanUp();
			void CleanUpPlatformSpecific();
			class cResult InitializePlatformSpecific(cResult& result);

#if defined( EAE6320_PLATFORM_GL)

			GLuint m_programId = 0;
#endif
			eae6320::Graphics::cShader* m_vertexShader = nullptr;
			eae6320::Graphics::cShader* m_fragmentShader = nullptr;


			eae6320::Graphics::cRenderState m_renderState;
		};
	}
}