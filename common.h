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
// SSE 1.5.73

#define ONPROJECTILEHIT_HOOKLOCATION							0x0074CB0A
#define ONPROJECTILEHIT_INNERFUNCTION							0x007531C0

#define PROJECTILE_GETACTORCAUSEFN								0x0074DCF0 // ??_7Projectile@@6B@			vtbl[51]

#define DAMAGEACTORVALUE_FN										0x0094A4C0
#define PUSHACTORAWAY_FN										0x00996340
#define DEBUGNOTIFICATION_FN									0x0096DDB0

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


#define DEFAULT_IMPACTEFFECT_FORMID	0x105F37  // staff of magnus
#define DEFAULT_SOUNDEFFECT_FORMID 0x3EDDA // ImpactAxeLargeVSFlesh