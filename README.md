# sql_warpper
封装mysql官方的cppconn c++接口的更高级封装，使增删改查变得更加modern c++。

测试代码:

```c++
#include "sql_warpper.h"

int main()
{
	sql_warpper sql_("tcp://127.0.0.1:3306", "root", "123456", "game");
	bool ret_ = false;

	// 创建
	ret_ = sql_.create("create table test (id_ double, name_ varchar(64), password_ varchar(64)); ");

	// 添加
	ret_ = sql_.insert("insert into test (id_, name_, password_) values (0.1, '0.2', '0.3');"),
	// 添加 —> 带参数
	ret_ = sql_.insert("insert into test (id_, name_, password_) values (?, ?, ?);",
		std::tuple<double, std::string, std::string>(0.11118, "0.2", "0.2"));
	// 添加 —> 带参数
	ret_ = sql_.insert("insert into test (id_, name_, password_) values (?, ?, ?);",
		std::tuple<double, std::string, std::string>(0.8, "0.2", "0.2"));

	// 删除
	ret_ = sql_.remove("delete from test where id_ = '0.3';");
	// 删除 —> 带参数
	ret_ = sql_.remove("delete from test where id_ = ?;", std::tuple<std::string>("0.11118"));

	// 更新
	ret_ = sql_.update("update test set id_ = 0.1, name_ = 'test' where password_ = 0.1;"),
	// 更新 —> 带参数
	ret_ = sql_.update("update test set id_ = ?, name_ = ? where password_ = ?;",
		std::tuple<double, std::string, std::string>(0.2, "humorly", "0.2"));

	std::vector<std::tuple<double, std::string, std::string>> user_content_;
	// 查询
	ret_ = sql_.select("select * from test;",
		std::tuple <>(),
		std::tuple<std::string, std::string, std::string>("id_", "name_", "password_"),
		user_content_);

	std::cout << "content:" << std::endl;
	for (auto& val : user_content_)
	{
		std::cout << "id_ = " << std::get<0>(val) << ", ";
		std::cout << "name_ = " << std::get<1>(val) << ", ";
		std::cout << "password_ = " << std::get<2>(val) << std::endl;
	}

	// 查询 —> 带参数
	user_content_.clear();
	ret_ = sql_.select("select id_, name_, password_ from test where password_ = ?;",
		std::tuple <std::string>("0.2"),
		std::tuple<std::string, std::string, std::string>("id_", "name_", "password_"),
		user_content_);

	std::cout << "content:" << std::endl;
	for (auto& val : user_content_)
	{
		std::cout << "id_ = " << std::get<0>(val) << ", ";
		std::cout << "name_ = " << std::get<1>(val) << ", ";
		std::cout << "password_ = " << std::get<2>(val) << std::endl;
	}

	return 0;
}
  
  ```
