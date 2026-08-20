
// Includes
//=========

#include "cFinalGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>

#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include<Engine/Math/cMatrix_transformation.h>
#include<Engine/GameObject/sGameObject.h>

#include<Engine/Animation/cAnimationClip.h>
#include<Engine/Animation/cAnimationPlayer.h>

#include "cChest.h"
#include <stdlib.h>
#include <time.h>

#include "Engine/Audio/Audio.h"


#include <windows.h>

//namespace
namespace
{

	// player Object

	eae6320::GameObject::sCameraObject G_PlayerCamera;

	// Effect

	eae6320::Graphics::cEffect* E_Default = nullptr;

	// Meshes

	//Background Meshes
	eae6320::Graphics::cMesh* SM_Ground = nullptr;
	eae6320::Graphics::cMesh* SM_Mountains = nullptr;
	eae6320::Graphics::cMesh* SM_Trees = nullptr;
	eae6320::Graphics::cMesh* SM_House = nullptr;
	eae6320::Graphics::cMesh* SM_Clouds = nullptr;
	eae6320::Graphics::cMesh* SM_Clouds1 = nullptr;
	eae6320::Graphics::cMesh* SM_StopSign = nullptr;

	//sun Mesh
	eae6320::Graphics::cMesh* SM_Sun = nullptr;

	//Car Mesh
	eae6320::Graphics::cMesh* SM_CarRed = nullptr;
	eae6320::Graphics::cMesh* SM_CarBlue = nullptr;
	eae6320::Graphics::cMesh* SM_CarGreen = nullptr;
	eae6320::Graphics::cMesh* SM_CarYellow = nullptr;
	eae6320::Graphics::cMesh* SM_CarPurple = nullptr;

	//swing Meshes
	eae6320::Graphics::cMesh* SM_Swing = nullptr;

	//Chest Mesh
	eae6320::Graphics::cMesh* SM_ChestBase = nullptr;
	eae6320::Graphics::cMesh* SM_ChestLid = nullptr;
	eae6320::Graphics::cMesh* SM_Gold = nullptr;

	//Game Objects

	//Background Objects
	eae6320::GameObject::sMeshObject G_Ground;
	eae6320::GameObject::sMeshObject G_Mountains;
	eae6320::GameObject::sMeshObject G_Trees;
	eae6320::GameObject::sMeshObject G_House;
	eae6320::GameObject::sMeshObject G_Clouds;
	eae6320::GameObject::sMeshObject G_Clouds1;
	eae6320::GameObject::sMeshObject G_Clouds2;
	eae6320::GameObject::sMeshObject G_Clouds3;
	eae6320::GameObject::sMeshObject G_Clouds4;
	eae6320::GameObject::sMeshObject G_Clouds5;
	eae6320::GameObject::sMeshObject G_Clouds6;
	eae6320::GameObject::sMeshObject G_Clouds7;
	//sun
	eae6320::GameObject::sMeshObject G_Sun;

	//stopSign Object
	eae6320::GameObject::sMeshObject G_StopSign1;
	eae6320::GameObject::sMeshObject G_StopSign2;
	
	//swing
	eae6320::GameObject::sMeshObject G_Swing1;
	eae6320::GameObject::sMeshObject G_Swing2;
	eae6320::GameObject::sMeshObject G_Swing3;
	eae6320::GameObject::sMeshObject G_Swing4;

	//Car Object 7
	eae6320::GameObject::sMeshObject G_Car1;
	eae6320::GameObject::sMeshObject G_Car2;
	eae6320::GameObject::sMeshObject G_Car3;
	eae6320::GameObject::sMeshObject G_Car4;
	eae6320::GameObject::sMeshObject G_Car5;
	eae6320::GameObject::sMeshObject G_Car6;
	eae6320::GameObject::sMeshObject G_Car7;

	//6 Chest Object
	Chest G_Chest[6];
	int16_t chestCount = 6;

