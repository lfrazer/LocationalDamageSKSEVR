// First params should be BSScript__Internal__VirtualMachine class ptr
// Maybe at: 141F81900

#pragma once

#include <skse64/GameRTTI.h>
#include <skse64/GameForms.h>
//#include <skse64/SafeWrite.h>
//#include <skse64/HookUtil.h>
#include <skse64/GameReferences.h>
#include <skse64/GameObjects.h>
#include <skse64/GameExtraData.h>

#define EXTRA_DEBUG_LOG 0

#if EXTRA_DEBUG_LOG || defined(_DEBUG)
#define _DEBUGMSG _MESSAGE
#else
#define _DEBUGMSG(...) 
#endif

#ifdef SKYRIMVR

#define ONPROJECTILEHIT_HOOKLOCATION							0x00777E2A  // in VR, this is not called from HealthDamageFunctor_CTOR, so we will try to call it from BeamProjectile_vf_sub_140777A30 instead
#define ONPROJECTILEHIT_INNERFUNCTION							0x0077E4E0

#define PROJECTILE_GETACTORCAUSEFN								0x00779010 // ??_7Projectile@@6B@			vtbl[51]

#define DAMAGEACTORVALUE_FN										0x009848B0
#define GETACTORVALUE_FN										0x00984E60
#define PUSHACTORAWAY_FN										0x009D0E60
#define DEBUGNOTIFICATION_FN									0x009A7E90
#define PLAYSOUND_FN											0x009EF150
#define PLAYIMPACTEFFECT_FN										0x009D06C0

#define DECAPITATEACTOR_FN										0x005FAC70

// CommonLib VR port
namespace Offset
{
	namespace BSAudioManager
	{
		// 1.00	0.90	-------	0000000140BEE580	sub_0000000140BEE580	0000000140C29430	sub_0000000140C29430	 	call reference matching	1	1	1	2	2	2	0	0	0
		constexpr std::uintptr_t GetSingleton = 0x00C29430;	// VR

		//1.00	0.99	-------	0000000140BEEE70	sub_0000000140BEEE70	0000000140C29D20	sub_0000000140C29D20	 	call reference matching	4	4	4	34	34	34	4	4	4
		constexpr std::uintptr_t SetUp = 0x00C29D20;		// VR
	
		//1.00	0.99	-------	0000000140BEF0B0	sub_0000000140BEF0B0	0000000140C29F60	sub_0000000140C29F60	 	call reference matching	12	12	12	121	121	121	17	17	17
		constexpr std::uintptr_t BuildSoundDataFromDescriptor = 0x00C29F60;	// VR
	}

	namespace SoundData
	{
		//1.00	0.99	-------	0000000140BED530	sub_0000000140BED530	0000000140C283E0	sub_0000000140C283E0	 	call reference matching	3	3	3	18	18	18	2	2	2
		constexpr std::uintptr_t Play = 0x00C283E0;			// VR
		// 1.00	0.99	-------	0000000140BEDB10	sub_0000000140BEDB10	0000000140C289C0	sub_0000000140C289C0	 	call reference matching	6	6	6	23	23	23	8	8	8
		constexpr std::uintptr_t SetNode = 0x00C289C0;		// VR
		//1.00	0.99	-------	0000000140BED920	sub_0000000140BED920	0000000140C287D0	sub_0000000140C287D0	 	call reference matching	4	4	4	27	27	27	4	4	4
		constexpr std::uintptr_t SetPosition = 0x00C287D0;	// VR
	}
}
#else
// new for RUNTIME_VERSION_1_5_97 

//1.00	0.99	-------	00000001407531C0	sub_00000001407531C0	000000014077E4E0	OnProjectileHitInner	 	edges flowgraph MD index	83	83	83	490	490	490	130	130	130


#define ONPROJECTILEHIT_HOOKLOCATION							0x0074CB0A
#define ONPROJECTILEHIT_INNERFUNCTION							0x007531C0


// 1.00	0.62	-------	00000001407CD9D0	sub_00000001407CD9D0	0000000140779010	ArrowProjectile_vf_sub_140779010	 	address sequence	1	1	1	2	2	2	0	0	0

#define PROJECTILE_GETACTORCAUSEFN								0x007CD9D0 // ??_7Projectile@@6B@			vtbl[51]

// 1.00	0.99	-------	000000014094A4C0	sub_000000014094A4C0	00000001409848B0	Actor__DamageActorValue_PSF	 	edges flowgraph MD index	12	12	12	68	68	68	16	16	16

