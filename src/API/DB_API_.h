#pragma once
#include "sqlite3.h"

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
    // Checks if the provided object is an SQLite database.
    bool is_sqlite_database(sqlite3 *&db);
};