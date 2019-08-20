#pragma once

#include <string>
#include <unordered_map>

#include <skse64/GameRTTI.h>
#include <skse64/GameForms.h>
#include <skse64/GameReferences.h>
#include <skse64/GameObjects.h>

#define EXTRA_DEBUG_LOG 0

#if EXTRA_DEBUG_LOG || defined(_DEBUG)
#define _DEBUGMSG _MESSAGE
#else
#define _DEBUGMSG(...) 
#endif

// information explaining attributes of a damage dealing spell/bow/ability
struct CDamageEntry
{
	UInt8			mFormType = 0;
	UInt32			mFormID = 0;
	float			mDamage = 0.0f;
	bool			mIsSpell = false;
	UInt8			mWeaponType = 0;
	std::string		mKeyword;
	std::string		mProjectileName;
};

class CDamageTracker
{
public:
	bool RegisterAttack(SpellItem* spell);
	bool RegisterAttack(TESObjectWEAP* weapon);
	CDamageEntry* LookupDamageEntry(Projectile* proj);

private:
	// lookup by FormType currently.. not sure if this will work though.
	std::unordered_map<UInt8, CDamageEntry>	mDamageMap;
};


MagicItem::EffectItem* GetDamageEffectForSpell(SpellItem* spell, const char** dmgKeywordStrOut = nullptr);
float GetSpellDamage(SpellItem* spell);

