#include "Settings.h"

#include <SimpleIni.h>

#include "Lovense/Connection.h"

namespace Skyrim
{
	void Settings::Initialize()
	{
		if (!fs::exists(INIPATH)) {
			logger::error("No Settings file (ini) in {}", INIPATH);
			return;
		}
		CSimpleIniA inifile{};
		inifile.SetUnicode();
		const auto ec = inifile.LoadFile(INIPATH);
		if (ec < 0) {
			logger::error("Failed to read .ini Settings, Error: {}", ec);
			return;
		}
		const auto ReadIni = [&inifile]<typename T>(const char* a_section, const char* a_option, T& a_out) {
			if (!inifile.GetValue(a_section, a_option))
				return;

			if constexpr (std::is_integral_v<T>) {
				a_out = static_cast<T>(inifile.GetLongValue(a_section, a_option));
			} else if constexpr (std::is_floating_point_v<T>) {
				a_out = static_cast<T>(inifile.GetDoubleValue(a_section, a_option));
			} else {
				a_out = inifile.GetValue(a_section, a_option);
			}
		};
#define READINI(section, out) ReadIni(section, #out, out)
		READINI("Connection", iMenuKey);
		READINI("Connection", bUseSSL);

		const char* sAddress = "";
		const char* iPort = "20010";
		READINI("Connection", sAddress);
		READINI("Connection", iPort);

		auto trim_quotes = [](const char* str) -> std::string {
			std::string result(str);
			if (result.empty())
				return result;
			if (result.front() == '"')
				result.erase(result.begin());
			if (result.back() == '"') {
				result.pop_back();
			}
			return result;
		};
		const auto address = trim_quotes(sAddress);
		const auto port = trim_quotes(iPort);

		if (address.find_first_not_of("0123456789.") != std::string::npos) {
			logger::error("Invalid IP Address: {}", address);
		} else if (!address.empty()) {
			Lovense::Connection::SetIP_ADDR(address);
		}
		if (port.find_first_not_of("0123456789") != std::string::npos) {
			logger::error("Invalid Port: {}", port);
		} else if (!port.empty()) {
			Lovense::Connection::SetPORT(port);
		}
#undef READINI

		logger::info("Finished loading .ini settings");
	}

}	 // namespace Skyrim