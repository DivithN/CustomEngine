#include "../cMesh.h"
#include "../sContext.h"
#include "../cVertexFormat.h"
#include "Includes.h"

#include<Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

eae6320::cResult eae6320::Graphics::cMesh::Initialize(eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[], const size_t vertexCount, uint16_t indices[], uint16_t indexCount, eae6320::Graphics::sTransform i_transform)
{
	auto result = eae6320::Results::Success;

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	// Vertex Format
	{
		if (!(result = eae6320::Graphics::cVertexFormat::Load(eae6320::Graphics::eVertexType::Mesh, m_vertexFormat,
			"data/Shaders/Vertex/vertexInputLayout_mesh.shader")))
		{
			EAE6320_ASSERTF(false, "Can't initialize geometry without vertex format");
			return result;
		}
	}

	//store transform
	m_transform = i_transform;



	// Vertex Buffer new
	{
		//eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] =
		//{
		//	{ -1.0f , 0.0f, 0.0f },
		//	{ 0.0f , 1.0f, 0.0f },
		//	{ 1.0f , 0.0f, 0.0f },

		//	{ -0.5f,  0.0f, 0.0f }, 
		//	{ -0.15f, 0.0f, 0.0f }, 
		//	{ -0.5f, -1.0f, 0.0f }, 
		//	{ -0.15f,-1.0f, 0.0f }, 

		//	{  0.15f, 0.0f, 0.0f }, 
		//	{  0.5f,  0.0f, 0.0f }, 
		//	{  0.5f, -1.0f, 0.0f }, 
		//	{ 0.15f , -1.0f, 0.0f},

		//	{ -0.15f, -0.5f, 0.0f },
		//	{  0.15f, -0.5f, 0.0f },

		//};
		//constexpr auto vertexCount = std::size(vertexData);

		const auto bufferSize = sizeof(vertexData[0]) * vertexCount;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());
		const auto bufferDescription = [bufferSize]
			{
				D3D11_BUFFER_DESC bufferDescription{};

				bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
				bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
				bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
				bufferDescription.MiscFlags = 0;
				bufferDescription.StructureByteStride = 0;	// Not used

				return bufferDescription;
			}();

		const auto initialData = [vertexData]
			{
				D3D11_SUBRESOURCE_DATA initialData{};

				initialData.pSysMem = vertexData;
				// (The other data members are ignored for non-texture buffers)

				return initialData;
			}();
		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_vertexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}

	//Index buffer
	{
		//uint16_t indices[] =
		//{
		//	0,1,2,

		//	3,4,5,
		//	5,4,6,

		//	7,8,9,
		//	10,7,9,

		//	4,7,12,
		//	11,4,12

		//};
		//m_indexCount = static_cast<uint16_t>(std::size(indices));
		m_indexCount = indexCount;
		const auto bufferSize = sizeof(uint16_t) * m_indexCount;

		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());
		const auto bufferDescription = [bufferSize]
			{
				D3D11_BUFFER_DESC bufferDescription{};

				bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
				bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
				bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
				bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
				bufferDescription.MiscFlags = 0;
				bufferDescription.StructureByteStride = 0;	// Not used

				return bufferDescription;
			}();

		const auto initialIndexData = [indices]
			{
				D3D11_SUBRESOURCE_DATA initialIndexData{};

				initialIndexData.pSysMem = indices;
				// (The other data members are ignored for non-texture buffers)

				return initialIndexData;
			}();
		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialIndexData, &m_indexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}

	/*// Vertex Buffer
	{
		constexpr unsigned int triangleCount = 7;
		constexpr unsigned int vertexCountPerTriangle = 3;
		constexpr auto vertexCount = triangleCount * vertexCountPerTriangle;
		eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[vertexCount];
		{
			// Direct3D is left-handed

			vertexData[0].x = -1.0f;
			vertexData[0].y = 0.0f;
			vertexData[0].z = 0.0f;

			vertexData[1].x = 0.0f;
			vertexData[1].y = 1.0f;
			vertexData[1].z = 0.0f;

			vertexData[2].x = 1.0f;
			vertexData[2].y = 0.0f;
			vertexData[2].z = 0.0f;

			//first rectangle left

			vertexData[3].x = -0.5f;
			vertexData[3].y = 0.0f;
			vertexData[3].z = 0.0f;

			vertexData[4].x = -0.15f;
			vertexData[4].y = 0.0f;
			vertexData[4].z = 0.0f;

			vertexData[5].x = -0.5f;
			vertexData[5].y = -1.0f;
			vertexData[5].z = 0.0f;

			vertexData[6].x = -0.15f;
			vertexData[6].y = 0.0f;
			vertexData[6].z = 0.0f;

			vertexData[7].x = -0.15f;
			vertexData[7].y = -1.0f;
			vertexData[7].z = 0.0f;

			vertexData[8].x = -0.5f;
			vertexData[8].y = -1.0f;
			vertexData[8].z = 0.0f;

			//second rectangle right

			vertexData[9].x = 0.5f;
			vertexData[9].y = 0.0f;
			vertexData[9].z = 0.0f;

			vertexData[10].x = 0.5f;
			vertexData[10].y = -1.0f;
			vertexData[10].z = 0.0f;

			vertexData[11].x = 0.15f;
			vertexData[11].y = 0.0f;
			vertexData[11].z = 0.0f;

			vertexData[12].x = 0.15f;
			vertexData[12].y = -1.0f;
			vertexData[12].z = 0.0f;

			vertexData[13].x = 0.15f;
			vertexData[13].y = 0.0f;
			vertexData[13].z = 0.0f;

			vertexData[14].x = 0.5f;
			vertexData[14].y = -1.0f;
			vertexData[14].z = 0.0f;

			//third triangle in the middle

			vertexData[15].x = -0.15f;
			vertexData[15].y = 0.0f;
			vertexData[15].z = 0.0f;

			vertexData[16].x = 0.15f;
			vertexData[16].y = 0.0f;
			vertexData[16].z = 0.0f;

			vertexData[17].x = -0.15f;
			vertexData[17].y = -0.5f;
			vertexData[17].z = 0.0f;

			vertexData[18].x = -0.15f;
			vertexData[18].y = -0.5f;
			vertexData[18].z = 0.0f;

			vertexData[19].x = 0.15f;
			vertexData[19].y = 0.0f;
			vertexData[19].z = 0.0f;

			vertexData[20].x = 0.15f;
			vertexData[20].y = -0.5f;
			vertexData[20].z = 0.0f;

		}
		constexpr auto bufferSize = sizeof(vertexData[0]) * vertexCount;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());
		constexpr auto bufferDescription = [bufferSize]
			{
				D3D11_BUFFER_DESC bufferDescription{};

				bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
				bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
				bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
				bufferDescription.MiscFlags = 0;
				bufferDescription.StructureByteStride = 0;	// Not used

				return bufferDescription;
			}();

		const auto initialData = [vertexData]
			{
				D3D11_SUBRESOURCE_DATA initialData{};

				initialData.pSysMem = vertexData;
				// (The other data members are ignored for non-texture buffers)

				return initialData;
			}();

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_vertexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}*/

	return result;
}


