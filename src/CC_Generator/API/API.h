#pragma once
#include "Dependencies/sqlite/sqlite3.h"
#include <sys/stat.h> 
#include <string>
#include <functional>
#include <memory>


namespace API
{
	bool check_file_exists(const std::string& filename);
	std::shared_ptr<sqlite3> read_db(const std::string& db_path);
	bool is_sqlite_database(sqlite3*& db);
}