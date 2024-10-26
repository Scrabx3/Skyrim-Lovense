#include "RequestHandler.h"

#include "Connection.h"

namespace Lovense
{
	bool RequestHandler::SendRequest(std::shared_ptr<Request> a_request)
	{
		{
			std::unique_lock lk{ _m };
			requests.push(a_request);
		}
		cv.notify_one();
		a_request->WaitForResult();
		return true;
	}

	void RequestHandler::Worker()
	{
		while (true) {
			std::unique_lock lk{ _m };
			cv.wait(lk, [this] { return !requests.empty() || terminate; });
			if (terminate) {
				break;
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

			std::string responseData{};
			const auto url = std::format("http://{}:{}/command", ipAddr, port);
			const auto platformHeader = std::format("X-Platform: {}", Plugin::NAME);
			struct curl_slist* headers = nullptr;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			headers = curl_slist_append(headers, platformHeader.c_str());
			static constexpr auto TIMEOUT = 400L;

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, command.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, command.size());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, TIMEOUT);
#ifndef NDEBUG
			constexpr auto filePath = "./Data/SKSE/Lovense/ErrLog.txt";
			FILE* file = nullptr;
			if (fopen_s(&file, filePath, "w") == 0) {
				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
				curl_easy_setopt(curl, CURLOPT_STDERR, file);
			} else {
				logger::error("Failed to open file: {}", filePath);
			}
#endif

			const auto res = curl_easy_perform(curl);
			curl_slist_free_all(headers);
			curl_easy_reset(curl);
#ifndef NDEBUG
			if (file) {
				fclose(file);
			}
#endif
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
				request->SetResult(json["data"]);
			} catch (const std::exception& e) {
				request->SetFailure(std::format("Failed to parse response: {}", e.what()));
			}
		}	 // end while
	}
	
	size_t RequestHandler::WriteCallBack(char* data, size_t size, size_t nmemb, std::string& clientp)
	{
		size_t realsize = size * nmemb;
		clientp.append(data, realsize);
		return realsize;
	}

	RequestHandler::~RequestHandler()
	{
		terminate = true;
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
