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

#else
// SSE 1.5.73

#define ONPROJECTILEHIT_HOOKLOCATION							0x0074CB0A
#define ONPROJECTILEHIT_INNERFUNCTION							0x007531C0

#define PROJECTILE_GETACTORCAUSEFN								0x0074DCF0 // ??_7Projectile@@6B@			vtbl[51]

#define DAMAGEACTORVALUE_FN										0x0094A4C0
#define PUSHACTORAWAY_FN										0x00996340
#define DEBUGNOTIFICATION_FN									0x0096DDB0

#endif

