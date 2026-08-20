#include "Graphics.h"

#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"

#include "cMesh.h"
#include "cEffect.h"

#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserOutput/UserOutput.h>


#include <Engine/ScopeGuard/cScopeGuard.h>

namespace
{
	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);

	eae6320::Graphics::cConstantBuffer s_constantBuffer_Draw(eae6320::Graphics::ConstantBufferTypes::DrawCall);


	// Submission Data
	//----------------
	const size_t s_MaxMeshEffectPair = 10;

	struct sMeshEffectPair
	{
		eae6320::Graphics::cMesh* Mesh = nullptr;
		eae6320::Graphics::cEffect* Effect = nullptr;
		eae6320::Graphics::ConstantBufferFormats::sDrawCall constantData_draw;
	};

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{

		eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
		float backGroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		sMeshEffectPair MeshEffect[s_MaxMeshEffectPair];
		size_t MeshEffectCount = 0;
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be in the process of being populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated and in the process of being rendered from in the render thread
	// (In other words, one is being produced while the other is being consumed)
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;

}

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_frame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;

}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		if (Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread))
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			if (!s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal())
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}



	//color stuff
	sContext::g_context.ClearColor(s_dataBeingRenderedByRenderThread->backGroundColor);


	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = s_dataBeingRenderedByRenderThread->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);


	}

	for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->MeshEffectCount; ++i)
	{
		if(s_dataBeingRenderedByRenderThread->MeshEffect[i].Effect)
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Effect->BindShadingData();

		auto& constantData_data = s_dataBeingRenderedByRenderThread->MeshEffect[i].constantData_draw;
		s_constantBuffer_Draw.Update(&constantData_data);

		if (s_dataBeingRenderedByRenderThread->MeshEffect[i].Mesh)
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Mesh->Draw();

		if (s_dataBeingRenderedByRenderThread->MeshEffect[i].Effect)
		{
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Effect->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Effect = nullptr;
		}

		if (s_dataBeingRenderedByRenderThread->MeshEffect[i].Mesh)
		{
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Mesh->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Mesh = nullptr;
		}

	}
	s_dataBeingRenderedByRenderThread->MeshEffectCount = 0;

	//swapbuffer
	sContext::g_context.SwapBuffer();

}

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without context");
		return result;
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_Draw.Initialize())
		{
			// There is only a single draw constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_Draw.Bind(
				// In our class both vertex and fragment shaders use per-draw constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without draw constant buffer");
			return result;
		}
	}
	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data has been submitted from the application thread");
			return result;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data can be submitted from the application thread");
			return result;
		}
	}

	// Initialize the views
	{
		if (!(result =sContext::g_context.InitializeViews(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the views");
			return result;
		}
	}
	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->MeshEffectCount; i++)
	{
		if (s_dataBeingRenderedByRenderThread->MeshEffect[i].Mesh)
		{
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Mesh->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Mesh = nullptr;
		}
		if(s_dataBeingRenderedByRenderThread->MeshEffect[i].Effect)
		{
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Effect->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->MeshEffect[i].Effect = nullptr;
		}
	}
	for (size_t i = 0; i < s_dataBeingSubmittedByApplicationThread->MeshEffectCount; i++)
	{
		if (s_dataBeingSubmittedByApplicationThread->MeshEffect[i].Mesh)
		{
			s_dataBeingSubmittedByApplicationThread->MeshEffect[i].Mesh->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->MeshEffect[i].Mesh = nullptr;
		}
		if (s_dataBeingSubmittedByApplicationThread->MeshEffect[i].Effect)
		{
			s_dataBeingSubmittedByApplicationThread->MeshEffect[i].Effect->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->MeshEffect[i].Effect = nullptr;
		}
	}

	{
		const auto result_constantBuffer_frame = s_constantBuffer_frame.CleanUp();
		if (!result_constantBuffer_frame)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_frame;
			}
		}
	}

	{
		const auto result_constantBuffer_draw = s_constantBuffer_Draw.CleanUp();
		if (!result_constantBuffer_draw)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_draw;
			}
		}
	}

	{
		const auto result_context = sContext::g_context.CleanUp();
		if (!result_context)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_context;
			}
		}
	}

	return result;
}

void eae6320::Graphics::SetBackgroundColor(float r, float g, float b, float a)
{
	s_dataBeingSubmittedByApplicationThread->backGroundColor[0] = r;
	s_dataBeingSubmittedByApplicationThread->backGroundColor[1] = g;
	s_dataBeingSubmittedByApplicationThread->backGroundColor[2] = b;
	s_dataBeingSubmittedByApplicationThread->backGroundColor[3] = a;
}

eae6320::cResult eae6320::Graphics::SubmitMeshEffect(cMesh* i_Mesh, cEffect* i_Effect, eae6320::Math::cMatrix_transformation i_transform)
{
	auto result = Results::Success;


	
	if (s_dataBeingSubmittedByApplicationThread->MeshEffectCount < s_MaxMeshEffectPair)
	{
		s_dataBeingSubmittedByApplicationThread->MeshEffect[s_dataBeingSubmittedByApplicationThread->MeshEffectCount].Effect = i_Effect;
		i_Effect->IncrementReferenceCount();

		s_dataBeingSubmittedByApplicationThread->MeshEffect[s_dataBeingSubmittedByApplicationThread->MeshEffectCount].constantData_draw.g_transform_localToWorld = i_transform;


		s_dataBeingSubmittedByApplicationThread->MeshEffect[s_dataBeingSubmittedByApplicationThread->MeshEffectCount].Mesh = i_Mesh;
		i_Mesh->IncrementReferenceCount();

		s_dataBeingSubmittedByApplicationThread->MeshEffectCount++;
	}
	else
	{
		EAE6320_ASSERTF(false, "MeshEffect Limit reached");
		Logging::OutputError("MeshEffect Limit reached");
		result = Results::OutOfMemory;
		return result;
	}


	return result;
}

eae6320::cResult eae6320::Graphics::SubmitCamera(eae6320::Math::cMatrix_transformation CamTransform, eae6320::Math::cMatrix_transformation i_CamSettings)
{
	auto result = Results::Success;
	s_dataBeingSubmittedByApplicationThread->constantData_frame.g_transform_worldToCamera = CamTransform;
	s_dataBeingSubmittedByApplicationThread->constantData_frame.g_transform_cameraToProjected = i_CamSettings;

	return result;
}