#define DAMAGEACTORVALUE_FN										0x0094A4C0
#define PUSHACTORAWAY_FN										0x00996340
#define DEBUGNOTIFICATION_FN									0x0096DDB0

// TODO: verify these
#define GETACTORVALUE_FN										0x0094AA70
#define PLAYSOUND_FN											0x009B4620
#define PLAYIMPACTEFFECT_FN										0x00995BA0

#define DECAPITATEACTOR_FN										0x005F2530

// Below is now outdated
/*
// SSE 1.5.73

#define ONPROJECTILEHIT_HOOKLOCATION							0x0074CB0A
#define ONPROJECTILEHIT_INNERFUNCTION							0x007531C0

#define PROJECTILE_GETACTORCAUSEFN								0x0074DCF0 // ??_7Projectile@@6B@			vtbl[51]

#define DAMAGEACTORVALUE_FN										0x0094A4C0
#define PUSHACTORAWAY_FN										0x00996340
#define DEBUGNOTIFICATION_FN									0x0096DDB0

// TODO: verify these
#define GETACTORVALUE_FN										0x0094AA70
#define PLAYSOUND_FN											0x009B4620
#define PLAYIMPACTEFFECT_FN										0x00995BA0
*/

// From CommonLibSE  -> Thanks to Ryan SniffleMan!
namespace BSAudioManager
{
	// E8 ? ? ? ? BA 33 00 00 00
	constexpr std::uintptr_t GetSingleton = 0x00BEE580;	// 1_5_73
	// E8 ? ? ? ? F3 0F 10 5E 5C
	constexpr std::uintptr_t SetUp = 0x00BEEE70;		// 1_5_73

	// IndirectSig: E8 ? ? ? ? C6 46 04 01
	constexpr std::uintptr_t BuildSoundDataFromDescriptor = 0x00BEF0B0;	// 1_5_80
}

namespace SoundData
{
	// E8 ? ? ? ? EB 0E 84 C0
	constexpr std::uintptr_t Play = 0x00BED530;			// 1_5_73
	// E8 ? ? ? ? F3 0F 10 5D 9F
	constexpr std::uintptr_t SetNode = 0x00BEDB10;		// 1_5_73
	// E8 ? ? ? ? 4C 8D 7E 20
	constexpr std::uintptr_t SetPosition = 0x00BED920;	// 1_5_73
}

#endif


#define DEFAULT_IMPACTEFFECT_FORMID	0x1A3FB // vampire drain (from dawngaurd esm - setup in constructor)
#define DEFAULT_SOUNDEFFECT_FORMID 0x19398 // (WPNImpactBluntVsFlesh)
#define DEFAULT_SOUNDEFFECT_SPELL_FORMID 0x639A7 // MAGRuneImpact

