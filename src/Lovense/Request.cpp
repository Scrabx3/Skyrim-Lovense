#include "Request.h"

#define MANDATORY_ATTRIBUTE(attr) j[#attr] = attr;
#define OPTIONAL_ATTRIBUTE(attr) if (attr.has_value()) { j[#attr] = attr.value(); }

namespace Lovense
{
  void Request::WaitForResult()
  {
    std::unique_lock lk{ _m };
    cv.wait(lk, [this] { return result.has_value() || error.has_value(); });
  }

  void Request::SetResult(const json& a_result)
  {
    std::lock_guard lk{ _m };
    result = a_result;
    cv.notify_all();
  }

  void Request::SetFailure(const std::string& a_str)
  {
    std::lock_guard lk{ _m };
    error = a_str;
    cv.notify_all();
  }

  std::string GetToys_Request::GetCommand() const
  {
    return R"({"command":"GetToys"})";
  }

  std::string GetToyName_Request::GetCommand() const
  {
    return R"({"command":"GetToyName"})";
  }

	Function_Request::Function_Request(std::vector<Action> a_action, std::vector<int> a_strength, double a_timeSec,
			std::optional<double> a_loopRunningSec, std::optional<double> a_loopPauseSec, std::optional<std::string> a_toy, std::optional<int> a_stopPrevious) :
		Request(false), action(a_action), strength(a_strength), timeSec(a_timeSec), loopRunningSec(a_loopRunningSec), loopPauseSec(a_loopPauseSec), toy(a_toy), stopPrevious(a_stopPrevious)
	{
		if (action.size() != strength.size()) {
			throw std::invalid_argument("Action and strength vectors must be the same size");
		}
    if (timeSec < 1.0 && timeSec != 0.0) {
      timeSec = 1.0;
    }
	}

	std::string Function_Request::GetCommand() const
  {
    json j;
    j["command"] = "Function";
		j["action"] = [this] {
			std::string ret = "";
			for (size_t i = 0; i < action.size(); ++i) {
				if (!ret.empty()) {
					ret += ",";
				}
				const auto act = magic_enum::enum_name(action[i]);
				const auto str = std::clamp(strength[i], ACTION_BOUNDS.at(action[i]).first, ACTION_BOUNDS.at(action[i]).second);
				ret += std::format("{}:{}", act, str);
			}
      return ret;
		}();
		MANDATORY_ATTRIBUTE(timeSec);
		OPTIONAL_ATTRIBUTE(loopRunningSec);
		OPTIONAL_ATTRIBUTE(loopPauseSec);
		OPTIONAL_ATTRIBUTE(toy);
		OPTIONAL_ATTRIBUTE(stopPrevious);
    MANDATORY_ATTRIBUTE(apiVer);
		return j.dump();
  }

  std::string Position_Request::GetCommand() const
  {
    json j;
    j["command"] = "Position";
    MANDATORY_ATTRIBUTE(value);
    OPTIONAL_ATTRIBUTE(toy);
    MANDATORY_ATTRIBUTE(apiVer);
    return j.dump();
  }

	Pattern_Request::Pattern_Request(const std::vector<Action>& a_actions, const std::vector<int>& a_strengths, int a_intervalMs, double a_timeSec, std::optional<std::string> a_toy) :
		Request(false), actions(a_actions), strengths(a_strengths), intervalMs(a_intervalMs), timeSec(a_timeSec), toy(a_toy)
	{
		if (strengths.size() > MAX_PATTERN_STRENGTH_COUNT) {
			throw std::invalid_argument(std::format("Strengths vector must be less than {}", MAX_PATTERN_STRENGTH_COUNT));
		}
		if (timeSec < 1.0 && timeSec != 0.0) {
			timeSec = 1.0;
		}
		}

  std::string Pattern_Request::GetCommand() const
  {
    json j;
    j["command"] = "Pattern";
    j["rule"] = std::format("V:{};F:{};S:{}#",
      1,
      std::ranges::fold_left(actions, std::string{}, [](auto acc, const auto& act) {
        const std::string_view str = ACTION_CHAR.at(act);
        if (!str.empty()) {
          if (!acc.empty()) {
            acc += ',';
          }
          acc += str;
        }
        return acc;
      }),
      intervalMs);
    j["strength"] = std::format("{}", std::ranges::fold_left(strengths, std::string{}, [](auto acc, const auto& strength) {
      if (!acc.empty()) {
        acc += ';';
      }
      acc += std::to_string(strength);
      return acc;
    }));
		MANDATORY_ATTRIBUTE(timeSec);
		OPTIONAL_ATTRIBUTE(toy);
		MANDATORY_ATTRIBUTE(apiVer);
		return j.dump();
	}

  std::string Preset_Request::GetCommand() const
  {
    json j;
    j["command"] = "Preset";
    j["name"] = magic_enum::enum_name(preset);
    MANDATORY_ATTRIBUTE(timeSec);
    OPTIONAL_ATTRIBUTE(toy);
    MANDATORY_ATTRIBUTE(apiVer);
    return j.dump();
  }

}   // namespace Lovense