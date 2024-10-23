#pragma once
#include <shared_mutex>

namespace Lovense
{
  struct Toy
	{
		enum class Category
		{
			Any = 0,

			Genital,
			Anal,
			Breasts,
		};

	public:
    Toy(const std::string& a_id, const std::string& a_name, Category a_category = Category::Any) :
      id(a_id), name(a_name), category(a_category) {}
    ~Toy() = default;

		std::string id;
		std::string name;
		Category category;
	};

	struct Connection final
	{
		Connection() = delete;

    static std::string GetIP_ADDR();
    static std::string GetPort();
    static void VisitToys(std::function<bool(const Toy&)> a_visitor);

    static void SetIP_ADDR(const std::string& a_addr);
    static void SetPORT(const std::string& a_port);
    static void UpdateToyList();

  private:
    static inline std::shared_mutex _m;
		static inline std::string IP_ADDR{ "" };
		static inline std::string PORT{ "" };
    static inline std::vector<Toy> devices{};
	};

} // namespace Lovense