void eae6320::Graphics::cMesh::Draw()

{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);
	// Bind a specific vertex buffer to the device as a data source
	{
		EAE6320_ASSERT(m_vertexBuffer != nullptr);
		constexpr unsigned int startingSlot = 0;
		constexpr unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		constexpr unsigned int bufferStride = sizeof(VertexFormats::sVertex_mesh);
		// It's possible to start streaming data in the middle of a vertex buffer
		constexpr unsigned int bufferOffset = 0;
		direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset);
	}
	// Specify what kind of data the vertex buffer holds
	{
		// Bind the vertex format (which defines how to interpret a single vertex)
		{
			EAE6320_ASSERT(m_vertexFormat != nullptr);
			m_vertexFormat->Bind();
		}
		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// the vertex buffer was defined as a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	// bind the index buffer
	{
		EAE6320_ASSERT(m_indexBuffer);
		constexpr DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
		// The indices start at the beginning of the buffer
		constexpr unsigned int offset = 0;
		direct3dImmediateContext->IASetIndexBuffer(m_indexBuffer, indexFormat, offset);
	}

	//// Render triangles from the currently-bound vertex buffer
	//{
	//	// As of this comment only a single triangle is drawn
	//	// (you will have to update this code in future assignments!)
	//	constexpr unsigned int triangleCount = 7;
	//	constexpr unsigned int vertexCountPerTriangle = 3;
	//	constexpr auto vertexCountToRender = triangleCount * vertexCountPerTriangle;
	//	// It's possible to start rendering primitives in the middle of the stream
	//	constexpr unsigned int indexOfFirstVertexToRender = 0;
	//	direct3dImmediateContext->Draw(vertexCountToRender, indexOfFirstVertexToRender);
	//}
	// It's possible to start rendering primitives in the middle of the stream
	{
		constexpr unsigned int indexOfFirstIndexToUse = 0;
		constexpr unsigned int offsetToAddToEachIndex = 0;
		direct3dImmediateContext->DrawIndexed(static_cast<unsigned int>(m_indexCount), indexOfFirstIndexToUse, offsetToAddToEachIndex);
	}
}

void eae6320::Graphics::cMesh::CleanUp()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (m_vertexFormat)
	{
		m_vertexFormat->DecrementReferenceCount();
		m_vertexFormat = nullptr;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
}