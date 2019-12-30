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
				mRightProjectileFullFormId = (modInfo->modIndex << 24) | (kRightProjectileFormId & 0x00FFFFFF);
				mLeftProjectileFullFormId = (modInfo->modIndex << 24) | (kLeftProjectileFormId & 0x00FFFFFF);

				mRightBoundProjectileFullFormId = (modInfo->modIndex << 24) | (kRightBoundProjectileFormId & 0x00FFFFFF);
				mLeftBoundProjectileFullFormId = (modInfo->modIndex << 24) | (kLeftBoundProjectileFormId & 0x00FFFFFF);

				const UInt32 rightWeaponBowFullFormId = (modInfo->modIndex << 24) | (kRightFakeBowFormId & 0x00FFFFFF);
				const UInt32 leftWeaponBowFullFormId = (modInfo->modIndex << 24) | (mLeftFakeBowFormId & 0x00FFFFFF);

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
		if (projectile->baseForm->formID == mLeftProjectileFullFormId)
		{
			return true;
		}
		else if (projectile->baseForm->formID == mLeftBoundProjectileFullFormId)
		{
			return true;
		}
	}
	
	return false;
}

bool CThrowTracker::IsThrownRightHandProjectile(Projectile* projectile) const 
{
	if (projectile->baseForm)
	{
		if (projectile->baseForm->formID == mRightProjectileFullFormId)
		{
			return true;
		}
		else if (projectile->baseForm->formID == mRightBoundProjectileFullFormId)
		{
			return true;
		}
	}

	return false;
}
