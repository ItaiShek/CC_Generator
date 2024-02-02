#include "Card.h"

std::mt19937 Card::m_rng{ std::random_device{}() };

/**
 * @brief Parses a string representing numeric ranges and populates the Card object.
 *
 * This function takes a string containing comma-separated numeric ranges and parses
 * them, populating the Card object with the extracted ranges.
 *
 * The input string should have the format "start-end,start-end,..." where start and
 * end are integers defining a numeric range. If a single number is provided without
 * a dash, it is considered a range with the same start and end values.
 *
 * @param ranges_str A string containing comma-separated numeric ranges.
 *
 * @note The function assumes a valid input format. No input validation is performed.
 *
 * @see Card
 */
void Card::parse_ranges(const std::string& ranges_str)
{
	std::istringstream iss(ranges_str);
	std::string range_token;
	while (std::getline(iss, range_token, ','))
	{
		std::istringstream range_stream(range_token);
		int start, end;
		char dash;
		range_stream >> start;
		if (range_stream >> dash >> end)
		{
			// range case
			m_ranges.emplace_back(start, end);
		}
		else
		{
			// single number case
			m_ranges.emplace_back(start, start);
		}
	}
}

/**
 * @brief Generates a random card number based on predefined ranges and Luhn's algorithm.
 *
 * This function generates a random card number within the predefined numeric ranges
 * associated with the Card object. It utilizes a combination of random number generation,
 * Luhn's algorithm, and predefined ranges to create a valid card number.
 *
 * The generated card number is appended to the provided std::ostringstream.
 *
 * @param oss The std::ostringstream to which the generated card number is appended.
 *
 * @note The function assumes that the Card object has valid ranges and length parameters.
 *       It uses a random number generator (m_rng) associated with the Card object.
 *       The generated card number adheres to Luhn's algorithm for validity.
 *
 * @see Card
 * @see parse_ranges()
 */
void Card::generate_card(std::ostringstream& oss) const
{
	// Randomly select a range
	std::uniform_int_distribution<int> range_dist(0, m_ranges.size() - 1);
	const auto& range = m_ranges[range_dist(m_rng)];

	// Generate a random number within the selected range
	std::uniform_int_distribution<int> num_dist(range.first, range.second);
	int prefix{ num_dist(m_rng) };

	// Generate a random card
	std::string rnd_card{ std::to_string(prefix) };
	std::uniform_int_distribution<int> dis(0, 9);

	for (int i = rnd_card.size(); i < m_len - 1; i++)
	{
		rnd_card += std::to_string(dis(m_rng));
	}

	// Apply Luhn's algorithm to generate the last digit
	int sum{ 0 };
	bool double_digit{ true };

	for (int i{ m_len - 2 }; i >= 0; i--)
	{
		int digit = rnd_card[i] - '0';

		if (double_digit && (digit *= 2) >= 10)
		{
			digit -= 9;
		}

		sum += digit;
		double_digit = !double_digit;
	}

	// Calculate the last digit to make the entire number valid
	rnd_card += std::to_string((10 - (sum % 10)) % 10);
	oss << rnd_card << "\n";
}

/**
 * @brief Validates the issuer string.
 *
 * This function checks if the provided issuer string contains at least one non-whitespace character.
 * Whitespace characters include space, tab, newline, vertical tab, form feed, and carriage return.
 *
 * @param issuer The issuer string to be validated.
 * @return true if the issuer string contains at least one non-whitespace character, false otherwise.
 *
 * @note This function considers whitespace characters to be any of the following: space, tab, newline,
 * vertical tab, form feed, or carriage return.
 */
bool Card::validate_issuer(const std::string& issuer)
{
	return issuer.find_first_not_of(" \t\n\v\f\r") != std::string::npos;
}

/**
 * @brief Validates a comma-separated string of numeric prefixes according to specific rules.
 *
 * This function validates a comma-separated string of numeric prefixes based on the following rules:
 * 1. Each token separated by a comma must be a non-empty string.
 * 2. Tokens cannot end with a comma.
 * 3. Each token must represent either a single numeric value or a numeric range in the format "start-end".
 * 4. Numeric ranges must be in increasing order.
 * 5. Numeric values and ranges must be positive integers and must not exceed the specified length.
 *
 * @param prefix The comma-separated string of numeric prefixes to be validated.
 * @param len The maximum length that a valid prefix can have.
 * @return true if the input string conforms to the specified rules, false otherwise.
 *
 * @details This function takes a comma-separated string of numeric prefixes and validates each token
 * separately. It checks if each token is a valid positive integer or a valid numeric range with a
 * start value less than or equal to the end value. It also ensures that the length of the prefix does
 * not exceed the specified maximum length.
 *
 */
bool Card::validate_prefix(const std::string& prefix, int len)
{
	if (prefix.empty())
	{
		return false;
	}
	if (prefix[prefix.size() - 1] == ',')
	{
		return false;
	}

	std::stringstream ss(prefix);
	std::string token;

	while (std::getline(ss, token, ','))
	{
		size_t start{ token.find_first_not_of(" ") };
		size_t end{ token.find_last_not_of(" ") };

		// if it's empty return false
		if (start == std::string::npos || end == std::string::npos)
		{
			return false;
		}

		token = token.substr(start, end - start + 1);

		// range
		if (std::regex_match(token, std::regex(R"(\d+\-\d+)")))
		{
			std::istringstream iss(token);
			int start, end;
			char dash;
			iss >> start >> dash >> end;
			// Range is not in increasing order
			if (end < start)
			{
				return false;
			}
			// the prefix is larger than the length		
			if (std::to_string(start).length() > len || std::to_string(end).length() > len || start == 0)
			{
				return false;
			}
		}
		// digit
		else if (std::regex_match(token, std::regex(R"(\d+)")))
		{
			// the prefix is larger than the length	or it's zero
			if (token.size() > len || std::stoi(token.c_str()) == 0)
			{
				return false;
			}
		}
		// NaN or negative number
		else
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Validates the length of a credit card number.
 *
 * This function checks whether the provided length is within the valid range
 * for a credit card number. Credit card numbers typically have lengths between
 * 2 and 32 characters (inclusive).
 *
 * @param len The length of the credit card number to be validated.
 * @return True if the length is within the valid range, false otherwise.
 */
bool Card::validate_length(int len)
{
	return len >= 2 && len <= 32;
}

/**
 * @brief Validates a credit card based on issuer, length, and prefixes.
 *
 * This function checks whether a credit card is valid by validating the issuer,
 * length, and prefixes. It combines three validation checks: issuer validation,
 * length validation, and prefix validation.
 *
 * @param issuer The issuer of the credit card.
 * @param length The expected length of the credit card number.
 * @param prefixes The valid prefixes for the credit card.
 * @return True if the credit card is valid, false otherwise.
 *
 * @see DB_API::validate_issuer() For issuer validation.
 * @see DB_API::validate_length() For length validation.
 * @see DB_API::validate_prefix() For prefix validation.
 *
 * @note This function assumes that the individual validation functions are
 *       implemented and correctly check the issuer, length, and prefixes.
 */
bool Card::validate_card(const std::string& issuer, int length, const std::string& prefixes)
{
	return (Card::validate_issuer(issuer) &&
		Card::validate_length(length) &&
		Card::validate_prefix(prefixes, length));
}