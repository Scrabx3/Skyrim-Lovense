#include "Connection.h"

namespace Lovense
{
	std::string Connection::GetIP_ADDR()
	{
		std::shared_lock lock(_m);
		return IP_ADDR;
	}

	std::string Connection::GetPort()
	{
		std::shared_lock lock(_m);
		return PORT;
	}

	void Connection::VisitToys(std::function<bool(const Toy&)> a_visitor)
	{
		std::shared_lock lock(_m);
		for (const auto& toy : devices) {
			if (!a_visitor(toy)) {
				break;
			}
		}
	}

	void Connection::SetIP_ADDR(const std::string& a_addr)
	{
		std::unique_lock lock(_m);
		IP_ADDR = a_addr;
	}

	void Connection::SetPORT(const std::string& a_port)
	{
		std::unique_lock lock(_m);
		PORT = a_port;
	}

	void Connection::UpdateToyList()
	{
		std::unique_lock lock(_m);
		// TODO: Implementation to update the devices vector
	}

}	 // namespace Lovense
