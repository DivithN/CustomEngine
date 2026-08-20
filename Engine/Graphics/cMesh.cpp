#include "cMesh.h"
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>
#include <new>
#include <External/Lua/Includes.h>
#include <iostream>
#include <Engine/Platform/Platform.h>
#include <Engine/Math/cQuaternion.h>


//namespace
//{
//	eae6320::cResult LoadTableValues(lua_State& io_luaState, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, size_t& o_vertexCount, uint16_t*& o_indices, uint16_t& o_indexCount);
//	eae6320::cResult LoadTableValues_vertexCount(lua_State& io_luaState, size_t& o_vertexCount);
//	eae6320::cResult LoadTableValues_vertexData(lua_State& io_luaState, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData);
//	eae6320::cResult LoadTableValues_indexCount(lua_State& io_luaState, uint16_t& o_indexCount);
//	eae6320::cResult LoadTableValues_indexData(lua_State& io_luaState, uint16_t*& o_indices);
//
//	eae6320::cResult LoadAsset(const char* const i_path, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, size_t& o_vertexCount, uint16_t*& o_indices, uint16_t& o_indexCount);
//}




eae6320::cResult eae6320::Graphics::cMesh::Load(const std::string& MeshLocation, cMesh*& o_cMesh)
{
	auto result = eae6320::Results::Success;


	// data required for making mesh
	uint16_t vCount=0;
	eae6320::Graphics::VertexFormats::sVertex_mesh* vData = nullptr;
	uint16_t iCount=0;
	uint16_t* iData = nullptr;

	eae6320::Graphics::sTransform transform;

	//eae6320::Math::sVector translation;
	//eae6320::Math::cQuaternion rotation;
	//eae6320::Math::sVector scale;


	
	//loading asset
	//LoadAsset(MeshLocation.c_str(), vData, vCount, iData, iCount);
	Platform::sDataFromFile s_DataFile;
	std::string errMessage;



	if (!(result = Platform::LoadBinaryFile(MeshLocation.c_str(), s_DataFile, &errMessage)))
	{
		EAE6320_ASSERTF(false, "Failed to LoadBinaryFile");
		Logging::OutputError("Failed to LoadBinaryFile %s", errMessage.c_str());
	}


	auto currentOffset = reinterpret_cast<uintptr_t>(s_DataFile.data);
	const auto finalOffset = currentOffset + s_DataFile.size;


	
	//vertexData

	memcpy(&vCount, reinterpret_cast<void*>(currentOffset), sizeof(vCount));
	currentOffset += sizeof(vCount);

	vData = new eae6320::Graphics::VertexFormats::sVertex_mesh[vCount];

	memcpy(vData, reinterpret_cast<void*>(currentOffset), sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh) * vCount);
	currentOffset += sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh) * vCount;

	//indexdata

	memcpy(&iCount, reinterpret_cast<void*>(currentOffset), sizeof(iCount));
	currentOffset += sizeof(iCount);

	iData = new uint16_t[iCount];

	memcpy(iData, reinterpret_cast<void*>(currentOffset), sizeof(uint16_t) * iCount);
	currentOffset += sizeof(uint16_t) * iCount;

	//transform data
	memcpy(&transform.translation, reinterpret_cast<void*>(currentOffset), sizeof(transform.translation));
	currentOffset += sizeof(transform.translation);
	memcpy(&transform.rotation, reinterpret_cast<void*>(currentOffset), sizeof(transform.rotation));
	currentOffset += sizeof(transform.rotation);
	memcpy(&transform.scale, reinterpret_cast<void*>(currentOffset), sizeof(transform.scale));
	currentOffset += sizeof(transform.scale);


	cMesh* newMesh = nullptr;
	cScopeGuard scopeGuard([&o_cMesh, &result, &newMesh]
		{
			if (result)
			{
				EAE6320_ASSERT(newMesh != nullptr);
				o_cMesh = newMesh;
			}
			else
			{
				if (newMesh)
				{
					newMesh->DecrementReferenceCount();
					newMesh = nullptr;
				}
				o_cMesh = nullptr;
			}
		});
	{
		newMesh = new (std::nothrow) cMesh;
		if (!newMesh)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Failed to allocate memory for vertecies");
			//Logging::OutputError("Failed to allocate memory for the shader %s", fragmentShaderLocation.c_str());
			return result;
		}
	}

	// Initialize the platform-specific graphics API shader object
	if (!(result = newMesh->Initialize(vData,static_cast<size_t>(vCount),iData,iCount,transform)))
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed");
		return result;
	}

	if(vData)
		delete[] vData;

	if(iData)
		delete[] iData;

	return result;
}

