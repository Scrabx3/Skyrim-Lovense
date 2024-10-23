#pragma once

namespace Lovense
{
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

}	 // namespace Lovense
