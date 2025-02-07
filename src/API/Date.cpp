#include "Date.h"

/**
 * @brief Returns the number of days in a given month and year.
 *
 * This function accounts for leap years when determining the number of days
 * in February. For all other months, it returns the standard number of days.
 *
 * @param year The year used to determine if February has 28 or 29 days.
 * @param month The month (1–12) for which to retrieve the number of days.
 * @return The number of days in the specified month.
 */
int Date::days_in_month(int year, int month)
{
    // Array of days for each month (February will be adjusted for leap year)
    static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Adjust February for leap year
    if (month == 2)
    {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        {
            return 29; // Leap year
        }
    }

    return days_in_month[month - 1];
}

/**
 * @brief Constructs a std::chrono::year_month_day object from day, month, and year values.
 *
 * This function creates a date representation using the C++20 <chrono> library types.
 * It does not perform validation on the input values.
 *
 * @param d The day of the month (1–31, depending on the month).
 * @param m The month of the year (1–12).
 * @param y The year.
 * @return A std::chrono::year_month_day object representing the given date.
 *
 * @see std::chrono::year_month_day
 */
std::chrono::year_month_day Date::get_date_from_ymd(unsigned int d, unsigned int m, int y)
{
    return std::chrono::year_month_day{std::chrono::year{y}, std::chrono::month{m}, std::chrono::day{d}};
}

/**
 * @brief Validates that a second date is not earlier than the first date.
 *
 * This function checks whether the second date (d2, m2, y2) is greater than
 * or equal to the first date (d1, m1, y1). It uses std::chrono::year_month_day
 * for comparison and assumes all date components are valid.
 *
 * @param d1 Day of the first date.
 * @param m1 Month of the first date.
 * @param y1 Year of the first date.
 * @param d2 Day of the second date.
 * @param m2 Month of the second date.
 * @param y2 Year of the second date.
 * @return true if the second date is the same or later than the first date, false otherwise.
 *
 * @see Date::get_date_from_ymd
 * @see std::chrono::year_month_day
 */
bool Date::vali_date(int d1, int m1, int y1, int d2, int m2, int y2) // pun
{
    std::chrono::year_month_day date1{get_date_from_ymd(d1, m1, y1)};
    std::chrono::year_month_day date2{get_date_from_ymd(d2, m2, y2)};

    return date2 >= date1;
}

/**
 * @brief Converts a std::chrono::year_month_day object to a string.
 *
 * This function formats a given date into a string using the default
 * output format of std::ostringstream for std::chrono::year_month_day.
 *
 * @param date The date to convert.
 * @return A string representation of the date.
 *
 * @see std::chrono::year_month_day
 */
std::string Date::string_from_date(std::chrono::year_month_day date)
{
    std::ostringstream oss;
    oss << date;
    return oss.str();
}