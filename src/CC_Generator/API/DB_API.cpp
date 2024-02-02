#include "DB_API.h"

/**
 * @brief Checks if the provided SQLite database is valid and accessible.
 *
 * This function executes a simple query on the given SQLite database to check its validity.
 * If the database is valid and accessible, it returns true. If any error occurs during
 * the query execution or if the database is not valid, it returns false.
 *
 * @param db A reference to the SQLite database object to be checked for validity.
 * @return True if the database is valid and accessible; false otherwise.
 */
bool DB_API::is_sqlite_database(sqlite3*& db)
{
	// Execute a simple query to check if the database is valid
	const char* query{ "SELECT name FROM sqlite_master WHERE type='table' LIMIT 1" };
	char* err_msg{ nullptr };
	int result = sqlite3_exec(db, query, nullptr, nullptr, &err_msg);

	if (result != SQLITE_OK && err_msg != nullptr)
	{
		std::cerr << "Error executing query: " << err_msg << std::endl;
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return false;
	}

	return true;
}

/**
 * @brief Reads an SQLite database from the specified path.
 *
 * This function attempts to open an SQLite database located at the given
 * file path. If the database opens successfully and is a valid SQLite database,
 * it returns a shared pointer to the opened database. If any error occurs during
 * the opening process, or if the file is not a valid SQLite database, it returns nullptr.
 *
 * @param db_path The path to the SQLite database file to be opened.
 * @return A shared pointer to the opened SQLite database if successful; nullptr otherwise.
 *
 * @see DB_API::is_sqlite_database(sqlite3* db)
 */
std::shared_ptr<sqlite3> DB_API::read_db(const std::string& db_path)
{
	sqlite3* db;
	if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK)
	{
		return nullptr;
	}
	if (DB_API::is_sqlite_database(db) == false)
	{
		return nullptr;
	}
	return std::shared_ptr<sqlite3> {db, [](sqlite3* ptr) {if (ptr) { sqlite3_close(ptr); }}};
}

/**
 * @brief Checks if a file exists in the file system.
 *
 * This function determines whether a file with the given filename exists
 * in the file system. It checks the existence of the file by using the stat
 * system call. The function returns true if the file exists and is accessible,
 * and false otherwise.
 *
 * @param filename The path to the file to be checked for existence.
 * @return True if the file exists and is accessible; false otherwise.
 */
bool DB_API::check_file_exists(const std::string& filename)
{
	struct stat buffer;
	return ((filename.empty() == false) &&
		(stat(filename.c_str(), &buffer) == 0));
}

/**
 * @brief Reads credit card data from the database.
 *
 * This function retrieves credit card data from the database and populates
 * a vector with Card objects containing valid card information.
 *
 * @param db A shared pointer to the SQLite database.
 * @param data A vector to store Card objects with valid credit card information.
 * @param error_msg A string reference to store error messages, if any.
 * @return An SQLite error code. SQLITE_OK if successful, otherwise an error code.
 *
 */
int DB_API::read_cards(std::shared_ptr<sqlite3> db, std::vector<Card>& data, std::string& error_msg)
{
	char* err{};
	std::vector<Card> temp_vec{};
	const char* selectStatement{ "SELECT Issuer, Length, Prefixes FROM cards_table;" };
	sqlite3_stmt* stmt{ nullptr };

	int rc{ sqlite3_prepare_v2(db.get(), selectStatement, -1, &stmt, nullptr) };

	if (rc != SQLITE_OK)
	{
		const char* table_statement = "select * from sqlite_master where type='table' and name='cards_table';";
		if (sqlite3_prepare_v2(db.get(), table_statement, -1, &stmt, nullptr) == 0)
		{
			rc = SQLITE_OK;
		}
		sqlite3_finalize(stmt);
		error_msg = "Prepare statement error: " + std::string(sqlite3_errmsg(db.get()));
		return rc;
	}

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		std::string issuer{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)) };
		int length{ sqlite3_column_int(stmt, 1) };
		std::string prefixes{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) };

		// skip invalid cards
		if (Card::validate_card(issuer, length, prefixes))
		{
			temp_vec.push_back(Card{ issuer, length, prefixes });
		}
	}

	if (rc != SQLITE_DONE)
	{
		error_msg = "Step statement error: " + std::string(sqlite3_errmsg(db.get()));
		return rc;
	}

	sqlite3_finalize(stmt);
	data = temp_vec;
	return SQLITE_OK;
}

