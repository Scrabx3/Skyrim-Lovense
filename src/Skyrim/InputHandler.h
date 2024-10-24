#pragma once

namespace Skyrim
{
	using EventResult = RE::BSEventNotifyControl;

	class InputHandler :
		public Singleton<InputHandler>,
		public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		static void Register()
		{
			const auto input = RE::BSInputDeviceManager::GetSingleton();
			input->AddEventSink<RE::InputEvent*>(GetSingleton());
			logger::info("Registered InputHandler");
		}

	public:
		EventResult ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*) override;
	};
} // namespace Skyrim
