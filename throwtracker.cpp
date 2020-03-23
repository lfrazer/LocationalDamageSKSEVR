#include "throwtracker.h"
#include "common.h"
#include <skse64/PapyrusVM.h>
#include <skse64/GameData.h>

// WeaponThrowVR support (Thanks to Shizof for original code)

void CThrowTracker::Initialize()
{
	DataHandler* dataHandler = DataHandler::GetSingleton();

	if (dataHandler)
	{
		const ModInfo* modInfo = dataHandler->LookupModByName("WeaponThrowVR.esp");

		if (modInfo)
		{
			if (modInfo->modIndex > 0 && modInfo->modIndex != 0xFF) //If plugin is in the load order.
			{
				// load FULL projectile form IDs from mod index
				mRightProjectileDaggerFullFormId = (modInfo->modIndex << 24) | (kRightProjectileDaggerFormId & 0x00FFFFFF);
				mLeftProjectileDaggerFullFormId = (modInfo->modIndex << 24) | (kLeftProjectileDaggerFormId & 0x00FFFFFF);

				mRightProjectileAxeFullFormId = (modInfo->modIndex << 24) | (kRightProjectileAxeFormId & 0x00FFFFFF);
				mLeftProjectileAxeFullFormId = (modInfo->modIndex << 24) | (kLeftProjectileAxeFormId & 0x00FFFFFF);

				mRightProjectileSwordFullFormId = (modInfo->modIndex << 24) | (kRightProjectileSwordFormId & 0x00FFFFFF);
				mLeftProjectileSwordFullFormId = (modInfo->modIndex << 24) | (kLeftProjectileSwordFormId & 0x00FFFFFF);

				mRightProjectile2HSwordFullFormId = (modInfo->modIndex << 24) | (kRightProjectile2HSwordFormId & 0x00FFFFFF);
				mLeftProjectile2HSwordFullFormId = (modInfo->modIndex << 24) | (kLeftProjectile2HSwordFormId & 0x00FFFFFF);

				mRightProjectile2HAxeFullFormId = (modInfo->modIndex << 24) | (kRightProjectile2HAxeFormId & 0x00FFFFFF);
				mLeftProjectile2HAxeFullFormId = (modInfo->modIndex << 24) | (kLeftProjectile2HAxeFormId & 0x00FFFFFF);

				mRightProjectileShieldFullFormId = (modInfo->modIndex << 24) | (kRightProjectileShieldFormId & 0x00FFFFFF);
				mLeftProjectileShieldFullFormId = (modInfo->modIndex << 24) | (kLeftProjectileShieldFormId & 0x00FFFFFF);

				mRightBoundProjectileFullFormId = (modInfo->modIndex << 24) | (kRightBoundProjectileFormId & 0x00FFFFFF);
				mLeftBoundProjectileFullFormId = (modInfo->modIndex << 24) | (kLeftBoundProjectileFormId & 0x00FFFFFF);

				const UInt32 rightWeaponBowFullFormId = (modInfo->modIndex << 24) | (kRightFakeBowFormId & 0x00FFFFFF);
				const UInt32 leftWeaponBowFullFormId = (modInfo->modIndex << 24) | (kLeftFakeBowFormId & 0x00FFFFFF);

				TESForm* form = nullptr;
				if (rightWeaponBowFullFormId > 0)
				{
					form = nullptr;
					form = LookupFormByID(rightWeaponBowFullFormId);
					if (form)
					{
						if (form)
						{
							mRightFakeBow = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
						}
					}
				}

				if (leftWeaponBowFullFormId > 0)
				{
					form = nullptr;
					form = LookupFormByID(leftWeaponBowFullFormId);
					if (form)
					{
						if (form)
						{
							mLeftFakeBow = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
						}
					}
				}
				_MESSAGE("Found WeaponThrowVR. Registered formids. WeaponThrowVR support initialized successfully");
			}
		}
		else
		{
			_MESSAGE("WeaponThrowVR mod not found, could not initialize support");
		}
	}
}

bool CThrowTracker::IsThrownLeftHandProjectile(Projectile* projectile) const
{
	if (projectile->baseForm)
	{
		return projectile->baseForm->formID == mLeftProjectileDaggerFullFormId
			|| projectile->baseForm->formID == mLeftProjectileAxeFullFormId
			|| projectile->baseForm->formID == mLeftProjectileSwordFullFormId
			|| projectile->baseForm->formID == mLeftProjectile2HSwordFullFormId
			|| projectile->baseForm->formID == mLeftProjectile2HAxeFullFormId
			|| projectile->baseForm->formID == mLeftProjectileShieldFullFormId
			|| projectile->baseForm->formID == mLeftBoundProjectileFullFormId;
	}
	
	return false;
}

bool CThrowTracker::IsThrownRightHandProjectile(Projectile* projectile) const 
{
	if (projectile->baseForm)
	{
		return projectile->baseForm->formID == mRightProjectileDaggerFullFormId
			|| projectile->baseForm->formID == mRightProjectileAxeFullFormId
			|| projectile->baseForm->formID == mRightProjectileSwordFullFormId
			|| projectile->baseForm->formID == mRightProjectile2HSwordFullFormId
			|| projectile->baseForm->formID == mRightProjectile2HAxeFullFormId
			|| projectile->baseForm->formID == mRightProjectileShieldFullFormId
			|| projectile->baseForm->formID == mRightBoundProjectileFullFormId;
	}

	return false;
}
