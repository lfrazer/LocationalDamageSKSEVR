//#include <SKSE.h>
#include <skse64/PluginAPI.h>
//#include <skse64/DebugLog.h>
#include <skse64/GameRTTI.h>
#include <skse64/GameForms.h>
//#include <skse64/SafeWrite.h>
//#include <skse64/HookUtil.h>
#include <skse64/GameReferences.h>
#include <skse64/GameObjects.h>
#include <skse64/GameExtraData.h>
#include <skse64/NiNodes.h>
#include <skse64/GameCamera.h>

#include <random>

#include "iniSettings.h"

//class Actor
//DEFINE_MEMBER_FN(DamageActorValue, void, 0x006E0760, UInt32 unk1, UInt32 actorValueID, float damage, Actor* akAggressor);
//class ActorProcessManager
//DEFINE_MEMBER_FN(PushActorAway, void, 0x00723FE0, Actor* actor, float x, float y, float z, float force);

typedef std::pair<BSFixedString, UInt32> Pair;
std::unordered_map<std::string, std::vector<Pair>> locationalNodeMap = {
	{ "actors\\character\\defaultmale.hkx",{ { "NPC Head [Head]", 20 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\character\\defaultfemale.hkx",{ { "NPC Head [Head]", 20 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\draugr\\draugrproject.hkx",{ { "NPC Head [Head]", 20 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\spriggan\\spriggan.hkx",{ { "NPC Head [Head]", 20 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\draugr\\draugrskeletonproject.hkx",{ { "NPC Head [Head]", 20 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\vampirelord\\vampirelord.hkx",{ { "NPC Head [Head]", 20 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\hagraven\\hagravenproject.hkx",{ { "NPC Head [Head]", 20 },{ "NPC R Calf [LClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [RLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\dlc02\\benthiclurker\\benthiclurkerproject.hkx",{ { "NPC Head [Head]", 30 },{ "NPC R Calf [RClf]", 30 },{ "NPC L Calf [RClf]", 30 },{ "NPC R Forearm [RLar]", 30 },{ "NPC L Forearm [RLar]", 30 },{ "NPC Spine2 [Spn2]", 30 } } },
{ "actors\\giant\\giantproject.hkx",{ { "NPC Head [Head]", 30 },{ "NPC R Calf [RClf]", 30 },{ "NPC L Calf [RClf]", 30 },{ "NPC R Forearm [RLar]", 30 },{ "NPC L Forearm [RLar]", 30 },{ "NPC Spine2 [Spn2]", 30 } } },
{ "actors\\atronachfrost\\atronachfrostproject.hkx",{ { "NPC Head [Head]", 25 },{ "NPC R Calf [LClf]", 25 },{ "NPC L Calf [LClf]", 25 },{ "NPC R Forearm [RLar]", 25 },{ "NPC L Forearm [RLar]", 25 },{ "NPC Spine2 [Spn2]", 25 } } },
{ "actors\\dlc01\\vampirebrute\\vampirebruteproject.hkx",{ { "NPC Head [Head]", 25 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\troll\\trollproject.hkx",{ { "NPC Head [Head]", 25 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\werewolfbeast\\werewolfbeastproject.hkx",{ { "NPC Head [Head]", 25 },{ "NPC R Calf [LClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [RLar]", 20 },{ "NPC Spine2 [Spn2]", 20 } } },
{ "actors\\witchlight\\witchlightproject.hkx",{} },
{ "actors\\ambient\\hare\\hareproject.hkx",{} },
{ "actors\\ambient\\chicken\\chickenproject.hkx",{} },
{ "actors\\mudcrab\\mudcrabproject.hkx",{} },
{ "actors\\dlc02\\netch\netchproject.hkx",{} },
{ "actors\\dragon\\dragonproject.hkx",{} },
{ "actors\\canine\\dogproject.hkx",{ { "Canine_JawBone", 20 },{ "Canine_RBackLeg2", 15 },{ "Canine_LBackLeg2", 15 },{ "Canine_RFrontLeg2", 15 },{ "Canine_LFrontLeg2", 15 },{ "Canine_Ribcage", 20 } } },
{ "actors\\canine\\wolfproject.hkx",{ { "Canine_JawBone", 20 },{ "Canine_RBackLeg2", 15 },{ "Canine_LBackLeg2", 15 },{ "Canine_RFrontLeg2", 15 },{ "Canine_LFrontLeg2", 15 },{ "Canine_Ribcage", 20 } } },
{ "actors\\falmer\\falmerproject.hkx",{ { "NPC Head", 20 },{ "NPC R Calf", 20 },{ "NPC L Calf", 20 },{ "NPC R Forearm", 20 },{ "NPC L Forearm", 20 },{ "NPC Spine2", 20 } } },
{ "actors\\dlc02\\riekling\\rieklingproject.hkx",{ { "NPC Head", 20 },{ "NPC R Calf", 15 },{ "NPC L Calf", 15 },{ "NPC R Forearm", 15 },{ "NPC L Forearm", 15 },{ "NPC Spine2", 20 } } },
{ "actors\\bear\\bearproject.hkx",{ { "NPC Jaw", 25 },{ "NPC RLeg2", 20 },{ "NPC LLeg2", 20 },{ "NPC RArm2", 20 },{ "NPC LArm2", 20 },{ "NPC Ribcage", 25 } } },
{ "actors\\chaurus\\chaurusproject.hkx",{ { "NPC Head [Head]", 25 },{ "R Leg[RClf]", 20 },{ "LLeg[LClf]", 20 },{ "R UpperArm[RLar]", 20 },{ "L UpperArm[LLar]", 20 },{ "Ribcage[Spn3]", 20 } } },
{ "actors\\deer\\deerproject.hkx",{ { "ElkJaw", 20 },{ "ElkRTibia", 15 },{ "ElkLTibia", 15 },{ "ElkRRadius", 15 },{ "ElkLRadius", 15 },{ "ElkRibcage", 20 } } },
{ "actors\\dragonpriest\\dragon_priest.hkx",{ { "DragPriestNPC Head [Head]", 20 },{ "DragPriestNPC R Calf [RClf]", 20 },{ "DragPriestNPC L Calf [LClf]", 20 },{ "DragPriestNPC R Forearm [RLar]", 20 },{ "DragPriestNPC L Forearm [LLar]", 20 },{ "DragPriestNPC Spine2 [Spn2]", 20 } } },
{ "actors\\dwarvensteamcenturion\\steamproject.hkx",{ { "NPC LowerJaw", 30 },{ "NPC R Calf [RClf]", 30 },{ "NPC L Calf [LClf]", 30 },{ "NPC R Forearm [RLar]", 30 },{ "NPC L Forearm [LLar]", 30 },{ "NPC Spine2 [Spn2]", 30 } } },
{ "actors\\dwarvenspherecenturion\\spherecenturion.hkx",{ { "NPC Head", 20 },{ "NPC R Calf", 20 },{ "NPC L Calf", 20 },{ "NPC R Forearm", 20 },{ "NPC L Forearm", 20 },{ "NPC Spine2", 20 } } },
{ "actors\\atronachflame\\atronachflame.hkx",{ { "FireAtronach_Head [Head]", 20 },{ "FireAtronach_R Calf [RClf]", 20 },{ "FireAtronach_L Calf [LClf]", 20 },{ "FireAtronach_R Forearm [RLar]", 20 },{ "FireAtronach_L Forearm [LLar]", 20 },{ "FireAtronach_Spine2 [Spn2]", 20 } } },
{ "actors\\atronachstorm\\atronachstormproject.hkx",{ { "Jaw", 25 },{ "NPC R Calf [LClf]", 25 },{ "NPC L Calf [LClf]", 25 },{ "NPC R Forearm [RLar]", 25 },{ "NPC L Forearm [RLar]", 25 },{ "NPC Spine2 [Spn2]", 25 } } },
{ "actors\\goat\\goatproject.hkx",{ { "Goat_JawBone", 20 },{ "Goat_RBackLeg_Caf", 15 },{ "Goat_LBackLeg_Caf", 15 },{ "Goat_RFrontLeg_Upper", 15 },{ "Goat_LFrontLeg_Upper", 15 },{ "Goat_Ribcage", 20 } } },
{ "actors\\horker\\horkerproject.hkx",{ { "Horker_JawBone", 25 },{ "Horker_RBackLeg_Caf", 20 },{ "Horker_LBackLeg_Caf", 20 },{ "Horker_RFrontLeg_Upper", 20 },{ "Horker_LFrontLeg_Upper", 20 },{ "Horker_Ribcage", 25 } } },
{ "actors\\horse\\horseproject.hkx",{ { "HorseJaw", 30 },{ "HorseRTibia", 25 },{ "HorseLTibia", 25 },{ "HorseFrontRLegRadius", 25 },{ "HorseLRadius", 25 },{ "HorseRibcage", 30 } } },
{ "actors\\mammoth\\mammothproject.hkx",{ { "Mammoth Jaw", 60 },{ "Mammoth Back R Lowerleg", 40 },{ "Mammoth Back L Lowerleg", 40 },{ "Mammoth Front R Lowerleg", 40 },{ "Mammoth Front L Lowerleg", 40 },{ "Mammoth Spine 4", 60 } } },
{ "actors\\sabrecat\\sabrecatproject.hkx",{ { "Sabrecat_Head[jaw]", 25 },{ "Sabrecat_RightCalf[RClf]", 20 },{ "Sabrecat_LeftCalf[LClf]", 20 },{ "Sabrecat_RightForearm[RFar]", 20 },{ "Sabrecat_LeftForearm[LFar]", 20 },{ "Sabrecat_Ribcage[Spn4]", 25 } } },
{ "actors\\skeever\\skeeverproject.hkx",{ { "Jaw", 20 },{ "RLeg2", 15 },{ "LLeg2", 15 },{ "RArm_Forearm", 15 },{ "LArm_Forearm", 15 },{ "Torso", 20 } } },
{ "actors\\cow\\highlandcowproject.hkx",{ { "Jaw", 25 },{ "RTibia", 20 },{ "LTibia", 20 },{ "R_Radius", 20 },{ "L_Radius", 20 },{ "Ribcage", 25 } } },
{ "actors\\dlc01\\chaurusflyer\\chaurusflyer.hkx",{ { "ChaurusFlyerHead", 20 },{ "ChaurusFlyerRLeg2", 20 },{ "ChaurusFlyerLLeg2", 20 },{ "ChaurusFlyerRArm2", 20 },{ "ChaurusFlyerLArm2", 20 },{ "ChaurusFlyerTorso", 20 } } },
{ "actors\\dlc02\\boarriekling\\boarproject.hkx",{ { "Boar_Reikling_Head", 20 },{ "Boar_Reikling_RLeg2", 15 },{ "Boar_Reikling_LLeg2", 15 },{ "Boar_Reikling_R_UpArmForeArm1", 15 },{ "Boar_Reikling_L_UpArmForeArm1", 15 },{ "Boar_Reikling_Chest", 20 } } },
{ "actors\\dlc02\\scrib\\scribproject.hkx",{ { "Head [Head]", 20 },{ "Leg2Calf[RClf]", 15 },{ "Leg2Calf[LClf]", 15 },{ "Leg1Calf[RClf]", 15 },{ "Leg1Calf[LClf]", 15 },{ "Spine [Spn1]", 20 } } },
{ "actors\\dlc02\\dwarvenballistacenturion\\ballistacenturion.hkx",{ { "WeaponBow", 20 },{ "RBackLeg2", 15 },{ "LBackLeg2", 15 },{ "RFrontLeg2", 15 },{ "LFrontLeg2", 15 },{ "MainBody", 20 } } },
{ "actors\\dwarvenspider\\dwarvenspidercenturionproject.hkx",{ { "DwarvenSpiderHead_XYZ", 15 } } },
{ "actors\\frostbitespider\\frostbitespiderproject.hkx",{ { "NPC Head [Head]", 15 } } },
{ "actors\\icewraith\\icewraithproject.hkx",{ { "IW Head", 15 } } },
{ "actors\\slaughterfish\\slaughterfishproject.hkx",{ { "SlaughterfishHead", 15 } } },
{ "actors\\wisp\\wispproject.hkx",{ { "Wisp Head", 20 } } },
{ "actors\\dlc02\\hmdaedra\\hmdaedra.hkx",{ { "NPC M HeadNoodle01", 25 } } }
};

typedef void(*FnDebug_Notification)(const char*, bool, bool);
const FnDebug_Notification fnDebug_Notification = (FnDebug_Notification)0x008997A0;

static bool Probability(double value)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_real_distribution<> score(0.0, 100.0);

	if (score(mt) < value)
		return true;

	return false;
}

class EquipManager
{
public:
	virtual ~EquipManager();
	static EquipManager *   GetSingleton()
	{
		return *((EquipManager **)0x012E5FAC);
	}

	DEFINE_MEMBER_FN(EquipItem_internal, void, 0x006EF3E0, Actor * actor, TESForm * item, BaseExtraList * extraData, SInt32 count, BGSEquipSlot * equipSlot, bool withEquipSound, bool preventUnequip, bool showMsg, void * unk);
	DEFINE_MEMBER_FN(UnequipItem_internal, bool, 0x006EE560, Actor * actor, TESForm * item, BaseExtraList * extraData, SInt32 count, BGSEquipSlot * equipSlot, bool unkFlag1, bool preventEquip, bool unkFlag2, bool unkFlag3, void * unk);

	void EquipItem(Actor* actor, TESForm* item, BaseExtraList* extraData)
	{
		EquipItem_internal(actor, item, extraData, 1, nullptr, false, false, false, nullptr);
	}
	void UnequipItem(Actor* actor, TESForm* item, BaseExtraList* extraData)
	{
		UnequipItem_internal(actor, item, extraData, 1, nullptr, false, false, false, false, nullptr);
	}
};

static BaseExtraList* GetEquippedWeaponEx(Actor* actor, bool abLeftHand, TESForm* form)
{
	ExtraContainerChanges *exChanges = actor->extraData.GetByType<ExtraContainerChanges>();
	if (exChanges && exChanges->changes && exChanges->changes->entryList)
	{
		for (InventoryEntryData *pEntry : *exChanges->changes->entryList)
		{
			if (!pEntry || !pEntry->baseForm || pEntry->baseForm != form || !pEntry->extraList)
				continue;

			for (BaseExtraList *pExtraDataList : *pEntry->extraList)
			{
				if (pExtraDataList && ((!abLeftHand && pExtraDataList->HasType(ExtraDataType::Worn)) || (abLeftHand && pExtraDataList->HasType(ExtraDataType::WornLeft))))
					return pExtraDataList;
			}
		}
	}

	return nullptr;
}

struct FoundEquipArmor
{
	TESObjectARMO* pArmor;
	BaseExtraList* pExtraData;

	FoundEquipArmor() : pArmor(nullptr), pExtraData(nullptr)
	{
	}
};

static FoundEquipArmor GetEquippedArmorEx(Actor* actor, BGSBipedObjectForm::PartFlag slotMask)
{
	FoundEquipArmor equipArmor;

	ExtraContainerChanges *exChanges = actor->extraData.GetByType<ExtraContainerChanges>();
	if (exChanges && exChanges->changes && exChanges->changes->entryList)
	{
		for (InventoryEntryData *pEntry : *exChanges->changes->entryList)
		{
			if (!pEntry || !pEntry->baseForm || pEntry->baseForm->formType != FormType::Armor || !pEntry->extraList)
				continue;

			TESObjectARMO* armor = static_cast<TESObjectARMO*>(pEntry->baseForm);
			if (!armor->HasPartOf(slotMask))
				continue;

			for (BaseExtraList *pExtraDataList : *pEntry->extraList)
			{
				if (pExtraDataList && (pExtraDataList->HasType(ExtraDataType::Worn) || pExtraDataList->HasType(ExtraDataType::WornLeft)))
				{
					equipArmor.pArmor = armor;
					equipArmor.pExtraData = pExtraDataList;

					return equipArmor;
				}
			}
		}
	}

	return equipArmor;
}

static float GetLocationalDamage(Actor* actor, BGSAttackData* attackData, TESObjectWEAP* weapon, Actor* caster_actor, TESObjectARMO* armor, MultiplierType multiplierType)
{
	double damage = 0.0;

	if (weapon && (!attackData || !attackData->flags.ignoreWeapon))
	{
		damage = weapon->GetAttackDamage();

		if (caster_actor)
		{
			switch (weapon->type())
			{
			case TESObjectWEAP::GameData::kType_HandToHandMelee:
				damage = caster_actor->GetActorValueCurrent(35);
				break;
			case TESObjectWEAP::GameData::kType_OneHandSword:
			case TESObjectWEAP::GameData::kType_OneHandDagger:
			case TESObjectWEAP::GameData::kType_OneHandAxe:
			case TESObjectWEAP::GameData::kType_OneHandMace:
				damage *= 1.0 + caster_actor->GetActorValueCurrent(6) * 0.5 / 100.0;
				break;
			case TESObjectWEAP::GameData::kType_TwoHandSword:
			case TESObjectWEAP::GameData::kType_TwoHandAxe:
				damage *= 1.0 + caster_actor->GetActorValueCurrent(7) * 0.5 / 100.0;
				break;
			case TESObjectWEAP::GameData::kType_Bow:
			case TESObjectWEAP::GameData::kType_CrossBow:
				damage *= 1.0 + caster_actor->GetActorValueCurrent(8) * 0.5 / 100.0;
				break;
			default:
				break;
			}
		}
	}
	else if (caster_actor)
	{
		damage = caster_actor->GetActorValueCurrent(35);
	}

	if (attackData)
		damage *= attackData->damageMult;

	if (armor)
	{
		if (armor->IsHeavyArmor())
			damage *= ini.HeavyArmorDamageMultiplier;
		else if (armor->IsLightArmor())
			damage *= ini.LightArmorDamageMultiplier;
	}

	if (actor == g_thePlayer)
		damage *= ini.NPCToPlayer[multiplierType];
	else if (caster_actor == g_thePlayer)
		damage *= ini.PlayerToNPC[multiplierType];
	else
		damage *= ini.NPCToNPC[multiplierType];

	return (float)-damage;
}

static double GetLocationalEffectChance(Actor* actor, BGSAttackData* attackData, TESObjectWEAP* weapon, Actor* caster_actor, TESObjectARMO* armor, MultiplierType multiplierType)
{
	double chance = 1.0;

	if (weapon && (!attackData || !attackData->flags.ignoreWeapon))
		chance *= 1.0 + weapon->weight / 30.0;

	if (caster_actor)
	{
		double difference = (caster_actor->GetActorValueMaximum(24) - actor->GetActorValueMaximum(24)) * 0.00014;
		if (difference < -0.7)
			difference = -0.7;
		else if (difference > 0.7)
			difference = 0.7;
		chance *= 1.0 + difference;
	}

	if (armor)
	{
		if (armor->IsHeavyArmor())
			chance *= ini.HeavyArmorEffectChanceMultiplier;
		else if (armor->IsLightArmor())
			chance *= ini.LightArmorEffectChanceMultiplier;
	}

	if (actor == g_thePlayer)
		chance *= ini.NPCToPlayer[multiplierType];
	else if (caster_actor == g_thePlayer)
		chance *= ini.PlayerToNPC[multiplierType];
	else
		chance *= ini.NPCToNPC[multiplierType];

	return chance;
}

static void ApplyLocationalDamage(Actor* actor, UInt32 damageType, float dmg, Actor* akAggressor)
{
	if (dmg >= 0.0f)
		return;

	if ((damageType & 1) != 0)
		actor->DamageActorValue(2, 24, dmg, akAggressor);

	if ((damageType & 2) != 0)
		actor->DamageActorValue(2, 25, dmg, akAggressor);

	if ((damageType & 4) != 0)
		actor->DamageActorValue(2, 26, dmg, akAggressor);
}

static void ApplyLocationalEffect(Actor* actor, UInt32 effectType, double chance, FoundEquipArmor equipArmor, std::string pathString)
{
	if (chance <= 0.0)
		return;

	if ((effectType & 1) != 0 && ini.InterruptBaseChance != 0 && Probability(ini.InterruptBaseChance * chance))
	{
		static BSFixedString strAnimationRecoil = "recoilStart";
		actor->SendAnimationEvent(strAnimationRecoil);
	}

	if ((effectType & 2) != 0 && ini.StaggerBaseChance != 0 && Probability(ini.StaggerBaseChance * chance))
	{
		static BSFixedString strAnimationRecoilLarge = "recoilLargeStart";
		actor->SendAnimationEvent(strAnimationRecoilLarge);
	}

	if ((effectType & 4) != 0 && ini.KnockdownBaseChance != 0 && Probability(ini.KnockdownBaseChance * chance) && pathString != "actors\\dragonpriest\\dragon_priest.hkx" && (actor->flags04 & 0xE000000) == 0 && actor->processManager)
	{
		actor->processManager->PushActorAway(actor, 0.0f, 0.0f, 0.0f, 0.0f);
	}

	if ((effectType & 8) != 0 && ini.UnequipArmorBaseChance != 0 && Probability(ini.UnequipArmorBaseChance * chance) && equipArmor.pExtraData && !equipArmor.pExtraData->IsQuestItem())
	{
		EquipManager* em = EquipManager::GetSingleton();
		if (em)
			em->UnequipItem(actor, equipArmor.pArmor, equipArmor.pExtraData);
	}

	if ((effectType & 16) != 0 && ini.UnequipWeaponBaseChance != 0 && Probability(ini.UnequipWeaponBaseChance * chance))
	{
		EquipManager* em = EquipManager::GetSingleton();
		if (em)
		{
			bool abLeftHand = false;

			for (int i = 0; i < 2; i++)
			{
				TESForm* form = actor->GetEquippedObject(abLeftHand);
				if (form && form->formType == FormType::Weapon && (static_cast<TESObjectWEAP*>(form))->type() != TESObjectWEAP::GameData::kType_HandToHandMelee)
				{
					BaseExtraList* pExtraData = GetEquippedWeaponEx(actor, abLeftHand, form);
					if (pExtraData && !pExtraData->IsQuestItem())
						em->UnequipItem(actor, form, pExtraData);
				}

				abLeftHand = true;
			}
		}
	}
}

// Skyrim SE hook:
// typedef int64_t (*_OnProjectileHitFunction)(Projectile* akProjectile, TESObjectREFR* akTarget, NiPoint3* point,                                        
 //   UInt32 unk1, UInt32 unk2, UInt8 unk3);

// There is also a way to get "caster" lookup from Projectile*
/*
		UInt32* handle = Projectile_GetActorCauseFn(akProjectile);

#ifdef SKYRIMVR
		TESObjectREFR* refCaster = nullptr;
#else
		NiPointer<TESObjectREFR> refCaster = nullptr;
#endif

		if (handle && *handle != *g_invalidRefHandle)
		{

#ifdef SKYRIMVR
			LookupREFRByHandle(handle, &refCaster); // SKSE_VR takes handle ptr here, not ref? - also 2nd arg is double ptr not NiPointer template
#else
			LookupREFRByHandle(*handle, refCaster);
#endif
		}
*/

// Not sure how to get CELL or WEAPON..  (Maybe from Actor struct?)
// What is the difference between caster and caster_ref ?

// For weapon, maybe:  	TESForm * GetEquippedObject(bool abLeftHand); (Actor func)
// or 	SpellItem* leftHandSpell;						// 1C0
//	SpellItem* rightHandSpell;						// 1C8

static void Impact_Hook(UInt32 ecx, UInt32* stack)
{
	TESObjectCELL* cell = (TESObjectCELL*)stack[1];
	if (!cell)
		return;

	//If sweep attack, type is int?
	if (((UInt32*)stack[9])[35] < cell->objectList.GetSize())
		return;

	NiPoint3* hit_pos = (NiPoint3*)stack[5];
	TESObjectREFR* target = (TESObjectREFR*)stack[10];
	if (!hit_pos || !target)
		return;

	Actor* actor = DYNAMIC_CAST<Actor*>(target);
	if (!actor || actor->IsDead(true) || actor->IsInKillMove())
		return;

	TESRace* race = actor->GetRace();
	if (!race)
		return;

	NiNode *node = actor->GetNiNode();
	if (actor == g_thePlayer && g_thePlayer->loadedState)
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		node = camera && camera->IsFirstPerson() ? g_thePlayer->firstPersonSkeleton : g_thePlayer->loadedState->node;
	}

	if (!node)
		return;

	int gender = 0;
	TESNPC* base = actor->GetActorBase();
	if (base && base->GetSex())
		gender = 1;

	std::string pathString = std::string(race->behaviorGraph[gender].modelName);
	ini.ToLower(pathString);
	static std::vector<Pair> defaultNodeNames = { { "NPC Head [Head]", 20 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 } };
	std::vector<Pair> nodeNames = locationalNodeMap.count(pathString) >= 1 ? locationalNodeMap.at(pathString) : defaultNodeNames;

	if (nodeNames.empty())
		return;

	BGSAttackData* attackData = (BGSAttackData*)((UInt32*)stack[9])[9];
	TESObjectWEAP* weapon = (TESObjectWEAP*)((UInt32*)stack[9])[10];
	Projectile* projectile = (Projectile*)((UInt32*)stack[9])[32];
	TESObjectREFR* caster = (TESObjectREFR*)((UInt32*)stack[9])[35];

	int hitNode = -1;
	float scale = actor->GetScale();
	for (int i = 0; i < nodeNames.size(); i++)
	{
		NiAVObject *obj = node->GetObjectByName(nodeNames[i].first);
		if (obj)
		{
			NiPoint3 node_pos = obj->GetWorldTranslate();

			double dx = hit_pos->x - node_pos.x;
			double dy = hit_pos->y - node_pos.y;
			double dz = hit_pos->z - node_pos.z;
			double d2 = dx * dx + dy * dy + dz * dz;

			if (d2 < nodeNames[i].second * scale * nodeNames[i].second * scale)
			{
				hitNode = i;
				break;
			}
		}
	}

	if (hitNode != -1)
	{
		TESObjectREFR* caster_ref = nullptr;

		if (projectile)
		{
			RefHandle* handle = projectile->GetActorCause();
			if (handle && *handle != g_invalidRefHandle)
				TESObjectREFR::LookupByHandle(*handle, caster_ref);
		}
		else
		{
			caster_ref = caster;
		}

		Actor* caster_actor = nullptr;
		if (caster_ref)
			caster_actor = DYNAMIC_CAST<Actor*>(caster_ref);

		bool done = false;
		FoundEquipArmor equipArmor;

		switch (hitNode)
		{
		case 0:
			if (ini.EnableHead)
			{
				equipArmor = GetEquippedArmorEx(actor, BGSBipedObjectForm::kPart_Head);
				if (!equipArmor.pExtraData)
					equipArmor = GetEquippedArmorEx(actor, BGSBipedObjectForm::kPart_Hair);

				if (ini.EffectTypeHead != 0)
					ApplyLocationalEffect(actor, ini.EffectTypeHead, GetLocationalEffectChance(actor, attackData, weapon, caster_actor, equipArmor.pArmor, Type_HeadEffectChanceMultiplier), equipArmor, pathString);

				if (ini.DamageTypeHead != 0)
					ApplyLocationalDamage(actor, ini.DamageTypeHead, GetLocationalDamage(actor, attackData, weapon, caster_actor, equipArmor.pArmor, Type_HeadDamageMultiplier), caster_actor);

				if (ini.DisplayNotification && (actor == g_thePlayer || caster_actor == g_thePlayer))
				{
					std::string str = ini.HeadMessageFront + std::string(actor->GetReferenceName()) + ini.HeadMessageBack;
					fnDebug_Notification(str.c_str(), false, true);
				}

				done = true;
			}
			break;
		case 1:
		case 2:
			if (ini.EnableFoot)
			{
				equipArmor = GetEquippedArmorEx(actor, BGSBipedObjectForm::kPart_Feet);

				if (ini.EffectTypeFoot != 0)
					ApplyLocationalEffect(actor, ini.EffectTypeFoot, GetLocationalEffectChance(actor, attackData, weapon, caster_actor, equipArmor.pArmor, Type_FootEffectChanceMultiplier), equipArmor, pathString);

				if (ini.DamageTypeFoot != 0)
					ApplyLocationalDamage(actor, ini.DamageTypeFoot, GetLocationalDamage(actor, attackData, weapon, caster_actor, equipArmor.pArmor, Type_FootDamageMultiplier), caster_actor);

				if (ini.DisplayNotification && (actor == g_thePlayer || caster_actor == g_thePlayer))
				{
					std::string str = ini.FootMessageFront + std::string(actor->GetReferenceName()) + ini.FootMessageBack;
					fnDebug_Notification(str.c_str(), false, true);
				}

				done = true;
			}
			break;
		case 3:
		case 4:
			if (ini.EnableArms)
			{
				equipArmor = GetEquippedArmorEx(actor, BGSBipedObjectForm::kPart_Hands);

				if (ini.EffectTypeArms != 0)
					ApplyLocationalEffect(actor, ini.EffectTypeArms, GetLocationalEffectChance(actor, attackData, weapon, caster_actor, equipArmor.pArmor, Type_ArmsEffectChanceMultiplier), equipArmor, pathString);

				if (ini.DamageTypeArms != 0)
					ApplyLocationalDamage(actor, ini.DamageTypeArms, GetLocationalDamage(actor, attackData, weapon, caster_actor, equipArmor.pArmor, Type_ArmsDamageMultiplier), caster_actor);

				if (ini.DisplayNotification && (actor == g_thePlayer || caster_actor == g_thePlayer))
				{
					std::string str = ini.ArmsMessageFront + std::string(actor->GetReferenceName()) + ini.ArmsMessageBack;
					fnDebug_Notification(str.c_str(), false, true);
				}

				done = true;
			}
			break;
		case 5:
			if (ini.EnableHeart)
			{
				equipArmor = GetEquippedArmorEx(actor, BGSBipedObjectForm::kPart_Body);

				if (ini.EffectTypeHeart != 0)
					ApplyLocationalEffect(actor, ini.EffectTypeHeart, GetLocationalEffectChance(actor, attackData, weapon, caster_actor, equipArmor.pArmor, Type_HeartEffectChanceMultiplier), equipArmor, pathString);

				if (ini.DamageTypeHeart != 0)
					ApplyLocationalDamage(actor, ini.DamageTypeHeart, GetLocationalDamage(actor, attackData, weapon, caster_actor, equipArmor.pArmor, Type_HeartDamageMultiplier), caster_actor);

				if (ini.DisplayNotification && (actor == g_thePlayer || caster_actor == g_thePlayer))
				{
					std::string str = ini.HeartMessageFront + std::string(actor->GetReferenceName()) + ini.HeartMessageBack;
					fnDebug_Notification(str.c_str(), false, true);
				}

				done = true;
			}
			break;
		}

		if (done && ini.DisplayImpactEffect)
		{
			typedef void(*FnPlayImpactEffect)(TESObjectCELL*, float, const char*, NiPoint3*, NiPoint3*, float, UInt32, UInt32);
			const FnPlayImpactEffect fnPlayImpactEffect = (FnPlayImpactEffect)0x005F07C0;
			fnPlayImpactEffect(cell, 1.0f, "LocationalDamage\\LocDamageImpact01.nif", (NiPoint3*)stack[4], hit_pos, 1.0f, stack[7], stack[8]);
		}
	}
}

class LocationalDamagePlugin : public SKSEPlugin
{
public:
	LocationalDamagePlugin()
	{
	}

	virtual bool InitInstance() override
	{
		if (!Requires(kSKSEVersion_1_6_12))
			return false;

		SetName("LocationalDamage");
		SetVersion(1);

		return true;
	}

	virtual bool OnLoad() override
	{
		SKSEPlugin::OnLoad();

		return true;
	}

	virtual void OnModLoaded() override
	{
		ini.Load();

		HookRelCall(0x006C9014, Impact_Hook);
	}
} thePlugin;
