#include "LovenseMenu.h"
#include "Interface.h"

namespace Registry::Interface
{
	LovenseMenu::LovenseMenu() :
		RE::IMenu()
	{
		this->inputContext = Context::kMenuMode;
		this->depthPriority = 3;
		this->menuFlags.set(
				Flag::kCustomRendering,
				Flag::kAssignCursorToRenderer,
				Flag::kUsesCursor);

		auto scaleform = RE::BSScaleformManager::GetSingleton();
		[[maybe_unused]] bool success = scaleform->LoadMovieEx(this, FILEPATH, [](RE::GFxMovieDef* a_def) -> void {
			a_def->SetState(
					RE::GFxState::StateType::kLog,
					RE::make_gptr<FlashLogger<LovenseMenu>>().get());
		});
		assert(success);

		auto view = this->uiMovie;
		view->SetMouseCursorCount(0);
		FunctionManager::AttachSKSEFunctions(view);
	}

	void LovenseMenu::Register()
	{
		RE::UI::GetSingleton()->Register(NAME, Create);
		logger::info("Registered {}", NAME);
	}

	RE::UI_MESSAGE_RESULTS LovenseMenu::ProcessMessage(RE::UIMessage& a_message)
	{
		using Type = RE::UI_MESSAGE_TYPE;
		using Result = RE::UI_MESSAGE_RESULTS;

		switch (*a_message.type) {
		case Type::kShow:
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}

  // void Scaleform_ReConnect::Call(Params& a_args)
	// {
	// 	assert(a_args.argCount >= 2);
	// 	const auto ip = a_args.args[0].GetString();
	// 	const auto port = a_args.args[1].GetString();

	// 	// TODO: Implement reconnection logic
	// }
}