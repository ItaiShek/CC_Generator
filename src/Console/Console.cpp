#include <functional>
#include <algorithm>
#include <cmath>

#include "File.h"
#include "Console_.h"
#include "Console.h"
#include "DB_API.h"
#include "Date.h"

std::mt19937 File::m_gen(std::random_device{}());

std::atomic<bool> g_paused{false};
std::atomic<bool> g_started{false};
std::atomic<float> g_progress{0.0f};

/**
 * @brief Compares two strings for equality, ignoring case differences.
 *
 * This function compares two strings for equality while ignoring the case
 * differences between characters. It returns true if the strings are equal
 * (disregarding case) and false otherwise.
 *
 * @param input The first string to be compared.
 * @param comp The second string to be compared.
 *
 * @return True if the strings are equal (disregarding case), false otherwise.
 */
inline bool console::case_insensitive_equals(const std::string &input, const std::string &comp)
{
	return std::equal(input.begin(), input.end(), comp.begin(), comp.end(),
					  [](char a, char b)
					  {
						  return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
					  });
}

/**
 * @brief Checks if the input string corresponds to a quit command, ignoring case differences.
 *
 * This function checks whether the input string corresponds to a quit command,
 * indicating the user's intention to exit the application. The comparison is
 * case-insensitive, meaning it does not consider differences in letter case.
 * It returns true if the input string matches the quit command, and false otherwise.
 *
 * @param input The input string to be checked for the quit command.
 *
 * @return True if the input string represents a quit command, false otherwise.
 *
 * @see console::case_insensitive_equals(const std::string& input, const std::string& comp)
 */
bool console::check_quit(const std::string &input)
{
	return (console::case_insensitive_equals(input, "exit") || console::case_insensitive_equals(input, "quit") ||
			console::case_insensitive_equals(input, "e") || console::case_insensitive_equals(input, "q"));
}

/**
 * @brief Retrieves a shared pointer to an SQLite database.
 *
 * This function prompts the user to enter the path to an SQLite database file.
 * If the user enters a valid path, it attempts to open the database and returns
 * a shared pointer to the opened database. If the user enters a quit command or
 * if the database cannot be opened, it returns a nullptr.
 *
 * @return A shared pointer to the opened SQLite database if successful, nullptr otherwise.
 *
 * @see console::check_quit(const std::string& input)
 * @see DB_API::check_file_exists(const std::string& filepath)
 * @see DB_API::read_db(const std::string& filepath)
 */
std::shared_ptr<sqlite3> console::get_db(std::string &db_path, std::string &err_msg)
{
	clear();
	printw("\n");

	do
	{
		printw("Enter the path to the database (The default is \"cards.db\"): ");
		refresh();

		char buffer[MAX_PATH];	 // Assuming a maximum path length of 255 characters
		wgetstr(stdscr, buffer); // Read input into db_path
		db_path = buffer;
		if (console::check_quit(db_path))
		{
			return nullptr;
		}

		if (db_path.empty())
		{
			db_path = "cards.db";
		}

		clear();
		printw("Couldn't find the database: \"%s\"\n", db_path.c_str());

		// Remove single quotes from the file path
		if (db_path.size() >= 2 && db_path.front() == '\'' && db_path.back() == '\'')
		{
			db_path = db_path.substr(1, db_path.size() - 2);
		}

	} while (DB_API::check_file_exists(db_path) == false);

	std::shared_ptr<sqlite3> db = DB_API::read_db(db_path);

	if (db == nullptr)
	{
		err_msg = "Failed to open the database. Quitting.\n";
	}

	return db;
}

/**
 * @brief Prints the banner of the console application.
 *
 * This function prints the banner of the console application, including the
 * version number and URL.
 *
 * @param version The version number of the console application (e.g., "2.0.0").
 * @param url The URL where users can find more information about the application.
 */
void console::print_banner(const std::string &version, const std::string &url)
{
	printw("\n");
	printw("    $$$$$$\\   $$$$$$\\                                                                    $$\\                         \n");
	printw("   $$  __$$\\ $$  __$$\\                                                                   $$ |                        \n");
	printw("   $$ /  \\__|$$ /  \\__|       $$$$$$\\   $$$$$$\\  $$$$$$$\\   $$$$$$\\   $$$$$$\\  $$$$$$\\ $$$$$$\\    $$$$$$\\   $$$$$$\\  \n");
	printw("   $$ |      $$ |            $$  __$$\\ $$  __$$\\ $$  __$$\\ $$  __$$\\ $$  __$$\\ \\____$$\\\\_$$  _|  $$  __$$\\ $$  __$$\\ \n");
	printw("   $$ |      $$ |            $$ /  $$ |$$$$$$$$ |$$ |  $$ |$$$$$$$$ |$$ |  \\__|$$$$$$$ | $$ |    $$ /  $$ |$$ |  \\__|\n");
	printw("   $$ |  $$\\ $$ |  $$\\       $$ |  $$ |$$   ____|$$ |  $$ |$$   ____|$$ |     $$  __$$ | $$ |$$\\ $$ |  $$ |$$ |      \n");
	printw("   \\$$$$$$  |\\$$$$$$  |      \\$$$$$$$ |\\$$$$$$$\\ $$ |  $$ |\\$$$$$$$\\ $$ |     \\$$$$$$$ | \\$$$$  |\\$$$$$$  |$$ |      \n");
	printw("    \\______/  \\______/        \\____$$ | \\_______|\\__|  \\__| \\_______|\\__|      \\_______|  \\____/  \\______/ \\__|      \n");
	printw("                             $$\\   $$ |                                                                              \n");
	printw("                             \\$$$$$$  |                                                                              \n");
	printw("                              \\______/                                                                               \n");
	printw(std::string{"    Version " + version + "\n"}.c_str());
	printw(std::string{"    " + url + "\n"}.c_str());
}

