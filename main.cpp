﻿//#include <SKSE.h>
#include <skse64/PluginAPI.h>
//#include <skse64/DebugLog.h>
#include <skse64/NiNodes.h>
#include <skse64/GameCamera.h>
#include <skse64/GameTypes.h>
#include <skse64/PapyrusEvents.h>
#include <skse64/PapyrusVM.h>
#include <skse64/GameSettings.h>
#include <skse64/GameReferences.h>
#include <skse64/InternalTasks.h>

#include "skse64_common/skse_version.h"
#include "skse64_common/Relocation.h"
#include "skse64_common/SafeWrite.h"
#include "skse64_common/Utilities.h"

#include "libskyrim\BGSAttackData.h"

// Papyrus VR / SkyrimVRTools includes
#include "api/PapyrusVRTypes.h"
#include "api/OpenVRTypes.h"
#include "api/openvr.h"
#include "api/VRHookAPI.h"
#include "api/PapyrusVRAPI.h"

#include <random>

#include <shlobj.h>
#include <cinttypes>
#define XBYAK_NO_OP_NAMES // How do other plugins actually define this..?

#include <xbyak/xbyak.h>
#include "skse64_common/BranchTrampoline.h"

#include "iniSettings.h"
#include "damagetracker.h"
#include "throwtracker.h"
#include "common.h"
#include "timer.h"
#include "skse64/GameData.h"



namespace papyrusActor
{
	// call "DamageActorValue" script func directly by address
	typedef void(*_DamageActorValue)(VMClassRegistry* VMinternal, UInt32 stackId, Actor * thisActor, BSFixedString const &dmgValueName, float dmg);
	RelocAddr<_DamageActorValue> DamageActorValue(DAMAGEACTORVALUE_FN);

	// decapitate actor
	typedef void (*_Decapitate)(Actor* thisActor);
	RelocAddr<_Decapitate> Decapitate(DECAPITATEACTOR_FN);
}

namespace papyrusStatic
{
	// This function seems to only use paramter 4 (string) anyway
	typedef void(*_DebugNotification)(VMClassRegistry* VMinternal, UInt32 stackId, void* unk1, BSFixedString const &debugMsg);
	RelocAddr<_DebugNotification> DebugNotifcation(DEBUGNOTIFICATION_FN);
}

namespace papyrusSound
{
	typedef void(*_PlaySound)(VMClassRegistry* VMinternal, UInt32 stackId, TESSound* sound, TESObjectREFR* source);
	RelocAddr<_PlaySound> Play(PLAYSOUND_FN);
}

namespace papyrusObjRef
{
	typedef bool (*_PlayImpactEffect)(VMClassRegistry* VMinternal, UInt32 stackId, TESObjectREFR* obj, BGSImpactDataSet* impactData, BSFixedString const &asNodeName, float afPickDirX, float afPickDirY,
		float afPickDirZ, float afPickLength, bool abApplyNodeRotation, bool abUseNodeLocalRotation);
	RelocAddr<_PlayImpactEffect> PlayImpactEffect(PLAYIMPACTEFFECT_FN);
}


typedef int64_t(*_OnProjectileHitFunction)(Projectile* akProjectile, TESObjectREFR* akTarget, NiPoint3* point,
	uintptr_t unk1, UInt32 unk2, UInt8 unk3);
RelocAddr<_OnProjectileHitFunction> OnProjectileHitFunction(ONPROJECTILEHIT_INNERFUNCTION);
RelocAddr<uintptr_t> OnProjectileHitHookLocation(ONPROJECTILEHIT_HOOKLOCATION);

typedef UInt32*(*_GetActorCause)(TESObjectREFR* refr);
RelocAddr<_GetActorCause> Projectile_GetActorCauseFn(PROJECTILE_GETACTORCAUSEFN);

class SKSEPlayerActionEvent : public BSTEventSink <SKSEActionEvent>
{
public:
	virtual	EventResult ReceiveEvent(SKSEActionEvent * evn, EventDispatcher<SKSEActionEvent> * dispatcher);
};


// globals
SKSEPapyrusInterface* g_papyrus = nullptr;
SKSEMessagingInterface* g_messaging = nullptr;
#ifdef SKYRIMVR
SKSETrampolineInterface* g_trampolineInterface = nullptr;
#endif

PluginHandle g_pluginHandle = kPluginHandle_Invalid;
PapyrusVRAPI*	g_papyrusvr = nullptr;
SKSETaskInterface* g_task = nullptr;

EventDispatcher<SKSEActionEvent>* g_skseActionEventDispatcher;
SKSEPlayerActionEvent	g_PlayerActionEvent;
CDamageTracker			g_DamageTracker;
CThrowTracker			g_ThrowTracker; // WeaponThrowVR support - track thrown weapons
CTimer					g_Timer;
double					g_LastSpellTime = 0.0;
int						g_IsLeftHandMode = 0;

typedef SpellItem EquippedSpellObject;

int64_t OnProjectileHitFunctionHooked(Projectile* akProjectile, TESObjectREFR* akTarget, NiPoint3* point, uintptr_t unk1,
	UInt32 unk2, UInt8 unk3);

void OnVRButtonEvent(PapyrusVR::VREventType type, PapyrusVR::EVRButtonId buttonId, PapyrusVR::VRDevice deviceId);
void PlayTESSound(UInt32 formID);

