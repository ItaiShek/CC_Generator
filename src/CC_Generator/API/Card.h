#pragma once
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <regex>

/**
 * @class Card
 * @brief Represents a credit card with issuer information, length, and prefixes.
 *
 * The Card class provides functionality to parse numeric ranges and generate
 * random valid card numbers based on predefined parameters.
 */
class Card
{
public:
	// consructors
	Card() = default;

	/**
	 * @brief Parameterized constructor.
	 * @param issuer The issuer of the card.
	 * @param len The length of the card number.
	 * @param prefixes The numeric prefixes associated with the card.
	 */
	Card(const std::string& issuer, int len, const std::string& prefixes) : m_issuer{ issuer }, m_len{ len }, m_prefixes{ prefixes } { parse_ranges(prefixes); }

	// getters
	const std::string get_issuer() const { return m_issuer; }
	const int get_len() const { return m_len; }
	const std::string get_prefixes() const { return m_prefixes; }

	// setters
	void set_issuer(const std::string& issuer) { m_issuer = issuer; }
	void set_len(int len) { m_len = len; }
	void set_prefixes(const std::string& prefixes) { m_prefixes = prefixes; }

	// other methods
	bool empty() const { return m_issuer.empty() || m_prefixes.empty() || m_len == 0; }

	bool operator==(const Card& c) const
	{
		return (c.m_issuer == m_issuer && c.m_len == m_len && c.m_prefixes == m_prefixes);
	}

	// Parses a string representing numeric ranges and populates the Card object.
	void parse_ranges(const std::string& ranges_str);

	// Generates a random card number based on predefined ranges and Luhn's algorithm.
	// Appends the generated card number to the provided std::ostringstream.
	void generate_card(std::ostringstream& oss) const;

	// Validates a credit card based on issuer, length, and prefixes.
	static bool validate_card(const std::string& issuer, int length, const std::string& prefixes);

	// Validates the issuer of a credit card.
	static bool validate_issuer(const std::string& issuer);

	// Validates the prefix of a credit card based on its length.
	static bool validate_prefix(const std::string& prefix, int len);

	// Validates the length of a credit card number.
	static bool validate_length(int len);

private:
	std::string m_issuer{};							// The issuer of the card.
	int m_len{};									// The length of the card number.
	std::string m_prefixes{};						// The numeric prefixes associated with the card.
	std::vector<std::pair<int, int>> m_ranges;		// Parsed numeric ranges.
	static std::mt19937 m_rng;
};