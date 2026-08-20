#include "cChest.h"

void Chest::setChestPositionAndRotation(float i_rotationDegrees, eae6320::Math::sVector i_position, bool i_hasGold)
{
	if (i_hasGold)
	{
		setChestPositionAndRotationWithGold(i_rotationDegrees, i_position);
		hasGold = true;
	}
	else
	{
		setChestPositionAndRotation(i_rotationDegrees, i_position);
		hasGold = false;
	}
}

void Chest::setChestPositionAndRotation(float i_rotationDegrees, eae6320::Math::sVector i_position)
{
	if (i_rotationDegrees == 90)
	{
		chestBase.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(cosf(eae6320::Math::ConvertDegreesToRadians(90.0f) / 2.0f), 0.0f, sinf(eae6320::Math::ConvertDegreesToRadians(90.0f) / 2.0f), 0.0f);
		chestLid.m_RBSObject.orientation = chestBase.m_RBSObject.orientation;
		float tempx = chestLid.m_RBSObject.position.x;
		float tempz = chestLid.m_RBSObject.position.z;
		chestLid.m_RBSObject.position.x = tempz;
		chestLid.m_RBSObject.position.z = -tempx;
	}
	else if (i_rotationDegrees == -90)
	{
		chestBase.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(cosf(eae6320::Math::ConvertDegreesToRadians(-90.0f) / 2.0f), 0.0f, sinf(eae6320::Math::ConvertDegreesToRadians(-90.0f) / 2.0f), 0.0f);
		chestLid.m_RBSObject.orientation = chestBase.m_RBSObject.orientation;
		float tempx = chestLid.m_RBSObject.position.x;
		float tempz = chestLid.m_RBSObject.position.z;
		chestLid.m_RBSObject.position.x = -tempz;
		chestLid.m_RBSObject.position.z = tempx;
	}

	else if (i_rotationDegrees == 180)
	{
		chestBase.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(cosf(eae6320::Math::ConvertDegreesToRadians(180.0f) / 2.0f), 0.0f, sinf(eae6320::Math::ConvertDegreesToRadians(180.0f) / 2.0f), 0.0f);
		chestLid.m_RBSObject.orientation = chestBase.m_RBSObject.orientation;
		float tempx = chestLid.m_RBSObject.position.x;
		float tempz = chestLid.m_RBSObject.position.z;
		chestLid.m_RBSObject.position.x = -tempx;
		chestLid.m_RBSObject.position.z = -tempz;
	}
	//set chest position
	{
		chestBase.m_RBSObject.position = i_position;
		chestLid.m_RBSObject.position = chestLid.m_RBSObject.position + chestBase.m_RBSObject.position;
	}
}

void Chest::setChestPositionAndRotationWithGold(float i_rotationDegrees, eae6320::Math::sVector i_position)
{
	setChestPositionAndRotation(i_rotationDegrees, i_position);
	//with gold
	{
		gold.m_RBSObject.position = i_position;
		gold.m_RBSObject.orientation = eae6320::Math::cQuaternion::FromComponents(cosf(eae6320::Math::ConvertDegreesToRadians(i_rotationDegrees) / 2.0f), 0.0f, sinf(eae6320::Math::ConvertDegreesToRadians(i_rotationDegrees) / 2.0f), 0.0f);
	}
}