/**
 * @brief Writes credit card data to the database, updating existing entries.
 *
 * This function creates a table for credit card data if it doesn't exist,
 * then updates existing entries and inserts new ones based on the provided data.
 *
 * @param db A shared pointer to the SQLite database.
 * @param data A vector of Card objects containing credit card information.
 * @param error_msg A string reference to store error messages, if any.
 * @return An SQLite error code. SQLITE_OK if successful, otherwise an error code.
 *
 * @note The function assumes that the Card class has methods such as `get_issuer()`,
 *       `get_len()`, and `get_prefixes()` to retrieve the respective card details.
 */
int DB_API::write_cards(std::shared_ptr<sqlite3> db, const std::vector<Card>& data, std::string& error_msg)
{
	// create a table if it doesn't exist
	char* err{};
	int rc{ sqlite3_exec(db.get(), "CREATE TABLE IF NOT EXISTS cards_table (Issuer TEXT, Length INTEGER, Prefixes TEXT);", 0, 0, &err) };

	if (rc != SQLITE_OK)
	{
		error_msg = "Failed with error: " + std::string(err);
		return rc;
	}

	std::string query{};

	// loop through the vector - skip existing cards and write new cards
	sqlite3_exec(db.get(), "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

	bool first_iter{ true };
	std::stringstream ss;
	ss << "DELETE FROM cards_table";
	if (data.empty() == false)
	{
		ss << " WHERE ";
	}

	for (const auto& card : data)
	{
		// If the card doesn't exist in the database insert it
		query = "INSERT INTO cards_table (Issuer, Length, Prefixes) "
			"SELECT ?, ?, ? "
			"WHERE NOT EXISTS (SELECT 1 FROM cards_table WHERE Issuer = ? AND Length = ? AND Prefixes = ?)";

		sqlite3_stmt* stmt;
		rc = sqlite3_prepare_v2(db.get(), query.c_str(), -1, &stmt, nullptr);

		if (rc != SQLITE_OK)
		{
			error_msg = "Error preparing select statement: " + std::string(sqlite3_errmsg(db.get()));
			sqlite3_finalize(stmt);
			return rc;
		}
		std::string issuer_str = card.get_issuer();
		std::string prefixes_str = card.get_prefixes();

		const char* issuer = issuer_str.c_str();
		const char* prefixes = prefixes_str.c_str();

		sqlite3_bind_text(stmt, 1, issuer, -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, card.get_len());
		sqlite3_bind_text(stmt, 3, prefixes, -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, issuer, -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 5, card.get_len());
		sqlite3_bind_text(stmt, 6, prefixes, -1, SQLITE_STATIC);

		if ((rc = sqlite3_step(stmt)) != SQLITE_DONE)
		{
			error_msg = "Failed to insert values: " + std::string(sqlite3_errmsg(db.get()));
			sqlite3_finalize(stmt);
			return rc;
		}

		// delete query
		if (!first_iter)
		{
			ss << " AND ";
		}

		ss << "NOT (Issuer='" << issuer_str << "' AND Length='" << card.get_len() << "' AND Prefixes='" << prefixes_str << "')";
		first_iter = false;
	}

	ss << ";";

	std::string delete_query{ ss.str() };

	if ((rc = sqlite3_exec(db.get(), delete_query.c_str(), 0, 0, &err)) != SQLITE_OK)
	{
		error_msg = "Error deleting data from database: " + std::string(sqlite3_errmsg(db.get()));
		return rc;
	}

	sqlite3_exec(db.get(), "COMMIT", nullptr, nullptr, nullptr);
	return SQLITE_OK;
}
