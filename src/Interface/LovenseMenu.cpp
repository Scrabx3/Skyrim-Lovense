#include "LovenseMenu.h"

#include "Interface.h"
#include "Lovense/Connection.h"
#include "Lovense/Request.h"
#include "Lovense/RequestHandler.h"
#include "Lovense/Category.h"

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
			{
				std::vector<RE::GFxValue> args{};
				const auto names = magic_enum::enum_names<Lovense::Category>();
				for (auto&& name : names) {
					args.emplace_back(name);
				}
				this->uiMovie->InvokeNoReturn("_root.main.setCategories", args.data(), static_cast<uint32_t>(args.size()));
				__fallthrough;
			}
		case Type::kUpdate:
			{
				auto request = std::make_shared<Lovense::GetToys_Request>();
				Lovense::RequestHandler::GetSingleton()->SendRequest(request);
				std::vector<RE::GFxValue> args{};
				if (request->IsFailure()) {
					logger::error("Failed to update Connections: {}", request->GetError());
				} else {
					const auto& toys = request->GetResult()["toys"];
					Lovense::Connection::UpdateToyList(toys);
					Lovense::Connection::VisitToys([&args](const Lovense::Toy& toy) {
						std::string_view cat = magic_enum::enum_name(toy.category);
						RE::GFxValue toyObj;
						toyObj.SetMember("id", RE::GFxValue{ std::string_view{ toy.id } });
						toyObj.SetMember("name", RE::GFxValue{ std::string_view{ toy.name } });
						toyObj.SetMember("category", RE::GFxValue{ cat });
						args.emplace_back(toyObj);
						return true;
					});
				}
				this->uiMovie->InvokeNoReturn("_root.main.addItems", args.data(), static_cast<uint32_t>(args.size()));
				return Result::kHandled;
			}
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}

  void Scaleform_ReConnect::Call(Params& a_args)
	{
		assert(a_args.argCount >= 2);
		const auto ip = a_args.args[0].GetString();
		const auto port = a_args.args[1].GetString();

		Lovense::Connection::SetIP_ADDR(ip);
		Lovense::Connection::SetPORT(port);

		LovenseMenu::RequestUpdate();
	}
}