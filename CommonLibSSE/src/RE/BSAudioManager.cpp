#include "RE/BSAudioManager.h"

//#include "RE/Offsets.h"
#include "RE/SoundData.h"  // SoundData
#include "REL/Relocation.h"

#include "Util.h"
#include "../../../common.h"

namespace RE
{
	BSAudioManager* BSAudioManager::GetSingleton()
	{
		using func_t = function_type_t<decltype(&BSAudioManager::GetSingleton)>;
		REL::Offset<func_t*> func(Offset::BSAudioManager::GetSingleton);
		return func();
	}


	bool BSAudioManager::Play(BSISoundDescriptor* a_descriptor)
	{
		SoundData soundData;
		return BuildSoundDataFromDescriptor(soundData, a_descriptor) && soundData.Play();
	}


	bool BSAudioManager::BuildSoundDataFromDescriptor(SoundData& a_soundData, BSISoundDescriptor* a_descriptor, UInt32 a_flags)
	{
		using func_t = function_type_t<decltype(&BSAudioManager::BuildSoundDataFromDescriptor)>;
		REL::Offset<func_t*> func(Offset::BSAudioManager::BuildSoundDataFromDescriptor);
		return func(this, a_soundData, a_descriptor, a_flags);
	}
}

