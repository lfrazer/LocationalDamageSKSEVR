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

struct CSpellBonusDmgPerk
{
	UInt32 mPerkFormID = 0;
	float mDmgMult = 1.0f;
};

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
	void Init();
	bool IsFromSpellsiphon(TESForm* form) const;
	float GetSpellDamageBonus(SpellItem* spell, MagicItem::EffectItem* effectItem, Actor* caster_actor, const char* mgefKeyword) const;

	bool RegisterAttack(SpellItem* spell, Actor* actor);
	bool RegisterAttack(TESObjectWEAP* weapon);
	CDamageEntry* LookupDamageEntry(Projectile* proj);



private:
	// lookup by FormType currently.. not sure if this will work though.
	std::unordered_map<UInt8, CDamageEntry>	mDamageMap;
	UInt32									mSpellsiphonModIndex = 0;
	
	static const int						kNumBonusDmgPerks = 2;
	static const unsigned int				kSpellsiphonElementalOverloadPerkID = 0x0027448A;

	CSpellBonusDmgPerk						mFireDmgPerks[kNumBonusDmgPerks];
	CSpellBonusDmgPerk						mFrostDmgPerks[kNumBonusDmgPerks];
	CSpellBonusDmgPerk						mShockDmgPerks[kNumBonusDmgPerks];
};


MagicItem::EffectItem* GetDamageEffectForSpell(SpellItem* spell, const char** dmgKeywordStrOut = nullptr);
float GetSpellDamage(SpellItem* spell);

