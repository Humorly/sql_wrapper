#pragma once
#ifndef __H_SQL_WARPPER_H__
#define __H_SQL_WARPPER_H__

#include <iostream>
#include <string>
#include "log_writer.h"
#include "sql_operate_ipml.h"
#pragma comment (lib, "mysqlcppconn.lib")

class sql_warpper : public sql_operate_ipml
{
public:
	sql_warpper(const std::string & host, const std::string & user, 
		const std::string & pwd, const std::string & db) 
		: host_(host), user_(user), pwd_(pwd), db_(db)
	{
		/* Create a connection */
		driver_ = get_driver_instance();
		//con_ = driver_->connect("tcp://127.0.0.1:3306", "root", "407043");
		con_ = driver_->connect(host_.c_str(), user_.c_str(), pwd_.c_str());
		/* Connect to the MySQL test database */
		con_->setSchema(db_.c_str());

		stmt_ = con_->createStatement();
	}

	virtual ~sql_warpper() 
	{
		stmt_->close();
		con_->close();

		delete stmt_;
		delete con_;
	}

	// 创建
	bool create(const std::string & command) { return invoke(command); }
	// 增加
	bool insert(const std::string & command) { return invoke(command); }
	// 删除
	bool remove(const std::string & command) { return invoke(command); }

	// 删除
	template <typename __type, typename ... params>
	bool remove(const std::string& command, std::tuple<__type, params...> val)
	{
		return __invoke_template(command, val);
	}

	// 更新 
	bool update(const std::string & command) { return invoke(command); }
	// 执行
	bool execute(const std::string & command) { return invoke(command); }

	// 增加 —> insert命令由command指出
	template <typename __type, typename ... params>
	bool insert(const std::string& command, std::tuple<__type, params...> val)
	{
		return __invoke_template(command, val);
	}

	// 增加 —> 表名由参数table_name指出
	template <typename __set, typename __type, typename ... params>
	bool insert(const std::string& table_name, __set src, std::tuple<__type, params...> val)
	{
		// 合成前缀
		std::string command_ = "insert into " + table_name + " (";

		// 合成字段
		std::vector<std::string> parameter_;
		parameter_ = expand<0>(src);

		for (auto i = 0; i < parameter_.size(); i++)
		{
			command_ += parameter_[i];
			if (i != parameter_.size() - 1)
				command_ += ", ";
		}
		command_ += ") values (";

		for (auto i = 0; i < parameter_.size(); i++)
		{
			command_ += "?";
			if (i != parameter_.size() - 1)
				command_ += ",";
		}
		command_ += ");";

		return __invoke_template(command_, val);
	}

	// 增加 —> 表名在src中的第一个参数
	template <typename __table_type, typename ... params, typename __type>
	bool insert(std::tuple<__table_type, params...> src, __type val)
	{
		// 合成前缀
		std::string command_ = "insert into " + std::get<0>(src) + " (";

		// 合成字段
		// 合成字段
		std::vector<std::string> parameter_;
		parameter_ = expand<1>(src);

		for (auto i = 0; i < parameter_.size(); i++)
		{
			command_ += parameter_[i];
			if (i != parameter_.size() - 1)
				command_ += ", ";
		}
		command_ += ") values (";

		for (auto i = 0; i < parameter_.size(); i++)
		{
			command_ += "?";
			if (i != parameter_.size() - 1)
				command_ += ",";
		}
		command_ += ");";

		return __invoke_template(command_, val);
	}

	// 更新 —> update命令由command指出
	template <typename __type, typename ... params>
	bool update(const std::string& command, std::tuple<__type, params...> val)
	{
		return __invoke_template(command, val);
	}

	// 更新 
	template <typename __set, typename __type, typename ... params>
	bool update(const std::string& table_name, __set src, std::tuple<__type, params...> val)
	{
		// 合成前缀
		std::string command_ = "update " + table_name + " set ";

		// 合成字段
		std::vector<std::string> parameter_;
		parameter_ = expand<0>(src);

		for (auto i = 0; i < parameter_.size() - 1; i++)
		{
			command_ += parameter_[i] + " = ?";
			if (i != parameter_.size() - 2)
				command_ += ", ";
		}
		command_ += " where " + parameter_[parameter_.size() - 1] + " = ?;";

		return __invoke_template(command_, val);
	}

	// 更新
	template <typename __table_type, typename ... params, typename __type>
	bool update(std::tuple<__table_type, params...> src, __type val)
	{
		// 合成前缀
		std::string command_ = "update " + std::get<0>(src) + " set ";

		// 合成字段
		std::vector<std::string> parameter_;
		parameter_ = expand<1>(src);

		for (auto i = 0; i < parameter_.size() - 1; i++)
		{
			command_ += parameter_[i] + " = ?";
			if (i != parameter_.size() - 2)
				command_ += ", ";
		}
		command_ += " where " + parameter_[parameter_.size() - 1] + " = ?;";

		return __invoke_template(command_, val);
	}

	// 查询
	template <typename __holder_type, typename __set, typename __type, typename ... params>
	bool select(const std::string & command, 
		__holder_type holder, __set parm, 
		std::vector<std::tuple<__type, params...>> & dest)
	{
		try {
			// 构建命令
			pstmt_ = con_->prepareStatement(command);
			// 合成参数
			synthesis(pstmt_, holder);

			// 执行查询
			res_ = pstmt_->executeQuery();
			// 反向提取
			res_->afterLast();
			while (res_->previous())
			{
				separation<std::tuple<__type, params...>, sql::ResultSet, __set, __type, params...>(dest, res_, parm);
			}
			delete res_;
			delete pstmt_;
		}
		catch (sql::SQLException &e) {
			std::string str_logger_("sql error by create select -> code is " + 
				std::to_string(e.getErrorCode()) + " & describe is " + std::string(e.what()));
			wstd::log_writer::log_store(str_logger_, __FILE_LINE__);
			return false;
		}
		return true;
	}

private:
	// 执行命令
	bool invoke(const std::string & command)
	{
		try {
			//stmt_->execute("DROP TABLE IF EXISTS test");
			stmt_->execute(command.c_str());
		}
		catch (sql::SQLException &e) {
			std::string str_logger_("sql error by update command -> code is "
				+ std::to_string(e.getErrorCode()) + " & describe is " + std::string(e.what()));
			wstd::log_writer::log_store(str_logger_, __FILE_LINE__);
			return false;
		}
		return true;
	}

	// 执行模板
	template <typename __type>
	bool __invoke_template(const std::string& command, __type& val)
	{
		try {
			pstmt_ = con_->prepareStatement(command);
		}
		catch (sql::SQLException& e) {
			std::string str_logger_("sql error by create select -> code is " +
				std::to_string(e.getErrorCode()) + " & describe is " + std::string(e.what()));
			wstd::log_writer::log_store(str_logger_, __FILE_LINE__);
			return false;
		}

		try {
			// 合成参数
			synthesis(pstmt_, val);
			pstmt_->execute();
		}
		catch (sql::SQLException& e) {
			delete pstmt_;

			std::string str_logger_("sql error by create select -> code is " +
				std::to_string(e.getErrorCode()) + " & describe is " + std::string(e.what()));
			wstd::log_writer::log_store(str_logger_, __FILE_LINE__);
			return false;
		}

		delete pstmt_;
		return true;
	}

	std::string host_;
	std::string user_;
	std::string pwd_;
	std::string db_;

	sql::Driver * driver_;
	sql::Connection * con_;
	sql::Statement * stmt_;
	sql::ResultSet * res_;
	sql::PreparedStatement * pstmt_;
};

#endif

