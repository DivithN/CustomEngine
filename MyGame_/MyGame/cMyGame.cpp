
// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>

#include <Engine/Graphics/Graphics.h>

#include <Engine/Graphics/cMesh.h>

#include <Engine/Graphics/cEffect.h>

#include<Engine/Math/cMatrix_transformation.h>

#include<Engine/GameObject/sGameObject.h>

#include<Engine/Animation/cAnimationClip.h>
#include<Engine/Animation/cAnimationPlayer.h>

//namespace
namespace
{

	eae6320::Graphics::cMesh* s_Mesh1 = nullptr;
	eae6320::Graphics::cEffect* s_Effect1 = nullptr;

	eae6320::Graphics::cMesh* s_Mesh2=nullptr;
	eae6320::Graphics::cEffect* s_Effect2=nullptr;
	eae6320::Graphics::cEffect* s_Effect3 = nullptr;

	eae6320::Graphics::cMesh* s_Mesh3 = nullptr;
	eae6320::Graphics::cMesh* s_Mesh4 = nullptr;
	eae6320::Graphics::cMesh* s_Mesh5 = nullptr;

	eae6320::Graphics::cMesh* s_Mesh6 = nullptr;
	eae6320::Graphics::cMesh* s_Mesh7 = nullptr;
	eae6320::Graphics::cMesh* s_Mesh8 = nullptr;
	eae6320::Graphics::cMesh* s_Mesh9 = nullptr;

	eae6320::GameObject::sMeshObject G1;
	eae6320::GameObject::sMeshObject G2;
	eae6320::GameObject::sMeshObject G3;
	eae6320::GameObject::sMeshObject G4;
	eae6320::GameObject::sMeshObject G5;
	eae6320::GameObject::sMeshObject G6;
	eae6320::GameObject::sMeshObject G7;
	eae6320::GameObject::sMeshObject G8;
	eae6320::GameObject::sMeshObject G9;


	bool HideMesh = false;
	bool changeEffect = false;
}

//cameraGameObj

eae6320::GameObject::sCameraObject CameraObj;

eae6320::Math::sVector cameraPosition;
eae6320::Math::cQuaternion cameraOrientation;

eae6320::Animation::cAnimationClip* testClip;
eae6320::Animation::cAnimationPlayer player;

eae6320::Animation::cAnimationClip* SwingClip;
eae6320::Animation::cAnimationPlayer SwingPlayer;

eae6320::Animation::cAnimationClip* BallClip;
eae6320::Animation::cAnimationPlayer BallPlayer;


// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{

	CameraObj.RenderCamera(i_elapsedSecondCount_sinceLastSimulationUpdate);

	eae6320::Graphics::SetBackgroundColor(0.084f, 0.644f, 0.728f, 1.0);

	if (!HideMesh)
	{
		//eae6320::Graphics::SubmitMeshEffect(s_Mesh2, s_Effect2);
		
		//G1.setMeshEffect(s_Mesh2, s_Effect2);
	}
	else
	{
		//G1.setMeshEffect(s_Mesh1, s_Effect2);
	}
	if (changeEffect)
	{
		//eae6320::Graphics::SubmitMeshEffect(s_Mesh1, s_Effect2);
		//G2.setMeshEffect(s_Mesh1, s_Effect2);
	}
	else
	{
		//eae6320::Graphics::SubmitMeshEffect(s_Mesh1, s_Effect1);
		//G2.setMeshEffect(s_Mesh1, s_Effect1);

	}
	G2.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G1.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);

	G3.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G4.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G5.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G6.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G7.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G8.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	G9.RenderMesh(i_elapsedSecondCount_sinceLastSimulationUpdate);
	
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{
	// arrow keys to move object
	{
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
		{
			G1.m_RBSObject.velocity = eae6320::Math::sVector(-500.f, 0.0f, 0.0f);
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
		{
			G1.m_RBSObject.velocity = eae6320::Math::sVector(500.f, 0.0f, 0.0f);
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
		{
			G1.m_RBSObject.velocity = eae6320::Math::sVector(0.f, 500.f, 0.0f);

		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
		{
			G1.m_RBSObject.velocity = eae6320::Math::sVector(0.f, -500.f, 0.0f);
		}
		else
		{
			G1.m_RBSObject.velocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
		}
	}
	//spacial key to hide/show mesh
	{
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
		{
			HideMesh = true;
			player.Stop();
		}
		else
		{
			HideMesh = false;
		}
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Enter))
		{
			changeEffect = true;
			player.Play(testClip);
		}
		else
		{
			changeEffect = false;
		}
	}
	// wasd kye to move camera
		{
		if (UserInput::IsKeyPressed('A'))
		{
			CameraObj.m_RBSObject.velocity = eae6320::Math::sVector(500.0f, 0.f, 0.f);
		}
		else if (UserInput::IsKeyPressed('D'))
		{
			CameraObj.m_RBSObject.velocity = eae6320::Math::sVector(-500.0f, 0.f, 0.f);
		}
		else if (UserInput::IsKeyPressed('W'))
		{
			CameraObj.m_RBSObject.velocity = eae6320::Math::sVector(0.f, -500.0f, 0.f);
		}
		else if (UserInput::IsKeyPressed('S'))
		{
			CameraObj.m_RBSObject.velocity = eae6320::Math::sVector(0.f, 500.0f, 0.f);
		}
		else if (UserInput::IsKeyPressed('Q'))
		{
			CameraObj.m_RBSObject.velocity = eae6320::Math::sVector(0.f, 0.f, -500.f);
		}
		else if (UserInput::IsKeyPressed('E'))
		{
			CameraObj.m_RBSObject.velocity = eae6320::Math::sVector(0.f, 0.f, 500.f);
		}
		else
		{
			CameraObj.m_RBSObject.velocity = eae6320::Math::sVector(0.f, 0.f, 0.f);
		}
	}
}


void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	CameraObj.m_RBSObject.Update(i_elapsedSecondCount_sinceLastUpdate);
	G1.m_RBSObject.Update(i_elapsedSecondCount_sinceLastUpdate);
	if (player.IsPlaying())
	{
		player.Update(i_elapsedSecondCount_sinceLastUpdate);
		G1.m_RBSObject.position = player.GetCurrentTranslation();
		G1.m_RBSObject.orientation = player.GetCurrentRotation();
	}
	if( SwingPlayer.IsPlaying())
	{
		SwingPlayer.Update(i_elapsedSecondCount_sinceLastUpdate);
		G6.m_RBSObject.position = SwingPlayer.GetCurrentTranslation();
		G6.m_RBSObject.orientation = SwingPlayer.GetCurrentRotation();
	}
	if (BallPlayer.IsPlaying())
	{
		BallPlayer.Update(i_elapsedSecondCount_sinceLastUpdate);
		G8.m_RBSObject.position = BallPlayer.GetCurrentTranslation();
		G8.m_RBSObject.orientation = BallPlayer.GetCurrentRotation();
	}
}

