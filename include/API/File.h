#pragma once
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <array>
#include <string>
#include <atomic>
#include <thread>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "sqlite3.h"
#include "Card.h"
#include "DB_API.h"

extern std::atomic<bool> g_paused;
extern std::atomic<bool> g_started;
extern std::atomic<float> g_progress;
std::atomic<bool> export_finished = false;

/**
 * @struct date_struct
 * @brief Represents a date range with a generation flag.
 *
 * This structure holds a flag that determines whether a date range should be generated, along with the
 * start and end dates of the range. The dates are represented using `std::chrono::year_month_day`.
 */
struct date_struct
{
    /**
     * @brief Start date of the range.
     *
     * This field holds the start date of the range, represented as `std::chrono::year_month_day`.
     */
    std::chrono::year_month_day start{std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())}}; // current date

    /**
     * @brief End date of the range.
     *
     * This field holds the end date of the range, represented as `std::chrono::year_month_day`.
     */
    std::chrono::year_month_day end{start + std::chrono::years{3}}; // 3 years from now
};

/**
 * @class File
 * @brief Represents a file-related utility class.
 *
 * This class provides methods for exporting cards, estimating time, and estimating size.
 */
class File
{
public:
    /**
     * @brief Retrieves the indices of true values in a boolean vector.
     *
     * This function takes a boolean vector as input and returns a vector containing the indices
     * of the true values in the input vector.
     *
     * @param vec A boolean vector to process.
     * @return A vector containing the indices of true values in the input vector.
     *
     */
    static std::vector<int> get_true_vec(const std::vector<bool> &vec)
    {
        std::vector<int> true_vec;

        for (int i{}; i < vec.size(); i++)
        {
            if (vec[i])
            {
                true_vec.push_back(i);
            }
        }
        return true_vec;
    }

    /**
     * @brief Chooses a random index from a vector of integers.
     *
     * This function takes a vector of integers as input and returns a randomly chosen index
     * from that vector. The randomness is determined using a uniform distribution.
     *
     * @param vec A vector of integers from which to choose a random index.
     * @return A randomly selected index from the input vector.
     *
     */
    static int choose_random_index(const std::vector<int> &vec)
    {
        if (vec.empty())
        {
            return -1;
        }

        std::uniform_int_distribution<int> dist(0, static_cast<int>(vec.size() - 1));
        return vec[dist(m_gen)];
    }

    /**
     * @brief Exports a specified number of randomly selected cards to a file.
     *
     * This templated function exports a specified number of randomly selected cards from
     * the provided vector to the given file. It uses a selection vector to determine which
     * cards to export. The export is performed in chunks to enhance performance.
     *
     * @tparam T The type of the amount parameter.
     * @param file An output file stream to write the exported cards.
     * @param cards_vec A vector containing the cards to choose from.
     * @param selection_vec A vector of boolean values indicating the selection status of cards.
     * @param amount The number of cards to export.
     * @param db A shared pointer to the database.
     *
     */
    template <typename T>
    static void export_cards(std::ofstream &file, const std::vector<Card> &cards_vec, const std::vector<bool> &selection_vec, T amount, std::shared_ptr<sqlite3> db)
    {
        constexpr int chunk = 4096;
        std::vector<int> indexes_vec{get_true_vec(selection_vec)};
        std::ostringstream oss(std::ios_base::ate);
        T size{};
        int rnd_idx{};
        T i;
        for (i = 0; i < amount; i++)
        {
            while (g_paused)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                if (g_started == false)
                {
                    break;
                }
            }
            if (g_started == false)
            {
                break;
            }
            rnd_idx = choose_random_index(indexes_vec);
            cards_vec[rnd_idx].generate_card(oss);

            if (m_cvv_flag)
            {
                Card::generate_cvv(m_delimiter, oss);
            }

            if (m_date_flag)
            {
                Card::generate_date(m_delimiter, m_date.start, m_date.end, oss);
            }

            if (m_name_flag and db != nullptr)
            {
                std::string name = DB_API::get_random_name(db);
                if (name.empty() == false)
                {
                    oss << m_delimiter;
                    oss << name;
                }
            }

            oss << "\n";

            // write chunk to file
            if (oss.tellp() > chunk)
            {
                // Extract and write the first "chunk" bytes
                file << oss.str().substr(0, chunk);

                // Remove the bytes from the stringstream
                oss.str(oss.str().substr(chunk));

                // update the progress bar value
                g_progress = static_cast<float>(i) / amount;
            }
        }

