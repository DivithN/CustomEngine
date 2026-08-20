#include "cMeshBuilder.h"
#include <Engine/Results/Results.h>

#include <Engine/ScopeGuard/cScopeGuard.h>
#include <new>
#include <External/Lua/Includes.h>
#include <iostream>


#include <fstream>

#include <Engine/Platform/Platform.h>
#include <Tools/AssetBuildLibrary/Functions.h>

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments)
{
	cResult result = Results::Success;

	std::string errorMessage;

	// data required for making mesh
	uint16_t vCount;
	eae6320::Graphics::VertexFormats::sVertex_mesh* vData = nullptr;
	uint16_t iCount;
	uint16_t* iData = nullptr;

	Transform transform;
	std::vector<sKeyFrame> keyframes;


	//if (!(result= eae6320::Platform::CopyFile(m_path_source, m_path_target, false, true, &errorMessage)))
	//{
	//	OutputErrorMessageWithFileInfo(m_path_source, errorMessage.c_str());
	//	return Results::Failure;
	//}
	if(!(result = LoadAsset(m_path_source, vData, vCount, iData, iCount, transform, keyframes)))
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to load mesh asset");
		return result;
	}
	std::ofstream outfile(m_path_target, std::ofstream::binary|std::ofstream::out);

	uint16_t numOfFrames = static_cast<uint16_t>(keyframes.size());



	outfile.write(reinterpret_cast<const char*>(&vCount), sizeof(vCount));
	outfile.write(reinterpret_cast<const char*>(vData), sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh) * vCount);
	outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(iCount));
	outfile.write(reinterpret_cast<const char*>(iData), sizeof(uint16_t) * iCount);
	outfile.write(reinterpret_cast<const char*>(&transform.translation), sizeof(eae6320::Math::sVector));
	outfile.write(reinterpret_cast<const char*>(&transform.rotation), sizeof(eae6320::Math::cQuaternion));
	outfile.write(reinterpret_cast<const char*>(&transform.scale), sizeof(eae6320::Math::sVector));
	outfile.write(reinterpret_cast<const char*>(&numOfFrames), sizeof(uint16_t));

	for(const auto& keyframe : keyframes)
	{
		outfile.write(reinterpret_cast<const char*>(&keyframe.frame), sizeof(uint16_t));
		outfile.write(reinterpret_cast<const char*>(&keyframe.translation), sizeof(eae6320::Math::sVector));
		outfile.write(reinterpret_cast<const char*>(&keyframe.rotation), sizeof(eae6320::Math::cQuaternion));
	}

	outfile.close();



	return result;
}

//lua stuff


eae6320::cResult eae6320::Assets::cMeshBuilder::LoadAsset(const char* const i_path, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, uint16_t& o_vertexCount, uint16_t*& o_indices, uint16_t& o_indexCount, Transform& o_transform, std::vector<sKeyFrame>& o_keyframes)
{
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	eae6320::cScopeGuard scopeGuard_onExit([&luaState]
		{
			if (luaState)
			{
				// If I haven't made any mistakes
				// there shouldn't be anything on the stack
				// regardless of any errors
				EAE6320_ASSERT(lua_gettop(luaState) == 0);

				lua_close(luaState);
				luaState = nullptr;
			}
		});
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Failed to create a new Lua state");
			return result;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					EAE6320_ASSERTF(false, "Asset files must return a table");
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				EAE6320_ASSERTF(false, "Asset files must return a table");
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1

	// A scope guard is used to pop the asset as soon as the scope (this function) is left
	eae6320::cScopeGuard scopeGuard_popAssetTable([luaState]
		{
			lua_pop(luaState, 1);
		});

	// Load the values, assuming that the asset table is at index -1
	result = LoadTableValues(*luaState, o_vertexData, o_vertexCount, o_indices, o_indexCount, o_transform, o_keyframes);

	return result;
}


eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues(lua_State& io_luaState, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, uint16_t& o_vertexCount, uint16_t*& o_indices, uint16_t& o_indexCount, Transform& o_transform, std::vector<sKeyFrame>& o_keyframes)
{
	auto result = eae6320::Results::Success;

	if (!(result = LoadTableValues_vertexCount(io_luaState, o_vertexCount)))
	{
		OutputErrorMessageWithFileInfo(m_path_target,"Failed to load vertex count");
	}

	if (!(result = LoadTableValues_vertexData(io_luaState, o_vertexData)))
	{
		OutputErrorMessageWithFileInfo(m_path_target, "Failed to load vertex data");
	}

	if (!(result = LoadTableValues_indexCount(io_luaState, o_indexCount)))
	{
		OutputErrorMessageWithFileInfo(m_path_target, "Failed to load index count");
	}

	if (!(result = LoadTableValues_indexData(io_luaState, o_indices)))
	{
		OutputErrorMessageWithFileInfo(m_path_target, "Failed to load index data");
	}
	
	if(!(result = LoadTableValues_Transform(io_luaState, o_transform)))
	{
		OutputErrorMessageWithFileInfo(m_path_target, "Failed to load transform data");
	}
	if (!(result = LoadTableValues_Animation(io_luaState, o_keyframes)))
	{
		OutputErrorMessageWithFileInfo(m_path_target, "Failed to load animation data");
	}



	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_vertexCount(lua_State& io_luaState, uint16_t& o_vertexCount)
{
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "vertexCount";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	const auto value = lua_tonumber(&io_luaState, -1);

	o_vertexCount = static_cast<uint16_t>(value);

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_vertexData(lua_State& io_luaState, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData)
{
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "verticies";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	const auto vertexCount = static_cast<uint16_t>(luaL_len(&io_luaState, -1));

	eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	o_vertexData = new eae6320::Graphics::VertexFormats::sVertex_mesh[vertexCount];

	for (uint16_t i = 1; i <= vertexCount; i++)
	{
		lua_pushnumber(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		const auto vLength = luaL_len(&io_luaState, -1);
		float x[3] = {};
		uint8_t y[4] = {};
		eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		for (uint16_t j = 1; j <= vLength; j++)
		{
			lua_pushnumber(&io_luaState, j);
			lua_gettable(&io_luaState, -2);
			eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});
			const auto value = lua_tonumber(&io_luaState, -1);
			if (j <= 3)
				x[j - 1] = static_cast<float>(value);
			if (j > 3)
				y[j - 4] = static_cast<uint8_t>(std::round(value * 255));
		}
		if (vLength < 4)
		{
			y[0] = 255;
			y[1] = 255;
			y[2] = 255;
		}
		if (vLength < 7)
			y[3] = 255;
		o_vertexData[i - 1] = { x[0],x[1],x[2],y[0],y[1],y[2],y[3] };
	}

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_indexCount(lua_State& io_luaState, uint16_t& o_indexCount)
{
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "indexCount";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	const auto value = lua_tonumber(&io_luaState, -1);

	o_indexCount = static_cast<uint16_t>(value);

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_indexData(lua_State& io_luaState, uint16_t*& o_indices)
{
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "indices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	const auto indexCount = static_cast<uint16_t>(luaL_len(&io_luaState, -1));

	eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	o_indices = new uint16_t[indexCount];

	for (uint16_t i = 1; i <= indexCount; i++)
	{
		lua_pushnumber(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto index = lua_tonumber(&io_luaState, -1);
		o_indices[i - 1] = static_cast<uint16_t>(index);
	}


	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_Transform(lua_State& io_luaState, Transform& o_transform)
{
	auto result = eae6320::Results::Success;



	constexpr auto* const key = "meshTransform";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	if (!lua_istable(&io_luaState, -1))
	{
		//OutputErrorMessageWithFileInfo(m_path_target, "Does not contain Mesh transform");
		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		return result;
	}

	eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	//translation
	{
		constexpr auto* const key = "translation";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValue_translation([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		float x[3] = {};
		const auto length = static_cast<uint16_t>(luaL_len(&io_luaState, -1));
		for (uint16_t i = 1; i <= length; i++)
		{
			lua_pushnumber(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			eae6320::cScopeGuard scopeGuard_popValue_translation_component([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});
			const auto value = lua_tonumber(&io_luaState, -1);
			x[i - 1] = static_cast<float>(value);
		}
		o_transform.translation = { x[0],x[1],x[2] };
	}
	//rotation
	{
		constexpr auto* const key = "rotation";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValue_rotation([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		float y[4] = {};
		const auto length = static_cast<uint16_t>(luaL_len(&io_luaState, -1));
		for (uint16_t i = 1; i <= length; i++)
		{
			lua_pushnumber(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			eae6320::cScopeGuard scopeGuard_popValue_rotation_component([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});
			const auto value = lua_tonumber(&io_luaState, -1);
			y[i - 1] = static_cast<float>(value);
		}
		o_transform.rotation = eae6320::Math::cQuaternion::FromComponents(y[3],y[0], y[1], y[2]);
	}
	//scale
	{
		constexpr auto* const key_scale = "scale";
		lua_pushstring(&io_luaState, key_scale);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValue_scale([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		float z[3] = {};
		const auto length_scale = static_cast<uint16_t>(luaL_len(&io_luaState, -1));
		for (uint16_t i = 1; i <= length_scale; i++)
		{
			lua_pushnumber(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			eae6320::cScopeGuard scopeGuard_popValue_scale_component([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});
			const auto value = lua_tonumber(&io_luaState, -1);
			z[i - 1] = static_cast<float>(value);
		}
		o_transform.scale = { z[0],z[1],z[2] };
	}
	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_Animation(lua_State& io_luaState,std::vector<sKeyFrame>& o_keyframes)
{
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "animation";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	if (!lua_istable(&io_luaState, -1))
	{
		//OutputErrorMessageWithFileInfo(m_path_target, "Does not contain Animation Data");
		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		return result;
	}

	
	const auto keyframeCount = static_cast<uint16_t>(luaL_len(&io_luaState, -1));
	eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});



	for (uint16_t i = 1; i <= keyframeCount; ++i)
	{
		lua_pushnumber(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		sKeyFrame frame;
		{
			//frame number;

			lua_pushstring(&io_luaState, "frame");
			lua_gettable(&io_luaState, -2);
			frame.frame = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
			eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});

		}

		//translation
		{
			lua_pushstring(&io_luaState, "translation");
			lua_gettable(&io_luaState, -2);
			float x[3];
			for (int j = 1; j <= 3; ++j)
			{
				lua_pushnumber(&io_luaState, j);
				lua_gettable(&io_luaState, -2);
				x[j - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
			}
			frame.translation.x = x[0];
			frame.translation.y = x[1];
			frame.translation.z = x[2];
			eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});
		}

		//rotation
		{
			lua_pushstring(&io_luaState, "rotation");
			lua_gettable(&io_luaState, -2);
			float y[4];
			for (int j = 1; j <= 4; ++j)
			{
				lua_pushnumber(&io_luaState, j);
				lua_gettable(&io_luaState, -2);
				y[j - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
			}
			frame.rotation = eae6320::Math::cQuaternion::FromComponents(y[3], y[0], y[1], y[2]);
			eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});
		}
		o_keyframes.push_back(frame);

	}

	return result;
}
