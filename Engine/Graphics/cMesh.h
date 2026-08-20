#pragma once

#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>

#include "sContext.h"
#include "VertexFormats.h"

#include <Engine/Graphics/sTransform.h>

#include <string>
#include <cstdint>

#if defined( EAE6320_PLATFORM_D3D)
struct ID3D11Buffer;
struct ID3D11DeviceContext;
namespace eae6320 
{ 
	namespace Graphics 
	{ 
		class cVertexFormat; 
	} 
}
#elif defined(EAE6320_PLATFORM_GL)
#endif

namespace eae6320
{
	namespace Graphics
	{
		class cMesh
		{
		public:

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cMesh);

			//static cResult Load(eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[], const size_t vertexCount, uint16_t indices[], uint16_t indexCount, cMesh*& o_cMesh);
			static cResult Load(const std::string& MeshLocation, cMesh*& o_cMesh);

			
			void Draw();

			eae6320::Graphics::sTransform m_transform;

		private:

			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			cMesh() = default;
			~cMesh();

			cResult Initialize(eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[], const size_t vertexCount, uint16_t indices[], uint16_t indexCount, eae6320::Graphics::sTransform i_transform);
			void CleanUp();



#if defined(EAE6320_PLATFORM_D3D)

			// A vertex format
			eae6320::Graphics::cVertexFormat* m_vertexFormat = nullptr;

			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* m_vertexBuffer = nullptr;

			// A index buffer
			ID3D11Buffer* m_indexBuffer = nullptr;

#elif defined(EAE6320_PLATFORM_GL)
			// A vertex buffer holds the data for each vertex
			GLuint m_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint m_vertexArrayId = 0;

			// A index buffer
			GLuint m_indexBufferId = 0;
#endif
			uint16_t m_indexCount = 0;

		};
	}
}