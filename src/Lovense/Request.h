#pragma once

#include "Action.h"
#include "Preset.h"

namespace Lovense
{
	constexpr static inline size_t MAX_PATTERN_STRENGTH_COUNT = 50;

	struct Request
	{
		friend class RequestHandler;

	public:
		Request(bool a_blocking) :
			blocking(a_blocking) {}
		~Request() = default;

		virtual std::string GetCommand() const = 0;

	public:
		const json& GetResult() const { return result.value(); }
		const std::string GetError() const { return error.value(); }
		bool IsValid() const { return result.has_value(); }
		bool IsFailure() const { return error.has_value(); }
		void SetResult(const json& a_result);
		void SetFailure(const std::string& a_str);

	private:
		void WaitForResult();

	private:
		bool blocking;
		std::mutex _m{};
		std::condition_variable cv{};
		std::optional<json> result{};
		std::optional<std::string> error{};
	};

	struct GetToys_Request : public Request
	{
		GetToys_Request() :
			Request(true) {}
		~GetToys_Request() = default;
		std::string GetCommand() const override;
	};

	struct GetToyName_Request : public Request
	{
		GetToyName_Request() :
			Request(true) {}
		~GetToyName_Request() = default;
		std::string GetCommand() const override;
	};

	struct Function_Request : public Request
	{
		Function_Request(std::vector<Action> action, std::vector<int> strength, double timeSec,
				std::optional<double> loopRunningSec, std::optional<double> loopPauseSec, std::optional<std::string> toy, std::optional<int> stopPrevious);
		~Function_Request() = default;
		std::string GetCommand() const override;

	private:
		std::vector<Action> action;
		std::vector<int> strength;
		double timeSec;
		std::optional<double> loopRunningSec;
		std::optional<double> loopPauseSec;
		std::optional<std::string> toy;
		std::optional<int> stopPrevious;
		static inline constexpr int apiVer = 1;
	};

	struct Position_Request : public Request
	{
		Position_Request(int value, std::optional<std::string> toy = std::nullopt) :
			Request(false), value(std::to_string(std::clamp(value, 0, 100))), toy(toy) {};
		~Position_Request() = default;
		std::string GetCommand() const override;

	private:
		std::string value;
		std::optional<std::string> toy;
		static inline constexpr int apiVer = 1;
	};

	struct Pattern_Request : public Request
	{
		Pattern_Request(const std::vector<Action>& actions, const std::vector<int>& strengths, int intervalMs, double timeSec,
				std::optional<std::string> toy = std::nullopt);
		~Pattern_Request() = default;
		std::string GetCommand() const override;

	private:
	  std::vector<Action> actions;
		std::vector<int> strengths;
		int intervalMs;
		double timeSec;
		std::optional<std::string> toy;
		static inline constexpr int apiVer = 2;
	};

 struct Preset_Request : public Request
 {
	 Preset_Request(Preset preset, double timeSec, std::optional<std::string> toy = std::nullopt) :
		 Request(false), preset(preset), timeSec(timeSec), toy(toy) {};
	 ~Preset_Request() = default;
	 std::string GetCommand() const override;

	 private:
		 Preset preset;
		 double timeSec;
		 std::optional<std::string> toy;
		 static inline constexpr int apiVer = 1;
 };

}	 // namespace Lovense
