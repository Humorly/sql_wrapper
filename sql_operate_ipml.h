#pragma once

#include <tuple>
#include <vector>
#include "wstddef.h"

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class sql_operate_ipml
{
public:
	sql_operate_ipml() {}
	virtual ~sql_operate_ipml() {}

protected:
	template <typename __dest_type, typename __value_type, typename __set, typename T, typename ... params>
	std::vector<__dest_type>& separation(std::vector<__dest_type> & dest, __value_type * src, __set parm)
	{
		__dest_type tu_;
		constexpr int size_ = sizeof...(params);
		get_class<0, size_>::template get<__dest_type, __value_type, __set, T, params...>(tu_, src, parm);

		dest.push_back(tu_);
		return dest;
	}

	// 展开参数
	template<integer64_type N, integer64_type M>
	struct get_class
	{
		template <typename __dest, typename __src, typename __set, typename T, typename ... params>
		inline static void get(__dest & dest, __src * src, __set parm)
		{
			constexpr integer64_type index_ = N + 1;
			// 赋值元素N并递归展开
			std::get<N>(dest) = get_value_type<T, __src, const char *>::template get_value(src, std::get<N>(parm).c_str());
			get_class<index_, M>::template get<__dest, __src, __set, params...>(dest, src, parm);
		}
	};

	// 递归到末元素
	template<integer64_type M>
	struct get_class<M, M>
	{
		template <typename __dest, typename __src, typename __set, typename T>
		inline static void get(__dest & dest, __src * src, __set parm)
		{
			// 末元素赋值
			std::get<M>(dest) = get_value_type<T, __src, const char *>::template get_value(src, std::get<M>(parm).c_str());
		}
	};

	// 分离型别偏特化
	template<typename __value_type, typename __dest_type, typename __parm>
	struct get_value_type
	{
		inline static auto get_value(__dest_type * val, __parm parm) {}
	};

	// bool
	template<typename __dest_type, typename __parm>
	struct get_value_type<bool, __dest_type, __parm>
	{
		inline static auto get_value(__dest_type* val, __parm parm) -> decltype(val->getBoolean(parm))
		{
			return val->getBoolean(parm);
		}
	};

	// int
	template<typename __dest_type, typename __parm>
	struct get_value_type<int, __dest_type, __parm>
	{
		inline static auto get_value(__dest_type* val, __parm parm) -> decltype(val->getInt(parm))
		{
			return val->getInt(parm);
		}
	};

	// unsigned int
	template<typename __dest_type, typename __parm>
	struct get_value_type<unsigned int, __dest_type, __parm>
	{
		inline static auto get_value(__dest_type* val, __parm parm) -> decltype(val->getUInt(parm))
		{
			return val->getUInt(parm);
		}
	};

	// unsigned int 64
	template<typename __dest_type, typename __parm>
	struct get_value_type<unsigned long long, __dest_type, __parm>
	{
		inline static auto get_value(__dest_type* val, __parm parm) -> decltype(val->getUInt64(parm))
		{
			return val->getUInt64(parm);
		}
	};

	// bigint
	template<typename __dest_type, typename __parm>
	struct get_value_type<integer64_type, __dest_type, __parm>
	{
		inline static auto get_value(__dest_type * val, __parm parm) -> decltype(val->getInt64(parm))
		{
			return val->getInt64(parm);
		}
	};

	// varchar
	template<typename __dest_type, typename __parm>
	struct get_value_type<std::string, __dest_type, __parm>
	{
		inline static std::string get_value(__dest_type * val, __parm parm)
		{
			return val->getString(parm).c_str();
		}
	};

	// double
	template<typename __dest_type, typename __parm>
	struct get_value_type<double, __dest_type, __parm>
	{
		inline static auto get_value(__dest_type* val, __parm parm) -> decltype(val->getDouble(parm))
		{
			return val->getDouble(parm);
		}
	};

	// 合成参数
	template <typename __value_type, typename ... params>
	void synthesis(__value_type* statement, std::tuple<params...>& node)
	{
		constexpr integer64_type N = sizeof...(params);
		make_class<0, N - 1>::template make<__value_type, std::tuple<params...>, params...>(statement, node);
	}

	template <typename __value_type>
	void synthesis(__value_type* statement, std::tuple<>& node)
	{
	}

	// 展开参数
	template<integer64_type N, integer64_type M>
	struct make_class
	{
		template <typename __value_type, typename __set, typename __part_type, typename ... params>
		inline static void make(__value_type* statement, __set& node)
		{
			// 赋值元素N并递归展开
			constexpr integer64_type index_ = N + 1;
			make_value_type<__part_type, __value_type, integer64_type, __part_type>::template set_value(statement, N, std::get<N>(node));
			make_class<index_, M>::template make<__value_type, __set, params...>(statement, node);
		}
	};

	// 递归到末元素
	template<integer64_type M>
	struct make_class<M, M>
	{
		template <typename __value_type, typename __set, typename __part_type, typename ... params>
		inline static void make(__value_type* statement, __set& node)
		{
			// 末元素赋值
			make_value_type<__part_type, __value_type, integer64_type, __part_type>::template set_value(statement, M, std::get<M>(node));
		}
	};

	// 合成型别偏特化
	template<typename __part_type, typename __value_type, typename __index_type, typename __parm>
	struct make_value_type
	{
		inline static void set_value(__value_type* statement, __index_type N, __parm parm) {}
	};

	// bool
	template<typename __value_type, typename __index_type, typename __parm>
	struct make_value_type<bool, __value_type, __index_type, __parm>
	{
		inline static void set_value(__value_type* statement, __index_type N, __parm parm)
		{
			return statement->setBoolean(N + 1, parm);
		}
	};

	// int
	template<typename __value_type, typename __index_type, typename __parm>
	struct make_value_type<int, __value_type, __index_type, __parm>
	{
		inline static void set_value(__value_type* statement, __index_type N, __parm parm)
		{
			return statement->setInt(N + 1, parm);
		}
	};

	// unsigned int
	template<typename __value_type, typename __index_type, typename __parm>
	struct make_value_type<unsigned int, __value_type, __index_type, __parm>
	{
		inline static void set_value(__value_type* statement, __index_type N, __parm parm)
		{
			return statement->setUInt(N + 1, parm);
		}
	};
	 
	// int 64
	template<typename __value_type, typename __index_type, typename __parm>
	struct make_value_type<integer64_type, __value_type, __index_type, __parm>
	{
		inline static void set_value(__value_type* statement, __index_type N, __parm parm)
		{
			return statement->setInt64(N + 1, parm);
		}
	};

	// unsigned int 64
	template<typename __value_type, typename __index_type, typename __parm>
	struct make_value_type<unsigned long long, __value_type, __index_type, __parm>
	{
		inline static void set_value(__value_type* statement, __index_type N, __parm parm)
		{
			return statement->setUInt64(N + 1, parm);
		}
	};

	// double
	template<typename __value_type, typename __index_type, typename __parm>
	struct make_value_type<double, __value_type, __index_type, __parm>
	{
		inline static void set_value(__value_type* statement, __index_type N, __parm parm)
		{
			return statement->setDouble(N + 1, parm);
		}
	};

	// string
	template<typename __value_type, typename __index_type, typename __parm>
	struct make_value_type<std::string, __value_type, __index_type, __parm>
	{
		inline static void set_value(__value_type* statement, __index_type N, __parm parm)
		{
			return statement->setString(N + 1, parm.c_str());
		}
	};

	// 展开tuple
	template <integer64_type N, typename T, typename ... params>
	inline static std::vector<std::string> expand(std::tuple<T, params...>& parm)
	{
		std::vector<std::string> dest_;
		constexpr int size_ = sizeof...(params);
		// 展开tuple
		get_tuple<N, size_>::template get<std::tuple<T, params...>>(dest_, parm);

		return dest_;
	}

	// 展开参数
	template<integer64_type N, integer64_type M>
	struct get_tuple
	{
		template <typename __set>
		inline static void get(std::vector<std::string>& dest, __set& parm)
		{
			constexpr integer64_type index_ = N + 1;
			// 赋值元素N并递归展开
			dest.push_back(std::get<N>(parm));
			get_tuple<index_, M>::template get<__set>(dest, parm);
		}
	};

	// 递归到末元素
	template<integer64_type M>
	struct get_tuple<M, M>
	{
		template <typename __set>
		inline static void get(std::vector<std::string>& dest, __set& parm)
		{
			// 末元素赋值
			dest.push_back(std::get<M>(parm));
		}
	};
};

