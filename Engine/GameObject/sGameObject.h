#pragma once

#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Math/cMatrix_transformation.h>
#include<Engine/Math/Functions.h>


namespace eae6320
{
	namespace GameObject
	{
		struct sGameObject
		{
		public:

			eae6320::cResult setPosition(eae6320::Math::sVector i_pos);
			eae6320::cResult setRotation(eae6320::Math::cQuaternion i_rotation);
			//eae6320::Physics::sRigidBodyState &getRigidBody();
			eae6320::Math::cQuaternion getOrientation();
			
			eae6320::Physics::sRigidBodyState m_RBSObject;
		private:
			
		};
		 
		struct sMeshObject : sGameObject
		{
		public:
			sMeshObject() = default;
			~sMeshObject()= default;
			eae6320::cResult Initialize(eae6320::Graphics::cMesh*& i_Mesh, eae6320::Graphics::cEffect*& i_Effect);
			eae6320::cResult setMeshEffect(eae6320::Graphics::cMesh*& i_Mesh, eae6320::Graphics::cEffect*& i_Effect);
			eae6320::cResult RenderMesh(const float i_elapsedSecondCount_sinceLastSimulationUpdate);
			eae6320::cResult CleanUp();
		private:
			eae6320::Graphics::cMesh* m_Mesh = nullptr;
			eae6320::Graphics::cEffect* m_Effect = nullptr;
		};

		struct sCameraObject : sGameObject
		{
		public:
			void Initialize(eae6320::Math::sVector i_pos);
			void Initialize(eae6320::Math::sVector i_pos, eae6320::Math::cQuaternion i_Orentation);
			void setSettings(float i_fieldOfView, float i_asspectRatio, float i_nearClipping, float i_farClipping);
			eae6320::cResult RenderCamera(const float i_elapsedSecondCount_sinceLastSimulationUpdate);
		private:
			eae6320::Math::cMatrix_transformation m_settings;
		};
	}
}