#pragma once
#ifndef __H_SQL_WARPPER_H__
#define __H_SQL_WARPPER_H__

#include <iostream>
#include <string>
#include "log_writer.h"
#include "sql_operate_ipml.h"

class sql_warpper : public sql_operate_ipml
{
public:
	sql_warpper(const std::string & host, const std::string & user, 
		const std::string & pwd, const std::string & db) 
		: host_(host), user_(user), pwd_(pwd), db_(db)
	{
		try
		{
			// 创建连接
			driver_ = get_driver_instance();
			// 连接数据库
			con_ = driver_->connect(host_.c_str(), user_.c_str(), pwd_.c_str());
			// 指向指定路径
			con_->setSchema(db_.c_str());
			// 创建执行器
			stmt_ = con_->createStatement();
		}
		catch (sql::SQLException & e)
		{
			std::string str_logger_("sql error is "
				+ std::to_string(e.getErrorCode()) + " & describe is " + std::string(e.what()));
			wstd::log_writer::log_store(str_logger_, __FILE_LINE__);
		}
	}

	virtual ~sql_warpper() 
	{
		delete stmt_;
		delete con_;
	}

	// 创建
	bool create(const std::string & command) { return invoke(command); }
	// 增加
	bool insert(const std::string & command) { return invoke(command); }
	// 删除
	bool remove(const std::string & command) { return invoke(command); }
	// 更新 
	bool update(const std::string & command) { return invoke(command); }
	// 查询
	template <typename __set, typename __type, typename ... params>
	bool select(const std::string & command, std::vector<std::tuple<__type, params...>> & dest, __set parm)
	{
		try 
		{
			pstmt_ = con_->prepareStatement(command.c_str());
			res_ = pstmt_->executeQuery();

			res_->afterLast();
			while (res_->previous())
			{
				separation<std::tuple<__type, params...>, sql::ResultSet, __set, __type, params...>(dest, res_, parm);
			}
			delete res_;
			delete pstmt_;
		}
		catch (sql::SQLException &e) 
		{
			std::string str_logger_("sql error by select command -> code is " + 
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
		try 
		{
			if (nullptr != stmt_)
				stmt_->execute(command.c_str());
		}
		catch (sql::SQLException &e) 
		{
			std::string str_logger_("sql error by execute command -> code is "
				+ std::to_string(e.getErrorCode()) + " & describe is " + std::string(e.what()));
			wstd::log_writer::log_store(str_logger_, __FILE_LINE__);
			return false;
		}
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

