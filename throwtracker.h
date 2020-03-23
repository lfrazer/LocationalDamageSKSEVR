#pragma once

#include <skse64/GameForms.h>
#include <skse64/GameReferences.h>
#include <skse64/GameObjects.h>

// WeaponThrowVR support (Thanks to Shizof for original code)
// for tracking weapons thrown (WeaponThrowVR mod compatibility)
class CThrowTracker
{
private:
	
	TESObjectWEAP* mRightFakeBow = nullptr;
	TESObjectWEAP* mLeftFakeBow = nullptr;

	const UInt32 kRightFakeBowFormId = 0x000DA1;
	const UInt32 kLeftFakeBowFormId = 0x000DA2;

	const UInt32 kRightBoundProjectileFormId = 0x00C00D;
	UInt32 mRightBoundProjectileFullFormId = 0x000000;
	const UInt32 kLeftBoundProjectileFormId = 0x00C00E;
	UInt32 mLeftBoundProjectileFullFormId = 0x000000;

	const UInt32 kRightProjectileDaggerFormId = 0x800;
	UInt32 mRightProjectileDaggerFullFormId = 0x000000;
	const UInt32 kLeftProjectileDaggerFormId = 0xDA4;
	UInt32 mLeftProjectileDaggerFullFormId = 0x000000;

	const UInt32 kRightProjectileSwordFormId = 0x3EB3B;
	UInt32 mRightProjectileSwordFullFormId = 0x000000;
	const UInt32 kLeftProjectileSwordFormId = 0x3EB3C;
	UInt32 mLeftProjectileSwordFullFormId = 0x000000;

	const UInt32 kRightProjectileAxeFormId = 0x3EB3D;
	UInt32 mRightProjectileAxeFullFormId = 0x000000;
	const UInt32 kLeftProjectileAxeFormId = 0x3EB3E;
	UInt32 mLeftProjectileAxeFullFormId = 0x000000;

	const UInt32 kRightProjectile2HAxeFormId = 0x3EB3F;
	UInt32 mRightProjectile2HAxeFullFormId = 0x000000;
	const UInt32 kLeftProjectile2HAxeFormId = 0x3EB40;
	UInt32 mLeftProjectile2HAxeFullFormId = 0x000000;

	const UInt32 kRightProjectile2HSwordFormId = 0x3EB41;
	UInt32 mRightProjectile2HSwordFullFormId = 0x000000;
	const UInt32 kLeftProjectile2HSwordFormId = 0x3EB42;
	UInt32 mLeftProjectile2HSwordFullFormId = 0x000000;

	const UInt32 kRightProjectileShieldFormId = 0x3EB43;
	UInt32 mRightProjectileShieldFullFormId = 0x000000;
	const UInt32 kLeftProjectileShieldFormId = 0x3EB44;
	UInt32 mLeftProjectileShieldFullFormId = 0x000000;

public:
	void Initialize();
	bool IsThrownLeftHandProjectile(Projectile* proj) const;
	bool IsThrownRightHandProjectile(Projectile* proj) const;

	TESObjectWEAP* GetLeftThrowWeapon() const { return mLeftFakeBow; }
	TESObjectWEAP* GetRightThrowWeapon() const { return mRightFakeBow; }
};