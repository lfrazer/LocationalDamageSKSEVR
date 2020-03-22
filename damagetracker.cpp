#include "damagetracker.h"
#include "common.h"
#include <skse64/PapyrusVM.h>
#include <skse64/GameData.h>

namespace papyrusActor
{
	typedef float (*_GetActorValue)(VMClassRegistry* VMinternal, UInt32 stackId, Actor * thisActor, BSFixedString const &dmgValueName);
	RelocAddr<_GetActorValue> GetActorValue(GETACTORVALUE_FN);
}

void CDamageTracker::Init()
{
	const ModInfo* DGModInfo = DataHandler::GetSingleton()->LookupModByName("Dawnguard.esm");
	if (DGModInfo)
	{
		mDawnguardModIndex = DGModInfo->GetPartialIndex();
	}

	const ModInfo* dragonbornModInfo = DataHandler::GetSingleton()->LookupModByName("Dragonborn.esm");
	if (dragonbornModInfo)
	{
		mDragonbornModIndex = dragonbornModInfo->GetPartialIndex();
	}

	const char* spellsiphonESP = "Spellsiphon.esp";
	const ModInfo* ssModInfo = DataHandler::GetSingleton()->LookupModByName(spellsiphonESP);
	
	if (ssModInfo)
	{
		mSpellsiphonModIndex = ssModInfo->GetPartialIndex();
		_MESSAGE("Found spellsiphon mod! Idx = %d", mSpellsiphonModIndex);
	}

	// form ID then damage mult - descending order so the code can check for the higher damage perk first
	mFireDmgPerks[0] = { 0x0010FCF8, 1.5f };
	mFireDmgPerks[1] = { 0x000581E7, 1.25f };

	mFrostDmgPerks[0] = {0x0010FCF9, 1.5f };
	mFrostDmgPerks[1] = {0x000581EA, 1.25f };

	mShockDmgPerks[0] = {0x0010FCFA, 1.5f};
	mShockDmgPerks[1] = {0x00058200, 1.25f };
}

bool CDamageTracker::IsFromSpellsiphon(TESForm* form) const
{
	if (mSpellsiphonModIndex)
	{
		return (form->formID >> 24) == mSpellsiphonModIndex;
	}

	return false;
}

float CDamageTracker::GetSpellDamageBonus(SpellItem* spell, MagicItem::EffectItem* effectItem, Actor* caster_actor, const char* mgefKeyword) const
{
	float dmg = effectItem->magnitude;


	// apply spellsiphon perk damage modifier (based on magicka regen rate)
	if (IsFromSpellsiphon(spell))
	{
		const float magickaRateMult = papyrusActor::GetActorValue((*g_skyrimVM)->GetClassRegistry(), 0, caster_actor, "MagickaRateMult");
		//_MESSAGE("MagickaRateMult -  float = %f ",magickaRateMult);
		if (magickaRateMult > 0.0f) // rare case with mods where this can become negative
		{
			dmg *= (magickaRateMult * 0.01f);
		}

		// look for "elemental overload" perk for weaving circle
		const UInt32 elementalOverloadPerkID = (mSpellsiphonModIndex << 24) | kSpellsiphonElementalOverloadPerkID;
		BGSPerk* perk = DYNAMIC_CAST(LookupFormByID(elementalOverloadPerkID), TESForm, BGSPerk);

		if (perk && CALL_MEMBER_FN(caster_actor, HasPerk)(perk))
		{
			dmg *= 2.0f;
		}
	}
	
	auto GetPerkBonusDmg = [caster_actor](const CSpellBonusDmgPerk& perkBonusDmg) -> float
	{
		BGSPerk* perk = DYNAMIC_CAST(LookupFormByID(perkBonusDmg.mPerkFormID), TESForm, BGSPerk);
		if (perk)
		{
			if (CALL_MEMBER_FN(caster_actor, HasPerk)(perk))
			{
				//_MESSAGE("Found perk 0x%x - spell dmg mult = %f", perkBonusDmg.mPerkFormID, perkBonusDmg.mDmgMult);
				return perkBonusDmg.mDmgMult;
			}
		}

		return 1.0f;
	};

	// apply all perk modifiers for damage
	if (strstr(mgefKeyword, "Fire"))
	{
		for (int i = 0; i < kNumBonusDmgPerks; ++i)
		{
			float spellDmgMult = GetPerkBonusDmg(mFireDmgPerks[i]);
			if (spellDmgMult > 1.0f)
			{
				dmg *= spellDmgMult;
				break;
			}
		}
	}
	else if (strstr(mgefKeyword, "Frost"))
	{
		for (int i = 0; i < kNumBonusDmgPerks; ++i)
		{
			float spellDmgMult = GetPerkBonusDmg(mFrostDmgPerks[i]);
			if (spellDmgMult > 1.0f)
			{
				dmg *= spellDmgMult;
				break;
			}
		}
	}
	else if (strstr(mgefKeyword, "Shock"))
	{
		for (int i = 0; i < kNumBonusDmgPerks; ++i)
		{
			float spellDmgMult = GetPerkBonusDmg(mShockDmgPerks[i]);
			if (spellDmgMult > 1.0f)
			{
				dmg *= spellDmgMult;
				break;
			}
		}
	}

	return dmg;
}