/**
 * @brief Draws a set of buttons at the bottom of the console screen.
 *
 * This function takes a vector of buttons and the index of the currently selected button.
 * It draws each button on the last row of the console screen, highlighting the selected button.
 *
 * @param buttons A vector of console::Button objects representing the buttons to be drawn.
 * @param curr_btn_idx The index of the currently selected button in the 'buttons' vector.
 *
 * The function uses the ncurses library to interact with the console screen.
 * It calculates the screen dimensions and positions each button horizontally with a spacing of 15 characters.
 * The selected button is highlighted using a reverse attribute.
 *
 *
 * @see console::Button
 * @see console::getmaxyx
 */
void console::draw_buttons(const std::vector<console::Button> &buttons, size_t curr_btn_idx)
{
	int y_max{}, x_max{};
	getmaxyx(stdscr, y_max, x_max);

	// Draw buttons at the bottom of the screen
	for (size_t i{}; i < buttons.size(); i++)
	{
		if (i == curr_btn_idx)
		{
			attron(A_REVERSE); // Highlight the selected item
		}
		mvprintw(y_max - 1, static_cast<int>(i) * 15, buttons[i].m_label.c_str());

		attroff(A_REVERSE);
	}
}

/**
 * @brief Displays a yes/no prompt on the console and returns the user's selection.
 *
 * This function presents a prompt with a specified message and two buttons (Yes and No).
 * The user can navigate between buttons using the left and right arrow keys and confirm their selection with the Enter key.
 * The function returns the index of the selected button (0 for "No", 1 for "Yes").
 *
 * @param str The message to be displayed as part of the yes/no prompt.
 * @return The index of the selected button (0 for "No", 1 for "Yes").
 *
 * The function uses the ncurses library to interact with the console screen.
 * It handles user input for button navigation and confirmation.
 * The Enter key (or its corresponding code) is used to confirm the selection.
 *
 * @see console::draw_buttons
 * @see console::Button
 */
