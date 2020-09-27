# sql_warpper
封装mysql官方的cppconn c++接口的更高级封装，使增删改查变得更加modern c++。

测试代码:

```c++
	sql_warpper sql_("tcp://127.0.0.1:3306", "root", "123456", "gamed");
	bool ret_ = false;
	ret_ = sql_.create("create table test (id_ double, name_ varchar(64), password_ varchar(64)); ");
	ret_ = sql_.insert("insert into test values(5.12056683, \"humorly\", \"chen123\");");
	ret_ = sql_.insert("insert into test values(5.12056684, \"humorly\", \"chen123\");");
	ret_ = sql_.insert("insert into test values(5.12056685, \"humorly\", \"chen123\");");
	ret_ = sql_.remove("delete from test  where id_ = 5.12056684;");
	ret_ = sql_.update("update game.test set id_ = 5.55555555 where id_ = 5.12056683;");

	// 待搜索的字段信息
	std::tuple<std::string, std::string, std::string> params_("id_", "name_", "password_");
	std::vector<std::tuple<double, std::string, std::string>> user_content_;
	ret_ = sql_.select("SELECT * FROM test", user_content_, params_);

	std::cout << "content:" << std::endl;
	for (auto & val : user_content_)
	{
		std::cout << "id_ = " << std::get<0>(val) << ", ";
		std::cout << "name_ = " << std::get<1>(val) << ", ";
		std::cout << "password_ = " << std::get<2>(val) << std::endl;
	}

	return 0;
  
  ```
