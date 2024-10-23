#pragma once

#pragma warning(push)
#pragma warning(disable : 4200)
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#pragma warning(pop)

#include <nlohmann/json.hpp>
#include <magic_enum.hpp>
static_assert(magic_enum::is_magic_enum_supported);

#pragma warning(push)
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

namespace logger = SKSE::log;
namespace fs = std::filesystem;
using namespace std::literals;
using json = nlohmann::json;

#include "Random.h"
#include "Singleton.h"

#ifdef SKYRIM_SUPPORT_AE
#define RELID(SE, AE) REL::ID(AE)
#define OFFSET(SE, AE) AE
#else
#define RELID(SE, AE) REL::ID(SE)
#define OFFSET(SE, AE) SE
#endif

namespace stl
{
	using namespace SKSE::stl;
	
	inline bool read_string(SKSE::SerializationInterface* a_intfc, std::string& a_str)
	{
		std::size_t size = 0;
		if (!a_intfc->ReadRecordData(size)) {
			return false;
		}
		a_str.reserve(size);
		if (!a_intfc->ReadRecordData(a_str.data(), static_cast<std::uint32_t>(size))) {
			return false;
		}
		return true;
	}

	template <class S>
	inline bool write_string(SKSE::SerializationInterface* a_intfc, const S& a_str)
	{
		std::size_t size = a_str.length() + 1;
		return a_intfc->WriteRecordData(size) && a_intfc->WriteRecordData(a_str.data(), static_cast<std::uint32_t>(size));
	}
}

namespace Papyrus
{
#define REGISTERFUNC(func, classname) a_vm->RegisterFunction(#func##sv, classname, func)
#define REGISTERFUNCND(func, classname) a_vm->RegisterFunction(#func##sv, classname, func, true)

	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
}

#define DLLEXPORT __declspec(dllexport)
#include "Plugin.h"
