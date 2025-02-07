#include <iostream>
#include <sys/stat.h>

#include "DB_API_.h"
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
bool DB_API::is_sqlite_database(sqlite3 *&db)
{
    // Execute a simple query to check if the database is valid
    const char *query{"SELECT name FROM sqlite_master WHERE type='table' LIMIT 1;"};
    char *err_msg{nullptr};
    int result = sqlite3_exec(db, query, nullptr, nullptr, &err_msg);

    if (result != SQLITE_OK)
    {
        // Print error message
        std::cerr << "Error executing query: " << err_msg << std::endl;
        sqlite3_free(err_msg); // Free the error message
        sqlite3_close(db);     // Close the database immediately after error
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
std::shared_ptr<sqlite3> DB_API::read_db(const std::string &db_path)
{
    sqlite3 *db;
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK)
    {
        return nullptr;
    }
    if (DB_API::is_sqlite_database(db) == false)
    {
        return nullptr;
    }
    return std::shared_ptr<sqlite3>{db, [](sqlite3 *ptr)
                                    {if (ptr) { sqlite3_close(ptr); } }};
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
bool DB_API::check_file_exists(const std::string &filename)
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
int DB_API::read_cards(std::shared_ptr<sqlite3> db, std::vector<Card> &data, std::string &error_msg)
{
    const char *selectStatement = "SELECT Issuer, Length, Prefixes FROM cards_table;";
    sqlite3_stmt *stmt = nullptr;
    data.clear(); // Ensure the output vector is empty on entry

    int rc = sqlite3_prepare_v2(db.get(), selectStatement, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        // Check if the table exists
        sqlite3_stmt *check_stmt = nullptr;
        const char *check_table = "SELECT name FROM sqlite_master WHERE type='table' AND name='cards_table';";

        if (sqlite3_prepare_v2(db.get(), check_table, -1, &check_stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(check_stmt) != SQLITE_ROW)
            {
                // Table does not exist — allow graceful fallback (e.g., empty DB)
                sqlite3_finalize(check_stmt);
                return SQLITE_OK;
            }
            sqlite3_finalize(check_stmt);
        }

        error_msg = "Prepare statement error: " + std::string(sqlite3_errmsg(db.get()));
        return rc;
    }

    // Table exists — proceed to fetch data
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const char *issuer_c = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        int length = sqlite3_column_int(stmt, 1);
        const char *prefixes_c = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

        if (issuer_c && prefixes_c)
        {
            std::string issuer{issuer_c};
            std::string prefixes{prefixes_c};

            if (Card::validate_card(issuer, length, prefixes))
            {
                data.emplace_back(issuer, length, prefixes);
            }
        }
    }

    if (rc != SQLITE_DONE)
    {
        error_msg = "Step statement error: " + std::string(sqlite3_errmsg(db.get()));
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
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
int DB_API::write_cards(std::shared_ptr<sqlite3> db, const std::vector<Card> &data, std::string &error_msg)
{
    char *err = nullptr;
    int rc;

    // Step 1: Ensure main table exists
    rc = sqlite3_exec(db.get(),
                      "CREATE TABLE IF NOT EXISTS cards_table (Issuer TEXT, Length INTEGER, Prefixes TEXT);",
                      nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
    {
        error_msg = "Failed to create cards_table: " + std::string(err);
        sqlite3_free(err);
        return rc;
    }

    // Step 2: Start transaction
    rc = sqlite3_exec(db.get(), "BEGIN TRANSACTION;", nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
    {
        error_msg = "Failed to begin transaction: " + std::string(err);
        sqlite3_free(err);
        return rc;
    }

    // Step 3: Create temporary table
    rc = sqlite3_exec(db.get(),
                      "DROP TABLE IF EXISTS temp_cards;"
                      "CREATE TEMP TABLE temp_cards (Issuer TEXT, Length INTEGER, Prefixes TEXT);",
                      nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
    {
        error_msg = "Failed to create temp table: " + std::string(err);
        sqlite3_free(err);
        sqlite3_exec(db.get(), "ROLLBACK;", nullptr, nullptr, nullptr);
        return rc;
    }

    // Step 4: Prepare insert into temp_cards
    sqlite3_stmt *insert_stmt = nullptr;
    rc = sqlite3_prepare_v2(db.get(),
                            "INSERT INTO temp_cards (Issuer, Length, Prefixes) VALUES (?, ?, ?);",
                            -1, &insert_stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        error_msg = "Failed to prepare insert into temp_cards: " + std::string(sqlite3_errmsg(db.get()));
        sqlite3_exec(db.get(), "ROLLBACK;", nullptr, nullptr, nullptr);
        return rc;
    }

    for (const auto &card : data)
    {
        std::string issuer = card.get_issuer();
        std::string prefixes = card.get_prefixes();
        int length = card.get_len();

        sqlite3_bind_text(insert_stmt, 1, issuer.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(insert_stmt, 2, length);
        sqlite3_bind_text(insert_stmt, 3, prefixes.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(insert_stmt);
        if (rc != SQLITE_DONE)
        {
            error_msg = "Failed to insert into temp_cards: " + std::string(sqlite3_errmsg(db.get()));
            sqlite3_finalize(insert_stmt);
            sqlite3_exec(db.get(), "ROLLBACK;", nullptr, nullptr, nullptr);
            return rc;
        }

        sqlite3_reset(insert_stmt);
        sqlite3_clear_bindings(insert_stmt);
    }
    sqlite3_finalize(insert_stmt);

    // Step 5: Delete cards not in the new input
    rc = sqlite3_exec(db.get(),
                      "DELETE FROM cards_table "
                      "WHERE NOT EXISTS ("
                      "  SELECT 1 FROM temp_cards "
                      "  WHERE temp_cards.Issuer = cards_table.Issuer "
                      "    AND temp_cards.Length = cards_table.Length "
                      "    AND temp_cards.Prefixes = cards_table.Prefixes"
                      ");",
                      nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
    {
        error_msg = "Failed to delete outdated cards: " + std::string(err);
        sqlite3_free(err);
        sqlite3_exec(db.get(), "ROLLBACK;", nullptr, nullptr, nullptr);
        return rc;
    }

    // Step 6: Insert new records that are not already in cards_table
    rc = sqlite3_exec(db.get(),
                      "INSERT INTO cards_table (Issuer, Length, Prefixes) "
                      "SELECT Issuer, Length, Prefixes FROM temp_cards "
                      "WHERE NOT EXISTS ("
                      "  SELECT 1 FROM cards_table "
                      "  WHERE cards_table.Issuer = temp_cards.Issuer "
                      "    AND cards_table.Length = temp_cards.Length "
                      "    AND cards_table.Prefixes = temp_cards.Prefixes"
                      ");",
                      nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
    {
        error_msg = "Failed to insert new cards: " + std::string(err);
        sqlite3_free(err);
        sqlite3_exec(db.get(), "ROLLBACK;", nullptr, nullptr, nullptr);
        return rc;
    }

    // Step 7: Drop the temp table and commit
    sqlite3_exec(db.get(), "DROP TABLE IF EXISTS temp_cards;", nullptr, nullptr, nullptr);
    rc = sqlite3_exec(db.get(), "COMMIT;", nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
    {
        error_msg = "Failed to commit transaction: " + std::string(err);
        sqlite3_free(err);
        return rc;
    }

    return SQLITE_OK;
}

/**
 * @brief Retrieves a random full name from the database.
 *
 * This function selects a random first name from the "Fnames" table and a random
 * last name from the "Lnames" table in the SQLite database, then concatenates
 * them into a full name. If either query fails or returns no result, an empty
 * string is returned.
 *
 * @param db A shared pointer to an open SQLite3 database connection.
 *           The database must contain "Fnames" and "Lnames" tables,
 *           each with a "name" column.
 *
 * @return A string containing the random full name (e.g., "John Doe"),
 *         or an empty string if an error occurs or no name is found.
 */
std::string DB_API::get_random_name(std::shared_ptr<sqlite3> db)
{
    const char *f_name_statement = "SELECT name FROM Fnames ORDER BY RANDOM() LIMIT 1;";
    const char *l_name_statement = "SELECT name FROM Lnames ORDER BY RANDOM() LIMIT 1;";
    sqlite3_stmt *stmt = nullptr;

    // Helper lambda to get a name
    auto get_name = [&](const char *sql_query) -> std::string
    {
        sqlite3_stmt *local_stmt = nullptr;
        if (sqlite3_prepare_v2(db.get(), sql_query, -1, &local_stmt, nullptr) != SQLITE_OK)
        {
            sqlite3_finalize(local_stmt);
            return "";
        }

        std::string result;
        if (sqlite3_step(local_stmt) == SQLITE_ROW)
        {
            const unsigned char *text = sqlite3_column_text(local_stmt, 0);
            if (text)
            {
                result = reinterpret_cast<const char *>(text);
            }
        }

        sqlite3_finalize(local_stmt);
        return result;
    };

    std::string first_name{get_name(f_name_statement)};
    std::string last_name{get_name(l_name_statement)};

    if (first_name.empty() || last_name.empty())
    {
        return "";
    }

    return first_name + " " + last_name;
}