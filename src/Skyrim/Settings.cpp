#include "Settings.h"

#include <SimpleIni.h>

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
				logger::error("Unknown Type for option {} in section {}", a_option, a_section);
			}
		};
#define READINI(section, out) ReadIni(section, #out, out)
		READINI("Connection", iMenuKey);
		READINI("Connection", bUseSSL);

#undef READINI

		logger::info("Finished loading .ini settings");
	}

}	 // namespace Skyrim