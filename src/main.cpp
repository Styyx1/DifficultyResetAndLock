#include "config.h"
#include "events.h"
#include "serialisation.h"

void WriteNewGameSetting() {
	const auto isc = RE::INIPrefSettingCollection::GetSingleton();
	if (!isc) {
		logs::info("Ini setting collection singleton fail");
		return;
	}
	auto diff = isc->GetSetting(Config::Constants::difficulty_name);
	if (!diff) {
		return;
	}		
	diff->data.i = 2;
	isc->WriteSetting(diff);
	auto player = RE::PlayerCharacter::GetSingleton();
	player->GetGameStatsData().difficulty = diff->data.i;
}

void Listener(SKSE::MessagingInterface::Message* a_msg) {
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Config::Forms::GetSingleton()->LoadForms();
		Events::RegisterEvents();
		break;

	case SKSE::MessagingInterface::kNewGame:
		if(Config::Settings::start_game_change_adept.GetValue())
			WriteNewGameSetting();
		Events::DifficultyManager::UpdateFromPlayer();
		break;

	case SKSE::MessagingInterface::kPostLoadGame:
		Events::DifficultyManager::UpdateFromPlayer();
		Events::DifficultyManager::ApplyLockedDifficulty();
		
		break;

	default:
		break;
	}	
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	Config::Settings::GetSingleton()->LoadSettings();
	SKSE::GetMessagingInterface()->RegisterListener(Listener);

	if (auto serialisation = SKSE::GetSerializationInterface()) {
		serialisation->SetUniqueID(Serialisation::ID);
		serialisation->SetSaveCallback(&Serialisation::SaveCallback);
		serialisation->SetLoadCallback(&Serialisation::LoadCallback);
		serialisation->SetRevertCallback(&Serialisation::RevertCallback);
	}

	return true;
}
