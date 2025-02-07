#pragma once

#include <vector>
#include <memory>
#include <string>

#include "sqlite3.h"
#include "Card.h"

/**
 * @namespace DB_API
 * @brief Provides functions for interacting with an SQLite database and managing credit card data.
 *
 * The DB_API namespace contains functions for various database-related operations
 * such as checking file existence, reading an SQLite database, and writing/reading
 * credit card data. It encapsulates all related database utilities in a single, logical scope.
 */
namespace DB_API
{
    // Checks if a file exists.
    bool check_file_exists(const std::string &filename);

    // Reads an SQLite database and returns a shared pointer to it.
    std::shared_ptr<sqlite3> read_db(const std::string &db_path);

    // Writes credit card data to the database, updating existing entries.
    int write_cards(std::shared_ptr<sqlite3> db, const std::vector<Card> &data, std::string &error_msg);

    // Reads credit card data from the database and stores it in a vector.
    int read_cards(std::shared_ptr<sqlite3> db, std::vector<Card> &data, std::string &error_msg);

    // Reads a random first and last name and returns it as a string.
    std::string get_random_name(std::shared_ptr<sqlite3> db);
};