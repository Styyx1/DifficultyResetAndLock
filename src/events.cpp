#include "events.h"

using namespace Config::Constants;

namespace Events {

	void RegisterEvents()
	{
		MenuEvent::GetSingleton()->Register();
		CellChangeEvent::GetSingleton()->RegisterCellEvent();
		WriteIniHook::Install();
	}
	void MenuEvent::Register()
	{		
		if (auto ui = RE::UI::GetSingleton(); ui) {
			ui->AddEventSink(this);
			logs::info("Registered for {}", typeid(RE::MenuOpenCloseEvent).name());
		}
	}
	EventResult MenuEvent::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_src)
	{
		if (!a_event || a_event->opening || (a_event->menuName != RE::JournalMenu::MENU_NAME && a_event->menuName != RE::RaceSexMenu::MENU_NAME))
			return EventResult::kContinue;

		if(a_event->menuName == RE::RaceSexMenu::MENU_NAME && a_event->opening)
			return EventResult::kContinue;

		if (a_event->menuName == RE::RaceSexMenu::MENU_NAME && !a_event->opening) {
			if(!DifficultyManager::raceMenuReady && !DifficultyManager::hardcoreInitialized)
				DifficultyManager::raceMenuReady = true;
		}

		if (!Config::Settings::enable_hardcore_mode.GetValue() || !DifficultyManager::IsLocked()){
			DifficultyManager::UpdateFromPlayer();
		}

		return EventResult::kContinue;
	}

	void CellChangeEvent::RegisterCellEvent()
	{
		if (auto player = RE::PlayerCharacter::GetSingleton(); player) {
			player->AsBGSActorCellEventSource()->AddEventSink(this);
			logs::info("Registered for {}", typeid(RE::BGSActorCellEvent).name());
		}
	}
	EventResult CellChangeEvent::ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>* a_src)
	{
		if(DifficultyManager::hardcoreInitialized)
			return EventResult::kContinue;

		if(!Config::Settings::enable_hardcore_mode.GetValue())
			return EventResult::kContinue;

		if(!a_event)
			return EventResult::kContinue;

		auto actor = a_event->actor.get().get();
		auto player = RE::PlayerCharacter::GetSingleton();
		if (actor != player || a_event->flags.get() != RE::BGSActorCellEvent::CellFlag::kLeave || !DifficultyManager::raceMenuReady)
			return EventResult::kContinue;

		DifficultyManager::LockDifficulty();

		return EventResult::kContinue;
	}

	void WriteIniHook::Install()	{
		REL::Relocation<std::uintptr_t> iniVTABLE{ RE::VTABLE_INIPrefSettingCollection[0] };
		func = iniVTABLE.write_vfunc(0x03, WritePrefIni);
		logs::info("HOOK:PrefsIni Write");
	}
	void WriteIniHook::WritePrefIni(RE::INIPrefSettingCollection* a_this, RE::Setting* setting)
	{
		auto diff = a_this->GetSetting(Config::Constants::difficulty_name);
		if (diff && setting == diff) {

			if (Config::Settings::enable_hardcore_mode.GetValue() && DifficultyManager::IsLocked()) {
				logs::info("Hardcore mode active — blocking difficulty INI write");
				setting->data.i = DifficultyManager::lockedDifficulty;
			}
			else {
				DifficultyManager::UpdateFromPlayer();
			}
		}
		func(a_this, setting);
	}

	void DifficultyManager::ApplyGlobals(int32_t diff)
	{
		using gd = GameDifficulty;
		float loot = 0, reverse = 100;

		switch (static_cast<gd>(diff)) {
		case gd::kApprentice:
			loot = 20;
			reverse = 80;
			break;
		case gd::kAdept:
			loot = 40;
			reverse = 60;
			break;
		case gd::kExpert:
			loot = 60;
			reverse = 40;
			break;
		case gd::kMaster:
			loot = 80;
			reverse = 20;
			break;
		case gd::kLegendary:
			loot = 100;
			reverse = 0;
			break;
		default:
			loot = 0;
			reverse = 100;
			break;
		}
		Config::Forms::DifficultyGlobal->value = diff + 1.0f;
		Config::Forms::LootChanceGlobal->value = loot;
		Config::Forms::ReverseLootChanceGlobal->value = reverse;

		if (Config::Settings::enable_logging.GetValue()) {
			logs::info("Globals updated: Difficulty={}, Loot={}, ReverseLoot={}", diff, loot, reverse);
		}
	}

	void DifficultyManager::UpdateFromPlayer()
	{
		auto player = RE::PlayerCharacter::GetSingleton();
		auto diff = player->GetGameStatsData().difficulty;
		ApplyGlobals(diff);
	}

	void DifficultyManager::LockDifficulty()
	{
		auto player = RE::PlayerCharacter::GetSingleton();
		lockedDifficulty = player->GetGameStatsData().difficulty;
		hardcoreInitialized = true;
		ApplyGlobals(lockedDifficulty);
		logs::info("Hardcore Mode: Locked difficulty at {}", lockedDifficulty);
	}
}

