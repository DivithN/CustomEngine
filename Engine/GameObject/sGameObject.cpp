#include "sGameObject.h"

eae6320::cResult eae6320::GameObject::sMeshObject::RenderMesh(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	eae6320::Math::cMatrix_transformation transform = m_RBSObject.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	auto result = Results::Success;
	if (!m_Mesh)
	{
		EAE6320_ASSERT(m_Mesh);
		result = Results::InvalidFile;
	}
	if (!m_Effect)
	{
		EAE6320_ASSERT(m_Effect);
		result = Results::InvalidFile;
	}
	if (!(result = eae6320::Graphics::SubmitMeshEffect(m_Mesh, m_Effect, transform)))
	{
		result = Results::Failure;
	}

	return result;
}

eae6320::cResult eae6320::GameObject::sMeshObject::CleanUp()
{
	if (m_Mesh)
	{
		m_Mesh->DecrementReferenceCount();
		m_Mesh = nullptr;
	}
	if (m_Effect)
	{
		m_Effect->DecrementReferenceCount();
		m_Effect = nullptr;
	}
	return eae6320::Results::Success;
}
eae6320::cResult eae6320::GameObject::sMeshObject::Initialize(eae6320::Graphics::cMesh*& i_Mesh, eae6320::Graphics::cEffect*& i_Effect)
{
	auto result = Results::Success;
	m_Mesh = i_Mesh;
	if (m_Mesh)
		i_Mesh->IncrementReferenceCount();
	m_Effect = i_Effect;
	if (m_Effect)
		i_Effect->IncrementReferenceCount();

	if (m_Mesh)
	{
		setRotation(i_Mesh->m_transform.rotation);
		setPosition(i_Mesh->m_transform.translation);
	}
	else
	{
		setRotation(eae6320::Math::cQuaternion::FromComponents(1.f, 0.f, 0.f, 0.f));
		setPosition(eae6320::Math::sVector(0.0f, 0.0f, 0.0f));
	}
	return result;
}

eae6320::cResult eae6320::GameObject::sMeshObject::setMeshEffect(eae6320::Graphics::cMesh*& i_Mesh, eae6320::Graphics::cEffect*& i_Effect)
{
	auto result = Results::Success;
	if(i_Mesh)
		m_Mesh = i_Mesh;
	if (i_Effect)
		m_Effect = i_Effect;
	return result;
}

eae6320::cResult eae6320::GameObject::sGameObject::setPosition(eae6320::Math::sVector i_pos)
{
	m_RBSObject.position = i_pos;
	return eae6320::Results::Success;
}

eae6320::cResult eae6320::GameObject::sGameObject::setRotation(eae6320::Math::cQuaternion i_rotation)
{
	m_RBSObject.orientation = i_rotation;
	return eae6320::Results::Success;
}


eae6320::Math::cQuaternion eae6320::GameObject::sGameObject::getOrientation()
{
	return m_RBSObject.orientation;
}

void eae6320::GameObject::sCameraObject::Initialize(eae6320::Math::sVector i_pos)
{
	eae6320::Math::cQuaternion tempOrentation = eae6320::Math::cQuaternion::FromComponents(1.f, 0.f, 0.f, 0.f);
	Initialize(i_pos, tempOrentation);
}

void eae6320::GameObject::sCameraObject::Initialize(eae6320::Math::sVector i_pos, eae6320::Math::cQuaternion i_Orentation)
{	
	//m_transform = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(i_Orentation, i_pos);

	setPosition(i_pos);
	setRotation(i_Orentation);
}

void eae6320::GameObject::sCameraObject::setSettings(float i_fieldOfView,float i_asspectRatio, float i_nearClipping, float i_farClipping)
{
	m_settings= eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(eae6320::Math::ConvertDegreesToRadians(i_fieldOfView), i_asspectRatio, i_nearClipping, i_farClipping);
}

eae6320::cResult eae6320::GameObject::sCameraObject::RenderCamera(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	/*eae6320::Math::cQuaternion ori = m_RBSObject.orientation;
	eae6320::Math::sVector pos= m_RBSObject.position;
	m_transform = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(ori, pos);*/
	
	
	//eae6320::Math::cMatrix_transformation transform = m_RBSObject.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	eae6320::Math::cMatrix_transformation transform = m_RBSObject.PredictFutureTransformForCamera(i_elapsedSecondCount_sinceLastSimulationUpdate);

	auto result = Results::Success;
	if (!(result = eae6320::Graphics::SubmitCamera(transform,m_settings)))
	{
		result = Results::Failure;
	}
	return eae6320::cResult();
}
