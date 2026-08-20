#pragma once
/*
	This class builds meshes
*/

#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

// Includes
//=========

#include <External/Lua/Includes.h>

#include <Tools/AssetBuildLibrary/iBuilder.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Math/cQuaternion.h>
#include <Engine/Math/sVector.h>

#include <Engine/Windows/Includes.h>
// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cMeshBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			struct Transform
			{
				eae6320::Math::sVector translation;
				eae6320::Math::cQuaternion rotation;
				eae6320::Math::sVector scale;
			};

			struct sKeyFrame
			{
				uint16_t frame;
				eae6320::Math::sVector translation;
				eae6320::Math::cQuaternion rotation;
			};

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) final;

			// Implementation
			//===============

			//function to load entire table
			eae6320::cResult LoadTableValues(lua_State& io_luaState, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData,
											uint16_t& o_vertexCount, uint16_t*& o_indices, uint16_t& o_indexCount, Transform& o_transform, 
											std::vector<sKeyFrame>& o_keyframes);
			//functions to load mesh data
			eae6320::cResult LoadTableValues_vertexCount(lua_State& io_luaState, uint16_t& o_vertexCount);
			eae6320::cResult LoadTableValues_vertexData(lua_State& io_luaState, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData);
			eae6320::cResult LoadTableValues_indexCount(lua_State& io_luaState, uint16_t& o_indexCount);
			eae6320::cResult LoadTableValues_indexData(lua_State& io_luaState, uint16_t*& o_indices);

			//functions to load transform data
			eae6320::cResult LoadTableValues_Transform(lua_State& io_luaState, Transform& o_transform);
			
			//functions to load animation data
			eae6320::cResult LoadTableValues_Animation(lua_State& io_luaState, std::vector<sKeyFrame>& o_keyframes);

			//function to load entire asset
			eae6320::cResult LoadAsset(const char* const i_path, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, 
										uint16_t& o_vertexCount, uint16_t*& o_indices, uint16_t& o_indexCount, Transform& o_transform, 
										std::vector<sKeyFrame>& o_keyframes);


		private:
		};
	}
}

#endif	// EAE6320_CMESHBUILDER_H