        // Write any remaining content in the stringstream
        if (g_started)
        {
            // int size = oss.tellp();
            std::streamoff size = oss.tellp();

            if (size > 0)
            {
                size -= 1;
                oss.str(oss.str().substr(0, size));
            }
            file << oss.str();
            g_progress = static_cast<float>(i) / amount;
        }

        file.close();
        g_started = false;
    }

    /**
     * @brief Estimates the time taken to generate a single card.
     *
     * This templated function estimates the time taken to generate a single card
     * using a temporary card object and a specified number of iterations. The time
     * is calculated by measuring the duration it takes to generate the card the
     * specified number of times and then dividing by the number of iterations.
     *
     * @tparam T The duration type (e.g., std::chrono::milliseconds, std::chrono::seconds).
     * @return The estimated time taken to generate a single card.
     *
     */
    template <typename T>
    static T estimate_time()
    {
        Card temp_card = Card{"temp", 16, "1,2,3,4,5-6"};
        size_t div{1000};
        size_t i{};
        T duration{};
        std::ostringstream temp_oss;
        auto start{std::chrono::high_resolution_clock::now()};
        for (i = 0; i < div; i++)
        {
            temp_card.generate_card(temp_oss);
        }
        auto end{std::chrono::high_resolution_clock::now()};

        return std::chrono::duration_cast<T>(end - start) / div;
    }

    /**
     * @brief Estimates the average time to generate a CVV using the Card::generate_cvv function.
     *
     * This function measures the time taken to generate a CVV using the `Card::generate_cvv`
     * function multiple times (defaulted to 1000 iterations). It calculates the average time
     * per CVV generation and returns it in the specified duration type.
     *
     * @tparam T The type of duration to return (e.g., std::chrono::milliseconds).
     * @return The average time taken to generate a CVV, in the specified duration type.
     *
     * @see Card::generate_cvv
     */
    template <typename T>
    static T estimate_cvv_time()
    {
        size_t div{1000};
        size_t i{};
        T duration{};
        std::ostringstream temp_oss;
        auto start{std::chrono::high_resolution_clock::now()};
        for (i = 0; i < div; i++)
        {
            Card::generate_cvv(m_delimiter, temp_oss);
        }
        auto end{std::chrono::high_resolution_clock::now()};

        return std::chrono::duration_cast<T>(end - start) / div;
    }

    /**
     * @brief Estimates the average time to generate a date using the Card::generate_date function.
     *
     * This function measures the time taken to generate a date using the `Card::generate_date`
     * function multiple times (defaulted to 1000 iterations). It calculates the average time
     * per generation and returns it in the specified duration type.
     *
     * @tparam T The type of duration to return (e.g., std::chrono::milliseconds).
     * @return The average time taken to generate a date, in the specified duration type.
     *
     * @see Card::generate_date
     */
    template <typename T>
    static T estimate_date_time()
    {
        size_t div{1000};
        size_t i{};
        T duration{};
        std::ostringstream temp_oss;
        auto start{std::chrono::high_resolution_clock::now()};
        for (i = 0; i < div; i++)
        {
            Card::generate_date(m_delimiter, m_date.start, m_date.end, temp_oss);
        }
        auto end{std::chrono::high_resolution_clock::now()};

        return std::chrono::duration_cast<T>(end - start) / div;
    }

    /**
     * @brief Estimates the average time to retrieve a random name from the database.
     *
     * This function measures the time taken to retrieve a random name from the
     * database multiple times (defaulted to 10 iterations). It calculates the
     * average time per retrieval and returns it in the specified duration type.
     * If any condition fails (e.g., the database does not exist, cannot be opened,
     * or no names are found), it returns a zero duration.
     *
     * @tparam T The type of duration to return (e.g., std::chrono::milliseconds).
     * @param db_path The path to the database file.
     * @return The average time taken to retrieve a random name from the database, in the specified duration type.
     *
     * @see DB_API::check_file_exists
     * @see DB_API::read_db
     * @see DB_API::get_random_name
     */
    template <typename T>
    static T estimate_name_time(std::string db_path)
    {
        size_t div{10};
        size_t i{};
        T duration{};
        std::ostringstream temp_oss;

        if (DB_API::check_file_exists(db_path) == false)
        {
            return std::chrono::duration_cast<T>(T{0});
        }

        std::shared_ptr<sqlite3> db{DB_API::read_db(db_path)};

        if (db == nullptr)
        {
            return std::chrono::duration_cast<T>(T{0});
        }

        if (DB_API::get_random_name(db).empty())
        {
            return std::chrono::duration_cast<T>(T{0});
        }

        auto start{std::chrono::high_resolution_clock::now()};
        for (i = 0; i < div; i++)
        {
            DB_API::get_random_name(db);
        }
        auto end{std::chrono::high_resolution_clock::now()};

        return std::chrono::duration_cast<T>(end - start) / div;
    }

    /**
     * @brief Formats a time string for a given duration.
     *
     * This function takes a std::chrono::... as input and returns a formatted string of it.
     *
     * @param duration The duration to format.
     * @return The formatted duration string as "X Years Y Months Z Days Hours:Minutes:Seconds:Milliseconds".
     */
    template <typename T, typename U>
    static inline std::string format_time(T duration)
    {
        std::ostringstream estimated_time;

        auto years{std::chrono::duration_cast<std::chrono::duration<U, std::ratio<31557600>>>(duration)};
        duration -= std::chrono::duration_cast<std::chrono::duration<int, std::ratio<31557600>>>(years);

        auto months{std::chrono::duration_cast<std::chrono::duration<int, std::ratio<2629800>>>(duration)};
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(months);

        auto days{std::chrono::duration_cast<std::chrono::duration<int, std::ratio<86400>>>(duration)};
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(days);

        auto hours{std::chrono::duration_cast<std::chrono::hours>(duration)};
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(hours);

        auto minutes{std::chrono::duration_cast<std::chrono::minutes>(duration)};
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(minutes);

        auto seconds{std::chrono::duration_cast<std::chrono::seconds>(duration)};
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(seconds);

        auto milliseconds{std::chrono::duration_cast<std::chrono::milliseconds>(duration)};

        if (years.count() > 0)
        {
            estimated_time << std::to_string(years.count()) << " years ";
        }
        if (months.count() > 0)
        {
            estimated_time << std::to_string(months.count()) << " months ";
        }
        if (days.count() > 0)
        {
            estimated_time << std::to_string(days.count()) << " days ";
        }

        estimated_time
            << std::setw(2) << std::setfill('0') << hours.count() << ":"
            << std::setw(2) << std::setfill('0') << minutes.count() << ":"
            << std::setw(2) << std::setfill('0') << seconds.count() << ":"
            << std::setw(3) << std::setfill('0') << milliseconds.count();

        return estimated_time.str();
    }

    /**
     * @brief Estimates the size in human-readable format based on the given amount.
     *
     * This templated function estimates the size in a human-readable format (e.g., B, KB, MB)
     * based on the provided amount. The size is calculated by multiplying the amount with the
     * size of a representative string and then converting it to a suitable unit.
     *
     * @tparam T The type of the amount parameter.
     * @param amount The amount for which to estimate the size.
     * @return A string representing the estimated size in human-readable format.
     *
     */
    template <typename T>
    static inline std::string estimate_size(T amount)
    {
        constexpr double name_size{15};
        constexpr double cvv_size{4};
        constexpr double date_size{11};
        constexpr double card_size{17};

        double total_size = card_size;
        total_size += m_cvv_flag ? cvv_size : 0;
        total_size += m_name_flag ? name_size : 0;
        total_size += m_date_flag ? date_size : 0;

        double size{amount * total_size};
        static std::array<const char *, 5> sizes{"B", "KB", "MB", "GB", "TB"};
        size_t si{};

        for (si = 0; si < sizes.size(); si++)
        {
            if (size < 1024)
            {
                break;
            }
            size /= 1024.0f;
        }
        si = si < sizes.size() ? si : sizes.size() - 1;

        return std::to_string(size) + " [" + sizes[si] + "]";
    }

