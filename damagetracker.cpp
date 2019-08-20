#include "damagetracker.h"


bool CDamageTracker::RegisterAttack(SpellItem* spell)
{
	CDamageEntry dmgEntry;
	const char* dmgKeyword;

	MagicItem::EffectItem* effectItem = GetDamageEffectForSpell(spell, &dmgKeyword);
	if (effectItem)
	{
		dmgEntry.mFormType = effectItem->mgef->properties.projectile->formType;
		dmgEntry.mFormID = effectItem->mgef->properties.projectile->formID;
		dmgEntry.mDamage = effectItem->magnitude;
		dmgEntry.mIsSpell = true;
		dmgEntry.mKeyword = dmgKeyword;
		dmgEntry.mProjectileName = spell->dispObj.worldStatic->texSwap.GetModelName(); //effectItem->mgef->properties.projectile->fullName.GetName();
		
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
				if (damage > 0.0)
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