	//animations clips
	
	//Chest Lid Open Animation Clip
	eae6320::Animation::cAnimationClip* AS_ChestLidOpenClip = nullptr;
	//swing animation clip
	eae6320::Animation::cAnimationClip* AS_SwingClip = nullptr;

	//Car Animation Clip
	//car1 animation clip
	eae6320::Animation::cAnimationClip* AS_Car1Clip = nullptr;
	eae6320::Animation::cAnimationClip* AS_Car2Clip = nullptr;
	eae6320::Animation::cAnimationClip* AS_Car3Clip = nullptr;
	eae6320::Animation::cAnimationClip* AS_Car4Clip = nullptr;
	eae6320::Animation::cAnimationClip* AS_Car5Clip = nullptr;
	eae6320::Animation::cAnimationClip* AS_Car6Clip = nullptr;
	eae6320::Animation::cAnimationClip* AS_Car7Clip = nullptr;

	//animation players
	//swing animation players
	eae6320::Animation::cAnimationPlayer AP_SwingPlayer;
	//car1 animation player
	eae6320::Animation::cAnimationPlayer AP_Car1Player;
	eae6320::Animation::cAnimationPlayer AP_Car2Player;
	eae6320::Animation::cAnimationPlayer AP_Car3Player;
	eae6320::Animation::cAnimationPlayer AP_Car4Player;
	eae6320::Animation::cAnimationPlayer AP_Car5Player;
	eae6320::Animation::cAnimationPlayer AP_Car6Player;
	eae6320::Animation::cAnimationPlayer AP_Car7Player;

	//audio
	std::vector<Audio> G_Audio;

}

namespace map
{
	static std::vector<eae6320::Math::sVector> spawnLocations =
	{
		// --- Top Left (X < -600, Z > 600) ---
				{ -4200.f, 0.f, 4200.f },
				{ -3500.f, 0.f, 3000.f },
				{ -4800.f, 0.f, 2500.f },
				{ -2600.f, 0.f, 3800.f },
				{ -3900.f, 0.f, 1800.f },

				// --- Top Right (X > 600, Z > 600) ---
				{  4200.f, 0.f, 4200.f },
				{  3500.f, 0.f, 3000.f },
				{  4800.f, 0.f, 2500.f },
				{  2600.f, 0.f, 3800.f },
				{  3900.f, 0.f, 1800.f },

				// --- Bottom Left (X < -600, Z < -600) ---
				{ -4200.f, 0.f, -4200.f },
				{ -3500.f, 0.f, -3000.f },
				{ -4800.f, 0.f, -2500.f },
				{ -2600.f, 0.f, -3800.f },
				{ -3900.f, 0.f, -1800.f },

				// --- Bottom Right (X > 600, Z < -600) ---
				{  4200.f, 0.f, -4200.f },
				{  3500.f, 0.f, -3000.f },
				{  4800.f, 0.f, -2500.f },
				{  2600.f, 0.f, -3800.f },
				{  3900.f, 0.f, -1800.f }
	};
	eae6320::Math::sVector GetRandomSpawnLocation()
	{
		if (spawnLocations.empty())
			return { 0,0,0 };

		int index = rand() % spawnLocations.size();
		eae6320::Math::sVector pos = spawnLocations[index];

		spawnLocations.erase(spawnLocations.begin() + index);
		return pos;
	}
}

namespace actor
{
	static float getDistance(eae6320::GameObject::sGameObject player,eae6320::GameObject::sGameObject G2)
	{
		eae6320::Math::sVector playerPos = -player.m_RBSObject.position;

		// get distance only on XZ plane
		float deltaX = playerPos.x - G2.m_RBSObject.position.x;
		float deltaZ = playerPos.z - G2.m_RBSObject.position.z;
		return sqrtf((deltaX * deltaX) + (deltaZ * deltaZ));
	}
}


// Inherited Implementation
//=========================

