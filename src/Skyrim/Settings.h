#pragma once

namespace Skyrim
{
  static inline constexpr auto INIPATH = L"Data/SKSE/Plugins/SkyrimLovense.ini";

  struct Settings
	{
		static inline void Initialize();
    
    static inline int iMenuKey = 68;
		static inline bool bUseSSL = false;
	};

} // namespace Skyrim
