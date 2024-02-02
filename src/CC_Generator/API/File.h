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
#include "Card.h"

extern std::atomic<bool> g_paused;
extern std::atomic<bool> g_started;
extern std::atomic<float> g_progress;

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
    static std::vector<int> get_true_vec(const std::vector<bool>& vec)
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
    static int choose_random_index(const std::vector<int>& vec)
    {
        if (vec.empty())
        {
            return -1;
        }

        std::uniform_int_distribution<int> dist(0, vec.size() - 1);
        return vec[dist(gen)];
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
     *
     */
    template<typename T>
    static void export_cards(std::ofstream& file, const std::vector<Card>& cards_vec, const std::vector<bool>& selection_vec, T amount)
    {
        constexpr int chunk = 4096;
        std::vector<int> indexes_vec{ get_true_vec(selection_vec) };
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
            int size = oss.tellp();
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
        Card temp_card = Card{ "temp", 16, "1,2,3,4,5-6" };
        size_t div{ 1000 };
        size_t i{};
        T duration{};
        std::ostringstream temp_oss;
        auto start{ std::chrono::high_resolution_clock::now() };
        for (i = 0; i < div; i++)
        {
            temp_card.generate_card(temp_oss);
        }
        auto end{ std::chrono::high_resolution_clock::now() };

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

        auto years{ std::chrono::duration_cast<std::chrono::duration<U, std::ratio<31557600>>>(duration) };
        duration -= std::chrono::duration_cast<std::chrono::duration<int, std::ratio<31557600>>>(years);

        auto months{ std::chrono::duration_cast<std::chrono::duration<int, std::ratio<2629800>>>(duration) };
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(months);

        auto days{ std::chrono::duration_cast<std::chrono::duration<int, std::ratio<86400>>>(duration) };
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(days);

        auto hours{ std::chrono::duration_cast<std::chrono::hours>(duration) };
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(hours);

        auto minutes{ std::chrono::duration_cast<std::chrono::minutes>(duration) };
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(minutes);

        auto seconds{ std::chrono::duration_cast<std::chrono::seconds>(duration) };
        duration -= std::chrono::duration_cast<std::chrono::microseconds>(seconds);

        auto milliseconds{ std::chrono::duration_cast<std::chrono::milliseconds>(duration) };

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
        double size{ amount * static_cast<double>(std::string("11111111111111111111111111111111\n").size()) };
        static std::array<const char*, 5> sizes{ "B", "KB", "MB", "GB", "TB" };
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
    static std::mt19937 gen;

    File() = delete;
};