struct DoAddHook_Code : Xbyak::CodeGenerator
{
	DoAddHook_Code(void* buf, uintptr_t hook_OnProjectileHit) : CodeGenerator(4096, buf)
	{
		Xbyak::Label retnLabel;
		Xbyak::Label funcLabel;

		//  .text:000000014074CFBA                 lea     r9, [r13 + 0Ch]; a4
		lea(r9, ptr[r13 + 0x0C]);
		// 	.text:000000014074CFBE                 mov[rsp + 180h + a6], 0; a6  (WARNING NOTE: Stack Offsets changed to 0x28, 0x20 fix Impact VFX bug in VR but this may be totally wrong in SE!!)
		mov(byte[rsp + 0x28], 0);
		// 	.text:000000014074CFC3                 mov[rsp + 180h + a5], ecx; a5
		mov(dword[rsp + 0x20], ecx);
		// 	.text:000000014074CFC7                 mov     r8, r13; a3
		mov(r8, r13);
		// 	.text:000000014074CFCA                 mov     rdx, rbx; a2
		mov(rdx, rbx);
		// 	.text:000000014074CFCD                 mov     rcx, r14; a1
		mov(rcx, r14);
		// 	.text:000000014074CFD0                 call    sub_140753670
		// int64_t OnProjectileHitFunctionHooked(Projectile * akProjectile, TESObjectREFR * akTarget, NiPoint3 * point, UInt32 unk1, UInt32 unk2, UInt8 unk3)
		call(ptr[rip + funcLabel]);
		// 0x1B
		//  .text:000000014074CFD5                 lea     r8, [r14+0F0h]			
		// exit 74CFD5
		jmp(ptr[rip + retnLabel]);

		L(funcLabel);
		dq(hook_OnProjectileHit);

		L(retnLabel);
		dq(OnProjectileHitHookLocation.GetUIntPtr() + 0x1B);
	}
};


class TaskPlayImpactVFX : public TaskDelegate
{
public:
	virtual void Run() override;
	virtual void Dispose() override;

	TaskPlayImpactVFX(UInt32 formId, Actor* actor, const BSFixedString& nodeName);

private:
	UInt32 mFormId = 0;
	Actor* mActor = nullptr;
	BSFixedString mNodeName;
};

class TaskPlaySound : public TaskDelegate
{
public:
	virtual void Run() override;
	virtual void Dispose() override;

	TaskPlaySound(UInt32 formId, Actor* actor);

private:
	UInt32 mFormId = 0;
	Actor* mActor = nullptr;
};

//Listener for PapyrusVR Messages
void OnPapyrusVRMessage(SKSEMessagingInterface::Message* msg)
{
	if (msg)
	{
		if (msg->type == kPapyrusVR_Message_Init && msg->data)
		{
			_MESSAGE("PapyrusVR Init Message received with valid data, waiting for init.");
			g_papyrusvr = (PapyrusVRAPI*)msg->data;

		}
	}
}

void SKSEMessageHandler(SKSEMessagingInterface::Message* msg)
{
	static bool bVRToolsListenerValid = false;
	static const size_t TRAMPOLINE_SIZE = 256;

	switch (msg->type)
	{
		case SKSEMessagingInterface::kMessage_PostLoad:
		{
			_MESSAGE("SKSE PostLoad message received, registering for PapyrusVR messages from SkyrimVRTools");  // This log msg may happen before XML is loaded
			bVRToolsListenerValid = g_messaging->RegisterListener(g_pluginHandle, "SkyrimVRTools", OnPapyrusVRMessage);
			break;
		}
		case SKSEMessagingInterface::kMessage_DataLoaded:
		{
			_MESSAGE("SKSE Message: Data Loaded");


			ini.Load();

#ifdef SKYRIMVR
			// NEW SKSEVR feature: trampoline interface object from QueryInterface() - Use SKSE existing process code memory pool - allow Skyrim to run without ASLR
			if (g_trampolineInterface && ini.UseSKSETrampolineInterface) 
			{
				void* branch = g_trampolineInterface->AllocateFromBranchPool(g_pluginHandle, TRAMPOLINE_SIZE);
				if (!branch) {
					_ERROR("couldn't acquire branch trampoline from SKSE. this is fatal. skipping remainder of init process.");
					return;
				}

				g_branchTrampoline.SetBase(TRAMPOLINE_SIZE, branch);

				void* local = g_trampolineInterface->AllocateFromLocalPool(g_pluginHandle, TRAMPOLINE_SIZE);
				if (!local) {
					_ERROR("couldn't acquire codegen buffer from SKSE. this is fatal. skipping remainder of init process.");
					return;
				}

				g_localTrampoline.SetBase(TRAMPOLINE_SIZE, local);

				_MESSAGE("Using new SKSEVR trampoline interface memory pool alloc for codegen buffers.");
			}
			else  // otherwise if using an older SKSEVR version, fall back to old code
#endif
			{

				if (!g_branchTrampoline.Create(TRAMPOLINE_SIZE))  // don't need such large buffers
				{
					_FATALERROR("[ERROR] couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
					return;
				}

				if (!g_localTrampoline.Create(TRAMPOLINE_SIZE, nullptr))
				{
					_FATALERROR("[ERROR] couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
					return;
				}

				_MESSAGE("Using legacy SKSE trampoline creation.");
			}

			void* codeBuf = g_localTrampoline.StartAlloc();
			DoAddHook_Code code(codeBuf, (uintptr_t)OnProjectileHitFunctionHooked);
			g_localTrampoline.EndAlloc(code.getCurr());

			g_branchTrampoline.Write6Branch(OnProjectileHitHookLocation.GetUIntPtr(), uintptr_t(code.getCode()));

			_MESSAGE("Code hooked successfully!");

			Setting* iniLeftHandSetting = GetINISetting("bLeftHandedMode:VRInput");
			if (iniLeftHandSetting)
			{
				g_IsLeftHandMode = (int)iniLeftHandSetting->data.u8;
			}

			if (bVRToolsListenerValid && g_papyrusvr)
			{
				g_papyrusvr->GetVRManager()->RegisterVRButtonListener(OnVRButtonEvent);
				_MESSAGE("Registering PapyrusVR OnVRButtonEvent with SkyrimVRTools.");
			}
			else
			{
				// Player action event listener was too unreliable to be useful...
			/*
				void * dispatchPtr = g_messaging->GetEventDispatcher(SKSEMessagingInterface::kDispatcher_ActionEvent);
				g_skseActionEventDispatcher = (EventDispatcher<SKSEActionEvent>*)dispatchPtr;

				g_skseActionEventDispatcher->AddEventSink(&g_PlayerActionEvent);

				_MESSAGE("Registering PlayerActionEvent listener since SkyrimVRTools is not available.");
			*/
				_MESSAGE("SkyrimVRTools not found.  Disabling accurate damage tracking for spellcasting. ");
			}

			g_DamageTracker.Init();

			break;
		}
		case SKSEMessagingInterface::kMessage_InputLoaded:
		{
			_MESSAGE("SKSE Message: Input Loaded");
			break;
		}
		case SKSEMessagingInterface::kMessage_PostLoadGame:
		{
			if (msg->data != nullptr)
			{
				g_ThrowTracker.Initialize();
			}
			break;
		}
	}
}


extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
	{

#ifdef SKYRIMVR
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, R"(\My Games\Skyrim VR\SKSE\LocationalDamageVR.log)");
#else
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\LocationalDamage64.log");
#endif
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->version = 0x72;

#ifdef SKYRIMVR
		_MESSAGE("LocationalDamageSKSEVR Plugin Version %x", info->version);
		info->name = "LocationalDamageSKSEVR Plugin";
#else
		info->name = "LocationalDamageSKSE64 Plugin";
		_MESSAGE("LocationalDamageSKSE64 Plugin Version %x", info->version);
#endif

		g_pluginHandle = skse->GetPluginHandle();

		if (skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");
			return false;
		}
#ifdef SKYRIMVR
		else if (skse->runtimeVersion != RUNTIME_VR_VERSION_1_4_15)
#else
		else if (skse->runtimeVersion != RUNTIME_VERSION_1_5_97)
#endif
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);
			return false;
		}

		g_messaging = static_cast<SKSEMessagingInterface *>(skse->QueryInterface((kInterface_Messaging)));
		if (!g_messaging)
		{
			_FATALERROR("[ERROR] couldn't get messaging interface");
			return false;
		}

