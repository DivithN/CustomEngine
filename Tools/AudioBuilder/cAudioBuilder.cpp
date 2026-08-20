#include "cAudioBuilder.h"
#include <Engine/Results/Results.h>

#include <Engine/Platform/Platform.h>
#include <Tools/AssetBuildLibrary/Functions.h>

eae6320::cResult eae6320::Assets::cAudioBuilder::Build(const std::vector<std::string>& i_arguments)
{
	cResult result = Results::Success;
	std::string errorMessage;
	if (!(result= eae6320::Platform::CopyFile(m_path_source, m_path_target, false, true, &errorMessage)))
	{
		OutputErrorMessageWithFileInfo(m_path_source, errorMessage.c_str());
		return Results::Failure;
	}
	return result;
}