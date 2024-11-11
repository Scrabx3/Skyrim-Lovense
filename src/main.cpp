#include "Lovense/RequestHandler.h"
#include "Skyrim/InputHandler.h"
#include "Skyrim/Interface/LovenseMenu.h"
#include "Skyrim/Papyrus/Functions.h"
#include "Skyrim/Settings.h"

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Skyrim::InputHandler::Register();
		break;
	}
}

#ifndef XMAKE
#ifdef SKYRIM_SUPPORT_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("Scrab Joséline"sv);
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });
	// v.CompatibleVersions({ SKSE::RUNTIME_1_6_353 });
	return v;
}();
#else
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION.pack();
	return true;
}
#endif
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#ifndef XMAKE
	const auto project_name = Plugin::NAME;
	const auto project_version = Plugin::VERSION;
#else
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	const auto project_name = plugin->GetName();
	const auto project_version = plugin->GetVersion();
#endif
	const auto InitLogger = [&]() -> bool {
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path)
			return false;
		*path /= std::format("{}.log", project_name);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif
		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
#ifndef NDEBUG
		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);
#else
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
#endif
		spdlog::set_default_logger(std::move(log));
#ifndef NDEBUG
		spdlog::set_pattern("%s(%#): [%T] [%^%l%$] %v"s);
#else
		spdlog::set_pattern("[%T] [%^%l%$] %v"s);
#endif

		logger::info("{} v{}", project_name, project_version);
		return true;
	};
	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible");
		return false;
	} else if (!InitLogger()) {
		return false;
	}

	SKSE::Init(a_skse);

	if (!Lovense::RequestHandler::GetSingleton()->Initialize()) {
		logger::critical("Failed to initialize RequestHandler");
		return false;
	}
	const auto papyrus = SKSE::GetPapyrusInterface();
	if (!papyrus) {
		logger::critical("Failed to get papyurs interface");
		return false;
	}
	papyrus->Register(Papyrus::Register);
	const auto msging = SKSE::GetMessagingInterface();
	if (!msging->RegisterListener("SKSE", SKSEMessageHandler)) {
	 	logger::critical("Failed to register Listener");
	 	return false;
	 }
	 Interface::LovenseMenu::Register();
	 Skyrim::Settings::Initialize();

	logger::info("{} loaded", project_name);

	return true;
}
