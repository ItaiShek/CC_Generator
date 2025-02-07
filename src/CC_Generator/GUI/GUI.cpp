#include "GUI.h"

std::mt19937 File::gen(std::random_device{}());
std::atomic<bool> g_paused{ false };
std::atomic<bool> g_started{ false };
std::atomic<float> g_progress{ 0.0f };

/**
 * @brief Initializes a vector of Card objects from a database.
 *
 * This function initializes a vector of Card objects by reading data from a database file.
 * If the database file does not exist or there is an issue reading the database, an empty
 * vector is returned.
 *
 * @return A vector of Card objects initialized from the database.
 */
std::vector<Card> initialize_cards_vec()
{
    std::vector<Card> temp_vec{};
    std::string db_path = "cards.db";

    if (DB_API::check_file_exists(db_path) == false)
    {
        return temp_vec;
    }

    std::shared_ptr<sqlite3> db{ DB_API::read_db(db_path) };

    if (db == nullptr)
    {
        return temp_vec;
    }

    std::string err_msg{};
    DB_API::read_cards(db, temp_vec, err_msg);

    return temp_vec;
}

class App : public gui::GuiApp<App>
{
public:
    App(const std::string& title, const std::string& url, const std::string& license, const int window_w, const int window_h) : GuiApp{ title, url, window_w, window_h }, m_title{ title }, m_url{ url }, m_license{ license } {};
    ~App() = default;

    void startup()
    {
        m_window_flags |= ImGuiWindowFlags_NoDecoration;
        m_window_flags |= ImGuiWindowFlags_NoMove;
        m_window_flags |= ImGuiWindowFlags_NoCollapse;
        m_window_flags |= ImGuiWindowFlags_NoTitleBar;
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        m_duration = File::estimate_time< std::chrono::nanoseconds>();
    }

    void update()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        static bool disable_start_btn;

        // ImGui content
        ImGui::Begin("MainWindow", NULL, m_window_flags);

        ImVec2 main_window_size{ ImGui::GetWindowSize() };
        ImVec2 popup_min_window_size{ ImVec2(main_window_size.x * 0.5f, main_window_size.y * 0.5f) };

        static std::vector<Card> cards_vec{ initialize_cards_vec()};
        static std::vector<bool> cards_selection( cards_vec.size(), false );
        static int current_card{-1};