bool CDamageTracker::RegisterAttack(SpellItem* spell, Actor* actor)
{
	CDamageEntry dmgEntry;
	const char* dmgKeyword;

	MagicItem::EffectItem* effectItem = GetDamageEffectForSpell(spell, &dmgKeyword);
	// additional safety check (some special case weapons like Dawnbreaker seem to have spell effects attached to them..?)
	if (effectItem && effectItem->mgef)
	{
		if (effectItem->mgef->properties.projectile)
		{
			dmgEntry.mFormType = effectItem->mgef->properties.projectile->formType;
			dmgEntry.mFormID = effectItem->mgef->properties.projectile->formID;
		}
		else // in this case there is no projectile attached to the MGEF, but it can still do damage (for example lightning bolts and all spellsiphon spell MGEFs)
		{
			dmgEntry.mFormType = kFormType_Projectile; // some default values that should still allow for lookup - since atm we just assume this formType if its not an arrow
			dmgEntry.mFormID = effectItem->mgef->formID; // save the MGEF's formID instead of projectiles in this case
		}

		dmgEntry.mDamage = GetSpellDamageBonus(spell, effectItem, actor, dmgKeyword);
		dmgEntry.mIsSpell = true;
		dmgEntry.mKeyword = dmgKeyword;

#ifdef _DEBUG // TODO: This seems to still crash for certain staves.. disable only for staves?
		if (spell->dispObj.worldStatic != nullptr)
		{
			dmgEntry.mProjectileName = spell->dispObj.worldStatic->texSwap.GetModelName(); //effectItem->mgef->properties.projectile->fullName.GetName();
		}
#endif

		this->mDamageMap[dmgEntry.mFormType] = dmgEntry;

		_DEBUGMSG("Registering spell attack FormType: %d FormID: 0x%X Damage: %f Keyword: %s ProjectileName: %s", dmgEntry.mFormType, dmgEntry.mFormID, dmgEntry.mDamage, dmgEntry.mKeyword.c_str(), dmgEntry.mProjectileName.c_str());

		return true;
	}

	return false;
}

bool CDamageTracker::RegisterAttack(TESObjectWEAP* weapon)
{
	CDamageEntry dmgEntry;
	dmgEntry.mFormType = kFormType_Arrow; // assume we are shooting arrow in the non-spell case.  should be true unless crossbow bolts are another type?
	dmgEntry.mDamage = weapon->damage.GetAttackDamage();
	dmgEntry.mWeaponType = weapon->type();

	_DEBUGMSG("Registering arrow attack FormType: %d Damage: %f", dmgEntry.mFormType, dmgEntry.mDamage);

	mDamageMap[dmgEntry.mFormType] = dmgEntry;

	return true;
}