// Initialize / Clean Up
//----------------------


eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Graphics::cEffect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/customShader.shader", s_Effect1);
	eae6320::Graphics::cMesh::Load("data/Meshes/Plane.Divith", s_Mesh1);
	eae6320::Graphics::cEffect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/customShader2.shader", s_Effect2);
	eae6320::Graphics::cMesh::Load("data/Meshes/House.Divith", s_Mesh5);
	eae6320::Graphics::cMesh::Load("data/Meshes/Roof.Divith", s_Mesh3);
	eae6320::Graphics::cMesh::Load("data/Meshes/Tree.Divith", s_Mesh4);
	eae6320::Graphics::cMesh::Load("data/Meshes/Swing.Divith", s_Mesh6);
	eae6320::Graphics::cMesh::Load("data/Meshes/Fence.Divith", s_Mesh7);
	eae6320::Graphics::cMesh::Load("data/Meshes/Ball.Divith", s_Mesh8);
	eae6320::Graphics::cMesh::Load("data/Meshes/BackGround.Divith", s_Mesh9);

	eae6320::Graphics::cMesh::Load("data/Meshes/cube.Divith", s_Mesh2);
	eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/cube.Divith", testClip);
	eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Swing.Divith", SwingClip);
	eae6320::Animation::cAnimationClip::LoadFromFile("data/Meshes/Ball.Divith", BallClip);

	testClip->looped = true;
	testClip->frameRate = 12.0f;
	SwingClip->looped = true;
	BallClip->looped = true;

	SwingPlayer.Play(SwingClip);
	BallPlayer.Play(BallClip);




	CameraObj.setSettings(30.f, 1.7777f, 0.1f, 10000.f);
	CameraObj.Initialize(eae6320::Math::sVector(0.0f, 300.0f, 2000.f));

	G1.Initialize(s_Mesh2, s_Effect2);


	
	G2.Initialize(s_Mesh1, s_Effect2);
	G3.Initialize(s_Mesh3, s_Effect2);
	G4.Initialize(s_Mesh4, s_Effect2);
	G5.Initialize(s_Mesh5, s_Effect2);
	G6.Initialize(s_Mesh6, s_Effect2);
	G7.Initialize(s_Mesh7, s_Effect2);
	G8.Initialize(s_Mesh8, s_Effect2);
	G9.Initialize(s_Mesh9, s_Effect2);

	return Results::Success;

}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	if (s_Effect1)
	{
		s_Effect1->DecrementReferenceCount();
		s_Effect1 = nullptr;
	}
	if (s_Mesh1)
	{
		s_Mesh1->DecrementReferenceCount();
		s_Mesh1 = nullptr;
	}
	if (s_Effect2)
	{
		s_Effect2->DecrementReferenceCount();
		s_Effect2 = nullptr;
	}
	if (s_Mesh2)
	{
		s_Mesh2->DecrementReferenceCount();
		s_Mesh2 = nullptr;
	} 
	if (s_Mesh3)
	{
		s_Mesh3->DecrementReferenceCount();
		s_Mesh3 = nullptr;
	}
	if (s_Mesh4)
	{
		s_Mesh4->DecrementReferenceCount();
		s_Mesh4 = nullptr;
	}
	if( s_Mesh5)
	{
		s_Mesh5->DecrementReferenceCount();
		s_Mesh5 = nullptr;
	}
	if( s_Mesh6)
	{
		s_Mesh6->DecrementReferenceCount();
		s_Mesh6 = nullptr;
	}
	if( s_Mesh7)
	{
		s_Mesh7->DecrementReferenceCount();
		s_Mesh7 = nullptr;
	}
	if( s_Mesh8)
	{
		s_Mesh8->DecrementReferenceCount();
		s_Mesh8 = nullptr;
	}
	if( s_Mesh9)
	{
		s_Mesh9->DecrementReferenceCount();
		s_Mesh9 = nullptr;
	}

	testClip->clearKeyFrames();
	BallClip->clearKeyFrames();
	SwingClip->clearKeyFrames();


	G1.CleanUp();
	G2.CleanUp();
	G3.CleanUp();
	G4.CleanUp();
	G5.CleanUp();
	G6.CleanUp();
	G7.CleanUp();
	G8.CleanUp();
	G9.CleanUp();
	return Results::Success;
}