// Run
//----

void eae6320::cFinalGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Escape))
	{
		// Exit the application
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	}
	//interaction code
	{
		//Open Chest
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::E))
		{
			for (int i = 0; i < chestCount; i++)
			{
				if (actor::getDistance(G_PlayerCamera, G_Chest[i].chestBase) < 500.f)
				{
					if (G_Chest[i].isOpen == false)
					{
						G_Chest[i].isOpen = true;
						if (!G_Chest[i].AP_chestLidOpenPlayer.IsPlaying())
						{
							G_Chest[i].AP_chestLidOpenPlayer.Play(AS_ChestLidOpenClip);
						}
					}
				}
			}
		}
	}
}

void eae6320::cFinalGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	// Render Player Camera
	G_PlayerCamera.RenderCamera(i_elapsedSecondCount_sinceLastSimulationUpdate);

	// Set Background Color
	eae6320::Graphics::SetBackgroundColor(0.084f, 0.644f, 0.728f, 1.0);

	// Render game objects
	G_Ground.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Mountains.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Trees.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_House.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Clouds.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Clouds1.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Clouds2.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Clouds3.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Clouds4.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Clouds5.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Clouds6.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Clouds7.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);

	//render sun
	G_Sun.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);

	//render stop sign
	G_StopSign1.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_StopSign2.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);

	//render swings
	G_Swing1.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Swing2.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Swing3.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Swing4.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);

	//render car
	G_Car1.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Car2.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Car3.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Car4.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Car5.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Car6.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G_Car7.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);


	for(int i=0;i< chestCount;i++)
	{
		G_Chest[i].chestBase.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
		G_Chest[i].chestLid.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
		if (G_Chest[i].HasGold())
		{
			if (G_Chest[i].isOpen == true)
			{
				G_Chest[i].gold.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
			}
		}
	}
}

void eae6320::cFinalGame::UpdateSimulationBasedOnInput()
{
	//Movement code
	{

		eae6320::Math::sVector camForward = G_PlayerCamera.m_RBSObject.orientation.CalculateForwardDirection();
		eae6320::Math::sVector camRight = eae6320::Math::Cross(G_PlayerCamera.m_RBSObject.orientation.CalculateForwardDirection(), eae6320::Math::sVector(0.0f, 1.0f, 0.0f));
		camRight.Normalize();

		float walkSpeed = 200.0f;
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Shift)) //sprint
		{
			walkSpeed = 400.0f;
		}
		else
		{
			walkSpeed = 200.0f;
		}

		//WASD for movement
		{
			if (UserInput::IsKeyPressed(UserInput::KeyCodes::W))
			{
				G_PlayerCamera.m_RBSObject.velocity.x = (camForward * walkSpeed).x;
				G_PlayerCamera.m_RBSObject.velocity.z = (camForward * -walkSpeed).z;
			}
			else if (UserInput::IsKeyPressed(UserInput::KeyCodes::S))
			{
				G_PlayerCamera.m_RBSObject.velocity.x = (camForward * -walkSpeed).x;
				G_PlayerCamera.m_RBSObject.velocity.z = (camForward * walkSpeed).z;
			}
			else if (UserInput::IsKeyPressed(UserInput::KeyCodes::A))
			{
				G_PlayerCamera.m_RBSObject.velocity.x = (camRight * walkSpeed).x;
				G_PlayerCamera.m_RBSObject.velocity.z = (camRight * -walkSpeed).z;
			}
			else if (UserInput::IsKeyPressed(UserInput::KeyCodes::D))
			{
				G_PlayerCamera.m_RBSObject.velocity.x = (camRight * -walkSpeed).x;
				G_PlayerCamera.m_RBSObject.velocity.z = (camRight * walkSpeed).z;
			}
			else
			{
				G_PlayerCamera.m_RBSObject.velocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
			}
		}

		//Arrow keys for rotation
		{
			if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
				G_PlayerCamera.m_RBSObject.angularSpeed = -1.5f;
			else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
				G_PlayerCamera.m_RBSObject.angularSpeed = +1.5f;
			else
				G_PlayerCamera.m_RBSObject.angularSpeed = 0.0f;
		}
	}
	
}


