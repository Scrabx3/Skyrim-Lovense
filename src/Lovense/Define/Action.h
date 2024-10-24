#pragma once

#include <frozen/map.h>

namespace Lovense
{
	enum class Action
	{
		Vibrate,
		Rotate,
		Pump,
		Thrusting,
		Fingering,
		Suction,
		Depth,
		Stroke,
		All,
	};

  constexpr static inline frozen::map<Action, std::string_view, 9> ACTION_CHAR{
    { Action::Vibrate, "v" },
    { Action::Rotate, "r" },
    { Action::Pump, "p" },
    { Action::Thrusting, "t" },
    { Action::Fingering, "f" },
    { Action::Suction, "s" },
    { Action::Depth, "d" },
    { Action::Stroke, "" },
    { Action::All, "" },
  };

	constexpr static inline frozen::map<Action, std::pair<int, int>, 9> ACTION_BOUNDS{
		{ Action::Vibrate, { 0, 20 } },
		{ Action::Rotate, { 0, 20 } },
		{ Action::Pump, { 0, 3 } },
		{ Action::Thrusting, { 0, 20 } },
		{ Action::Fingering, { 0, 20 } },
		{ Action::Suction, { 0, 20 } },
		{ Action::Depth, { 0, 3 } },
		{ Action::Stroke, { 0, 100 } },
		{ Action::All, { 0, 20 } },
	};

} // namespace Lovense
