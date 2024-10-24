#include "InputHandler.h"

#include "Settings.h"
#include "Interface/LovenseMenu.h"

namespace Skyrim
{
	EventResult InputHandler::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
	{
		if (!a_event || Interface::LovenseMenu::IsOpen())
			return EventResult::kContinue;

		const auto intfcStr = RE::InterfaceStrings::GetSingleton();
		const auto ui = RE::UI::GetSingleton();
		if (ui->IsMenuOpen(intfcStr->console) || ui->GameIsPaused())
			return EventResult::kContinue;

		const auto menukey = Settings::iMenuKey >= SKSE::InputMap::kMacro_GamepadOffset ?
														 SKSE::InputMap::GamepadKeycodeToMask(Settings::iMenuKey) :
														 static_cast<uint32_t>(Settings::iMenuKey);
		for (const RE::InputEvent* input = *a_event; input; input = input->next) {
			const auto event = input->AsButtonEvent();
			if (!event || !event->IsDown())
				continue;

			const auto idcode = event->GetIDCode();
			if (idcode != menukey)
				continue;

			Interface::LovenseMenu::Show();
			break;
		}
		return EventResult::kContinue;
	}
} // namespace Skyrim
