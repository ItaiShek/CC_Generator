#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <sys/stat.h>
#include "Dependencies/sqlite/sqlite3.h"
#include "Card.h"
#include "File.h"

/**
 * @class DB_API
 * @brief Provides functionality for interacting with an SQLite database and validating credit card data.
 *
 * The DB_API class contains static member functions for various database-related operations
 * such as checking file existence, reading a database, validating issuer, prefix, and length,
 * and writing/reading credit card data to/from the database.
 */
class DB_API
{
public:
    // Checks if a file exists.
    static bool check_file_exists(const std::string& filename);

    // Reads an SQLite database and returns a shared pointer to it.
    static std::shared_ptr<sqlite3> read_db(const std::string& db_path);

    // Checks if the provided object is an SQLite database.
    static bool is_sqlite_database(sqlite3*& db);

    // Writes credit card data to the database, updating existing entries.
    static int write_cards(std::shared_ptr<sqlite3> db, const std::vector<Card>& data, std::string& error_msg);

    // Reads credit card data from the database and stores it in a vector.
    static int read_cards(std::shared_ptr<sqlite3> db, std::vector<Card>& data, std::string& error_msg);

	DB_API() = delete;
};