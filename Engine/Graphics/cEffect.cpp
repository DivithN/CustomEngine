#include "cEffect.h"
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>
#include <new>

eae6320::cResult eae6320::Graphics::cEffect::Load(const std::string& vertexShaderLocation, const std::string& fragmentShaderLocation, cEffect*& o_effect)
{
	auto result = eae6320::Results::Success;

	cEffect* newEffect = nullptr;
	cScopeGuard scopeGuard([&o_effect, &result, &newEffect]
		{
			if (result)
			{
				EAE6320_ASSERT(newEffect != nullptr);
				o_effect = newEffect;
			}
			else
			{
				if (newEffect)
				{
					newEffect->DecrementReferenceCount();
					newEffect = nullptr;
				}
				o_effect = nullptr;
			}
		});

	{
		newEffect = new (std::nothrow) cEffect;
		if (!newEffect)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the shader %s", vertexShaderLocation.c_str());
			Logging::OutputError("Failed to allocate memory for the shader %s", fragmentShaderLocation.c_str());
			return result;
		}
	}
	// Initialize the platform-specific graphics API shader object
	if (!(result = newEffect->Initialize(vertexShaderLocation,fragmentShaderLocation)))
	{
		EAE6320_ASSERTF(false, "Initialization of new effect failed");
		return result;
	}

	return result;
}

eae6320::Graphics::cEffect::~cEffect()
{
	CleanUp();
}

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const std::string& vertexShaderLocation, const std::string& fragmentShaderLocation)
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::Load(vertexShaderLocation,
		m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return result;
	}
	if (!(result = eae6320::Graphics::cShader::Load(fragmentShaderLocation,
		m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return result;
	}
	{
		constexpr auto renderStateBits = []
			{
				uint8_t renderStateBits = 0;

				eae6320::Graphics::RenderStates::EnableAlphaTransparency(renderStateBits);
				eae6320::Graphics::RenderStates::EnableDepthTesting(renderStateBits);
				eae6320::Graphics::RenderStates::EnableDepthWriting(renderStateBits);
				eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides(renderStateBits);

				return renderStateBits;
			}();
		if (!(result = m_renderState.Initialize(renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}
	eae6320::Graphics::cEffect::InitializePlatformSpecific(result);


	return result;
}


void eae6320::Graphics::cEffect::CleanUp()
{
	auto result = Results::Success;

	CleanUpPlatformSpecific();


	if (m_vertexShader)
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}
	if (m_fragmentShader)
	{
		m_fragmentShader->DecrementReferenceCount();
		m_fragmentShader = nullptr;
	}

}