#pragma once

namespace Registry::Interface
{
	class LovenseMenu :
		public RE::IMenu
	{
		using GRefCountBaseStatImpl::operator new;
		using GRefCountBaseStatImpl::operator delete;

	public:
		static constexpr std::string_view NAME{ "LovenseMenu" };
		static constexpr std::string_view FILEPATH{ "Lovense" };

		LovenseMenu();
		~LovenseMenu() = default;
		static void Register();
		static RE::IMenu* Create() { return new LovenseMenu(); }

	public:
		static void Show() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr); }
		static void Hide() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr); }
		static void ForceHide() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kForceHide, nullptr); }
		static void RequestUpdate() { RE::UIMessageQueue::GetSingleton()->AddMessage(NAME, RE::UI_MESSAGE_TYPE::kUpdate, nullptr); }
		static bool IsOpen() { return RE::UI::GetSingleton()->IsMenuOpen(NAME); }

	protected:
		// IMenu
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;
	};

	struct Scaleform_ReConnect : public RE::GFxFunctionHandler
	{
		void Call(Params& a_args) override;
	};

	struct Scaleform_Help : public RE::GFxFunctionHandler
	{
		void Call(Params& a_args) override;
	};
}