CDamageEntry* CDamageTracker::LookupDamageEntry(Projectile* proj)
{
	/*
	Useful info about Projectile class from po3:
	Projectile unk150 is BGSExplosion
	also 158 is MagicItem, not SpellItem
	there's a NiTransform at unk0A8, probably local
	*/

	UInt8 formTypeLookup = 0;

	if (proj->formType == kFormType_Arrow) // arrow type seems are always just arrows
	{
		formTypeLookup = kFormType_Arrow;
	}
	else // situation is more complex for spells
	{
		// NOTE: This is a hack fix for difference between BSGProjectile (from Spell/EffectItem data structure) and Projectile (the actual physics projectile structure)
		// BSGProjectile type (which is used for registration) always has type == kFormType_Projectile, while it seems "Projectile"  type can have a variety of form types - i.e. Missile, FlameProjectile etc
		// Ideally we could have a lookup table for what BSGProjectile/SpellItem will produce what kind of Projectile formtype but we do not have that data yet.  
		formTypeLookup = kFormType_Projectile;
	}

	auto it = mDamageMap.find(formTypeLookup);
	if (it != mDamageMap.end())
	{
		CDamageEntry* dmgEntry = &it->second;
		_DEBUGMSG("LookupDamageEntry() ProjectileFormType: %d FormType: %d FormID: 0x%X Damage: %f Keyword: %s ProjectileName: %s", proj->formType, dmgEntry->mFormType, dmgEntry->mFormID, dmgEntry->mDamage, dmgEntry->mKeyword.c_str(), dmgEntry->mProjectileName.c_str());

		return dmgEntry;
	}
	else
	{
		_DEBUGMSG("LookupDamageEntry(): Failed with FormTypeLookup: %d FormType: %d formID: 0x%X", formTypeLookup, proj->formType, proj->formID);
	}

	return nullptr;
}


// spell damage helper functions:

MagicItem::EffectItem* GetDamageEffectForSpell(SpellItem* spell, const char** dmgKeywordStrOut)
{
	// get damage from magnitude

	auto FindDamageEffect = [&dmgKeywordStrOut](MagicItem::EffectItem* pEI)->float
	{
		float dmgOut = 0.0f;
		const int numKeywords = pEI->mgef->keywordForm.numKeywords;
		const float maxDmg = 150.0f;  // cutoff maximum damage (sanity check), if the damage is higher than this it might be an error due to picking up on the magic effect for Disintegrate perk -> PerkDisintegrateConcAimed
		
		if (pEI->magnitude <= maxDmg) // don't check any of the keywords if the sanity check fails
		{
			for (int i = 0; i < numKeywords; ++i)
			{
				if (strstr(pEI->mgef->keywordForm.keywords[i]->keyword.c_str(), "Damage") != nullptr)
				{
					dmgOut = pEI->magnitude;

					if (dmgKeywordStrOut)
					{
						*dmgKeywordStrOut = pEI->mgef->keywordForm.keywords[i]->keyword.c_str();
					}

					return dmgOut;
				}
			}
		}
		return dmgOut;
	};

	float damage = 0.0f;

	for (UInt32 i = 0; i < spell->effectItemList.count; i++)
	{
		MagicItem::EffectItem* pEI = NULL;
		spell->effectItemList.GetNthItem(i, pEI);
		if (pEI)
		{
			if (pEI->mgef)
			{
				damage = FindDamageEffect(pEI);
				if (damage > 0.0f)
				{
					return pEI;
				}
			}
		}
	}

	return nullptr;
}


float GetSpellDamage(SpellItem* spell)
{
	const auto* dmgEffect = GetDamageEffectForSpell(spell);
	if (dmgEffect)
	{
		return dmgEffect->magnitude;
	}

	return 0.0f;
}




// SKSE reference:
/*
struct SKSEActionEvent
{
	enum {
		kType_WeaponSwing = 0,
		kType_SpellCast = 1,
		kType_SpellFire = 2,
		kType_VoiceCast = 3,
		kType_VoiceFire = 4,
		kType_BowDraw = 5,
		kType_BowRelease = 6,
		kType_BeginDraw = 7,
		kType_EndDraw = 8,
		kType_BeginSheathe = 9,
		kType_EndSheathe = 10
	};
	enum {
		kSlot_Left = 0,
		kSlot_Right = 1,
		kSlot_Voice = 2
	};
	UInt32 type;
	Actor * actor;
	TESForm	* sourceForm;
	UInt32	slot;

	SKSEActionEvent(UInt32 a_type, Actor * a_actor, TESForm * a_source, UInt32 a_slot) :
	type(a_type), actor(a_actor), sourceForm(a_source), slot(a_slot) {}
};
*/

/*
//Shizof's mod reference

	class MYSKSEActionEvent : public BSTEventSink <SKSEActionEvent>
	{
	public:
		virtual	EventResult ReceiveEvent(SKSEActionEvent * evn, EventDispatcher<SKSEActionEvent> * dispatcher);
	};

	extern EventDispatcher<SKSEActionEvent>* g_skseActionEventDispatcher;
	extern MYSKSEActionEvent mySKSEActionEvent;
*/

