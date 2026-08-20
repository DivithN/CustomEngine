#pragma once
/*
	This class builds audio assets
*/

#ifndef EAE6320_CAUDIOBUILDER_H
#define EAE6320_CAUDIOBUILDER_H

// Includes
//=========
#include <Tools/AssetBuildLibrary/iBuilder.h>

#include <Engine/Windows/Includes.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cAudioBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================
		private:
			// Build
			//------
			cResult Build(const std::vector<std::string>& i_arguments) final;
		};
	}
}

#endif