#ifdef __EMSCRIPTEN__

    /**
     * @brief Holds the state for an ongoing export operation.
     *
     * This structure stores all necessary information required to manage
     * the asynchronous export process, including output buffering,
     * progress tracking, card selection, and file destination.
     */
    struct ExportState
    {
        std::ostringstream oss;          ///< Output buffer used to accumulate export content.
        int i = 0;                       ///< Current export iteration index.
        int amount;                      ///< Total number of cards to export.
        std::vector<Card> cards_vec;     ///< Vector of available cards to export from.
        std::vector<bool> selection_vec; ///< Selection vector indicating which cards are selected.
        std::shared_ptr<sqlite3> db;     ///< Shared pointer to the SQLite database for name generation.
        std::vector<int> indexes_vec;    ///< Precomputed list of selected indexes for efficient access.
        std::string vfs_path;            ///< Virtual filesystem path where the output file will be written.
    };

    /**
     * @brief Performs a single asynchronous step in the card export process.
     *
     * This function is designed to be repeatedly scheduled with `emscripten_async_call`
     * to allow non-blocking export of card data to the virtual file system (VFS).
     *
     * It checks for pause and stop signals, writes data in chunks to avoid memory issues,
     * and finalizes the export by flushing the remaining content once all items are processed.
     *
     * @param Unused A dummy pointer required by `emscripten_async_call`. Ignored in this implementation.
     */
    static void export_step(void *)
    {
        if (g_started == false || export_state == nullptr)
            return;

        constexpr int chunk = 4096;
        auto &s = *export_state;

        if (g_paused)
        {
            emscripten_async_call(export_step, nullptr, 250);
            return;
        }

        if (s.i >= s.amount)
        {
            // Write any remaining content in the stringstream before closing file
            std::streamoff size = s.oss.tellp();

            if (size > 0)
            {
                size -= 1;
                std::ofstream file(s.vfs_path, std::ios::app); // append mode
                s.oss.str(s.oss.str().substr(0, size));
                file << s.oss.str();
                file.close();
            }

            export_finished = true;
            g_progress = 1.0f;
            g_started = false;
            delete export_state;
            export_state = nullptr;
            return;
        }

        int rnd_idx = choose_random_index(s.indexes_vec);
        s.cards_vec[rnd_idx].generate_card(s.oss);

        if (m_cvv_flag)
            Card::generate_cvv(m_delimiter, s.oss);
        if (m_date_flag)
            Card::generate_date(m_delimiter, m_date.start, m_date.end, s.oss);
        if (m_name_flag && s.db)
        {
            std::string name = DB_API::get_random_name(s.db);
            if (name.empty() == false)
            {
                s.oss << m_delimiter << name;
            }
        }

        s.oss << "\n";

        if (s.oss.tellp() > chunk)
        {
            std::ofstream file(s.vfs_path, std::ios::app);
            file << s.oss.str().substr(0, chunk);
            file.close();
            s.oss.str(s.oss.str().substr(chunk));
        }

        g_progress = static_cast<float>(s.i) / s.amount;
        s.i++;

        emscripten_async_call(export_step, nullptr, 0); // Schedule next chunk
    }

    /**
     * @brief Initializes and starts the asynchronous export process.
     *
     * This function prepares the internal export state, clears the target file in the virtual file system (VFS),
     * and begins the asynchronous card export routine via `emscripten_async_call`.
     *
     * @param cards A vector of `Card` objects used to generate the exported data.
     * @param selection A vector of booleans indicating which cards are selected for export.
     * @param amount The number of cards to export.
     * @param db A shared pointer to the SQLite3 database used for generating additional card data (e.g., names).
     * @param vfs_path The VFS path where the export file will be written. This file is truncated at the beginning.
     */
    static void start_export(const std::vector<Card> &cards, const std::vector<bool> &selection, int amount, std::shared_ptr<sqlite3> db, const std::string &vfs_path)
    {
        // Overwrite (truncate) the file at the start
        std::ofstream truncate_file(vfs_path);
        truncate_file.close();

        export_state = new ExportState{
            std::ostringstream(std::ios_base::ate),
            0,
            amount,
            cards,
            selection,
            db,
            get_true_vec(selection),
            vfs_path};

        g_started = true;
        export_finished = false;
        emscripten_async_call(export_step, nullptr, 0);
    }

    static inline ExportState *export_state{nullptr};
#endif

    static std::mt19937 m_gen;
    static inline date_struct m_date;
    static inline char m_delimiter{';'};
    static inline bool m_date_flag{false};
    static inline bool m_cvv_flag{false};
    static inline bool m_name_flag{false};
    File() = delete;
};
