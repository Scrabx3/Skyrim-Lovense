#pragma once

#include <curl/curl.h>
#include <frozen/map.h>

#include "Request.h"

namespace Lovense
{
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

	class RequestHandler :
		public Singleton<RequestHandler>
	{
		bool SendRequest(std::shared_ptr<Request> a_request);

	public:
		RequestHandler() :
			_t(&RequestHandler::Worker, this) {}
		~RequestHandler();
		bool Initialize();

	private:
    void Worker();
		size_t WriteCallback(void* a_ptr, size_t a_size, size_t a_nmemb, std::string& a_data);

		std::mutex _m{};
    std::thread _t{};
    std::atomic_bool terminate{ false };
    std::condition_variable cv{};
    std::queue<std::shared_ptr<Request>> requests{};
		CURL* curl{ nullptr };
	};

}	 // namespace Lovense