#ifdef SKYRIMVR
		g_trampolineInterface = static_cast<SKSETrampolineInterface*>(skse->QueryInterface(kInterface_Trampoline));
		if (!g_trampolineInterface)
		{
			_MESSAGE("WARNING: Could not get new trampoline alloc interface, Using legacy SKSE VR");
		}
#endif

		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse)
	{
		bool res = g_messaging->RegisterListener(g_pluginHandle, "SKSE", SKSEMessageHandler);
		if (!res)
		{
			_MESSAGE("Failed to register SKSE Message handler.");
		}

		// get the task interface
		g_task = (SKSETaskInterface*)skse->QueryInterface(kInterface_Task);
		if (!g_task)
		{
			_FATALERROR("couldn't get task interface");
			return false;
		}

		return true;
	}
};

// Mappings of mesh physics assets to collision shapes
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
{ "actors\\dragon\\dragonproject.hkx",{ { "NPC Jaw", 60 } } },
{ "actors\\canine\\dogproject.hkx",{ { "Canine_JawBone", 20 },{ "Canine_RBackLeg2", 15 },{ "Canine_LBackLeg2", 15 },{ "Canine_RFrontLeg2", 15 },{ "Canine_LFrontLeg2", 15 },{ "Canine_Ribcage", 20 } } },
{ "actors\\canine\\wolfproject.hkx",{ { "Canine_JawBone", 20 },{ "Canine_RBackLeg2", 15 },{ "Canine_LBackLeg2", 15 },{ "Canine_RFrontLeg2", 15 },{ "Canine_LFrontLeg2", 15 },{ "Canine_Ribcage", 20 } } },
{ "actors\\falmer\\falmerproject.hkx",{ { "NPC Head", 20 },{ "NPC R Calf", 20 },{ "NPC L Calf", 20 },{ "NPC R Forearm", 20 },{ "NPC L Forearm", 20 },{ "NPC Spine2", 20 } } },
{ "actors\\dlc02\\riekling\\rieklingproject.hkx",{ { "NPC Head", 20 },{ "NPC R Calf", 15 },{ "NPC L Calf", 15 },{ "NPC R Forearm", 15 },{ "NPC L Forearm", 15 },{ "NPC Spine2", 20 } } },
{ "actors\\bear\\bearproject.hkx",{ { "NPC Jaw", 25 },{ "NPC RLeg2", 20 },{ "NPC LLeg2", 20 },{ "NPC RArm2", 20 },{ "NPC LArm2", 20 },{ "NPC Ribcage", 25 } } },
{ "actors\\chaurus\\chaurusproject.hkx",{ { "NPC Head [Head]", 25 },{ "R Leg[RClf]", 20 },{ "LLeg[LClf]", 20 },{ "R UpperArm[RLar]", 20 },{ "L UpperArm[LLar]", 20 },{ "Ribcage[Spn3]", 20 } } },
{ "actors\\deer\\deerproject.hkx",{ { "ElkJaw", 20 },{ "ElkRTibia", 15 },{ "ElkLTibia", 15 },{ "ElkRRadius", 15 },{ "ElkLRadius", 15 },{ "ElkRibcage", 20 } } },
{ "actors\\dragonpriest\\dragon_priest.hkx",{ { "DragPriestNPC Head [Head]", 25 },{ "DragPriestNPC R Calf [RClf]", 20 },{ "DragPriestNPC L Calf [LClf]", 20 },{ "DragPriestNPC R Forearm [RLar]", 20 },{ "DragPriestNPC L Forearm [LLar]", 20 },{ "DragPriestNPC Spine2 [Spn2]", 20 } } },
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

void fnDebug_Notification(const char* str, bool flag1, bool flag2)
{
	papyrusStatic::DebugNotifcation((*g_skyrimVM)->GetClassRegistry(), 0, nullptr, str);
	OutputDebugStringA(str);
}