void eae6320::cFinalGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	//update player camera
	G_PlayerCamera.m_RBSObject.Update(i_elapsedSecondCount_sinceLastUpdate);

	//update animations
	{
		//Chest Lid Animation
		{
			for (int i = 0; i < chestCount; i++)
			{
				G_Chest[i].chestLid.m_RBSObject.Update(i_elapsedSecondCount_sinceLastUpdate);
				if (G_Chest[i].AP_chestLidOpenPlayer.IsPlaying())
				{
					G_Chest[i].AP_chestLidOpenPlayer.Update(i_elapsedSecondCount_sinceLastUpdate);
					//removing position cause i want to be able to move the chest around without affecting the animation
					Math::cQuaternion localRot = G_Chest[i].AP_chestLidOpenPlayer.GetCurrentRotation();
					Math::cQuaternion parentRot = G_Chest[i].chestBase.m_RBSObject.orientation;
					G_Chest[i].chestLid.m_RBSObject.orientation = parentRot * localRot;
				}

			}
			//exit game after opening chest with gold (for now)
			if (G_Chest[0].AP_chestLidOpenPlayer.IsPlaying() == false && G_Chest[0].isOpen == true)
			{
				const auto result = Exit(EXIT_SUCCESS);
				EAE6320_ASSERT(result);
			}
		}
		//swing Animation
		{
			if( AP_SwingPlayer.IsPlaying())
			{
				AP_SwingPlayer.Update(i_elapsedSecondCount_sinceLastUpdate);
				G_Swing1.m_RBSObject.orientation = AP_SwingPlayer.GetCurrentRotation();
				G_Swing2.m_RBSObject.orientation = AP_SwingPlayer.GetCurrentRotation();
				G_Swing3.m_RBSObject.orientation = AP_SwingPlayer.GetCurrentRotation();
				G_Swing4.m_RBSObject.orientation = AP_SwingPlayer.GetCurrentRotation();
			}
		}
		//car1 Animation
		{
			if (AP_Car1Player.IsPlaying())
			{
				AP_Car1Player.Update(i_elapsedSecondCount_sinceLastUpdate);
				G_Car1.m_RBSObject.position = AP_Car1Player.GetCurrentTranslation();
				G_Car1.m_RBSObject.orientation = AP_Car1Player.GetCurrentRotation();
			}
			if( AP_Car2Player.IsPlaying())
			{
				AP_Car2Player.Update(i_elapsedSecondCount_sinceLastUpdate);
				G_Car2.m_RBSObject.position = AP_Car2Player.GetCurrentTranslation();
				G_Car2.m_RBSObject.orientation = AP_Car2Player.GetCurrentRotation();
			}
			if (AP_Car3Player.IsPlaying())
			{
				AP_Car3Player.Update(i_elapsedSecondCount_sinceLastUpdate);
				G_Car3.m_RBSObject.position = AP_Car3Player.GetCurrentTranslation();
				G_Car3.m_RBSObject.orientation = AP_Car3Player.GetCurrentRotation();
			}
			if (AP_Car4Player.IsPlaying())
			{
				AP_Car4Player.Update(i_elapsedSecondCount_sinceLastUpdate);
				G_Car4.m_RBSObject.position = AP_Car4Player.GetCurrentTranslation();
				G_Car4.m_RBSObject.orientation = AP_Car4Player.GetCurrentRotation();
			}
			if (AP_Car5Player.IsPlaying())
			{
				AP_Car5Player.Update(i_elapsedSecondCount_sinceLastUpdate);
				G_Car5.m_RBSObject.position = AP_Car5Player.GetCurrentTranslation();
				G_Car5.m_RBSObject.orientation = AP_Car5Player.GetCurrentRotation();
			}
			if (AP_Car6Player.IsPlaying())
			{
				AP_Car6Player.Update(i_elapsedSecondCount_sinceLastUpdate);
				G_Car6.m_RBSObject.position = AP_Car6Player.GetCurrentTranslation();
				G_Car6.m_RBSObject.orientation = AP_Car6Player.GetCurrentRotation();
			}
			if (AP_Car7Player.IsPlaying())
			{
				AP_Car7Player.Update(i_elapsedSecondCount_sinceLastUpdate);
				G_Car7.m_RBSObject.position = AP_Car7Player.GetCurrentTranslation();
				G_Car7.m_RBSObject.orientation = AP_Car7Player.GetCurrentRotation();
			}
		}
	}
	//play audio spatial update
	{
		G_Audio[0].SpatialVolumeUpdate(G_PlayerCamera.m_RBSObject.position);
	}
}

