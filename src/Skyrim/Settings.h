#pragma once

namespace Skyrim
{
	static inline constexpr auto INIPATH{ "Data/SKSE/Plugins/Lovense.ini" };

	struct Settings
	{
		static void Initialize();
    
    static inline int iMenuKey = 68;
		static inline bool bUseSSL = false;
	};

} // namespace Skyrim
