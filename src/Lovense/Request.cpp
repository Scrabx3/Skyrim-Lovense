#include "Request.h"

#include "Connection.h"

namespace Lovense
{
	size_t RequestHandler::WriteCallback(void* a_ptr, size_t a_size, size_t a_nmemb, std::string& a_data)
  {
    const auto size = a_size * a_nmemb;
    a_data.append(static_cast<char*>(a_ptr), size);
    return size;
	}

	bool RequestHandler::SendRequest(Request& a_request)
	{
		std::unique_lock lk{ _m, std::defer_lock };
    if (!lk.try_lock()) {
      if (!a_request.IsBlocking()) {
        std::this_thread::sleep_for(3ms);
        return false;
			}
			lk.lock();
		}
		const auto ipAddr = Connection::GetIP_ADDR();
    const auto port = Connection::GetPort();
		if (ipAddr.empty() || port.empty() || !curl) {
			logger::error("Connection not initialized");
      return false;
		}
		const auto command = a_request.GetCommand();
    if (command.empty()) {
      logger::error("Empty command");
      return false;
    }
		curl_easy_reset(curl);

		std::string responseData{};
		const auto url = std::format("https://{}:{}/command", ipAddr, port);
		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		const auto platformHeader = std::format("X-platform: {}", Plugin::NAME);
		headers = curl_slist_append(headers, platformHeader.c_str());

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, command.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, command.length());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &RequestHandler::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

		const auto res = curl_easy_perform(curl);
		curl_slist_free_all(headers);
		if (res != CURLE_OK) {
			logger::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
			return false;
    }

		try {
			const auto json = json::parse(responseData);
      const auto errCode = json["code"].get<size_t>();
      if (errCode != LOVENSE_OK) {
        logger::error("Invalid command: {}", ERROR_CODES.at(errCode));
        return false;
      }
      a_request.SetResult(std::move(json));
      return true;
		} catch (const std::exception& e) {
			logger::error("Failed to parse response: {}", e.what());
      return false;
		}
	}

	RequestHandler::~RequestHandler()
	{
		_m.lock();
		if (curl) {
      curl_easy_cleanup(curl);
      curl = nullptr;
		}
    curl_global_cleanup();
		_m.unlock();
	}

	bool RequestHandler::Initialize()
	{
		const auto res = curl_global_init(CURL_GLOBAL_DEFAULT);
		if (res != CURLE_OK) {
			logger::critical("curl_global_init() failed: {}", curl_easy_strerror(res));
			return false;
		}
		curl = curl_easy_init();
		if (!curl) {
			logger::critical("Failed to initialize CURL");
			return false;
		}
    return true;
	}

}   // namespace Lovense