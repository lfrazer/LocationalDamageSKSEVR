#pragma once

#include <skse64/GameForms.h>
#include <skse64/GameReferences.h>
#include <skse64/GameObjects.h>

// WeaponThrowVR support (Thanks to Shizof for original code)
// for tracking weapons thrown (WeaponThrowVR mod compatibility)
class CThrowTracker
{
private:

	UInt32 mRightProjectileFullFormId = 0x000000;
	UInt32 mLeftProjectileFullFormId = 0x000000;

	UInt32 mRightBoundProjectileFullFormId = 0x000000;
	UInt32 mLeftBoundProjectileFullFormId = 0x000000;

	TESObjectWEAP* mRightFakeBow = nullptr;
	TESObjectWEAP* mLeftFakeBow = nullptr;

	const UInt32 kRightFakeBowFormId = 0x000DA1;
	const UInt32 mLeftFakeBowFormId = 0x000DA2;

	const UInt32 kRightProjectileFormId = 0x000800;
	const UInt32 kLeftProjectileFormId = 0x000DA4;

	const UInt32 kRightBoundProjectileFormId = 0x00C00D;
	const UInt32 kLeftBoundProjectileFormId = 0x00C00E;

public:
	void Initialize();
	bool IsThrownLeftHandProjectile(Projectile* proj) const;
	bool IsThrownRightHandProjectile(Projectile* proj) const;

	TESObjectWEAP* GetLeftThrowWeapon() const { return mLeftFakeBow; }
	TESObjectWEAP* GetRightThrowWeapon() const { return mRightFakeBow; }
};