#include "serialisation.h"
#include "events.h"
#include "config.h"

namespace Serialisation {
	void SaveCallback(SKSE::SerializationInterface* a_interface)
	{
		if (!a_interface->OpenRecord(SerializationType, SerializationVersion)) {
			logs::error("Failed to open Stored Version record.");
			return;
		}
		else {
			auto hardcore_diff = Events::DifficultyManager::lockedDifficulty;
			auto hardcore_mode_enabled = Events::DifficultyManager::hardcoreInitialized;
			if (!a_interface->WriteRecordData(hardcore_diff)) {
				logs::error("Failed to write size of record data");
				return;
			}
			if (!a_interface->WriteRecordData(hardcore_mode_enabled)) {
				logs::error("Failed to write size of record data");
				return;
			}
			else {
				logs::info("saved hardcore mode difficulty: {}", hardcore_diff);
				logs::info("saved hardcore mode enabled: {}", hardcore_mode_enabled ? "true" : "false");
			}
		}
	}
	void LoadCallback(SKSE::SerializationInterface* a_interface)
	{
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		a_interface->GetNextRecordInfo(type, version, length);

		if (type != SerializationType) {
			return;
		}

		if (version != SerializationVersion) {
			logs::error("Unable to load data");
			return;
		}

		int32_t hardcore_diff;
		bool hardcore_mode_enabled;

		if (!a_interface->ReadRecordData(hardcore_diff)) {
			logs::error("Failed to load size");
			return;
		}
		if (!a_interface->ReadRecordData(hardcore_mode_enabled)) {
			logs::error("Failed to load size");
			return;
		}

		else {
			Events::DifficultyManager::lockedDifficulty = hardcore_diff;
			logs::info("loaded hardcore mode difficulty: {}", hardcore_diff);
			Events::DifficultyManager::hardcoreInitialized = hardcore_mode_enabled;
			logs::info("loaded hardcore mode enabled: {}", hardcore_mode_enabled ? "true" : "false");
		}


	}
	void RevertCallback(SKSE::SerializationInterface* a_interface)
	{
		Events::DifficultyManager::lockedDifficulty = 2;
		Events::DifficultyManager::hardcoreInitialized = false;
	}
}