enum class eActorValue : UInt32
{
	kAggresion = 0,
	kConfidence = 1,
	kEnergy = 2,
	kMorality = 3,
	kMood = 4,
	kAssistance = 5,
	kOneHanded = 6,
	kTwoHanded = 7,
	kArchery = 8,
	kBlock = 9,
	kSmithing = 10,
	kHeavyArmor = 11,
	kLightArmor = 12,
	kPickpocket = 13,
	kLockpicking = 14,
	kSneak = 15,
	kAlchemy = 16,
	kSpeech = 17,
	kAlteration = 18,
	kConjuration = 19,
	kDestruction = 20,
	kIllusion = 21,
	kRestoration = 22,
	kEnchanting = 23,
	kHealth = 24,
	kMagicka = 25,
	kStamina = 26,
	kHealRate = 27,
	kMagickaRate = 28,
	StaminaRate = 29,
	kSpeedMult = 30,
	kInventoryWeight = 31,
	kCarryWeight = 32,
	kCriticalChance = 33,
	kMeleeDamage = 34,
	kUnarmedDamage = 35,
	kMass = 36,
	kVoicePoints = 37,
	kVoiceRate = 38,
	kDamageResist = 39,
	kPoisonResist = 40,
	kResistFire = 41,
	kResistShock = 42,
	kResistFrost = 43,
	kResistMagic = 44,
	kResistDisease = 45,
	kUnknown46 = 46,
	kUnknown47 = 47,
	kUnknown48 = 48,
	kUnknown49 = 49,
	kUnknown50 = 50,
	kUnknown51 = 51,
	kUnknown52 = 52,
	kParalysis = 53,
	kInvisibility = 54,
	kNightEye = 55,
	kDetectLifeRange = 56,
	kWaterBreathing = 57,
	kWaterWalking = 58,
	kUnknown59 = 59,
	kFame = 60,
	kInfamy = 61,
	kJumpingBonus = 62,
	kWardPower = 63,
	kRightItemCharge = 64,
	kArmorPerks = 65,
	kShieldPerks = 66,
	kWardDeflection = 67,
	kVariable01 = 68,
	kVariable02 = 69,
	kVariable03 = 70,
	kVariable04 = 71,
	kVariable05 = 72,
	kVariable06 = 73,
	kVariable07 = 74,
	kVariable08 = 75,
	kVariable09 = 76,
	kVariable10 = 77,
	kBowSpeedBonus = 78,
	kFavorActive = 79,
	kFavorsPerDay = 80,
	kFavorsPerDayTimer = 81,
	kLeftItemCharge = 82,
	kAbsorbChance = 83,
	kBlindness = 84,
	kWeaponSpeedMult = 85,
	kShoutRecoveryMult = 86,
	kBowStaggerBonus = 87,
	kTelekinesis = 88,
	kFavorPointsBonus = 89,
	kLastBribedIntimidated = 90,
	kLastFlattered = 91,
	kMovementNoiseMult = 92,
	kBypassVendorStolenCheck = 93,
	kBypassVendorKeywordCheck = 94,
	kWaitingForPlayer = 95,
	kOneHandedModifier = 96,
	kTwoHandedModifier = 97,
	kMarksmanModifier = 98,
	kBlockModifier = 99,
	kSmithingModifier = 100,
	kHeavyArmorModifier = 101,
	kLightArmorModifier = 102,
	kPickpocketModifier = 103,
	kLockpickingModifier = 104,
	kSneakingModifier = 105,
	kAlchemyModifier = 106,
	kSpeechcraftModifier = 107,
	kAlterationModifier = 108,
	kConjurationModifier = 109,
	kDestructionModifier = 110,
	kIllusionModifier = 111,
	kRestorationModifier = 112,
	kEnchantingModifier = 113,
	kOneHandedSkillAdvance = 114,
	kTwoHandedSkillAdvance = 115,
	kMarksmanSkillAdvance = 116,
	kBlockSkillAdvance = 117,
	kSmithingSkillAdvance = 118,
	kHeavyArmorSkillAdvance = 119,
	kLightArmorSkillAdvance = 120,
	kPickpocketSkillAdvance = 121,
	kLockpickingSkillAdvance = 122,
	kSneakingSkillAdvance = 123,
	kAlchemySkillAdvance = 124,
	kSpeechcraftSkillAdvance = 125,
	kAlterationSkillAdvance = 126,
	kConjurationSkillAdvance = 127,
	kDestructionSkillAdvance = 128,
	kIllusionSkillAdvance = 129,
	kRestorationSkillAdvance = 130,
	kEnchantingSkillAdvance = 131,
	kLeftWeaponSpeedMultiply = 132,
	kDragonSouls = 133,
	kCombatHealthRegenMultiply = 134,
	kOneHandedPowerModifier = 135,
	kTwoHandedPowerModifier = 136,
	kMarksmanPowerModifier = 137,
	kBlockPowerModifier = 138,
	kSmithingPowerModifier = 139,
	kHeavyArmorPowerModifier = 140,
	kLightArmorPowerModifier = 141,
	kPickpocketPowerModifier = 142,
	kLockpickingPowerModifier = 143,
	kSneakingPowerModifier = 144,
	kAlchemyPowerModifier = 145,
	kSpeechcraftPowerModifier = 146,
	kAlterationPowerModifier = 147,
	kConjurationPowerModifier = 148,
	kDestructionPowerModifier = 149,
	kIllusionPowerModifier = 150,
	kRestorationPowerModifier = 151,
	kEnchantingPowerModifier = 152,
	kDragonRend = 153,
	kAttackDamageMult = 154,
	kHealRateMult = 155,
	kMagickaRateMult = 156,
	StaminaRateMult = 157,
	kWerewolfPerks = 158,
	kVampirePerks = 159,
	kGrabActorOffset = 160,
	kGrabbed = 161,
	kUnknown162 = 162,
	kReflectDamage = 163,

	kTotal
};
