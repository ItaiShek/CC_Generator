#include "API.h"
#include <iostream>


bool API::is_sqlite_database(sqlite3*& db)
{
	// Execute a simple query to check if the database is valid
	const char* query = "SELECT name FROM sqlite_master WHERE type='table' LIMIT 1";
	char* errMsg = nullptr;
	int result = sqlite3_exec(db, query, nullptr, nullptr, &errMsg);

	if (result != SQLITE_OK && errMsg != nullptr)
	{
		std::cerr << "Error executing query: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return false;
	}

	return true;
}


std::shared_ptr<sqlite3> API::read_db(const std::string& db_path)
{
	sqlite3* db;
	if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK)
	{
		return nullptr;
	}
	if (API::is_sqlite_database(db) == false)
	{
		return nullptr;
	}
	return std::shared_ptr<sqlite3> {db, [](sqlite3* ptr) {if (ptr) { sqlite3_close(ptr); }}};
}


bool API::check_file_exists(const std::string& filename)
{
	struct stat buffer;
	return ((filename.empty() == false) && 
			(stat(filename.c_str(), &buffer) == 0));
}