// Initialize / Clean Up
//----------------------


eae6320::cResult eae6320::cFinalGame::Initialize()
{

	//initialize Camera
	{
		//Player Camera 0.0f, -165.0f, -480.f
		G_PlayerCamera.setSettings(60.f, 1.7777f, 0.1f, 18000.f);
		G_PlayerCamera.Initialize(eae6320::Math::sVector(-1000.0f, -165.0f, -4800.f), eae6320::Math::cQuaternion::FromComponents(1,0,0,0));
	}

	//Load default effect
	{
		eae6320::Graphics::cEffect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/PlainShader.shader", E_Default);
	}

	//Load Meshes
	{
		//Ground
		eae6320::Graphics::cMesh::Load("data/Meshes/Ground.Divith", SM_Ground);
		//Mountains
		eae6320::Graphics::cMesh::Load("data/Meshes/Mountains.Divith", SM_Mountains);
		//Trees
		eae6320::Graphics::cMesh::Load("data/Meshes/Trees.Divith", SM_Trees);
		//House
		eae6320::Graphics::cMesh::Load("data/Meshes/House.Divith", SM_House);
		//Clouds
		eae6320::Graphics::cMesh::Load("data/Meshes/Clouds.Divith", SM_Clouds);
		eae6320::Graphics::cMesh::Load("data/Meshes/Clouds1.Divith", SM_Clouds1);
		//Sun
		eae6320::Graphics::cMesh::Load("data/Meshes/Sun.Divith", SM_Sun);
		//Chest
		eae6320::Graphics::cMesh::Load("data/Meshes/ChestBase.Divith", SM_ChestBase);
		eae6320::Graphics::cMesh::Load("data/Meshes/ChestLid.Divith", SM_ChestLid);
		eae6320::Graphics::cMesh::Load("data/Meshes/ChestGold.Divith", SM_Gold);
		//Stop Sign
		eae6320::Graphics::cMesh::Load("data/Meshes/StopSign.Divith", SM_StopSign);

		//Swing
		eae6320::Graphics::cMesh::Load("data/Meshes/Swing.Divith", SM_Swing);

		//Car
		eae6320::Graphics::cMesh::Load("data/Meshes/Car1.Divith", SM_CarRed);
		eae6320::Graphics::cMesh::Load("data/Meshes/Car2.Divith", SM_CarBlue);
		eae6320::Graphics::cMesh::Load("data/Meshes/Car3.Divith", SM_CarGreen);
		eae6320::Graphics::cMesh::Load("data/Meshes/Car4.Divith", SM_CarYellow);
		eae6320::Graphics::cMesh::Load("data/Meshes/Car5.Divith", SM_CarPurple);
		eae6320::Graphics::cMesh::Load("data/Meshes/Car6.Divith", SM_CarRed);
		eae6320::Graphics::cMesh::Load("data/Meshes/Car7.Divith", SM_CarBlue);

	}

	//Load Animation Clips
	{
		//Chest Lid Open Animation Clip
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/ChestLid.Divith", AS_ChestLidOpenClip);
		AS_ChestLidOpenClip->looped = false;

		//swing animation clip
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Swing.Divith", AS_SwingClip);
		AS_SwingClip->looped = true;
		//initialize swing animation player
		AP_SwingPlayer.Play(AS_SwingClip);

		//car1 animation clip
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Car1.Divith", AS_Car1Clip);
		AS_Car1Clip->looped = true;
		AP_Car1Player.Play(AS_Car1Clip);
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Car2.Divith", AS_Car2Clip);
		AS_Car2Clip->looped = true;
		AP_Car2Player.Play(AS_Car2Clip);
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Car3.Divith", AS_Car3Clip);
		AS_Car3Clip->looped = true;
		AP_Car3Player.Play(AS_Car3Clip);
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Car4.Divith", AS_Car4Clip);
		AS_Car4Clip->looped = true;
		AP_Car4Player.Play(AS_Car4Clip);
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Car5.Divith", AS_Car5Clip);
		AS_Car5Clip->looped = true;
		AP_Car5Player.Play(AS_Car5Clip);
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Car6.Divith", AS_Car6Clip);
		AS_Car6Clip->looped = true;
		AP_Car6Player.Play(AS_Car6Clip);
		eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Car7.Divith", AS_Car7Clip);
		AS_Car7Clip->looped = true;
		AP_Car7Player.Play(AS_Car7Clip);


	}

	//Initialize Game Objects
	{
		//Ground
		G_Ground.Initialize(SM_Ground, E_Default);
		//Mountains
		G_Mountains.Initialize(SM_Mountains, E_Default);
		//Trees
		G_Trees.Initialize(SM_Trees, E_Default);
		//House
		G_House.Initialize(SM_House, E_Default);
		//Clouds
		G_Clouds.Initialize(SM_Clouds, E_Default);
		G_Clouds1.Initialize(SM_Clouds1, E_Default);
		G_Clouds2.Initialize(SM_Clouds, E_Default);
		G_Clouds2.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(0.7071f, 0.f, 0.7071f, 0.0f);
		G_Clouds3.Initialize(SM_Clouds1, E_Default);
		G_Clouds3.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(0.7071f, 0.f, 0.7071f, 0.0f);
		G_Clouds4.Initialize(SM_Clouds, E_Default);
		G_Clouds4.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(0.f, 0.f, 1.f, 0.0f);
		G_Clouds5.Initialize(SM_Clouds1, E_Default);
		G_Clouds5.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(0.f, 0.f, 1.f, 0.0f);
		G_Clouds6.Initialize(SM_Clouds, E_Default);
		G_Clouds6.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(0.7071f, 0.f, -0.7071f, 0.0f);
		G_Clouds7.Initialize(SM_Clouds1, E_Default);
		G_Clouds7.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(0.7071f, 0.f, -0.7071f, 0.0f);

		//all positions for clouds to 2000 height
		G_Clouds.m_RBSObject.position = eae6320::Math::sVector(0.f, 2000.f, 0.f);
		G_Clouds1.m_RBSObject.position = eae6320::Math::sVector(0.f, 2000.f, 0.f);
		G_Clouds2.m_RBSObject.position = eae6320::Math::sVector(0.f, 2000.f, 0.f);
		G_Clouds3.m_RBSObject.position = eae6320::Math::sVector(0.f, 2000.f, 0.f);
		G_Clouds4.m_RBSObject.position = eae6320::Math::sVector(0.f, 2000.f, 0.f);
		G_Clouds5.m_RBSObject.position = eae6320::Math::sVector(0.f, 2000.f, 0.f);
		G_Clouds6.m_RBSObject.position = eae6320::Math::sVector(0.f, 2000.f, 0.f);
		G_Clouds7.m_RBSObject.position = eae6320::Math::sVector(0.f, 2000.f, 0.f);

		//sun
		G_Sun.Initialize(SM_Sun, E_Default);
		G_Sun.m_RBSObject.position = eae6320::Math::sVector(0.f, 3000.f, 0.f);



		//Stop Sign
		G_StopSign1.Initialize(SM_StopSign, E_Default);
		G_StopSign2.Initialize(SM_StopSign, E_Default);
		G_StopSign2.m_RBSObject.position = -(G_StopSign2.m_RBSObject.position);
		//trun 180 degrees
		G_StopSign2.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(0.f, 0.f, 1.f, 0.f);

		//Swing Objects
		G_Swing1.Initialize(SM_Swing, E_Default);
		G_Swing2.Initialize(SM_Swing, E_Default);
		G_Swing2.m_RBSObject.position = eae6320::Math::sVector(1463.f, 210.f, 1709.f);
		G_Swing3.Initialize(SM_Swing, E_Default);
		G_Swing3.m_RBSObject.position = eae6320::Math::sVector(1981.f, 210.f, 2000.f);
		G_Swing4.Initialize(SM_Swing, E_Default);
		G_Swing4.m_RBSObject.position = eae6320::Math::sVector(1575.f, 210.f, 2500.f);

		//car Object
		G_Car1.Initialize(SM_CarRed, E_Default);
		G_Car2.Initialize(SM_CarBlue, E_Default);
		G_Car3.Initialize(SM_CarGreen, E_Default);
		G_Car4.Initialize(SM_CarYellow, E_Default);
		G_Car5.Initialize(SM_CarPurple, E_Default);
		G_Car6.Initialize(SM_CarRed, E_Default);
		G_Car7.Initialize(SM_CarBlue, E_Default);


		
		//Chest1
		{
			G_Chest[0].chestBase.Initialize(SM_ChestBase, E_Default);
			G_Chest[0].chestLid.Initialize(SM_ChestLid, E_Default);
			G_Chest[0].gold.Initialize(SM_Gold, E_Default);
		}
		//Chest2-6
		for(int i=1;i< chestCount;i++)
		{
			G_Chest[i].chestBase.Initialize(SM_ChestBase, E_Default);
			G_Chest[i].chestLid.Initialize(SM_ChestLid, E_Default);
		}
		// random location and rotation for chests (Have to change later)
		srand(static_cast <unsigned>(time(0)));
		for(int i=0;i< chestCount;i++)
		{
			float rotations[] = { 0.f,90.f,-90.f,180.f };
			int randIndex = rand() % 4;
			float randRotation = rotations[randIndex];
			eae6320::Math::sVector randPosition = map::GetRandomSpawnLocation();
			G_Chest[i].setChestPositionAndRotation(rotations[randIndex],randPosition,(i==0));
		}

	}

	//initialize Audio
	{
		Audio::CreateAudiosFromJson("data/audio/AudioClips.json", G_Audio);

		G_Audio[0].Play(true);
		G_Audio[0].SetVolume(0.f);
		G_Audio[0].SetSpatial(-G_Chest[0].chestBase.m_RBSObject.position, 1500.f, 1.f, 0.f);
	}

	return Results::Success;

}


