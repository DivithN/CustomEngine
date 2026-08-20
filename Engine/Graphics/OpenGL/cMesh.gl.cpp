#include "../cMesh.h"

#include "Engine/Asserts/Asserts.h"
#include <Engine/Logging/Logging.h>

eae6320::cResult eae6320::Graphics::cMesh::Initialize(eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[], const size_t vertexCount, uint16_t indices[], uint16_t indexCount, eae6320::Graphics::sTransform i_transform)
{
	auto result = eae6320::Results::Success;

	// Create a vertex array object and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &m_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(m_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	// Create a vertex buffer object and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &m_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}

	//new vertex buffer
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
		//const auto vertexCount = std::size(vertexData);

		const auto bufferSize = sizeof(vertexData[0]) * vertexCount;
		EAE6320_ASSERT(bufferSize <= static_cast<size_t>(std::numeric_limits<GLsizeiptr>::max()));
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), reinterpret_cast<GLvoid*>(vertexData),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}


	/*// Assign the data to the buffer
	{
		constexpr unsigned int triangleCount = 7;
		constexpr unsigned int vertexCountPerTriangle = 3;
		const auto vertexCount = triangleCount * vertexCountPerTriangle;
		eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[vertexCount];
		{
			// OpenGL is right-handed

			vertexData[0].x = -1.0f;
			vertexData[0].y = 0.0f;
			vertexData[0].z = 0.0f;

			vertexData[1].x = 1.0f;
			vertexData[1].y = 0.0f;
			vertexData[1].z = 0.0f;

			vertexData[2].x = 0.0f;
			vertexData[2].y = 1.0f;
			vertexData[2].z = 0.0f;

			//first rectangle left

			vertexData[3].x = -0.5f;
			vertexData[3].y = 0.0f;
			vertexData[3].z = 0.0f;

			vertexData[4].x = -0.5f;
			vertexData[4].y = -1.0f;
			vertexData[4].z = 0.0f;

			vertexData[5].x = -0.15f;
			vertexData[5].y = 0.0f;
			vertexData[5].z = 0.0f;

			vertexData[6].x = -0.15f;
			vertexData[6].y = -1.0f;
			vertexData[6].z = 0.0f;

			vertexData[7].x = -0.15f;
			vertexData[7].y = 0.0f;
			vertexData[7].z = 0.0f;

			vertexData[8].x = -0.5f;
			vertexData[8].y = -1.0f;
			vertexData[8].z = 0.0f;

			//second rectangle right

			vertexData[9].x = 0.5f;
			vertexData[9].y = 0.0f;
			vertexData[9].z = 0.0f;

			vertexData[10].x = 0.15f;
			vertexData[10].y = 0.0f;
			vertexData[10].z = 0.0f;

			vertexData[11].x = 0.5f;
			vertexData[11].y = -1.0f;
			vertexData[11].z = 0.0f;

			vertexData[12].x = 0.15f;
			vertexData[12].y = -1.0f;
			vertexData[12].z = 0.0f;

			vertexData[13].x = 0.5f;
			vertexData[13].y = -1.0f;
			vertexData[13].z = 0.0f;

			vertexData[14].x = 0.15f;
			vertexData[14].y = 0.0f;
			vertexData[14].z = 0.0f;


			//third triangle in the middle

			vertexData[15].x = -0.15f;
			vertexData[15].y = 0.0f;
			vertexData[15].z = 0.0f;

			vertexData[16].x = -0.15f;
			vertexData[16].y = -0.5f;
			vertexData[16].z = 0.0f;

			vertexData[17].x = 0.15f;
			vertexData[17].y = 0.0f;
			vertexData[17].z = 0.0f;

			vertexData[18].x = -0.15f;
			vertexData[18].y = -0.5f;
			vertexData[18].z = 0.0f;

			vertexData[19].x = 0.15f;
			vertexData[19].y = -0.5f;
			vertexData[19].z = 0.0f;

			vertexData[20].x = 0.15f;
			vertexData[20].y = 0.0f;
			vertexData[20].z = 0.0f;

		}
		constexpr auto bufferSize = sizeof(vertexData[0]) * vertexCount;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<GLsizeiptr>::max());
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), reinterpret_cast<GLvoid*>(vertexData),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}*/

	//index buffer
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &m_indexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	//index array
	{
		//uint16_t indices[] =
		//{
		//	1,0,2,

		//	4,3,5,
		//	4,5,6,

		//	8,7,9,
		//	7,10,9,

		//	7,4,12,
		//	4,11,12
		//};
		//m_indexCount = static_cast<uint16_t>(std::size(indices));


		for (int i = 0; i <= indexCount; i=i+3)
		{
			if (i >= indexCount)
				break;
			std::swap(indices[i+1], indices[i + 2]);
		}

		m_indexCount = indexCount;

		const auto bufferSize = sizeof(uint16_t) * m_indexCount;
		EAE6320_ASSERT(bufferSize <= static_cast<uint16_t>(std::numeric_limits<GLsizeiptr>::max()));
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), reinterpret_cast<GLvoid*>(indices),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}

	//add transform
	m_transform = i_transform;

	// Initialize vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		constexpr auto stride = static_cast<GLsizei>(sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh));

		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			constexpr GLint elementCount = 3;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sVertex_mesh, x)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		{
			constexpr GLuint vertexElementLocation = 1;
			constexpr GLint elementCount = 4;
			constexpr GLboolean notNormalized = GL_TRUE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sVertex_mesh, r)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
	}

	return result;
}

void eae6320::Graphics::cMesh::Draw()
{
	// Bind a specific vertex buffer to the device as a data source
	{
		EAE6320_ASSERT(m_vertexArrayId != 0);
		glBindVertexArray(m_vertexArrayId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	// Render triangles from the currently-bound vertex buffer
	{
		// The mode defines how to interpret multiple vertices as a single "primitive";
		// a triangle list is defined
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		constexpr GLenum mode = GL_TRIANGLES;
		// It's possible to start rendering primitives in the middle of the stream
		const GLvoid* const offset = 0;
		glDrawElements(mode, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_SHORT, offset);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}

void eae6320::Graphics::cMesh::CleanUp()
{
	auto result = Results::Success;
	if (m_vertexArrayId != 0)
	{
		// Make sure that the vertex array isn't bound
		{
			// Unbind the vertex array
			glBindVertexArray(0);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
		}
		constexpr GLsizei arrayCount = 1;
		glDeleteVertexArrays(arrayCount, &m_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the vertex array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_vertexArrayId = 0;
	}
	if (m_vertexBufferId != 0)
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &m_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_vertexBufferId = 0;
	}
	if (m_indexBufferId != 0)
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &m_indexBufferId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_indexBufferId = 0;
	}
}