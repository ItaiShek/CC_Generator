#include <chrono>
#include <string>

/**
 * @namespace Date
 * @brief Provides utility functions for working with dates, including validation, conversion, and date calculations.
 *
 * This namespace contains functions for handling dates, such as determining the number of days in a month,
 * validating if one date is earlier than another, converting a date to a string, and constructing date objects
 * from day, month, and year values.
 */
namespace Date
{
    // Returns the number of days in a given month and year.
    int days_in_month(int year, int month);

    // Constructs a std::chrono::year_month_day object from day, month, and year values.
    std::chrono::year_month_day get_date_from_ymd(unsigned int d, unsigned int m, int y);

    // Validates that a second date is not earlier than the first date.
    bool vali_date(int d1, int m1, int y1, int d2, int m2, int y2);

    // Converts a std::chrono::year_month_day object to a string.
    std::string string_from_date(std::chrono::year_month_day date);
}