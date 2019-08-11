#pragma once

#include <string>
#include <unordered_map>

#include <skse64/GameRTTI.h>
#include <skse64/GameForms.h>
#include <skse64/GameReferences.h>
#include <skse64/GameObjects.h>


// information explaining attributes of a damage dealing spell/bow/ability
struct CDamageEntry
{
	UInt8			mFormType = 0;
	float			mDamage = 0.0f;
	bool			mIsSpell = false;
	std::string		mKeyword;
	std::string		mProjectileName;
};

class CDamageTracker
{
public:
	bool RegisterAttack(SpellItem* spell);
	bool RegisterAttack(TESObjectWEAP* weapon);
	const CDamageEntry* LookupDamageEntry(Projectile* proj) const;

private:
	// lookup by FormType currently.. not sure if this will work though.
	std::unordered_map<UInt8, CDamageEntry>	mDamageMap;
};


MagicItem::EffectItem* GetDamageEffectForSpell(SpellItem* spell, const char** dmgKeywordStrOut = nullptr);
float GetSpellDamage(SpellItem* spell);