eae6320::Graphics::cMesh::~cMesh()
{
	CleanUp();
}


////lua stuff
//
//namespace
//{
//	eae6320::cResult LoadAsset(const char* const i_path, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, size_t& o_vertexCount, uint16_t*& o_indices, uint16_t& o_indexCount)
//	{
//		auto result = eae6320::Results::Success;
//
//		// Create a new Lua state
//		lua_State* luaState = nullptr;
//		eae6320::cScopeGuard scopeGuard_onExit([&luaState]
//			{
//				if (luaState)
//				{
//					// If I haven't made any mistakes
//					// there shouldn't be anything on the stack
//					// regardless of any errors
//					EAE6320_ASSERT(lua_gettop(luaState) == 0);
//
//					lua_close(luaState);
//					luaState = nullptr;
//				}
//			});
//		{
//			luaState = luaL_newstate();
//			if (!luaState)
//			{
//				result = eae6320::Results::OutOfMemory;
//				EAE6320_ASSERTF(false, "Failed to create a new Lua state");
//				return result;
//			}
//		}
//
//		// Load the asset file as a "chunk",
//		// meaning there will be a callable function at the top of the stack
//		const auto stackTopBeforeLoad = lua_gettop(luaState);
//		{
//			const auto luaResult = luaL_loadfile(luaState, i_path);
//			if (luaResult != LUA_OK)
//			{
//				result = eae6320::Results::Failure;
//				// Pop the error message
//				lua_pop(luaState, 1);
//				return result;
//			}
//		}
//		// Execute the "chunk", which should load the asset
//		// into a table at the top of the stack
//		{
//			constexpr int argumentCount = 0;
//			constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
//			constexpr int noMessageHandler = 0;
//			const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
//			if (luaResult == LUA_OK)
//			{
//				// A well-behaved asset file will only return a single value
//				const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
//				if (returnedValueCount == 1)
//				{
//					// A correct asset file _must_ return a table
//					if (!lua_istable(luaState, -1))
//					{
//						result = eae6320::Results::InvalidFile;
//						EAE6320_ASSERTF(false, "Asset files must return a table");
//						// Pop the returned non-table value
//						lua_pop(luaState, 1);
//						return result;
//					}
//				}
//				else
//				{
//					result = eae6320::Results::InvalidFile;
//					EAE6320_ASSERTF(false, "Asset files must return a table");
//					// Pop every value that was returned
//					lua_pop(luaState, returnedValueCount);
//					return result;
//				}
//			}
//			else
//			{
//				result = eae6320::Results::InvalidFile;
//				// Pop the error message
//				lua_pop(luaState, 1);
//				return result;
//			}
//		}
//
//		// If this code is reached the asset file was loaded successfully,
//		// and its table is now at index -1
//
//		// A scope guard is used to pop the asset as soon as the scope (this function) is left
//		eae6320::cScopeGuard scopeGuard_popAssetTable([luaState]
//			{
//				lua_pop(luaState, 1);
//			});
//
//		// Load the values, assuming that the asset table is at index -1
//		result = LoadTableValues(*luaState, o_vertexData, o_vertexCount, o_indices, o_indexCount);
//
//		return result;
//	}
//
//
//	eae6320::cResult LoadTableValues(lua_State& io_luaState, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, size_t& o_vertexCount, uint16_t*& o_indices, uint16_t& o_indexCount)
//	{
//		auto result = eae6320::Results::Success;
//
//		if (!(result = LoadTableValues_vertexCount(io_luaState, o_vertexCount)))
//		{
//			eae6320::Logging::OutputError("Failed to load vertex count");
//		}
//
//		if (!(result = LoadTableValues_vertexData(io_luaState, o_vertexData)))
//		{
//			eae6320::Logging::OutputError("Failed to load vertex data");
//		}
//
//		if (!(result = LoadTableValues_indexCount(io_luaState, o_indexCount)))
//		{
//			eae6320::Logging::OutputError("Faild to load index count");
//		}
//
//		if (!(result = LoadTableValues_indexData(io_luaState, o_indices)))
//		{
//			eae6320::Logging::OutputError("Faild to load index data");
//		}
//
//
//
//		return result;
//	}
//
//	eae6320::cResult LoadTableValues_vertexCount(lua_State& io_luaState, size_t& o_vertexCount)
//	{
//		auto result = eae6320::Results::Success;
//
//		constexpr auto* const key = "vertexCount";
//		lua_pushstring(&io_luaState, key);
//		lua_gettable(&io_luaState, -2);
//
//		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
//			{
//				lua_pop(&io_luaState, 1);
//			});
//		const auto value = lua_tonumber(&io_luaState, -1);
//
//		o_vertexCount = static_cast<size_t>(value);
//
//		return result;
//	}
//
//	eae6320::cResult LoadTableValues_vertexData(lua_State& io_luaState, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData)
//	{
//		auto result = eae6320::Results::Success;
//
//		constexpr auto* const key = "verticies";
//		lua_pushstring(&io_luaState, key);
//		lua_gettable(&io_luaState, -2);
//
//		const auto vertexCount =static_cast<unsigned int>( luaL_len(&io_luaState, -1));
//
//		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
//			{
//				lua_pop(&io_luaState, 1);
//			});
//
//		o_vertexData = new eae6320::Graphics::VertexFormats::sVertex_mesh[vertexCount];
//
//		for (unsigned int i = 1; i <= vertexCount; i++)
//		{
//			lua_pushnumber(&io_luaState, i);
//			lua_gettable(&io_luaState, -2);
//			const auto vLength = luaL_len(&io_luaState, -1);
//			float x[3] = {};
//			uint8_t y[4] = {};
//			eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
//			{
//				lua_pop(&io_luaState, 1);
//			});
//			for (unsigned int j = 1; j <= vLength; j++)
//			{
//				lua_pushnumber(&io_luaState, j);
//				lua_gettable(&io_luaState, -2);
//				eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
//					{
//						lua_pop(&io_luaState, 1);
//					});
//				const auto value = lua_tonumber(&io_luaState, -1);
//				if (j <= 3)
//					x[j-1] = static_cast<float>(value);
//				if (j > 3)
//					y[j - 4] = static_cast<uint8_t>(std::round(value*255));
//			}
//			if (vLength < 4)
//			{
//				y[0] = 255;
//				y[1] = 255;
//				y[2] = 255;
//			}
//			if (vLength < 7)
//				y[3] = 255;
//			o_vertexData[i - 1] = { x[0],x[1],x[2],y[0],y[1],y[2],y[3]};
//		}
//
//		return result;
//	}
//
//	eae6320::cResult LoadTableValues_indexCount(lua_State& io_luaState, uint16_t& o_indexCount)
//	{
//		auto result = eae6320::Results::Success;
//
//		constexpr auto* const key = "indexCount";
//		lua_pushstring(&io_luaState, key);
//		lua_gettable(&io_luaState, -2);
//
//		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
//			{
//				lua_pop(&io_luaState, 1);
//			});
//		const auto value = lua_tonumber(&io_luaState, -1);
//
//		o_indexCount = static_cast<uint16_t>(value);
//
//		return eae6320::cResult();
//	}
//
//	eae6320::cResult LoadTableValues_indexData(lua_State& io_luaState, uint16_t*& o_indices)
//	{
//		auto result = eae6320::Results::Success;
//
//		constexpr auto* const key = "indices";
//		lua_pushstring(&io_luaState, key);
//		lua_gettable(&io_luaState, -2);
//
//		const auto indexCount =static_cast<unsigned int>(luaL_len(&io_luaState, -1));
//
//		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState]
//			{
//				lua_pop(&io_luaState, 1);
//			});
//
//		o_indices = new uint16_t[indexCount];
//
//		for (unsigned int i = 1; i <= indexCount; i++)
//		{
//			lua_pushnumber(&io_luaState, i);
//			lua_gettable(&io_luaState, -2);
//			eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
//				{
//					lua_pop(&io_luaState, 1);
//				});
//			const auto index = lua_tonumber(&io_luaState, -1);
//			o_indices[i - 1] = static_cast<uint16_t>(index);
//		}
//
//
//		return result;
//	}
//}