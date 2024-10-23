#include "RequestHandler.h"

#include "Connection.h"

namespace Lovense
{
	bool RequestHandler::SendRequest(std::shared_ptr<Request> a_request)
	{
    {
      std::unique_lock lk{ _m };
      if (!a_request->blocking && requests.size() > MAX_REQUEST_NONBLOCKING) {
        return false;
      } else if (requests.size() > MAX_REQUESTS) {
        return false;
      }
			requests.push(a_request);
		}
		cv.notify_one();
    if (a_request->blocking) {
			a_request->WaitForResult();
		}
    return true;
	}

	void RequestHandler::Worker()
	{
		while (true) {
			std::unique_lock lk{ _m };
			cv.wait(lk, [this] { return !requests.empty() || terminate; });
			if (terminate) {
				return;
			}
			auto request = requests.front();
			requests.pop();
			const auto ipAddr = Connection::GetIP_ADDR();
			const auto port = Connection::GetPort();
			if (ipAddr.empty() || port.empty() || !curl) {
				request->SetFailure("Connection not initialized");
				continue;
			}
			const auto command = request->GetCommand();
			if (command.empty()) {
				request->SetFailure("Empty command");
				continue;
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
				request->SetFailure(std::format("curl_easy_perform() failed: {}", curl_easy_strerror(res)));
				continue;
			}

			try {
				const auto json = json::parse(responseData);
				const auto errCode = json["code"].get<size_t>();
				if (errCode != LOVENSE_OK) {
					request->SetFailure(std::format("Invalid command: {}", ERROR_CODES.at(errCode)));
					continue;
				}
				request->SetResult(json);
			} catch (const std::exception& e) {
				request->SetFailure(std::format("Failed to parse response: {}", e.what()));
			}
		} // end while
	}

	size_t RequestHandler::WriteCallback(void* a_ptr, size_t a_size, size_t a_nmemb, std::string& a_data)
	{
		const auto size = a_size * a_nmemb;
		a_data.append(static_cast<char*>(a_ptr), size);
		return size;
	}

	RequestHandler::~RequestHandler()
	{
    {
      std::unique_lock lk{ _m };
      terminate = true;
    }
    cv.notify_one();
    _t.join();
    curl_easy_cleanup(curl);
    curl_global_cleanup();
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

}	 // namespace Lovense