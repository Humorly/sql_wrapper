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
	std::vector<__dest_type> & separation(std::vector<__dest_type> & dest, __value_type * src, __set parm)
	{
		__dest_type tu_;
		constexpr int size_ = sizeof...(params);
		get_class<0, size_>::template get<__dest_type, __value_type, __set, T, params...>(tu_, src, parm);

		dest.push_back(tu_);
		return dest;
	}

	// 展开参数
	template<integer_type N, integer_type M>
	struct get_class
	{
		template <typename __dest, typename __src, typename __set, typename T, typename ... params>
		inline static void get(__dest & dest, __src * src, __set parm)
		{
			constexpr integer_type index_ = N + 1;
			// 赋值元素N并递归展开
			std::get<N>(dest) = get_value_type<T, __src, const char *>::template get_value(src, std::get<N>(parm).c_str());
			get_class<index_, M>::template get<__dest, __src, __set, params...>(dest, src, parm);
		}
	};

	// 递归到末元素
	template<integer_type M>
	struct get_class<M, M>
	{
		template <typename __dest, typename __src, typename __set, typename T>
		inline static void get(__dest & dest, __src * src, __set parm)
		{
			// 末元素赋值
			std::get<M>(dest) = get_value_type<T, __src, const char *>::template get_value(src, std::get<M>(parm).c_str());
		}
	};

	// 获取参数
	template<typename __value_type, typename __dest_type, typename __parm>
	struct get_value_type
	{
		inline static auto get_value(__dest_type * val, __parm parm) {}
	};

	// bigint
	template<typename __dest_type, typename __parm>
	struct get_value_type<integer_type, __dest_type, __parm>
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
		inline static auto get_value(__dest_type * val, __parm parm) -> decltype(val->getDouble(parm))
		{
			return val->getDouble(parm);
		}
	};

};

