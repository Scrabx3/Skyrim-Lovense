#pragma once
#include <shared_mutex>

#include "Define/Category.h"

namespace Lovense
{
	struct Toy
	{
		Toy(const std::string& a_id, const std::string& a_name, Category a_category) :
			id(a_id), name(a_name), category(a_category) {}
		~Toy() = default;

		std::string id;
		std::string name;
		Category category;

	public:
		bool operator==(const Toy& a_rhs) const { return id == a_rhs.id; }
	};

	struct Connection final
	{
		Connection() = delete;

		static std::string GetIP_ADDR();
		static std::string GetPort();
		static Category GetCategory(std::string_view a_id);
		static void VisitToys(std::function<bool(const Toy&)> a_visitor);

		static void SetIP_ADDR(const std::string& a_addr);
		static void SetPORT(const std::string& a_port);
		static void UpdateToyList(const json& a_toys);
		static void ClearToyList();
		static void AssignCategory(std::string_view a_id, Category a_category);

	private:
		static Category GetCategoryImpl(std::string_view a_id);

		static inline std::shared_mutex _m;
		static inline std::string IP_ADDR{ "" };
		static inline std::string PORT{ "" };
		static inline std::vector<Toy> devices{};
	};

}	 // namespace Lovense
