#include "LovenseMenu.h"

#include "Interface.h"
#include "Lovense/Connection.h"
#include "Lovense/Request.h"
#include "Lovense/RequestHandler.h"
#include "Lovense/Define/Category.h"

namespace Interface
{
	LovenseMenu::LovenseMenu() :
		RE::IMenu()
	{
		this->inputContext = Context::kMenuMode;
		this->depthPriority = 3;
		this->menuFlags.set(
				Flag::kUsesMenuContext,
				Flag::kCustomRendering,
				Flag::kApplicationMenu,
				Flag::kUsesCursor,
				Flag::kPausesGame);

		auto scaleform = RE::BSScaleformManager::GetSingleton();
		[[maybe_unused]] bool success = scaleform->LoadMovieEx(this, FILEPATH, [](RE::GFxMovieDef* a_def) -> void {
			a_def->SetState(
					RE::GFxState::StateType::kLog,
					RE::make_gptr<FlashLogger<LovenseMenu>>().get());
		});
		assert(success);

		auto view = this->uiMovie;
		view->SetMouseCursorCount(1);
		auto lovenseObj = FunctionManager::MakeFunctionObject(view, "Lovense");
		if (!lovenseObj) {
			throw std::runtime_error("Failed to create function object");
		}
		FunctionManager::AttachFunction<Scaleform_ReConnect>(view, *lovenseObj, "ReConnect");
		FunctionManager::AttachFunction<Scaleform_Help>(view, *lovenseObj, "Help");
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
				const auto ipAddr = Lovense::Connection::GetIP_ADDR();
				if (!ipAddr.empty()) {
					RE::GFxValue arg{ ipAddr };
					this->uiMovie->InvokeNoReturn("_root.main.setIP", &arg, 1);
				}
				const auto port = Lovense::Connection::GetPort();
				if (!port.empty()) {
					RE::GFxValue arg{ port };
					this->uiMovie->InvokeNoReturn("_root.main.setPort", &arg, 1);
				}
				UpdateToyList();
			}
			return Result::kHandled;
		case Type::kUpdate:
			try {
				auto request = std::make_shared<Lovense::GetToys_Request>();
				Lovense::RequestHandler::GetSingleton()->SendRequest(request);
				if (request->IsFailure()) {
					const auto err = request->GetError();
					logger::error("Failed to update Connections: {}", err);
					Lovense::Connection::ClearToyList();
				} else {
					const auto& toys = request->GetResult()["toys"];
					const auto toyStr = toys.get<std::string>();
					const auto jToys = json::parse(toyStr);
					Lovense::Connection::UpdateToyList(jToys);
				}
				UpdateToyList();
				return Result::kHandled;
			} catch (const std::exception& e) {
				logger::error("Failed to update Connections: {}", e.what());
				return Result::kHandled;
			}
		case Type::kHide:
			{
				auto items = RE::GFxValue{};
				this->uiMovie->GetVariable(&items, "_root.main.item_list.items");
				assert(items.IsArray());
				auto obj = RE::GFxValue{}, idObj = RE::GFxValue{}, catObj = RE::GFxValue{};
				for (uint32_t i = 0; i < items.GetArraySize(); i++) {
					items.GetElement(i, &obj);
					obj.GetMember("id", &idObj);
					obj.GetMember("category", &catObj);
					const auto id = idObj.GetString();
					const auto cat = static_cast<int>(catObj.GetNumber());
					const auto category = magic_enum::enum_cast<Lovense::Category>(cat);
					if (category) {
						Lovense::Connection::AssignCategory(id, category.value());
					} else {
						logger::error("Invalid category for toy: {}: {}", id, cat);
					}
				}
				return Result::kHandled;
			}
		default:
			return RE::IMenu::ProcessMessage(a_message);
		}
	}

	void LovenseMenu::UpdateToyList() const
	{
		std::vector<RE::GFxValue> args{};
		Lovense::Connection::VisitToys([&](const Lovense::Toy& toy) {
			std::optional<size_t> catIdx = magic_enum::enum_index(toy.category);
			if (!catIdx) {
				logger::error("Invalid category for toy: {} ({})", toy.name, toy.id);
				catIdx = std::make_optional(0);
			}
			const auto idx = std::to_string(*catIdx);
			RE::GFxValue toyObj{};
			this->uiMovie->CreateObject(&toyObj);
			toyObj.SetMember("id", RE::GFxValue{ std::string_view{ toy.id } });
			toyObj.SetMember("name", RE::GFxValue{ std::string_view{ toy.name } });
			toyObj.SetMember("category", RE::GFxValue{ std::string_view{ idx } });
			args.emplace_back(toyObj);
			return true;
		});
		this->uiMovie->InvokeNoReturn("_root.main.addItems", args.data(), static_cast<uint32_t>(args.size()));
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

	void Scaleform_Help::Call(Params&)
	{
		std::string url = "https://www.nexusmods.com";
		ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		logger::info("Help requested");
	}
}