        ImGui::BeginDisabled(g_started);
        // Child 1 - Database
        {
            static ImGuiTextFilter db_filter{};
            ImVec2 child_window_size{ ImGui::GetContentRegionAvail() };
            ImGui::BeginChild("Child_L", ImVec2(child_window_size.x * 0.5f, child_window_size.y), false, ImGuiWindowFlags_NoDecoration);
            // Child 1.1 - Database list
            {
                ImGui::BeginChild("Child_L_1", ImVec2(0, ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

                if (ImGui::BeginTable("cards_columns", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
                {
                    static std::array<const char*, 4> table_header{ "", "Issuer" ,"Length" ,"Prefixes" };
                    for (const auto& header_col : table_header)
                    {
                        ImGui::TableNextColumn();
                        ImGui::TextColored(ImVec4(255, 0, 0, 1), header_col);
                    }

                    bool sel{};
                    disable_start_btn = false;
                    for(size_t i{}; i<cards_vec.size();i++)
                    {
                        std::string issuer_str{ cards_vec[i].get_issuer()};
                        std::string length_str{ std::to_string(cards_vec[i].get_len()) };
                        std::string prefixes_str{ cards_vec[i].get_prefixes() };

                        const char* issuer{ issuer_str.c_str() };
                        const char* length{ length_str.c_str() };
                        const char* prefixes{ prefixes_str.c_str() };
                        if (db_filter.PassFilter(issuer) || db_filter.PassFilter(length) || db_filter.PassFilter(prefixes))
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            // if you have a better and more efficient solution then please suggest it =)
                            sel = cards_selection[i];
                            ImGui::Checkbox(("##checkbox_"+std::to_string(i)).c_str(), &sel);
                            cards_selection[i] = sel;
                            disable_start_btn |= sel;

                            ImGui::TableNextColumn();
                            if (ImGui::Selectable(issuer, current_card == i, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                if (current_card == i)
                                {
                                    current_card = -1;  // deselect
                                }
                                else
                                {
                                    current_card = i;   // select
                                }
                            }

                            ImGui::TableNextColumn();
                            ImGui::Text(length);
                            ImGui::TableNextColumn();
                            ImGui::Text(prefixes);
                        }

                    }
                    disable_start_btn = !disable_start_btn;
                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }
            ImGui::Separator();
            // Child 1.2 - Filter
            {
                ImGui::BeginChild("Child_L_2", ImVec2(0, 0), false, ImGuiWindowFlags_NoDecoration);
                ImGui::Text("Filter:");
                ImGui::SameLine();
                db_filter.Draw("##db_filter");
                ImGui::EndChild();
            }
            ImGui::EndChild();
        }
        ImGui::EndDisabled();


        ImGui::SameLine();

        // Child 2 - Actions
        {
            ImGui::BeginChild("Child_R", ImVec2(0, ImGui::GetContentRegionAvail().y), false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::SeparatorText("Database Actions");
            // Child 2.1 - Database actions
            {
                ImGui::BeginChild("Child_R1", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.3f), false, ImGuiWindowFlags_HorizontalScrollbar);
                static const std::array<const char*, 8> db_actions{
                    "Clear",
                    "Open",
                    "Save",
                    "Add",
                    "Edit",
                    "Remove",
                    "Check all",
                    "Uncheck all",
                };
                static std::string err_msg{};
                static size_t action;
                action = db_actions.size();
                static constexpr const char* filters{ "Sqlite database files (*.db){.db},All files (*.*){.*}" };

                ImVec2 window_size{ ImGui::GetContentRegionAvail() };
                ImVec2 button_size{ -FLT_MIN,window_size.y * 0.175f };
                if (ImGui::BeginTable("Database_actions", 3, ImGuiTableFlags_SizingStretchSame))
                {
                    for (size_t i{}; i < db_actions.size(); i++)
                    {
                        ImGui::TableNextColumn();
                        ImGui::BeginDisabled(g_started);
                        if (ImGui::Button(db_actions[i], button_size))
                        {
                            action = i;
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::EndTable();
                }

                // Handle db actions
                switch (action)
                {
                case 0: // clear cards
                    if (cards_vec.empty() == false)
                    {
                        ImGui::OpenPopup("Clear");
                    }
                    break;
                case 1:  // open database
                    ImGuiFileDialog::Instance()->OpenDialog("OpenDlg", "Open Database", filters, ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
                    //action = db_actions.size();
                    break;
                case 2: // save database
                    ImGuiFileDialog::Instance()->OpenDialog("SaveDlg", "Save Database", filters, ".", "", 1, nullptr, ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
                    break;
                case 3: // add new card
                    ImGui::OpenPopup("Add");
                    break;
                case 4: // edit card
                    if (current_card < cards_vec.size())
                    {
                        ImGui::OpenPopup("Edit");
                    }
                    break;
                case 5: // remove card
                    if (current_card < cards_vec.size())
                    {
                        cards_vec.erase(std::next(cards_vec.begin(), current_card), std::next(cards_vec.begin(), current_card + 1));
                        cards_selection.erase(std::next(cards_selection.begin(), current_card), std::next(cards_selection.begin(), current_card + 1));
                    }
                    break;
                case 6: // select all
                    std::fill(cards_selection.begin(), cards_selection.end(), true);
                    break;
                case 7:
                    std::fill(cards_selection.begin(), cards_selection.end(), false);
                    break;
                default:
                    break;
                }

                // clear
                ImGui::SetNextWindowSizeConstraints(ImVec2(main_window_size.x * 0.25f, main_window_size.y * 0.25f), ImVec2(FLT_MAX, FLT_MAX));
                if (ImGui::BeginPopupModal("Clear"))
                {
                    ImVec2 popup_window_size{ ImGui::GetWindowSize() };
                    ImVec2 button_size{ ImVec2(popup_window_size.x * 0.3f, popup_window_size.y * 0.15f) };
                    ImGui::TextWrapped("Do you want to clear the current database?");
                    ImGui::SetCursorPos(ImVec2(popup_window_size.x / 2 - button_size.x, popup_window_size.y - button_size.y * 2));

                    if (ImGui::Button("Yes", button_size))
                    {
                        cards_vec.clear();
                        cards_selection.clear();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("No", button_size))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                // open database
                if (ImGuiFileDialog::Instance()->Display("OpenDlg", ImGuiWindowFlags_NoCollapse, popup_min_window_size))
                {
                    // action if OK
                    if (ImGuiFileDialog::Instance()->IsOk())
                    {
                        std::string db_path = ImGuiFileDialog::Instance()->GetFilePathName();

                        if (DB_API::check_file_exists(db_path) == false)
                        {
                            ImGui::OpenPopup("DB Error");
                            err_msg = "File not found.";
                        }
                        else
                        {
                            std::shared_ptr<sqlite3> db{ DB_API::read_db(db_path) };

                            if (db == nullptr)
                            {
                                ImGui::OpenPopup("DB Error");
                                err_msg = "Failed to open the database.";
                            }
                            else
                            {
                                if (DB_API::read_cards(db, cards_vec, err_msg) == 0)
                                {
                                    cards_selection = std::vector<bool>(cards_vec.size(), false);
                                }
                                else
                                {
                                    ImGui::OpenPopup("DB Error");
                                }
                            }
                        }
                    }
                    ImGuiFileDialog::Instance()->Close();
                    action = db_actions.size();
                }

                // save database
                if (ImGuiFileDialog::Instance()->Display("SaveDlg", ImGuiWindowFlags_NoCollapse, popup_min_window_size))
                {
                    if (ImGuiFileDialog::Instance()->IsOk())
                    {
                        std::string db_path = ImGuiFileDialog::Instance()->GetFilePathName();

                        std::shared_ptr<sqlite3> db{ DB_API::read_db(db_path) };

                        if (db == nullptr)
                        {
                            ImGui::OpenPopup("DB Error");
                            err_msg = "Failed to open the database.";
                        }
                        else
                        {
                            if (DB_API::write_cards(db, cards_vec, err_msg) != 0)
                            {
                                ImGui::OpenPopup("DB Error");
                            }
                        }
                    }
                    ImGuiFileDialog::Instance()->Close();
                    action = db_actions.size();
                }

                // add new card
                static bool first_time{ true };
                static std::string issuer_add{};
                static int length_add{  };
                static std::string prefixes_add{};

                ImGui::SetNextWindowSizeConstraints(ImVec2(main_window_size.x * 0.25f, main_window_size.y * 0.25f), ImVec2(FLT_MAX, FLT_MAX));
                if (ImGui::BeginPopupModal("Add"))
                {
                    ImVec2 popup_window_size{ ImGui::GetWindowSize() };
                    ImVec2 button_size{ ImVec2(popup_window_size.x * 0.3f, popup_window_size.y * 0.15f) };

                    if (first_time)
                    {
                        issuer_add = "";
                        length_add = 16;
                        prefixes_add = "";
                        first_time = false;
                    }

                    ImGui::Text("Issuer:  ");
                    ImGui::SameLine();
                    ImGui::InputText("##issuer_input", &issuer_add);

                    ImGui::Text("Length:  ");
                    ImGui::SameLine();
                    ImGui::DragInt("##length_input", &length_add, 1.0f, 2, 32, "%d", ImGuiSliderFlags_AlwaysClamp);

                    ImGui::Text("Prefixes:");
                    ImGui::SameLine();
                    ImGui::InputText("##prefiexes_input", &prefixes_add);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    {
                        ImGui::SetTooltip("Comma delimited, either digits and/or ascending ranges larger then the length.\ne.g.: 1,2,4-5,7,89-1000");
                    }

                    ImGui::SetCursorPos(ImVec2(popup_window_size.x / 2 - button_size.x, popup_window_size.y - button_size.y * 2));
                    ImGui::BeginDisabled(Card::validate_card(issuer_add, length_add, prefixes_add) == false);
                    if (ImGui::Button("Add", button_size))
                    {
                        Card new_card{ std::string(issuer_add), length_add, std::string(prefixes_add) };
                        auto it = std::find(cards_vec.begin(), cards_vec.end(), new_card);
                        if (it == cards_vec.end())
                        {
                            cards_vec.push_back(new_card);
                            cards_selection.push_back(false);
                        }
                        first_time = true;
                        ImGui::CloseCurrentPopup();
                        action = db_actions.size();
                    }
                    ImGui::EndDisabled();
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    {
                        ImGui::SetTooltip("Duplicates will be ignored");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", button_size))
                    {
                        ImGui::CloseCurrentPopup();
                        action = db_actions.size();
                        first_time = true;
                    }
                    ImGui::EndPopup();
                }

                // edit card
                ImGui::SetNextWindowSizeConstraints(ImVec2(main_window_size.x * 0.25f, main_window_size.y * 0.25f), ImVec2(FLT_MAX, FLT_MAX));
                if (ImGui::BeginPopupModal("Edit"))
                {
                    ImVec2 popup_window_size{ ImGui::GetWindowSize() };
                    ImVec2 button_size{ ImVec2(popup_window_size.x * 0.3f, popup_window_size.y * 0.15f) };

                    if (first_time)
                    {
                        issuer_add = cards_vec[current_card].get_issuer();
                        length_add = cards_vec[current_card].get_len();
                        prefixes_add = cards_vec[current_card].get_prefixes();
                        first_time = false;
                    }

                    ImGui::Text("Issuer:  ");
                    ImGui::SameLine();
                    ImGui::InputText("##issuer_input", &issuer_add);

                    ImGui::Text("Length:  ");
                    ImGui::SameLine();
                    ImGui::DragInt("##length_input", &length_add, 1.0f, 2, 32, "%d", ImGuiSliderFlags_AlwaysClamp);

                    ImGui::Text("Prefixes:");
                    ImGui::SameLine();
                    ImGui::InputText("##prefiexes_input", &prefixes_add);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    {
                        ImGui::SetTooltip("Comma delimited, either digits and/or ascending ranges larger then the length.\ne.g.: 1,2,4-5,7,89-1000");
                    }

                    ImGui::SetCursorPos(ImVec2(popup_window_size.x / 2 - button_size.x, popup_window_size.y - button_size.y * 2));
                    ImGui::BeginDisabled(Card::validate_card(issuer_add, length_add, prefixes_add) == false);
                    if (ImGui::Button("Save", button_size))
                    {
                        bool found = std::any_of(cards_vec.begin(), cards_vec.end(), [](const Card& item) -> bool {
                            return (item.get_issuer() == issuer_add) && (item.get_len() == length_add) && (item.get_prefixes() == prefixes_add);
                            });

                        if (found == false)
                        {
                            cards_vec[current_card].set_issuer(issuer_add);
                            cards_vec[current_card].set_len(length_add);
                            cards_vec[current_card].set_prefixes(prefixes_add);
                        }
                        ImGui::CloseCurrentPopup();
                        action = db_actions.size();
                        first_time = true;
                    }
                    ImGui::EndDisabled();
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    {
                        ImGui::SetTooltip("Duplicates will be ignored");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", button_size))
                    {
                        ImGui::CloseCurrentPopup();
                        action = db_actions.size();
                        first_time = true;
                    }
                    ImGui::EndPopup();
                }

                // error message box
                ImGui::SetNextWindowSizeConstraints(ImVec2(main_window_size.x * 0.25f, main_window_size.y * 0.25f), ImVec2(FLT_MAX, FLT_MAX));
                if (ImGui::BeginPopupModal("DB Error"))
                {
                    ImVec2 popup_window_size{ ImGui::GetWindowSize() };
                    ImVec2 button_size{ ImVec2(popup_window_size.x * 0.3f, popup_window_size.y * 0.15f) };
                    ImGui::TextWrapped(err_msg.c_str());
                    ImGui::SetCursorPos(ImVec2(popup_window_size.x / 2 - button_size.x / 2, popup_window_size.y - button_size.y * 2));

                    if (ImGui::Button("OK", button_size))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                //action = db_actions.size();
                ImGui::EndChild();
            }

            static constexpr DATATYPE min_amount{ 1 };
            static constexpr DATATYPE max_amount{ std::numeric_limits<DATATYPE>::max() / 2 };
            static DATATYPE amount{ min_amount };

            // Child 2.2 - Database input
            ImGui::Separator();
            ImGui::BeginDisabled(g_started);
            {

                ImGui::BeginChild("Child_R2", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.5f), false, ImGuiWindowFlags_HorizontalScrollbar);
                {
                    ImGui::Text("Amount:");
                    ImGui::SameLine();
                    static std::string f_time{ File::format_time<std::chrono::nanoseconds, DATATYPE>(std::chrono::duration_cast<std::chrono::nanoseconds>(m_duration * amount)) };
                    ImGui::DragScalar("##amount_slider", imgui_data_type, &amount, 1.0f, &min_amount, &max_amount, 0, ImGuiSliderFlags_AlwaysClamp);
                    static std::string size_str{ File::estimate_size(amount) };

                    // Calculate the total size of the file including newline characters
                    if(ImGui::IsItemEdited())
                    {
                        size_str = File::estimate_size(amount);
                        f_time = File::format_time<std::chrono::nanoseconds, DATATYPE>(std::chrono::duration_cast<std::chrono::nanoseconds>(m_duration * amount));
                    }

                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    {
                        ImGui::SetTooltip("Hold and drag, or double click to enter a value.");
                    }

                    // print estimated file size
                    ImGui::Text("Approximate size:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(255, 0, 0, 1), size_str.c_str());

                    // print estimated time
                    ImGui::Text("ETA:");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(255, 0, 0, 1), f_time.c_str());

                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    {
                        ImGui::SetTooltip("HH:mm:ss:ms");
                    }
                }

                ImGui::EndChild();
            }
            ImGui::EndDisabled();

            // Child 2.3 - Generate widgets
            {
                ImGui::SeparatorText("Genarate");
                ImGui::BeginChild("Child_R3", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.5f), false, ImGuiWindowFlags_HorizontalScrollbar);
                {
                    ImVec2 window_size{ ImGui::GetContentRegionAvail() };
                    ImVec2 button_size{ ImVec2(window_size.x * 0.2f,window_size.y * 0.35f) };

                    static std::string start_button_text{ "Start" };

                    ImGui::BeginDisabled(disable_start_btn);
                    if (ImGui::Button(start_button_text.c_str(), button_size))
                    {
                        if (g_started)
                        {
                            g_paused = !g_paused;
                            if (g_paused)
                            {
                                start_button_text = "Resume";
                            }
                            else
                            {
                                start_button_text = "Pause";
                            }
                        }
                        else
                        {
                            ImGuiFileDialog::Instance()->OpenDialog("GenerateDlg", "Save Cards", "Text Documents (*.txt){.txt},All files (*.*){.*}", ".", "", 1, nullptr, ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
                        }
                    }
                    ImGui::EndDisabled();

                    if (ImGuiFileDialog::Instance()->Display("GenerateDlg", ImGuiWindowFlags_NoCollapse, popup_min_window_size))
                    {
                        if (ImGuiFileDialog::Instance()->IsOk())
                        {
                            std::string exp_path = ImGuiFileDialog::Instance()->GetFilePathName();
                            static std::ofstream output_file;
                            output_file.open(exp_path.c_str(), std::ios::trunc);

                            if (output_file.is_open())
                            {
                                start_button_text = "Pause";
                                g_started = true;
                                g_progress = 0.0f;
                                std::thread write_thread(&File::export_cards<DATATYPE>, std::ref(output_file), cards_vec, cards_selection, amount);
                                write_thread.detach();
                            }
                            else
                            {
                                ImGui::OpenPopup("File Error");
                            }
                        }
                        ImGuiFileDialog::Instance()->Close();
                    }

                    ImGui::SetNextWindowSizeConstraints(ImVec2(main_window_size.x * 0.25f, main_window_size.y * 0.25f), ImVec2(FLT_MAX, FLT_MAX));
                    if (ImGui::BeginPopupModal("File Error"))
                    {
                        ImVec2 popup_window_size{ ImGui::GetWindowSize() };
                        ImVec2 button_size{ ImVec2(popup_window_size.x * 0.3f, popup_window_size.y * 0.15f) };
                        ImGui::TextWrapped("Couldn't open file");
                        ImGui::SetCursorPos(ImVec2(popup_window_size.x / 2 - button_size.x / 2, popup_window_size.y - button_size.y * 2));

                        if (ImGui::Button("OK", button_size))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::SameLine();
                    ImGui::BeginDisabled(g_started == false);
                    static bool old_pause;
                    if (ImGui::Button("Stop", button_size))
                    {
                        old_pause = g_paused;
                        g_paused = true;
                        ImGui::OpenPopup("Stop");
                    }
                    ImGui::EndDisabled();

                    ImGui::SetNextWindowSizeConstraints(ImVec2(main_window_size.x * 0.25f, main_window_size.y * 0.25f), ImVec2(FLT_MAX, FLT_MAX));
                    if (ImGui::BeginPopupModal("Stop"))
                    {
                        ImVec2 popup_window_size{ ImGui::GetWindowSize() };
                        ImVec2 button_size{ ImVec2(popup_window_size.x * 0.3f, popup_window_size.y * 0.15f) };
                        ImGui::TextWrapped("Are you sure you want to stop?");
                        ImGui::SetCursorPos(ImVec2(popup_window_size.x / 2 - button_size.x, popup_window_size.y - button_size.y * 2));

                        if (ImGui::Button("Yes", button_size))
                        {
                            g_started = false;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("No", button_size))
                        {
                            g_paused = old_pause;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }

                    if (g_started == false)
                    {
                        g_paused = false;
                        start_button_text = "Start";
                    }

                    // Calculate the Y position to align the progress bar
                    ImVec2 progress_bar_size{ -1, window_size.y * 0.35f };
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - progress_bar_size.y);
                    ImGui::ProgressBar(g_progress , progress_bar_size);
                }
                ImGui::EndChild();
            }

            // Child 2.4 - Style and about widgets
            {
                ImGui::BeginChild("Child_R4", ImVec2(0, ImGui::GetContentRegionAvail().y), false, ImGuiWindowFlags_HorizontalScrollbar);

                ImVec2 window_size{ ImGui::GetContentRegionAvail() };
                ImVec2 button_size{ ImVec2(window_size.x * 0.25f,window_size.y * 0.4f) };
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + window_size.y - button_size.y);

                static bool enable_dark_mode{ true };
                static std::string style_button_text{ "Light Mode" };
                // Style button
                if (ImGui::Button(style_button_text.c_str(), button_size))
                {
                    enable_dark_mode = !enable_dark_mode;
                    if (enable_dark_mode)
                    {
                        style_button_text = "Light Mode";
                        ImGui::StyleColorsDark();
                    }
                    else
                    {
                        style_button_text = "Dark Mode";
                        ImGui::StyleColorsLight();
                    }
                }

                ImGui::SameLine();

                // About button
                if (ImGui::Button("About", button_size))
                {
                    ImGui::OpenPopup("About");
                }

                ImGui::SetNextWindowSizeConstraints(popup_min_window_size, ImVec2(FLT_MAX, FLT_MAX));
                if (ImGui::BeginPopupModal("About"))
                {
                    ImGui::Text(m_title.c_str());
                    ImGui::Separator();
                    static std::string about_str = "An open-source tool designed to generate random credit card numbers. It operates offline, ensuring your privacy.\nThe database is storred loccally in an SQLite3 file.\n\nThe software is released under the " + m_license + " license.";
                    ImGui::TextWrapped(about_str.c_str());

                    ImVec2 cur_pos = ImGui::GetCursorPos();
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), m_url.c_str());
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetCursorPos(cur_pos);
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), m_url.c_str());
                    }
                    if (ImGui::IsItemClicked())
                    {

#if defined(_WIN64) || defined(_WIN32)
                        static std::string open_url = "start " + m_url;
#elif __linux__
                        static std::string open_url = "xdg-open " + m_url;
#elif __APPLE__
                        static std::string open_url = "open " + m_url;
#endif
                        system(open_url.c_str());
                    }
                    ImVec2 popup_window_size{ ImGui::GetWindowSize() };
                    ImVec2 button_size{ ImVec2(popup_window_size.x * 0.25f, popup_window_size.y * 0.1f) };
                    ImGui::SetCursorPos(ImVec2((popup_window_size.x - button_size.x) / 2, popup_window_size.y - button_size.y*1.5f));

                    if (ImGui::Button("Close", button_size))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();
        }

        // draw a vertical separator in the middle
        float spaceBetween = ImGui::GetStyle().ItemSpacing.x;
        ImGui::GetWindowDrawList()->AddLine(ImVec2(main_window_size.x * 0.5f + spaceBetween * 0.5f, 0), ImVec2(main_window_size.x * 0.5f + spaceBetween * 0.5f, main_window_size.y), ImGui::GetColorU32(ImGuiCol_Separator));

        ImGui::End();
    }
private:
    ImGuiWindowFlags m_window_flags{};
    std::string m_title{};
    std::string m_url{};
    std::string m_license{};
    std::chrono::nanoseconds m_duration{};
};

/**
 * @brief Runs the GUI application with a specified version and URL.
 *
 * This function initializes and runs the GUI application using the provided version
 * and URL information. It creates an instance of the App class with the specified title,
 * URL, and window dimensions and then starts the application loop.
 *
 * @param version The version information to be displayed in the application title.
 * @param url The URL information to be used by the application.
 */
void gui::run(const std::string& version, const std::string& url, const std::string& license)
{
    const std::string title{ "CC_Generator - Version " + version };
    App app{ title , url, license , 1080, 640 };
    app.run();
}