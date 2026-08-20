#pragma once

#include "Engine/Math/sVector.h"
#include "Engine/GameObject/sGameObject.h"
#include "Engine/Animation/cAnimationPlayer.h"


struct Chest
{


public:
	Chest() = default;
	~Chest() = default;
	void setChestPositionAndRotation(float i_rotationDegrees, eae6320::Math::sVector i_position, bool i_hasGold);
	bool isOpen = false;

	bool HasGold()
	{
		return hasGold;
	}

	void setGold(bool i_hasGold)
	{
		hasGold = i_hasGold;
	}

	eae6320::GameObject::sMeshObject chestBase;
	eae6320::GameObject::sMeshObject chestLid;
	eae6320::GameObject::sMeshObject gold;
	eae6320::Animation::cAnimationPlayer AP_chestLidOpenPlayer;


private:
	void setChestPositionAndRotation(float i_rotationDegrees, eae6320::Math::sVector i_position);
	void setChestPositionAndRotationWithGold(float i_rotationDegrees, eae6320::Math::sVector i_position);


	bool hasGold = false;

};