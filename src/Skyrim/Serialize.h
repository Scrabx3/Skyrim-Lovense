#pragma once

#include "Lovense/Connection.h"

namespace Skyrim
{
	class Serialization
	{
		static inline std::string GetTypeName(uint32_t a_type)
		{
			constexpr auto size = sizeof(uint32_t);
			std::string ret{};
			ret.resize(size);
			const char* it = reinterpret_cast<char*>(&a_type);
			for (size_t i = 0, j = size - 2; i < size - 1; i++, j--)
				ret[j] = it[i];

			return ret;
		}

	public:
		enum : std::uint32_t
		{
			_Version = 1,

			_IpAddr = 'ipdr',
			_Port = 'port',
		};

		static void SaveCallback(SKSE::SerializationInterface* a_intfc)
		{
 			const auto ipAddr = Lovense::Connection::GetIP_ADDR();
			if (!a_intfc->OpenRecord(_IpAddr, _Version))
				logger::error("Failed to open record <_IpAddr>");
			else if (!stl::write_string(a_intfc, ipAddr))
				logger::error("Failed to serialize record <_IpAddr>");

			const auto port = Lovense::Connection::GetPort();
			if (!a_intfc->OpenRecord(_Port, _Version))
				logger::error("Failed to open record <_Port>");
			else if (!stl::write_string(a_intfc, port))
				logger::error("Failed to serialize record <_Port>");

			logger::info("Finished saving data to cosave");
		}

		static void LoadCallback(SKSE::SerializationInterface* a_intfc)
		{
			uint32_t type;
			uint32_t version;
			uint32_t length;
			while (a_intfc->GetNextRecordInfo(type, version, length)) {
				const auto typeStr = GetTypeName(type);
				const auto _v = static_cast<uint32_t>(_Version);
				if (version != _v) {
					logger::info("Invalid Version for loaded Data of Type {}. Expected {} but got {}", typeStr, _v, version);
					continue;
				}
				logger::info("Loading record {}", typeStr);
				std::string argStr;
				stl::read_string(a_intfc, argStr);
				switch (type) {
				case _IpAddr:
					Lovense::Connection::SetIP_ADDR(argStr);
					break;
				case _Port:
					Lovense::Connection::SetPORT(argStr);
					break;
				default:
					logger::info("Unrecognized type");
					break;
				}
			}
			logger::info("Finished loading data from cosave");
		}

		static void RevertCallback(SKSE::SerializationInterface*) {}
		static void FormDeleteCallback(RE::VMHandle) {}
	};
}	 // namespace Skyrim