int console::yes_no(const std::string &str)
{
	bool flag{true};
	std::string err_msg{};

	std::vector<console::Button> buttons{
		console::Button("No", 0),
		console::Button("Yes", 1)};

	int curr_btn_idx{};
	int selected_action{};
	while (flag)
	{
		clear();
		printw("Use left/right arrow keys for buttons, confirm with enter.\n\n");
		printw(str.c_str());
		draw_buttons(buttons, curr_btn_idx); // Draw buttons at the bottom of the screen
		refresh();
		int ch = getch(); // Get user input
		switch (ch)
		{
			// button scrolling
		case KEY_LEFT:
			curr_btn_idx = (curr_btn_idx - 1 + static_cast<int>(buttons.size())) % static_cast<int>(buttons.size());
			break;
		case KEY_RIGHT:
			curr_btn_idx = (curr_btn_idx + 1) % static_cast<int>(buttons.size());
			break;

#if defined(_WIN64) || defined(_WIN32)
		case 13:
		case PADENTER:
#else
		case 10:
#endif
		{
			selected_action = buttons[curr_btn_idx].m_action;
			switch (selected_action)
			{
			case 0: // no
			case 1: // yes
				flag = false;
				break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}

	return selected_action;
}

/**
 * @brief Adds a new card by obtaining user input for issuer, length, and prefixes.
 *
 * This function guides the user through the process of adding a new card by prompting for issuer, length,
 * and prefixes. It validates the input and returns a Card object based on the provided information.
 *
 * @return A Card object representing the newly added card.
 *
 * The function uses the ncurses library for console interaction.
 * It provides prompts for the user to input issuer, length, and prefixes with specific validation criteria.
 * The user can enter "exit" at any prompt to cancel the card addition process.
 *
 * @see Card
 * @see DB_API::validate_issuer
 * @see DB_API::validate_length
 * @see DB_API::validate_prefix
 * @see console::check_quit
 */
Card console::add_card()
{
	std::array<const char *, 3> line{"Issuer: ", "Length: ", "Prefixes: "};
	std::string issuer;
	std::string length;
	std::string prefixes;

	printw("The issuer can be anything except an empty string\n");
	printw("The length can be any positive number up to 32\n");
	printw("The prefix needs to be comma-separated digits and/or hyphen to indicate range (e.g. \"23,54,53-89\")\n");
	printw("Enter \"exit\" to go back to the previous screen\n\n");

	std::array<std::string *, 3> inputs = {&issuer, &length, &prefixes};

	std::array<std::function<bool(const std::string &)>, 3> validators = {
		Card::validate_issuer,
		[](const std::string &input)
		{
			bool ret = true;
			if (input.empty())
			{
				ret = false;
			}
			for (char c : input)
			{
				if (std::isdigit(c) == false)
				{
					ret = false;
					break;
				}
			}

			return ret && Card::validate_length(std::stoi(input));
		},
		[&prefixes, &length](const std::string &input)
		{ return Card::validate_prefix(input, std::stoi(length)); }};

	for (size_t i = 0; i < inputs.size(); ++i)
	{
		while (true)
		{
			int y, x;			 // to store where you are
			getyx(stdscr, y, x); // save current pos

			printw(line[i]);
			refresh();
			char buffer[MAX_PATH];
			getstr(buffer);
			*(inputs[i]) = buffer;

			if (console::check_quit(*(inputs[i])))
			{
				return Card{};
			}

			if (validators[i](*(inputs[i])))
			{
				break;
			}

			printw("Illegal input");
			move(y, 0); // move to beginning of line
			clrtoeol(); // clear line
		}
		clrtobot(); // Clear the screen after each input
	}

	return Card{issuer, std::stoi(length), prefixes};
}

/**
 * @brief Generates a formatted string representation of a Card.
 *
 * The function uses the provided Card object to extract issuer, length, and prefixes information.
 * It pads each string with spaces to match the maximum length and concatenates them.
 * The resulting string provides a visually aligned representation of the Card's attributes.
 *
 * @param max_length The maximum length to pad each component of the Card.
 * @param card The Card object to generate a string representation for.
 * @return A formatted string representation of the Card.
 *
 * @see Card
 */
inline std::string console::get_card_string(int max_length, const Card &card)
{
	// Pad each string with spaces to match the maximum length
	std::string issuer{card.get_issuer()};
	int len{card.get_len()};

	// Truncate issuer if it exceeds max_length
	if (issuer.length() >= max_length)
	{
		int new_len = std::max(0, max_length - 4);
		issuer = issuer.substr(0, new_len) + std::string("... ");
	}
	std::string padded_str1 = issuer + std::string(std::max(0, max_length - static_cast<int>(issuer.length())), ' ');
	std::string padded_str2 = std::to_string(len) + std::string(std::max(0, max_length - static_cast<int>(std::to_string(len).length())), ' ');

	// Concatenate the padded strings
	return padded_str1 + padded_str2 + card.get_prefixes();
}

/**
 * @brief Draws a list of cards on the console, highlighting the currently selected row.
 *
 * This function takes a vector of Card objects and additional parameters to determine the layout and appearance
 * of the displayed cards. It draws the cards on the console, with an option to highlight the selected row.
 *
 * @param cards_vec A vector of Card objects representing the cards to be displayed.
 * @param start_row The index of the first card to be displayed.
 * @param visible_rows The number of rows visible on the console for displaying cards.
 * @param max_offset The maximum length for padding each component of the Card when drawing.
 * @param curr_row_idx The index of the currently selected row (to be highlighted).
 * @param selected_rows A vector of boolean values indicating which rows are selected.
 *
 * The function uses the ncurses library to interact with the console screen.
 * It iterates over the specified range of cards and draws each card along with an optional selection indicator.
 * The currently selected row is highlighted with a reverse attribute.
 *
 * @see Card
 * @see console::get_card_string
 */
void console::draw_cards(const std::vector<Card> &cards_vec, int start_row, int visible_rows, int max_offset, int curr_row_idx, const std::vector<bool> &selected_rows)
{
	int y_offset{3};
	int x_offset{5};

	for (int i{start_row}; i < start_row + visible_rows && i < cards_vec.size(); i++)
	{
		mvprintw(i + y_offset - start_row, x_offset, get_card_string(max_offset, cards_vec[i]).c_str());
		if (selected_rows[i])
		{
			mvprintw(i + y_offset - start_row, 0, "[X]");
		}
		else
		{
			mvprintw(i + y_offset - start_row, 0, "[ ]");
		}
	}

	if (curr_row_idx < cards_vec.size())
	{
		attron(A_REVERSE);
		mvprintw(curr_row_idx + y_offset - start_row, x_offset, get_card_string(max_offset, cards_vec[curr_row_idx]).c_str());
		attroff(A_REVERSE);
	}
}

/**
 * @brief Allows the user to choose and manipulate a list of cards.
 *
 * The function displays a list of cards, allowing the user to navigate, select, and perform various actions on them.
 * Actions include adding a new card, removing selected cards, editing a card, saving changes to the database, and more.
 * The user can navigate the list with arrow keys, select/deselect cards with the spacebar, and perform actions with the Enter key.
 *
 * @param db A shared pointer to the SQLite database.
 * @param db_path The path to the SQLite database.
 * @param cards_vec A vector of Card objects representing the available cards.
 * @param cards_selection A vector of boolean values indicating which cards are selected.
 * @return The index of the selected action (button) when the user exits the card selection interface.
 *
 * @see Card
 * @see console::draw_cards
 * @see console::add_card
 * @see DB_API::write_cards
 */
int console::choose_cards(std::shared_ptr<sqlite3> db, const std::string &db_path, std::vector<Card> &cards_vec, std::vector<bool> &cards_selection)
{
	int x_offset{5};
	int card_offset{25};
	std::string err_msg{};

	err_msg = "";
	std::vector<console::Button> buttons{
		console::Button("Back", 0),
		console::Button("Add", 3),
		console::Button("Remove", 4),
		console::Button("Edit", 5),
		console::Button("Next", 1),
		console::Button("Save", 6),
		console::Button("Exit", 2)};

	int curr_btn_idx{};
	int selected_action{};
	int start_row{};
	int current_row{};

	bool flag{true};

	while (flag)
	{
		int window_h{}, window_w{};
		getmaxyx(stdscr, window_h, window_w);										   // Get window size
		int visible_rows = std::min(window_h - 6, static_cast<int>(cards_vec.size())); // Calculate the number of visible rows
		clear();

		printw("Scroll the list with up/down arrow keys, select with space.\n");
		printw("Use left/right arrow keys for buttons, confirm with enter.\n");
		mvprintw(2, x_offset, "Issuer");
		mvprintw(2, x_offset + card_offset, "Length");
		mvprintw(2, x_offset + card_offset * 2, "Prefixes");

		draw_cards(cards_vec, start_row, visible_rows, card_offset, current_row, cards_selection);
		draw_buttons(buttons, curr_btn_idx); // Draw buttons at the bottom of the screen
		mvprintw(window_h - 2, 0, err_msg.c_str());
		refresh();
		int ch = getch(); // Get user input
		switch (ch)
		{
			// list scrolling
		case KEY_UP:
			current_row = std::max(0, current_row - 1);
			start_row = std::min(current_row, start_row);
			break;
		case KEY_DOWN:
			current_row = std::min(current_row + 1, std::max(0, static_cast<int>(cards_vec.size()) - 1));
			start_row = std::max(start_row, current_row - visible_rows + 1);
			break;
		case KEY_PPAGE:
			current_row = std::max(0, current_row - visible_rows);
			start_row = std::min(current_row, start_row);
			break;
		case KEY_NPAGE:
			current_row = std::min(std::max(0, static_cast<int>(cards_vec.size()) - 1), current_row + visible_rows);
			start_row = std::max(start_row, current_row - visible_rows + 1);
			break;
		case KEY_HOME:
			current_row = 0;
			start_row = 0;
			break;
		case KEY_END:
			current_row = std::max(static_cast<int>(cards_vec.size()) - 1, 0);
			start_row = std::max(start_row, current_row - visible_rows + 1);
			break;
		case ' ':
			cards_selection[current_row] = !cards_selection[current_row];
			break;

			// button scrolling
		case KEY_LEFT:
			curr_btn_idx = (curr_btn_idx - 1 + static_cast<int>(buttons.size())) % static_cast<int>(buttons.size());
			break;
		case KEY_RIGHT:
			curr_btn_idx = (curr_btn_idx + 1) % static_cast<int>(buttons.size());
			break;

#if defined(_WIN64) || defined(_WIN32)
		case 13:
		case PADENTER:
#else
		case 10:
#endif
		{
			// User selected the current button
			selected_action = buttons[curr_btn_idx].m_action;
			switch (selected_action)
			{
			case 1: // next
				if (std::any_of(cards_selection.begin(), cards_selection.end(), [](bool value)
								{ return value; }) == false)
				{
					err_msg = "First select cards";
					break;
				}
			case 0: // back
			case 2: // exit
				flag = false;
				break;

			case 3: // add
			{
				clear();
				Card new_card = add_card();
				if (new_card.empty() == false)
				{
					cards_vec.push_back(new_card);
					cards_selection.push_back(false);
				}
			}
			break;
			case 4: // remove
				if (cards_vec.empty())
				{
					break;
				}
				cards_vec.erase(std::next(cards_vec.begin(), current_row), std::next(cards_vec.begin(), current_row + 1));
				cards_selection.erase(std::next(cards_selection.begin(), current_row), std::next(cards_selection.begin(), current_row + 1));
				start_row = std::max(0, start_row - 1);
				current_row = std::max(0, std::min(static_cast<int>(cards_vec.size()) - 1, current_row));
				break;
			case 5: // edit
			{
				if (cards_vec.empty())
				{
					break;
				}
				clear();
				printw("The old card is:");
				mvprintw(1, x_offset, "Issuer");
				mvprintw(1, x_offset + card_offset, "Length");
				mvprintw(1, x_offset + card_offset * 2, "Prefixes\n");
				mvprintw(2, x_offset, get_card_string(card_offset, cards_vec[current_row]).c_str());

				printw("\n\n");
				Card new_card = add_card();
				if (new_card.empty() == false)
				{
					cards_vec[current_row] = new_card;
				}
			}
			break;
			case 6: // save
				if (DB_API::write_cards(db, cards_vec, err_msg) == 0)
				{
					err_msg = "Saved";
				}
				break;
			default:
				break;
			}
			break;
		}

		default:
			break;
		}
	}

	return selected_action;
}

/**
 * @brief Allows the user to choose the amount of cards to generate.
 *
 * The function displays the estimated time and size based on the chosen amount and allows the user
 * to adjust the amount using arrow keys, enter, or numeric input.
 * The user can navigate buttons for back, next, and exit.
 *
 * @param amount Reference to the DATATYPE variable representing the chosen amount.
 * @return The index of the selected action (button) when the user exits the amount selection interface.
 *
 * @tparam DATATYPE The data type used for representing the chosen amount.
 *
 * @see File::estimate_time
 * @see File::estimate_size
 * @see File::format_time
 */
int console::choose_amount(DATATYPE &amount, const std::string &db_path)
{
	std::vector<console::Button> buttons{
		console::Button("Back", 0),
		console::Button("Next", 1),
		console::Button("Exit", 2)};

	auto zero{static_cast<std::chrono::nanoseconds>(0)};
	auto est_duration = File::estimate_time<std::chrono::nanoseconds>();
	est_duration += File::m_cvv_flag ? File::estimate_cvv_time<std::chrono::nanoseconds>() : zero;
	est_duration += File::m_date_flag ? File::estimate_date_time<std::chrono::nanoseconds>() : zero;
	est_duration += File::m_name_flag ? File::estimate_name_time<std::chrono::nanoseconds>(db_path) : zero;

	static constexpr DATATYPE min_amount{1};
	static constexpr DATATYPE max_amount{std::numeric_limits<DATATYPE>::max() / 2};

	int curr_btn_idx{};
	int selected_action{};
	bool flag{true};
	int jump_value{100};

	while (flag)
	{
		clear();
		std::string f_time{File::format_time<std::chrono::nanoseconds, DATATYPE>(std::chrono::duration_cast<std::chrono::nanoseconds>(est_duration * amount))};
		std::string size_str{File::estimate_size(amount)};
		f_time = File::format_time<std::chrono::nanoseconds, DATATYPE>(std::chrono::duration_cast<std::chrono::nanoseconds>(est_duration * amount));
		size_str = File::estimate_size(amount);
		printw("Choose how many cards to generate\n");
		printw("Use left/right arrow keys for buttons, confirm with enter.\n\n");
		printw("Estimated time: %s\n", f_time.c_str());
		printw("Estimated size: %s\n", size_str.c_str());
		printw("Amount: %s", std::to_string(amount).c_str());

		draw_buttons(buttons, curr_btn_idx); // Draw buttons at the bottom of the screen
		refresh();
		int ch = getch(); // Get user input
		switch (ch)
		{
			// button scrolling
		case KEY_LEFT:
			curr_btn_idx = (curr_btn_idx - 1 + static_cast<int>(buttons.size())) % static_cast<int>(buttons.size());
			break;
		case KEY_RIGHT:
			curr_btn_idx = (curr_btn_idx + 1) % static_cast<int>(buttons.size());
			break;

#if defined(_WIN64) || defined(_WIN32)
		case 13:
		case PADENTER:
#else
		case 10:
#endif
		{
			selected_action = buttons[curr_btn_idx].m_action;
			switch (selected_action)
			{
			case 1: // next
				if (amount < min_amount)
				{
					break;
				}
			case 0: // back
			case 2: // exit
				flag = false;
				break;
			default:
				break;
			}
		}
		break;

		case KEY_UP:
			if (amount < max_amount)
			{
				amount++;
			}
			break;
		case KEY_DOWN:
			if (amount > 0)
			{
				amount--;
			}
			break;
		case KEY_PPAGE:
			amount = std::min(amount + jump_value, max_amount);
			break;
		case KEY_NPAGE:
			if (amount >= jump_value)
			{
				amount -= jump_value;
			}
			else
			{
				amount = 0;
			}
			break;
		case KEY_HOME:
			amount = 0;
			break;
		case KEY_END:
			amount = max_amount;
			break;
#if defined(_WIN64) || defined(_WIN32)
		case VK_BACK:
#else
		case KEY_BACKSPACE:
#endif
			amount /= 10;
			break;
		default:
		{
			int num = ch - '0';
			if (num < 0 || num > 9)
			{
				break;
			}

			if (std::to_string(amount).size() < std::to_string(max_amount).size())
			{
				amount = amount * 10 + num;
			}
		}
		break;
		}
	}

	return selected_action;
}

/**
 * @brief Prompts the user to enter a file path.
 *
 * This function interacts with the console using the ncurses library to prompt the user
 * for a file path. It validates the input, checks if the file already exists, and
 * prompts the user for confirmation if it does. The chosen path is stored in the provided string.
 *
 * @param path Reference to the string variable to store the user-entered file path.
 *
 * @see DB_API::check_file_exists
 * @see console::yes_no
 */
void console::get_path(std::string &path)
{
	bool flag{true};
	std::string err_msg{};

	while (flag)
	{
		clear();
		printw("%s\n", err_msg.c_str());
		printw("Enter path: ");
		refresh();

		char buffer[MAX_PATH];
		wgetstr(stdscr, buffer); // Read input into db_path
		path = buffer;
		if (path.empty())
		{
			err_msg = "Empty string is not allowed";
		}
		else if (DB_API::check_file_exists(path))
		{
			std::string msg = "The file " + path + " already exists, would you like to overwrite it?";
			if (yes_no(msg))
			{
				flag = false;
			}
		}
		else
		{
			flag = false;
		}
	}
}

/**
 * @brief Allows the user to choose a file path for exporting or generating data.
 *
 * This function presents a user interface for selecting a file path. The user can edit the
 * current file path, proceed to the next step (e.g., data generation), go back, or exit.
 *
 * @param exp_path Reference to the string variable storing the chosen file path.
 * @return The index of the selected action (button) when the user exits the file selection interface.
 *
 * @see console::get_path
 */
int console::choose_file(std::string &exp_path)
{
	std::vector<console::Button> buttons{
		console::Button("Back", 0),
		console::Button("Edit", 3),
		console::Button("Next", 1),
		console::Button("Exit", 2)};

	int curr_btn_idx{};
	bool flag{true};
	int selected_action{};

	while (flag)
	{
		clear();
		printw("Use left/right arrow keys for buttons, confirm with enter.\n");
		printw("Current file: %s", exp_path.c_str());
		draw_buttons(buttons, curr_btn_idx); // Draw buttons at the bottom of the screen
		refresh();
		int ch = getch(); // Get user input

		switch (ch)
		{
			// button scrolling
		case KEY_LEFT:
			curr_btn_idx = (curr_btn_idx - 1 + static_cast<int>(buttons.size())) % static_cast<int>(buttons.size());
			break;
		case KEY_RIGHT:
			curr_btn_idx = (curr_btn_idx + 1) % static_cast<int>(buttons.size());
			break;

#if defined(_WIN64) || defined(_WIN32)
		case 13:
		case PADENTER:
#else
		case 10:
#endif
		{
			selected_action = buttons[curr_btn_idx].m_action;
			switch (selected_action)
			{
			case 3: // edit
			{
				std::string p{exp_path};
				get_path(p);
				if (p.empty() == false)
				{
					exp_path = p;
				}
			}
			break;
			case 0: // back
			case 1: // next (generate)
			case 2: // exit
				flag = false;
				break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}

	return selected_action;
}

/**
 * @brief Initiates and manages the generation of data and exporting to a file.
 *
 * This function presents a user interface for controlling the generation and exporting of data.
 * It allows the user to start, pause, stop, go back, or exit the data generation process.
 * The progress is displayed on the screen.
 *
 * @param exp_path The file path for exporting the generated data.
 * @param cards_vec A vector of Card objects representing the available cards.
 * @param cards_selection A vector of boolean values indicating which cards are selected.
 * @param amount The amount of data to generate.
 * @param db A shared pointer to the database.
 * @return The index of the selected action (button) when the user exits the generation interface.
 *
 * @tparam DATATYPE The data type used for representing the amount of data to generate.
 * @see File::export_cards
 */
int console::generate(const std::string &exp_path, const std::vector<Card> &cards_vec, const std::vector<bool> &cards_selection, DATATYPE amount, std::shared_ptr<sqlite3> db)
{
	static std::ofstream output_file;
	bool flag{true};

	std::vector<console::Button> buttons{
		console::Button("Back", 0),
		console::Button("Start", 1),
		console::Button("Stop", 3),
		console::Button("Exit", 2)};

	int buttons_amount{static_cast<int>(buttons.size())};
	int curr_btn_idx{};
	int selected_action{};
	timeout(250); // Set a timeout of 250 milliseconds (0.25 seconds) for getch()

	while (flag)
	{
		int window_h{}, window_w{};
		getmaxyx(stdscr, window_h, window_w); // Get window size
		clear();
		printw("Use left/right arrow keys for buttons, confirm with enter.\n");
		if (g_started == false && buttons[1].m_label[0] != 'S')
		{
			buttons[1].m_label = "Start";
		}
		draw_buttons(buttons, curr_btn_idx); // Draw buttons at the bottom of the screen

		const int width = window_w / 2;
		const int bar_width = static_cast<int>(g_progress * width);
		std::stringstream prog_stream;
		prog_stream << "[" << std::string(bar_width, '#') << std::string(width - bar_width, ' ') << "] " << std::fixed << std::setprecision(0) << (g_progress * 100) << "%%";

		mvprintw(window_h - 3, 0, prog_stream.str().c_str());
		refresh();

		int ch = getch(); // Get user input

		if (ch == ERR)
		{
			continue;
		}

		switch (ch)
		{
			// button scrolling
		case KEY_LEFT:
			curr_btn_idx = (curr_btn_idx - 1 + buttons_amount) % buttons_amount;
			break;
		case KEY_RIGHT:
			curr_btn_idx = (curr_btn_idx + 1) % buttons_amount;
			break;

#if defined(_WIN64) || defined(_WIN32)
		case 13:
		case PADENTER:
#else
		case 10:
#endif
		{
			selected_action = buttons[curr_btn_idx].m_action;
			switch (selected_action)
			{
			case 1: // Start/Resume/Pause
				if (g_started == false)
				{
					output_file.open(exp_path.c_str(), std::ios::trunc);

					if (output_file.is_open() == false)
					{
						std::string msg = "Couldn't open file, would you like to choose another file?";
						if (yes_no(msg))
						{
							return 0;
						}
						return 2; // exit
					}

					g_started = true;
					g_paused = true;
					g_progress = 0.0f;
					std::thread write_thread(&File::export_cards<DATATYPE>, std::ref(output_file), cards_vec, cards_selection, amount, db);
					write_thread.detach();
				}
				g_paused = !g_paused;
				if (g_paused)
				{
					buttons[1].m_label = "Resume";
				}
				else
				{
					buttons[1].m_label = "Pause";
				}
				break;
			case 0: // Back
			case 2: // Exit
			case 3: // Stop
				if (g_started)
				{
					bool org_state = g_paused;
					g_paused = true;
					std::string msg = "Are you sure you want to stop?";
					if (yes_no(msg))
					{
						g_started = false;
						g_paused = false;
					}
					else
					{
						g_paused = org_state;
						break;
					}
				}
				if (selected_action % 2 == 0) // back or exit
				{
					flag = false;
				}
				break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}

	// Reset timeout to wait indefinitely
	timeout(-1);
	return selected_action;
}

/**
 * @brief Allows the user to choose a start and end date through a console interface.
 *
 * This function presents a user interface for selecting a start and end date.
 * The user can navigate between date fields using 'W', 'A', 'S', 'D' keys,
 * increment or decrement values with the arrow keys, and input numbers directly.
 * Three buttons are available: Save, Cancel, and Exit.
 *
 * If the dates are valid and the user selects Save, the start and end dates
 * are updated in the File::m_date object.
 *
 * @return The index of the selected action (button) when the user exits the interface:
 *         - 0: Save (if the date range is valid)
 *         - 1: Cancel
 *         - 2: Exit
 *
 * @see File::m_date
 * @see Date::days_in_month
 * @see Date::vali_date
 * @see Date::get_date_from_ymd
 */
int console::choose_date()
{
	int y_offset{4};
	int x_offset{5};
	std::vector<console::Button> buttons{
		console::Button("Save", 0),
		console::Button("Cancel", 1),
		console::Button("Exit", 2)};

	std::string err_msg{};
	int curr_btn_idx{};
	int selected_action{};
	int current_date_idx{};
	int inc_dec_arrow{};
	std::array<int, 6> dates_arr{
		// start date
		static_cast<int>(static_cast<unsigned>(File::m_date.start.day())),
		static_cast<int>(static_cast<unsigned>(File::m_date.start.month())),
		static_cast<int>(File::m_date.start.year()),
		// end date
		static_cast<int>(static_cast<unsigned>(File::m_date.end.day())),
		static_cast<int>(static_cast<unsigned>(File::m_date.end.month())),
		static_cast<int>(File::m_date.end.year())};

	bool flag{true};
	bool is_valid{};
	int digit_idx{};

	while (flag)
	{
		// start date
		dates_arr[1] = std::clamp(dates_arr[1], 1, 12);
		dates_arr[2] = std::clamp(dates_arr[2], 0, std::numeric_limits<int>::max());
		dates_arr[0] = std::clamp(dates_arr[0], 1, Date::days_in_month(dates_arr[2], dates_arr[1]));

		// end date
		dates_arr[4] = std::clamp(dates_arr[4], 1, 12);
		dates_arr[5] = std::clamp(dates_arr[5], dates_arr[2], std::numeric_limits<int>::max());
		dates_arr[3] = std::clamp(dates_arr[3], 1, Date::days_in_month(dates_arr[5], dates_arr[4]));

		is_valid = Date::vali_date(dates_arr[0], dates_arr[1], dates_arr[2], dates_arr[3], dates_arr[4], dates_arr[5]);
		if (is_valid)
		{
			err_msg = "";
		}
		else
		{
			err_msg = "End date must be after start date.";
		}

		int window_h{}, window_w{};
		getmaxyx(stdscr, window_h, window_w); // Get window size
		clear();

		printw("Choose the date with w/a/s/d and change it with the arrow keys or numbers.\n");
		printw("The date is in format: dd-mm-yyyy.\n");
		printw("Use left/right arrow keys for buttons, confirm with enter.\n");

		// print the date
		std::string start_date{std::format("{:02}-{:02}-{:04}", dates_arr[0], dates_arr[1], dates_arr[2])};
		std::string end_date{std::format("{:02}-{:02}-{:04}", dates_arr[3], dates_arr[4], dates_arr[5])};

		mvprintw(y_offset, x_offset, "Start date:");
		mvprintw(y_offset, x_offset + 15, start_date.c_str());
		mvprintw(1 + y_offset, x_offset, "End date:");
		mvprintw(1 + y_offset, x_offset + 15, end_date.c_str());

		// highlight the current dd/mm/yyyy
		attron(A_REVERSE);
		std::string selection{};
		if (current_date_idx % 3 == 2)
		{
			selection = std::format("{:04}", dates_arr[current_date_idx]);
		}
		else
		{
			selection = std::format("{:02}", dates_arr[current_date_idx]);
		}
		mvprintw(y_offset + current_date_idx / 3, x_offset + 15 + (current_date_idx % 3 * 3), selection.c_str());
		attroff(A_REVERSE);

		draw_buttons(buttons, curr_btn_idx); // Draw buttons at the bottom of the screen
		mvprintw(window_h - 2, 0, err_msg.c_str());

		refresh();
		inc_dec_arrow = -1;

		int ch = getch(); // Get user input

		// date scrolling
		switch (ch)
		{
		case 'w':
		case 'W':
			current_date_idx -= current_date_idx - 3 >= 0 ? 3 : 0;
			digit_idx = 0;
			break;
		case 'a':
		case 'A':
			current_date_idx -= current_date_idx - 1 >= 0 ? 1 : 0;
			digit_idx = 0;
			break;
		case 's':
		case 'S':
			current_date_idx += current_date_idx + 3 >= 6 ? 0 : 3;
			digit_idx = 0;
			break;
		case 'd':
		case 'D':
			current_date_idx += current_date_idx + 1 >= 6 ? 0 : 1;
			digit_idx = 0;
			break;

		case KEY_UP:
			inc_dec_arrow = 1;
		case KEY_DOWN:
			dates_arr[current_date_idx] += inc_dec_arrow;
			break;

			// button scrolling
		case KEY_LEFT:
			curr_btn_idx = (curr_btn_idx - 1 + static_cast<int>(buttons.size())) % static_cast<int>(buttons.size());
			break;
		case KEY_RIGHT:
			curr_btn_idx = (curr_btn_idx + 1) % static_cast<int>(buttons.size());
			break;

#if defined(_WIN64) || defined(_WIN32)
		case 13:
		case PADENTER:
#else
		case 10:
#endif
		{
			// User selected the current button
			selected_action = buttons[curr_btn_idx].m_action;
			switch (selected_action)
			{
			case 0: // save
				if (is_valid == false)
				{
					break;
				}
				File::m_date.start = Date::get_date_from_ymd(dates_arr[0], dates_arr[1], dates_arr[2]);
				File::m_date.end = Date::get_date_from_ymd(dates_arr[3], dates_arr[4], dates_arr[5]);
			case 1: // cancel
			case 2: // exit
				flag = false;
				break;

			default:
				break;
			}
			break;
		}

		default:
			if (ch < '0' || ch > '9')
			{
				break;
			}
			if (digit_idx >= selection.size())
			{
				digit_idx = 0;
			}

			selection[digit_idx] = ch;
			dates_arr[current_date_idx] = std::stoi(selection);
			digit_idx++;
			break;
		}
	}

	return selected_action;
}

/**
 * @brief Displays an interactive menu to configure data inclusion options and navigate using buttons.
 *
 * This function opens a user interface in the terminal where the user can:
 * - Toggle options like including CVV, name, and expiration date using the up/down arrow keys and spacebar.
 * - Navigate through a set of action buttons ("Back", "Set dates", "Next", "Exit") using the left/right arrow keys.
 * - Confirm their selection using the Enter key.
 *
 * Selected options are stored in static flags within the `File` class if the user chooses "Back" or "Next".
 * The menu closes when "Back", "Next", or "Exit" is selected.
 *
 * @return An integer representing the selected action:
 * - 0: Back
 * - 1: Next
 * - 2: Exit
 */
int console::choose_options()
{
	int y_offset{3};
	int x_offset{5};
	std::vector<console::Button> buttons{
		console::Button("Back", 0),
		console::Button("Set dates", 3),
		console::Button("Next", 1),
		console::Button("Exit", 2)};

	std::array<std::string, 4> options{"Include CVV", "Include name", "Include expiration date", "Delimiter:  "};
	std::array<bool, 3> selected_rows{File::m_cvv_flag, File::m_name_flag, File::m_date_flag};

	int curr_btn_idx{};
	int selected_action{};
	int current_row{};

	bool flag{true};

	while (flag)
	{
		int window_h{}, window_w{};
		getmaxyx(stdscr, window_h, window_w); // Get window size
		clear();

		printw("Scroll the list with up/down arrow keys, select with space.\n");
		printw("Use left/right arrow keys for buttons, confirm with enter.\n");

		for (int i{}; i < options.size() - 1; i++)
		{
			mvprintw(i + y_offset, x_offset, options[i].c_str());
			if (selected_rows[i])
			{
				mvprintw(i + y_offset, 0, "[X]");
			}
			else
			{
				mvprintw(i + y_offset, 0, "[ ]");
			}
		}

		options[3][options[3].size() - 1] = File::m_delimiter;
		mvprintw(3 + y_offset, x_offset, options[3].c_str());

		attron(A_REVERSE);
		mvprintw(current_row + y_offset, x_offset, options[current_row].c_str());
		attroff(A_REVERSE);

		draw_buttons(buttons, curr_btn_idx); // Draw buttons at the bottom of the screen
		refresh();
		int ch = getch(); // Get user input

		switch (ch)
		{
			// list scrolling
		case KEY_UP:
			current_row = std::max(0, current_row - 1);
			break;
		case KEY_DOWN:
			current_row = std::min(current_row + 1, static_cast<int>(options.size()) - 1);
			break;
		case ' ':
			selected_rows[current_row] = !selected_rows[current_row];
			break;

			// button scrolling
		case KEY_LEFT:
			curr_btn_idx = (curr_btn_idx - 1 + static_cast<int>(buttons.size())) % static_cast<int>(buttons.size());
			break;
		case KEY_RIGHT:
			curr_btn_idx = (curr_btn_idx + 1) % static_cast<int>(buttons.size());
			break;

#if defined(_WIN64) || defined(_WIN32)
		case 13:
		case PADENTER:
#else
		case 10:
#endif
		{
			// User selected the current button
			selected_action = buttons[curr_btn_idx].m_action;
			switch (selected_action)
			{
			case 0: // back
			case 1: // next
				File::m_cvv_flag = selected_rows[0];
				File::m_name_flag = selected_rows[1];
				File::m_date_flag = selected_rows[2];
			case 2: // exit
				flag = false;
				break;

			case 3: // set date range
				selected_action = console::choose_date();
				if (selected_action == 2)
				{
					flag = false;
				}
				break;

			default:
				break;
			}
			break;
		}

		default:
			if (current_row == options.size() - 1)
			{
				File::m_delimiter = ch;
			}
			break;
		}
	}

	return selected_action;
}

/**
 * @brief Runs the interactive console application for managing and generating data.
 *
 * This function initializes the console, displays a banner, and guides the user through
 * a series of steps for managing cards, choosing the amount of data to generate, selecting
 * an output file, and initiating the data generation process. The user can exit at any step.
 *
 * The function initializes the console environment and manages user interaction through a series of steps:
 *  - Open or create a database.
 *  - Display and choose cards from the database.
 *  - Choose the amount of data to generate and display estimation.
 *  - Choose the output file for exporting generated data.
 *  - Initiate the data generation process.
 *
 * @param version The version string of the application.
 * @param url The URL associated with the application.
 *
 * @tparam DATATYPE The data type used for representing the amount of data to generate.
 * @see console::get_db
 * @see console::choose_cards
 * @see console::choose_amount
 * @see console::choose_file
 * @see console::generate
 */
void console::run(const std::string &version, const std::string &url, const std::string &license)
{
	initscr();
	curs_set(0);		  // disable cursor visibility
	raw();				  // Disable line buffering
	keypad(stdscr, TRUE); // Enable special keys

	// display banner
	console::print_banner(version, url);
	printw("\nYou can enter \"Exit\"\\\"Quit\" at any time to exit\n\n");
	printw("Press any key to continue");
	refresh();
	getch();

	bool flag{true};
	int step{};
	int choice{};
	const int num_steps{5};
	std::string db_path{};
	std::shared_ptr<sqlite3> db;
	DATATYPE amount{1};
	std::string exp_path{"cards_output.txt"};
	std::string err_msg{};
	std::vector<Card> cards_vec{};
	std::vector<bool> cards_selection;

	while (flag)
	{
		switch (step)
		{
		case 0: // open db
			db = console::get_db(db_path, err_msg);
			if (db == nullptr)
			{
				choice = 2;
				break;
			}

			if (DB_API::read_cards(db, cards_vec, err_msg)) // read db
			{
				choice = 2;
				break;
			}

			choice = 1;
			cards_selection = std::vector<bool>(cards_vec.size(), false);
			break;
		case 1: // display database and choose cards
			choice = console::choose_cards(db, db_path, cards_vec, cards_selection);
			break;
		case 2: // choose cvv/date/name
			choice = choose_options();
			break;
		case 3: // choose amount and display estimation
			choice = console::choose_amount(amount, db_path);
			break;
		case 4: // choose output file
			choice = console::choose_file(exp_path);
			break;
		case 5:
			choice = console::generate(exp_path, cards_vec, cards_selection, amount, db);
			break;
		default:
			break;
		}

		// handle user's choice
		switch (choice)
		{
		case 0: // back
			step--;
			step = std::max(0, step);
			break;
		case 1: // next
			step++;
			step = step > num_steps ? 0 : step;
			break;
		case 2: // exit
			clear();
			printw(err_msg.c_str());
			flag = false;
			break;
		default: // continue
			break;
		}
	}
	printw("Press any key to quit");
	refresh();
	getch();

	endwin();
}