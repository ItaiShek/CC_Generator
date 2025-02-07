#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <thread>
#include <algorithm>
#include <limits>
#include <atomic>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

#include "ImGuiFileDialog.h"
#include "misc/cpp/imgui_stdlib.h"

#include "DB_API.h"
#include "Card.h"
#include "File.h"

#if defined(_WIN64) || defined(_WIN32)
#define NOMINMAX
#endif

#if defined(__x86_64) || defined(_M_X64)
static ImGuiDataType imgui_data_type = ImGuiDataType_U64;
#define DATATYPE ImU64
#elif defined(__i386) || defined(_M_IX86)
static ImGuiDataType imgui_data_type = ImGuiDataType_U32;
#define DATATYPE ImU32
#endif


// std::mt19937 File::gen(std::random_device{}());

// global variables
extern std::atomic<bool> g_paused;
extern std::atomic<bool> g_started;
extern std::atomic<float> g_progress;

namespace gui
{
    /**
     * @brief Function signature for the GUI application entry point.
     *
     * This function is the entry point for the GUI application and is meant
     * to be called from external code to start the application.
     *
     * @param version A string representing the version information.
     * @param url A string containing the URL information.
     * @param license A string containing the license information.
     */
    void run(const std::string& version, const std::string& url, const std::string& license);

    /**
    * @brief Template class for creating GUI applications using GLFW and ImGui.
    *
    * This template provides a basic structure for creating GUI applications using
    * GLFW for window management and ImGui for GUI rendering.
    *
    * @tparam DerivedClass The derived class implementing specific functionality.
    */
    template <typename DerivedClass>
    class GuiApp
    {
    public:
        /**
         * @brief Constructor for the GuiApp class.
         *
         * Initializes GLFW, creates a window, and sets up ImGui.
         *
         * @param title The title of the application window.
         * @param url A string containing the URL information.
         * @param window_w The width of the application window.
         * @param window_h The height of the application window.
         */
        GuiApp(const std::string& title, const std::string& url, const int window_w, const int window_h)
        {
            // Initialize GLFW
            if (!glfwInit())
            {
                std::cerr << "Couldn't initialize GLFW" << std::endl;
                std::exit(1);
            }

            // Create a windowed mode window and its OpenGL context
            m_window = glfwCreateWindow(window_w, window_h, title.c_str(), NULL, NULL);
            if (m_window == NULL)
            {
                glfwTerminate();
                std::cerr << "Couldn't create window" << std::endl;
                std::exit(1);
            }

            // center the window
            {
                // Get the primary monitor
                GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

                // Get the video mode of the primary monitor
                const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

                // Calculate the center position of the screen
                int screenWidth = mode->width;
                int screenHeight = mode->height;
                int windowWidth, windowHeight;
                glfwGetWindowSize(m_window, &windowWidth, &windowHeight);

                int xPos = (screenWidth - windowWidth) / 2;
                int yPos = (screenHeight - windowHeight) / 2;

                // Set the window position
                glfwSetWindowPos(m_window, xPos, yPos);
            }

            // Initialize Dear ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            glfwMakeContextCurrent(m_window);
            ImGuiIO& io = ImGui::GetIO(); (void)io;

            ImGui_ImplOpenGL3_Init("#version 330");
            glfwSwapInterval(1);    // idling
        }

        /**
        * @brief Destructor for the GuiApp class.
        *
        * Cleans up resources allocated during the initialization.
        */
        ~GuiApp()
        {
            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            glfwDestroyWindow(m_window);
            glfwTerminate();
        }

        /**
         * @brief Main loop of the GUI application.
         *
         * Enters the main loop where events are polled, ImGui frames are started,
         * and rendering is performed until the window is closed.
         */
        void run()
        {
            startup();

            // Main loop
            while (!glfwWindowShouldClose(m_window))
            {
                // Poll events and handle window close
                glfwPollEvents();
                if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                {
                    glfwSetWindowShouldClose(m_window, true);
                }

                // Start the ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                update();

                // Rendering
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(m_window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                // Swap buffers
                glfwSwapBuffers(m_window);
            }
        }

        /**
         * @brief Function called to update the GUI application state.
         *
         * This function should be implemented in the derived class to handle
         * application-specific updates.
         */
        void update()
        {
            static_cast<DerivedClass*>(this)->update();
        }

        /**
         * @brief Function called at the start of the GUI application.
         *
         * This function should be implemented in the derived class to handle
         * application-specific startup procedures.
         */
        void startup()
        {
            static_cast<DerivedClass*>(this)->startup();
        }

        GLFWwindow* m_window{}; ///< Pointer to the GLFW window object.
    };
}