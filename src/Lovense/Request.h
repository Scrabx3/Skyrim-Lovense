#pragma once

#include <curl/curl.h>
#include <frozen/map.h>

namespace Lovense
{
	struct Request;

	constexpr static inline size_t LOVENSE_OK = 200;
	constexpr static inline frozen::map<size_t, std::string_view, 7> ERROR_CODES{
		{ 500, "HTTP server not started or disabled" },
		{ 400, "Invalid Command" },
		{ 401, "Toy Not Found" },
		{ 402, "Toy Not Connecte" },
		{ 403, "Toy Doesn't Support This Command" },
		{ 404, "Invalid Parameter" },
		{ 506, "Server Error. Restart Lovense Connect" },
	};

	struct RequestHandler :
		public Singleton<RequestHandler>
	{
		bool SendRequest(Request& a_request);

	public:
		~RequestHandler();
		bool Initialize();

	private:
		size_t WriteCallback(void* a_ptr, size_t a_size, size_t a_nmemb, std::string& a_data);

		std::mutex _m{};
		CURL* curl{ nullptr };
	};

	struct Request
	{
		Request(bool a_blocking) :
			blocking(a_blocking) {}
		~Request() = default;
		
		virtual std::string GetCommand() const = 0;

	public:
		bool IsBlocking() const { return blocking; }

		std::string GetResult() const { return result.value(); }
		bool IsValid() const { return result.has_value(); }
		void SetResult(const json& a_result) { result = a_result; }
		void SetResult(const json&& a_result) { result = std::move(a_result); }

	private:
		bool blocking;
		std::optional<json> result;
	};

}	 // namespace Lovense