eae6320::cResult eae6320::cFinalGame::CleanUp()
{
	eae6320::cResult result = Results::Success;
	//Clean up Effects
	{
		if (E_Default)
		{
			E_Default->DecrementReferenceCount();
			E_Default = nullptr;
		}
	}

	//Clean up Meshes
	{
		if (SM_Ground)
		{
			SM_Ground->DecrementReferenceCount();
			SM_Ground = nullptr;
		}
		if (SM_Mountains)
		{
			SM_Mountains->DecrementReferenceCount();
			SM_Mountains = nullptr;
		}
		if (SM_ChestBase)
		{
			SM_ChestBase->DecrementReferenceCount();
			SM_ChestBase = nullptr;
		}
		if (SM_ChestLid)
		{
			SM_ChestLid->DecrementReferenceCount();
			SM_ChestLid = nullptr;
		}
		if (SM_Gold)
		{
			SM_Gold->DecrementReferenceCount();
			SM_Gold = nullptr;
		}
		if (SM_Trees)
		{
			SM_Trees->DecrementReferenceCount();
			SM_Trees = nullptr;
		}
		if( SM_House)
		{
			SM_House->DecrementReferenceCount();
			SM_House = nullptr;
		}
		if (SM_Clouds)
		{
			SM_Clouds->DecrementReferenceCount();
			SM_Clouds = nullptr;
		}
		if( SM_Clouds1)
		{
			SM_Clouds1->DecrementReferenceCount();
			SM_Clouds1 = nullptr;
		}
		if( SM_Swing)
		{
			SM_Swing->DecrementReferenceCount();
			SM_Swing = nullptr;
		}
		if (SM_CarRed)
		{
			SM_CarRed->DecrementReferenceCount();
			SM_CarRed = nullptr;
		}
		if( SM_CarBlue)
		{
			SM_CarBlue->DecrementReferenceCount();
			SM_CarBlue = nullptr;
		}
		if( SM_CarGreen)
		{
			SM_CarGreen->DecrementReferenceCount();
			SM_CarGreen = nullptr;
		}
		if( SM_CarYellow)
		{
			SM_CarYellow->DecrementReferenceCount();
			SM_CarYellow = nullptr;
		}
		if( SM_CarPurple)
		{
			SM_CarPurple->DecrementReferenceCount();
			SM_CarPurple = nullptr;
		}
		if( SM_StopSign)
		{
			SM_StopSign->DecrementReferenceCount();
			SM_StopSign = nullptr;
		}
		if( SM_Sun)
			{
			SM_Sun->DecrementReferenceCount();
			SM_Sun = nullptr;
		}
	}

	//Clean up Game Objects
	{
		//Background Objects
		G_Ground.CleanUp();
		G_Mountains.CleanUp();
		G_Trees.CleanUp();
		G_House.CleanUp();
		G_Clouds.CleanUp();
		G_Clouds1.CleanUp();
		G_Clouds2.CleanUp();
		G_Clouds3.CleanUp();
		G_Clouds4.CleanUp();
		G_Clouds5.CleanUp();
		G_Clouds6.CleanUp();
		G_Clouds7.CleanUp();
		G_Sun.CleanUp();


		//Stop Sign Objects
		G_StopSign1.CleanUp();
		G_StopSign2.CleanUp();


		//Swing Objects
		G_Swing1.CleanUp();
		G_Swing2.CleanUp();
		G_Swing3.CleanUp();
		G_Swing4.CleanUp();

		//Car Object
		G_Car1.CleanUp();
		G_Car2.CleanUp();
		G_Car3.CleanUp();
		G_Car4.CleanUp();
		G_Car5.CleanUp();
		G_Car6.CleanUp();
		G_Car7.CleanUp();
		
		//Chest Objects
		for (int i = 0; i < chestCount; i++)
		{
			G_Chest[i].chestBase.CleanUp();
			G_Chest[i].chestLid.CleanUp();

			G_Chest[i].gold.CleanUp();

		}
	}

	//Clean up Animation Clips and Players
	{
		AS_ChestLidOpenClip->clearKeyFrames();
		AS_SwingClip->clearKeyFrames();
		AS_Car1Clip->clearKeyFrames();
		AS_Car2Clip->clearKeyFrames();
		AS_Car3Clip->clearKeyFrames();
		AS_Car4Clip->clearKeyFrames();
		AS_Car5Clip->clearKeyFrames();
		AS_Car6Clip->clearKeyFrames();
		AS_Car7Clip->clearKeyFrames();

	}

	//clean up Audio
	{
		G_Audio[0].CleanUp();
	}

	return result;
}
