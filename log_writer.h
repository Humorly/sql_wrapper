#pragma once

#ifndef __H_LOG_WRITER_H__
#define __H_LOG_WRITER_H__

// write program log
// Add by humorly 2017/7/21

#include <ctime>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

namespace wstd
{
#define __FILE_LINE__	__FILE__, __LINE__

	// std::size_t to string
	inline std::string to_string(std::size_t value);

	class log_writer;
	// log writer set
	class log_writer
	{
	public:
		log_writer() {}
		virtual ~log_writer() 
		{
			if (!mutex_.try_lock())
			{
				mutex_.unlock();
			}
		}

		// lock
		void lock() 
		{
			mutex_.lock();
		}

		// unlock
		void unlock()
		{
			mutex_.unlock();
		}

		// write log
		static log_writer & log_store(const std::string str, const char * file, int line)
		{
			static log_writer log_;
			log_.lock();
			log_.write(str, file, line);
			log_.unlock();
			return log_;
		}

	private:

		// ´´½¨»¥³âËø
		std::mutex mutex_;

		enum
		{
			invalid_ptr = 0,
		};

		// write log
		void write(const std::string & str, const char * file = __FILE__, const int line = __LINE__) 
		{

			// get current time now
			std::time_t time_ = std::time(reinterpret_cast<time_t *>(invalid_ptr));
			tm * tm_ = std::localtime(&time_);
			assert(tm_);

			// make file name
			std::string name_ = std::string("log_writer_") + wstd::to_string(tm_->tm_year + 1900) + std::string("-") +
				wstd::to_string(tm_->tm_mon + 1) + std::string("-") + wstd::to_string(tm_->tm_mday) + std::string(".log");

			// make now time
			std::string now_time_ = wstd::to_string(tm_->tm_year + 1900) + std::string("-") +
				wstd::to_string(tm_->tm_mon + 1) + std::string("-") + wstd::to_string(tm_->tm_mday) + std::string("-") +
				wstd::to_string(tm_->tm_hour) + std::string(":") + wstd::to_string(tm_->tm_min) + std::string(":")
				+ wstd::to_string(tm_->tm_sec);

			// cut file name
			std::string file_(file);
			std::size_t npos_ = file_.rfind("\\");
			if (std::string::npos != npos_) 
			{
				file_ = file_.substr(npos_);
			}

			// write log to file
			std::ofstream of_(name_.c_str(), std::ios::out | std::ios::app);
			if (of_.is_open())
			{
				of_ << "[log start. ";
				of_ << now_time_ << ". ";
				of_ << "" << file_ << ". " << "line " << line << "]";
				of_ << " " << str << "\n";
				of_.close();
			}
		}
	};

	// std::size_t to string
	inline std::string to_string(std::size_t value)
	{
		char num_[128] = { 0 };
		std::memset(num_, 0, sizeof(num_));
		sprintf(num_, "%02zd", value);
		return std::string(num_);
	}
}

#endif // !__H_LOG_WRITER_H__
