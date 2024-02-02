#pragma once
#include "DB_API.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <functional>

#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#include "curses.h"
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT // Empty define for non-Windows platforms
#include <ncurses.h>
#include <linux/limits.h>
#define MAX_PATH PATH_MAX
#endif

#if defined(__x86_64) || defined(_M_X64)
#define DATATYPE unsigned long long
#elif defined(__i386) || defined(_M_IX86)
#define DATATYPE unsigned int
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

std::mt19937 File::gen(std::random_device{}());

// global variables
std::atomic<bool> g_paused{ false };
std::atomic<bool> g_started{ false };
std::atomic<float> g_progress{ 0.0f };

namespace console
{
	extern "C"
	{
		DLL_EXPORT void run(const std::string& version, const std::string& url, const std::string& license);
	}

	// Represents a button with a label and associated action.
	class Button
	{
	public:
		Button(std::string label, int action) : m_label(label), m_action(action) {}
		std::string m_label;
		int m_action;
	};

	// Internal utility functions for the console application.
	class internal
	{
	public:

		// Prints a banner with version and URL information.
		static void print_banner(const std::string& version, const std::string& url);

		// Checks if two strings are equal in a case-insensitive manner.
		static inline bool case_insensitive_equals(const std::string& input, const std::string& comp);

		// Checks if the input string indicates a quit command.
		static bool check_quit(const std::string& input);

		// Gets a SQLite database and returns it along with potential error messages.
		static std::shared_ptr<sqlite3> get_db(std::string& db_path, std::string& err_msg);

		// Draws buttons on the console.
		static void draw_buttons(const std::vector<console::Button>& buttons, size_t curr_btn_idx);

		// Displays a yes/no prompt on the console and returns the user's choice.
		static int yes_no(const std::string& str);

		// Adds a card with user-inputted details.
		static Card add_card();

		// Gets a formatted string representation of a card.
		static inline std::string get_card_string(int max_length, const Card& card);

		// Draws a list of cards on the console.
		static void draw_cards(const std::vector<Card>& cards_vec, int start_row, int visible_rows, int max_offset, int curr_row_idx, const std::vector<bool>& selected_rows);

		// Guides the user in choosing cards and returns the user's action.
		static int choose_cards(std::shared_ptr<sqlite3> db, const std::string& db_path, std::vector<Card>& cards_vec, std::vector<bool>& cards_selection);

		// Guides the user in choosing the amount of data to generate and returns the user's action.
		static int choose_amount(DATATYPE& amount);

		// Gets a valid file path from the user.
		static void get_path(std::string& path);

		// Guides the user in choosing a file and returns the user's action.
		static int choose_file(std::string& exp_path);

		// Guides the user in generating data and returns the user's action.
		static int generate(const std::string& exp_path, const std::vector<Card>& cards_vec, const std::vector<bool>& cards_selection, DATATYPE amount);

		internal() = delete;
	};
}