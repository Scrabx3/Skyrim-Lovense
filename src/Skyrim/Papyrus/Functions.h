#pragma once

namespace Papyrus
{
	int32_t GetConnectedCount(RE::StaticFunctionTag*);
	std::vector<RE::BSFixedString> GetToyIDs(RE::StaticFunctionTag*);
	std::vector<RE::BSFixedString> GetToyNames(RE::StaticFunctionTag*);

	RE::BSFixedString GetToyName(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString id);
	RE::BSFixedString GetToyCategory(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString id);
	std::vector<RE::BSFixedString> GetToysByCategory(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString category);

	bool StopRequest(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString toy);
	bool FunctionRequest(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::BSFixedString> actions, std::vector<int> strengths, float timeSec, float loopRunningSec, float loopPauseSec, RE::BSFixedString toy, bool stopPrevious);
	bool PatternRequest(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, std::vector<RE::BSFixedString> actions, std::vector<int> strengths, int intervalMs, float timeSec, RE::BSFixedString asToy = "");
	bool PresetReqest(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString preset, float timeSec, RE::BSFixedString toy);

	inline bool Register(VM* a_vm)
	{
		REGISTERFUNCND(GetConnectedCount, "Lovense");
		REGISTERFUNCND(GetToyIDs, "Lovense");
		REGISTERFUNCND(GetToyNames, "Lovense");

		REGISTERFUNC(GetToyName, "Lovense");
		REGISTERFUNC(GetToyCategory, "Lovense");
		REGISTERFUNC(GetToysByCategory, "Lovense");

		REGISTERFUNC(StopRequest, "Lovense");
		REGISTERFUNC(FunctionRequest, "Lovense");
		REGISTERFUNC(PatternRequest, "Lovense");
		REGISTERFUNC(PresetReqest, "Lovense");

		logger::info("Registered papyrus functions");
		return true;
	}
}	 // namespace Papyrus
