#pragma once

namespace Config {
	namespace Constants {
		constexpr const std::string_view mod_name = "SetDifficultyGlobal.esp";
		constexpr const int main_global_ID = 0x800;
		constexpr const int loot_chance_global_ID = 0x801;
		constexpr const int reverse_loot_chance_global_ID = 0x802;

		constexpr const std::string_view toml_path_default = "Data/SKSE/Plugins/DifficultyGlobal.toml";
		constexpr const std::string_view toml_path_custom = "Data/SKSE/Plugins/DifficultyGlobal_custom.toml";

		constexpr const std::string_view difficulty_name = "iDifficulty:Gameplay";
		
		inline int32_t LOCKED_DIFFICULTY = -1;
	}

	struct Settings : REX::Singleton<Settings> {
		static inline REX::TOML::Bool enable_hardcore_mode{ "Settings", "bEnableHardcoreMode", false };
		static inline REX::TOML::Bool enable_logging{ "Debugging", "bEnableLogging", false };
		static inline REX::TOML::Bool start_game_change_adept{ "Settings", "bSetDifficultyAtStart", false };

		inline void LoadSettings() {
			const auto toml = REX::TOML::SettingStore::GetSingleton();
			toml->Init(Constants::toml_path_default.data(), Constants::toml_path_custom.data());
			toml->Load();
			logs::info("...Settings loaded");
		}
		inline void SaveSettings() {
			const auto toml = REX::TOML::SettingStore::GetSingleton();
			toml->Init(Constants::toml_path_default.data(), Constants::toml_path_custom.data());
			toml->Save();
		}
	};

	using namespace Config::Constants;

	struct Forms : REX::Singleton<Forms> {
		static inline RE::TESDataHandler* dataHandler{ nullptr };

		static inline RE::TESGlobal* DifficultyGlobal{ nullptr };
		static inline RE::TESGlobal* LootChanceGlobal{ nullptr };
		static inline RE::TESGlobal* ReverseLootChanceGlobal{ nullptr };

		void LoadForms() {
			dataHandler = RE::TESDataHandler::GetSingleton();
			if (!dataHandler) {
				SKSE::stl::report_and_fail("Data Handler not found, try again");
			}
			DifficultyGlobal = dataHandler->LookupForm<RE::TESGlobal>(main_global_ID, mod_name);
			LootChanceGlobal = dataHandler->LookupForm<RE::TESGlobal>(loot_chance_global_ID, mod_name);
			ReverseLootChanceGlobal = dataHandler->LookupForm<RE::TESGlobal>(reverse_loot_chance_global_ID, mod_name);

			if (!DifficultyGlobal || !LootChanceGlobal || !ReverseLootChanceGlobal) {
				SKSE::stl::report_and_fail(std::format("Could not find Global Variable forms, enable {} and start again", mod_name));
			}
		}
	};
}