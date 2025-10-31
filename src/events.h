#pragma once
#include "config.h"


namespace Events {
	using EventResult = RE::BSEventNotifyControl;

	void RegisterEvents();


	enum class GameDifficulty : uint32_t {
		kNovice = 0,
		kApprentice = 1,
		kAdept = 2,
		kExpert = 3,
		kMaster = 4,
		kLegendary = 5,
	};

	namespace DifficultyManager
	{
		inline int32_t lockedDifficulty = -1;
		inline bool hardcoreInitialized = false;
		inline bool raceMenuReady = false;

		inline void ApplyGlobals(int32_t diff);
		inline void UpdateFromPlayer();
		inline void LockDifficulty();

		inline void WriteLockedSettings(int32_t difficulty) {
			const auto isc = RE::INIPrefSettingCollection::GetSingleton();
			if (!isc) {
				return;
			}
			auto diff = isc->GetSetting(Config::Constants::difficulty_name);
			if (!diff) {
				return;
			}
			diff->data.i = difficulty;
			isc->WriteSetting(diff);
		}

		inline void ApplyLockedDifficulty() {
			using namespace Config;

			if (!hardcoreInitialized)
				return;
			if (!Config::Settings::enable_hardcore_mode.GetValue()) {
				logs::info("Hardcore mode not active — skipping locked difficulty restore.");
				return;
			}
			if (lockedDifficulty < 0) {
				logs::warn("No locked difficulty found. Hardcore mode requires a new game!");
				return;
			}
			auto player = RE::PlayerCharacter::GetSingleton();
			player->GetGameStatsData().difficulty = lockedDifficulty;
			ApplyGlobals(lockedDifficulty);
			WriteLockedSettings(lockedDifficulty);
		};
		inline bool IsLocked() { return lockedDifficulty >= 0; }
	};



	struct MenuEvent : public REX::Singleton<MenuEvent>, public RE::BSTEventSink<RE::MenuOpenCloseEvent> {		

		void Register();

	private:
		EventResult ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_src) override;
	};

	struct CellChangeEvent : public REX::Singleton<CellChangeEvent>, public RE::BSTEventSink<RE::BGSActorCellEvent> {

		void RegisterCellEvent();
	private:
		EventResult ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>* a_src) override;
	};

	struct WriteIniHook {
		static void Install();
	private:
		static void WritePrefIni(RE::INIPrefSettingCollection* a_this, RE::Setting* setting);
		static inline REL::Relocation<decltype(WritePrefIni)> func;
	};

}
