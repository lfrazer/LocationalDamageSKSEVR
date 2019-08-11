#include "damagetracker.h"


bool CDamageTracker::RegisterAttack(SpellItem* spell)
{
	CDamageEntry dmgEntry;
	const char* dmgKeyword;

	MagicItem::EffectItem* effectItem = GetDamageEffectForSpell(spell, &dmgKeyword);
	if (effectItem)
	{
		dmgEntry.mFormType = effectItem->mgef->properties.projectile->formType;
		dmgEntry.mDamage = effectItem->magnitude;
		dmgEntry.mIsSpell = true;
		dmgEntry.mKeyword = dmgKeyword;
		dmgEntry.mProjectileName = effectItem->mgef->properties.projectile->fullName.GetName();
		
		this->mDamageMap[dmgEntry.mFormType] = dmgEntry;

		return true;
	}

	return false;
}

bool CDamageTracker::RegisterAttack(TESObjectWEAP* weapon)
{
	CDamageEntry dmgEntry;
	dmgEntry.mFormType = kFormType_Arrow; // assume we are shooting arrow in the non-spell case.  should be true unless crossbow bolts are another type?
	dmgEntry.mDamage = weapon->damage.GetAttackDamage();
	
	mDamageMap[dmgEntry.mFormType] = dmgEntry;

	return true;
}

const CDamageEntry* CDamageTracker::LookupDamageEntry(Projectile* proj) const
{
	auto it = mDamageMap.find(proj->formType);
	if (it != mDamageMap.end())
	{
		return &it->second;
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