static bool Probability(float value)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_real_distribution<> score(0.0, 100.0);

	if (score(mt) < value)
		return true;

	return false;
}


struct FoundEquipArmor
{
	TESObjectARMO* pArmor;
	BaseExtraList* pExtraData;

	FoundEquipArmor() : pArmor(nullptr), pExtraData(nullptr)
	{
	}
};

// Loop through all equipped armors and find one that matches the slot mask given - relevant API code changed a lot in SKSE64
static FoundEquipArmor GetEquippedArmorEx(Actor* actor, unsigned int slotMask)
{
	FoundEquipArmor equipArmor;
	ExtraContainerChanges* exChanges = static_cast<ExtraContainerChanges*>(actor->extraData.GetByType(kExtraData_ContainerChanges));

	if(exChanges && exChanges->data && exChanges->data->objList)
	{
		for (auto it = exChanges->data->objList->Begin(); !it.End(); ++it)
		{
			InventoryEntryData* pEntry = it.Get();

			if (!pEntry || !pEntry->type || pEntry->type->formType != FormType::kFormType_Armor || !pEntry->extendDataList)
				continue;

			TESObjectARMO* armor = static_cast<TESObjectARMO*>(pEntry->type);
			
			// TODO: fix slot mask filter // original def:  bool	HasPartOf(UInt32 flag) const	{ return (bipedObjectData.parts & flag) != 0; }
			if ((armor->bipedObject.data.parts & slotMask) == 0)
				continue;

			for (auto extraIt = pEntry->extendDataList->Begin(); !extraIt.End(); ++extraIt)
			{

				BaseExtraList* pExtraDataList = extraIt.Get();
				if (pExtraDataList && (pExtraDataList->HasType(ExtraDataType::kExtraData_Worn) || pExtraDataList->HasType(ExtraDataType::kExtraData_WornLeft)))
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

static SpellItem* GetCorrectSpellBySlot(Actor* player, int slot)
{
	if (g_IsLeftHandMode)
	{
		if (slot == SKSEActionEvent::kSlot_Right) // swap things up for left handed mode
		{
			return player->leftHandSpell;
		}

		return player->rightHandSpell;
	}
	else
	{
		if (slot == SKSEActionEvent::kSlot_Left)
		{
			return player->leftHandSpell;
		}

		return player->rightHandSpell;
	}
}


static float GetLocationalDamage(Actor* actor, BGSAttackData* attackData, TESObjectWEAP* weapon, EquippedSpellObject* spell, Projectile* projectile, Actor* caster_actor, TESObjectARMO* armor, MultiplierType multiplierType, const CDamageEntry* dmgEntry)
{
	float damage = 0.0f;
	bool isSpell = false;

	if (ini.DamageFollowers == false && (actor->flags1 & (UInt32)eActorFlags::kPlayerTeammate))
	{
		return 0.0f;
	}

	// add skill based damage to weapon bonus dmg (I'm pretty sure at least)
	auto AddWeaponSkillDamage = [&](UInt8 weaponType)
	{
		if (caster_actor)
		{
			const float inv100 = 1.0f / 100.f;

			switch (weaponType)
			{
			case TESObjectWEAP::GameData::kType_HandToHandMelee:
				damage = caster_actor->actorValueOwner.GetCurrent(35); //caster_actor->GetActorValueCurrent(35);
				break;
			case TESObjectWEAP::GameData::kType_OneHandSword:
			case TESObjectWEAP::GameData::kType_OneHandDagger:
			case TESObjectWEAP::GameData::kType_OneHandAxe:
			case TESObjectWEAP::GameData::kType_OneHandMace:
				damage *= 1.0 + caster_actor->actorValueOwner.GetCurrent(6) * 0.5f * inv100; //caster_actor->GetActorValueCurrent(6) * 0.5 / 100.0;
				break;
			case TESObjectWEAP::GameData::kType_TwoHandSword:
			case TESObjectWEAP::GameData::kType_TwoHandAxe:
				damage *= 1.0 + caster_actor->actorValueOwner.GetCurrent(7) * 0.5 * inv100;
				break;
			case TESObjectWEAP::GameData::kType_Bow:
			case TESObjectWEAP::GameData::kType_CrossBow:
				damage *= 1.0 + caster_actor->actorValueOwner.GetCurrent(8) * 0.5 * inv100;
				break;
			default:
				break;
			}
		}
	};


	// if we have a damage entry, get tracked stats from when the attack was launched (new feature)
	// this should fix a few issues where the user uses 2 destruction spells and increase compatibility with FEC / Spellsiphon potentially
	if (dmgEntry)
	{
		damage = dmgEntry->mDamage;

		if (dmgEntry->mIsSpell)
		{
			damage = damage * ini.SpellDamageMultiplier;
			isSpell = true;
		}
		else
		{
			AddWeaponSkillDamage(dmgEntry->mWeaponType);
		}
	}
	else if (weapon && (!attackData || !attackData->flags.ignoreWeapon))
	{
		damage = weapon->damage.GetAttackDamage(); // weapon->damage.attackDamage  might be safer
		AddWeaponSkillDamage(weapon->type());
	}
	else if (spell) // try to get spell damage
	{
		const char* dmgKeyword = nullptr;
		MagicItem::EffectItem* effectItem = GetDamageEffectForSpell(spell, &dmgKeyword);
		
		// important to check this for null (for NPCs?)
		// additional safety check (some special case weapons like Dawnbreaker seem to have spell effects attached to them..?)
		if (effectItem && effectItem->mgef)
		{ 
			damage = g_DamageTracker.GetSpellDamageBonus(spell, effectItem, caster_actor, dmgKeyword);
			damage = damage * ini.SpellDamageMultiplier;

			isSpell = true;
		}
	}

	if (attackData && !isSpell)
	{
		damage *= attackData->damageMult;
	}

	if (armor && !isSpell) // only apply armor dmg reduction if this is not a spell attack
	{
		
		// original definitions:
		//bool	IsHeavyArmor() const { return bipedObjectData.weightClass == kWeight_Heavy; }
		//bool	IsLightArmor() const { return bipedObjectData.weightClass == kWeight_Light; }

		// different dmg multipliers depending on armor weight class
		if (armor->bipedObject.data.weightClass == BGSBipedObjectForm::kWeight_Heavy)
		{
			damage *= ini.HeavyArmorDamageMultiplier;
		}
		else if (armor->bipedObject.data.weightClass == BGSBipedObjectForm::kWeight_Light)
		{
			damage *= ini.LightArmorDamageMultiplier;
		}
	}

	// subtract offset from damage (should be -1 on mult) since in this version of the plugin, the normal base damage is still applied 

	const float dmgMultOffset = 1.0;

	if (actor == *g_thePlayer)
		damage *= ( std::max<float>(ini.NPCToPlayer[multiplierType], 1.0) - dmgMultOffset);
	else if (caster_actor == *g_thePlayer)
		damage *= (std::max<float>(ini.PlayerToNPC[multiplierType], 1.0) - dmgMultOffset);
	else
		damage *= (std::max<float>(ini.NPCToNPC[multiplierType], 1.0) - dmgMultOffset);

	return (float)-damage;
}

static float GetLocationalEffectChance(Actor* actor, BGSAttackData* attackData, TESObjectWEAP* weapon, Actor* caster_actor, TESObjectARMO* armor, MultiplierType multiplierType)
{
	const float inv30 = 1.0f / 30.0f;
	float chance = 1.0;

	if (weapon && (!attackData || !attackData->flags.ignoreWeapon))
	{
		chance *= 1.0 + weapon->weight.weight * inv30;
	}

	if (caster_actor)
	{
		// NOTE: This seems to be a bug in the orignal ver (max - max)
		//float difference = (caster_actor->GetActorValueMaximum(24) - actor->GetActorValueMaximum(24)) * 0.00014;
		float difference = caster_actor->actorValueOwner.GetMaximum(24) - caster_actor->actorValueOwner.GetBase(24) * 0.00014;
		if (difference < -0.7f)
			difference = -0.7f;
		else if (difference > 0.7f)
			difference = 0.7f;
		chance *= 1.0f + difference;
	}

	if (armor)
	{
		if (armor->bipedObject.data.weightClass == BGSBipedObjectForm::kWeight_Heavy)
		{
			chance *= ini.HeavyArmorEffectChanceMultiplier;
		}
		else if (armor->bipedObject.data.weightClass == BGSBipedObjectForm::kWeight_Light)
		{
			chance *= ini.LightArmorEffectChanceMultiplier;
		}
	}

	if (actor == *g_thePlayer)
		chance *= (ini.NPCToPlayer[multiplierType]);
	else if (caster_actor == *g_thePlayer)
		chance *= ini.PlayerToNPC[multiplierType];
	else
		chance *= ini.NPCToNPC[multiplierType];

	return chance;
}

static void ApplyLocationalDamage(Actor* actor, UInt32 damageType, float dmg, Actor* akAggressor, Projectile* proj)
{
	if (dmg >= 0.0f || actor == nullptr)
		return;

	papyrusActor::DamageActorValue((*g_skyrimVM)->GetClassRegistry(), 0, actor, "Health", dmg);

	// do decapitation if locational damage will kill actor
	const float hp = actor->actorValueOwner.GetCurrent( (UInt32)eActorValue::kHealth );
	if (hp <= 0.0f)
	{
		auto GetActorName = [](Actor* actor)->const char*
		{
			const char* name = CALL_MEMBER_FN(actor, GetReferenceName)();
			return name != nullptr ? name : "NULL";
		};

		// avoid decapitations for all children / essential / protected actors
		if ((actor->race->data.raceFlags & TESRace::kRace_Child))
		{
			return;
		}

		if ((actor->flags2 & eActorFlags2::kEssential) != 0 || (actor->flags2 & eActorFlags2::kProtected) != 0) // essential or protected actor
		{
			return;
		}

		// check random chance of decapitation
		if (proj->formType == kFormType_Arrow) // for all weapons (this includes thrown weapons)
		{
			if (ini.WeaponDecapitationChance < 100 && (ini.WeaponDecapitationChance <= 0 || (rand() % 100) < ini.WeaponDecapitationChance))
			{
				return;
			}
		}
		else // for spells
		{
			if (ini.SpellDecapitationChance < 100 && (ini.SpellDecapitationChance <= 0 || (rand() % 100) < ini.SpellDecapitationChance))
			{
				return;
			}
		}

		_MESSAGE("Trying to decapitate actor %s with health %f", GetActorName(actor), hp);
		papyrusActor::Decapitate(actor);
	}
}

static void ApplyLocationalEffect(Actor* actor, UInt32 effectType, float chance, FoundEquipArmor equipArmor, std::string pathString)
{
	if (chance <= 0.0f)
		return;

	// TODO: implement later

	/*
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
	*/
}


int64_t OnProjectileHitFunctionHooked(Projectile* akProjectile, TESObjectREFR* akTarget, NiPoint3* point, uintptr_t unk1,
	UInt32 unk2, UInt8 unk3)
{
	if (akProjectile != nullptr && akTarget != nullptr)
	{
		NiPoint3* hit_pos = point;
		TESObjectREFR* target = akTarget; //(TESObjectREFR*)stack[10];

		Actor* actor = DYNAMIC_CAST(target, TESObjectREFR, Actor);

		if (!actor)
			return OnProjectileHitFunction(akProjectile, akTarget, point, unk1, unk2, unk3);

		// additional checks.  IsDead should be fixed now in new SKSE. Not sure about IsInKillMove
		const bool isAlive = !actor->IsDead(true);

		TESRace* race = actor->race;
		if (!race || !isAlive)
			return OnProjectileHitFunction(akProjectile, akTarget, point, unk1, unk2, unk3);;

		NiNode *node = actor->GetNiNode();
		if (actor == *g_thePlayer && (*g_thePlayer)->loadedState)
		{
			PlayerCamera* camera = PlayerCamera::GetSingleton();
#ifdef SKYRIMVR
			node = (*g_thePlayer)->firstPersonSkeleton; 
#else
			node = camera && camera->cameraState == camera->cameraStates[PlayerCamera::kCameraState_FirstPerson] ? (*g_thePlayer)->firstPersonSkeleton : (*g_thePlayer)->loadedState->node;
#endif
		}

		g_Timer.TimerUpdate(); // need to update this every relevant frame to setup GetLastTime()

		// Stop processing if a spell was cast AND scored bonus damage in the last 1 second
		bool spellBlockedByTimer = false;
		if (akProjectile && akProjectile->formType != kFormType_Arrow)
		{
			const double kSpellTimeout = ini.SpellTimeout;
			if (g_Timer.GetLastTime() - g_LastSpellTime < kSpellTimeout)
			{
				spellBlockedByTimer = true;
			}

		}

		if (!node || spellBlockedByTimer)
		{
			//_MESSAGE("node == NULL OR Spell processing blocked by timer. remaining time: %f", g_Timer.GetLastTime() - g_LastSpellTime);
			return OnProjectileHitFunction(akProjectile, akTarget, point, unk1, unk2, unk3);
		}

		int gender = 0;
		TESNPC* base = DYNAMIC_CAST(actor->baseForm, TESForm, TESNPC);
		if (base && CALL_MEMBER_FN(base, GetSex)())
			gender = 1;

		std::string pathString = std::string(race->behaviorGraph[gender].name);
		ini.ToLower(pathString);
		static std::vector<Pair> defaultNodeNames = { { "NPC Head [Head]", 20 },{ "NPC R Calf [RClf]", 20 },{ "NPC L Calf [LClf]", 20 },{ "NPC R Forearm [RLar]", 20 },{ "NPC L Forearm [LLar]", 20 } };
		std::vector<Pair> nodeNames = locationalNodeMap.count(pathString) >= 1 ? locationalNodeMap.at(pathString) : defaultNodeNames;

		if (nodeNames.empty())
			return OnProjectileHitFunction(akProjectile, akTarget, point, unk1, unk2, unk3);

		// Original code from Skyrim LE referencing stack commented for reference
		BGSAttackData* attackData = nullptr;  //(BGSAttackData*)((UInt32*)stack[9])[9];
		TESObjectWEAP* weapon = nullptr; //(TESObjectWEAP*)((UInt32*)stack[9])[10];
		Projectile* projectile = akProjectile; //(Projectile*)((UInt32*)stack[9])[32];
		TESObjectREFR* caster = nullptr; //(TESObjectREFR*)((UInt32*)stack[9])[35];
		EquippedSpellObject* spell = nullptr; // NEW: Try to track equipped spell used to shoot projectile
		CDamageEntry* dmgEntry = nullptr;

		int hitNode = -1;
		BSFixedString hitNodeName;
		const float scale = CALL_MEMBER_FN(actor, GetBaseScale)(); //actor->GetScale();
		for (size_t i = 0; i < nodeNames.size(); i++)
		{
			const char* nodeNameKey = (const char*)nodeNames[i].first;
			NiAVObject *obj = node->GetObjectByName(&nodeNameKey);  // arg type of this changed... is it safe?
			if (obj)
			{
				NiPoint3 node_pos = obj->m_worldTransform.pos;

				const float dx = hit_pos->x - node_pos.x;
				const float dy = hit_pos->y - node_pos.y;
				const float dz = hit_pos->z - node_pos.z;
				const float d2 = dx * dx + dy * dy + dz * dz;

				if (d2 < nodeNames[i].second * scale * nodeNames[i].second * scale)
				{
					hitNode = i;
					hitNodeName = nodeNames[i].first;
					break;
				}
			}
		}

		if (hitNode != -1)
		{
			TESObjectREFR* caster_ref = nullptr;

			if (projectile)
			{
				const bool ThrownWeaponLeft = g_ThrowTracker.IsThrownLeftHandProjectile(projectile);
				const bool ThrownWeaponRight = g_ThrowTracker.IsThrownRightHandProjectile(projectile);
				
				UInt32* handle = Projectile_GetActorCauseFn(projectile);
				

				NiPointer<TESObjectREFR> refCaster = nullptr;

				if (handle && *handle != *g_invalidRefHandle)
				{

					LookupREFRByHandle(*handle, refCaster);

					caster_ref = (TESObjectREFR*)refCaster;
				}
				else
				{
					// this line was pointless..
					//caster_ref = caster;
				}

				Actor* caster_actor = nullptr;
				if (caster_ref)
				{
					caster_actor = DYNAMIC_CAST(caster_ref, TESObjectREFR, Actor);
				}
				
				if(ThrownWeaponLeft || ThrownWeaponRight)
				{
					caster_actor = (*g_thePlayer);
				}

				// check if we have valid castor actor
				if (caster_actor)
				{
					// if caster_actor was the player, try to get damage entry (as long as it was not a thrown weapon)
					if (caster_actor == *g_thePlayer && !(ThrownWeaponLeft || ThrownWeaponRight))
					{
						dmgEntry = g_DamageTracker.LookupDamageEntry(projectile);
					}

					// IF this was an arrow projectile, try to get weapon
					if (ThrownWeaponLeft || ThrownWeaponRight)
					{
						//WeaponThrowVR support
						if(ThrownWeaponRight && g_ThrowTracker.GetRightThrowWeapon())
						{
							weapon = g_ThrowTracker.GetRightThrowWeapon();
						}
						else if(ThrownWeaponLeft && g_ThrowTracker.GetLeftThrowWeapon())
						{
							weapon = g_ThrowTracker.GetLeftThrowWeapon();
						}
					}
					else
					{
						if (akProjectile->formType == kFormType_Arrow)
						{
							TESForm* equippedForm = caster_actor->GetEquippedObject(false);
							weapon = DYNAMIC_CAST(equippedForm, TESForm, TESObjectWEAP);

							// try the other hand too
							if (!weapon)
							{
								equippedForm = caster_actor->GetEquippedObject(true);
								weapon = DYNAMIC_CAST(equippedForm, TESForm, TESObjectWEAP);
							}
						}
						else // otherwise this should be a spell cast
						{
							// at this point we just guess and hope we grab the right spell (TODO: this could be improved a lot)
							TESForm* equippedForm = caster_actor->GetEquippedObject(false);
							spell = DYNAMIC_CAST(equippedForm, TESForm, SpellItem);

							if (!spell || GetSpellDamage(spell) <= 0.0f) // if right hand was not a spell OR the spell damage was 0, try left hand
							{
								TESForm* equippedForm = caster_actor->GetEquippedObject(true);
								spell = DYNAMIC_CAST(equippedForm, TESForm, SpellItem);
							}
						}
					}
				}
				else
				{
					_DEBUGMSG("caster_actor was NULL.  Could not lookup or calculate damage!");
				}

				auto GetActorName = [](Actor* actor)->const char*
				{
					const char* name = CALL_MEMBER_FN(actor, GetReferenceName)();
					return name != nullptr ? name : "NULL";
				};

				auto GetProjectileType = [dmgEntry](Projectile* proj)->const char*
				{
					if (dmgEntry && dmgEntry->mKeyword != "")
					{
						return dmgEntry->mKeyword.c_str();
					}
					else
					{
						if (proj->formType == kFormType_Arrow)
						{
							return "Arrow";
						}
						else
						{
							return "Spell";
						}
					}
				};

				// TODO
				auto GetProjectileName = [](Projectile* proj)->const char*
				{
					return "";
				};

				// Print on screen / debug notifications about locational damage
				auto DoNotificationConditional = [&](const char* msgFront, const char* msgBack, float dmgVal)
				{
					dmgVal = fabsf(dmgVal); // dmg value is usually negative but we want to print the absolute val
					if ((ini.DisplayNotification || ini.LogNotification) && dmgVal >= (float)ini.DisplayNotificationMinDamage && (actor == *g_thePlayer || caster_actor == *g_thePlayer))
					{
						static char sMsgBuff[1024] = { 0 }; // static because I'm cautious about putting much on the stack of unknown size (I don't know what the game is doing with the stack)
						sprintf_s(sMsgBuff, "%s %s %s Damage: %f (%s)", msgFront, GetActorName(actor), msgBack, dmgVal, GetProjectileType(projectile));

						if (ini.DisplayNotification)
						{
							fnDebug_Notification(sMsgBuff, false, true);
						}
						if(ini.LogNotification)
						{
							_MESSAGE("[%.2f] %s", g_Timer.GetLastTime(), sMsgBuff);
						}
					}
				};


				bool done = false;
				FoundEquipArmor equipArmor;
				float locationalDmgVal = 0.0f;

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
						{
							locationalDmgVal = GetLocationalDamage(actor, attackData, weapon, spell, projectile, caster_actor, equipArmor.pArmor, Type_HeadDamageMultiplier, dmgEntry);
							ApplyLocationalDamage(actor, ini.DamageTypeHead, locationalDmgVal, caster_actor, projectile);
						}

						DoNotificationConditional(ini.HeadMessageFront.c_str(), ini.HeadMessageBack.c_str(), locationalDmgVal);
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
						{
							locationalDmgVal = GetLocationalDamage(actor, attackData, weapon, spell, projectile, caster_actor, equipArmor.pArmor, Type_FootDamageMultiplier, dmgEntry);
							ApplyLocationalDamage(actor, ini.DamageTypeFoot, locationalDmgVal, caster_actor, projectile);
						}

						DoNotificationConditional(ini.FootMessageFront.c_str(), ini.FootMessageBack.c_str(), locationalDmgVal);
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
						{
							locationalDmgVal = GetLocationalDamage(actor, attackData, weapon, spell, projectile, caster_actor, equipArmor.pArmor, Type_ArmsDamageMultiplier, dmgEntry);
							ApplyLocationalDamage(actor, ini.DamageTypeArms, locationalDmgVal, caster_actor, projectile);
						}

						DoNotificationConditional(ini.ArmsMessageFront.c_str(), ini.ArmsMessageBack.c_str(), locationalDmgVal);
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
						{
							locationalDmgVal = GetLocationalDamage(actor, attackData, weapon, spell, projectile, caster_actor, equipArmor.pArmor, Type_HeartDamageMultiplier, dmgEntry);
							ApplyLocationalDamage(actor, ini.DamageTypeHeart, locationalDmgVal, caster_actor, projectile);
						}
						
						DoNotificationConditional(ini.HeartMessageFront.c_str(), ini.HeartMessageBack.c_str(), locationalDmgVal);
						done = true;
					}
					break;
				}

				const float impactVFXDmgCutOff = 1.0f;
				if (done && fabsf(locationalDmgVal) > impactVFXDmgCutOff)
				{
					if (ini.DisplayImpactEffect)
					{
						g_task->AddTask(new TaskPlayImpactVFX(ini.ImpactEffectFormID, actor, hitNodeName));
					}

					if (ini.PlaySoundEffect)
					{
						UInt32 soundFormId = ini.SoundEffectFormID;
						Actor* actorToPlaySoundAt = actor;

						if (projectile->formType != kFormType_Arrow) // if it was a spell projectile, use spell sfx
						{
							soundFormId = ini.SoundEffectSpellFormID;
						}

						if (!ini.PlaySoundAtEnemyLocation || !actorToPlaySoundAt)
						{
							actorToPlaySoundAt = *g_thePlayer;
						}

						g_task->AddTask(new TaskPlaySound(soundFormId, actorToPlaySoundAt));
					}

					// track last time of spell bonus damage
					if (projectile->formType != kFormType_Arrow)
					{
						g_LastSpellTime = g_Timer.GetLastTime();
					}
				}				
			}
		}
	}
	
	return OnProjectileHitFunction(akProjectile, akTarget, point, unk1, unk2, unk3);
}


EventResult SKSEPlayerActionEvent::ReceiveEvent(SKSEActionEvent * evn, EventDispatcher<SKSEActionEvent> * dispatcher)
{
	auto player = DYNAMIC_CAST(LookupFormByID(0x14), TESForm, Actor);

	if (evn->actor == player)
	{

		if (evn->type == SKSEActionEvent::kType_SpellCast)
		{
			SpellItem* spell = GetCorrectSpellBySlot(player, evn->slot);
			if (spell)
			{
				g_DamageTracker.RegisterAttack(spell, player);
			}
		}
		else if (evn->type == SKSEActionEvent::kType_EndDraw)
		{
			TESForm* equippedForm = player->GetEquippedObject(false);
			TESObjectWEAP* weapon = DYNAMIC_CAST(equippedForm, TESForm, TESObjectWEAP);

			// try the other hand too
			if (!weapon)
			{
				equippedForm = player->GetEquippedObject(true);
				weapon = DYNAMIC_CAST(equippedForm, TESForm, TESObjectWEAP);
			}

			if (weapon)
			{
				g_DamageTracker.RegisterAttack(weapon);
			}
		}
	}

	return EventResult::kEvent_Continue;
}

// Legacy API event handler
void OnVRButtonEvent(PapyrusVR::VREventType type, PapyrusVR::EVRButtonId buttonId, PapyrusVR::VRDevice deviceId)
{
	// matches PapyrusVR::VRDevice enum
	static int deviceToSlotLookup[3] = {0, SKSEActionEvent::kSlot_Right, SKSEActionEvent::kSlot_Left};

	// Use button presses here
	if (buttonId == PapyrusVR::k_EButton_SteamVR_Trigger && (type == PapyrusVR::VREventType_Pressed || type == PapyrusVR::VREventType_Released))
	{
		auto player = DYNAMIC_CAST(LookupFormByID(0x14), TESForm, Actor);
		TESForm* equippedForm = player->GetEquippedObject(false);
		TESObjectWEAP* weapon = DYNAMIC_CAST(equippedForm, TESForm, TESObjectWEAP);

		// try the other hand too
		if (!weapon)
		{
			equippedForm = player->GetEquippedObject(true);
			weapon = DYNAMIC_CAST(equippedForm, TESForm, TESObjectWEAP);
		}

		if (weapon &&
			(weapon->type() == TESObjectWEAP::GameData::kType_Bow
				|| weapon->type() == TESObjectWEAP::GameData::kType_Bow2
				|| weapon->type() == TESObjectWEAP::GameData::kType_CBow
				|| weapon->type() == TESObjectWEAP::GameData::kType_CrossBow))
		{
			g_DamageTracker.RegisterAttack(weapon);
		}
		else
		{
			SpellItem* spell = GetCorrectSpellBySlot(player, deviceToSlotLookup[deviceId]);
			if (spell)
			{
				g_DamageTracker.RegisterAttack(spell, player);
			}
		}

	}

}

TaskPlayImpactVFX::TaskPlayImpactVFX(UInt32 formId, Actor* actor, const BSFixedString& nodeName)
{
	mFormId = formId;
	mActor = actor;
	mNodeName = nodeName;
}


void TaskPlayImpactVFX::Run()
{
	const UInt32 impactVFXFormID = mFormId;
	auto* impactForm = LookupFormByID(impactVFXFormID);
	if (impactForm)
	{
		BGSImpactDataSet* impactData = DYNAMIC_CAST(impactForm, TESForm, BGSImpactDataSet);

		if (impactData)
		{	
			// thanks to shizof for fixed version that always plays at head
			bool success = papyrusObjRef::PlayImpactEffect((*g_skyrimVM)->GetClassRegistry(), 0, mActor, impactData, mNodeName.c_str(), 0.0f, 0.0f, 0.0f, 1.0f, false, false);

			/*
			if (!success)
			{
				_MESSAGE("PlayImpactEffect failed :(");
			}
			*/
		}
		else
		{
			_MESSAGE("Could not cast from Form to Impact Data FormID = 0x%x", impactVFXFormID);
		}
	}
	else
	{
		_MESSAGE("Could not lookup impact data FormID = 0x%x", impactVFXFormID);
	}



}

void TaskPlayImpactVFX::Dispose()
{
	delete this;
}


// SFX tasks

TaskPlaySound::TaskPlaySound(UInt32 formId, Actor* actor)
{
	mFormId = formId;
	mActor = actor;
}

void TaskPlaySound::Run()
{
	// also try to play sound

	{
		auto* soundForm = LookupFormByID(mFormId);
		if (soundForm)
		{
			auto* soundDesc = DYNAMIC_CAST(soundForm, TESForm, TESSound);
			if (soundDesc)
			{
				papyrusSound::Play((*g_skyrimVM)->GetClassRegistry(), 0, soundDesc, mActor);
			}
			else
			{
				_MESSAGE("Could not cast sound form to TESSound, FormID = 0x%x", mFormId);
			}
		}
		else
		{
			_MESSAGE("Could not lookup sound form data FormID = 0x%x", mFormId);
		}
	}
}

void TaskPlaySound::Dispose()
{
	delete this;
}



