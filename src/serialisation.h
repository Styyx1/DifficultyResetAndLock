#pragma once

namespace Serialisation
{
	constexpr std::uint32_t SerializationVersion = 1;
	constexpr std::uint32_t ID = 'STDG';
	constexpr std::uint32_t SerializationType = 'SLHM';

	void SaveCallback(SKSE::SerializationInterface* a_interface);

	void LoadCallback(SKSE::SerializationInterface* a_interface);

	void RevertCallback(SKSE::SerializationInterface* a_interface);
}