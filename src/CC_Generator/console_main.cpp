/**
 * @file main.cpp
 * @brief Entry point for the console application.
 *
 * This file contains the main function which initializes the console application.
 * It calls the `console::run` function to start the application with version, URL, and license information.
 *
 * @see Console.h for the declaration of the console::run function.
 * @see ProjectInfo_ for version, project URL, and license details.
 */

 #include "Console.h"
 #include "ProjectInfo_.h"

 /**
  * @brief Main function to run the console version of the application.
  *
  * The main function retrieves version, project URL, and license information from
  * the `ProjectInfo` class and then calls the `console::run` function to start
  * the console application.
  *
  * @param argc The number of command-line arguments.
  * @param argv The array of command-line argument strings.
  *
  * @return Returns 0 if the application runs successfully.
  */
 int main(int argc, char* argv[])
 {
     // Run the console application with the specified project details
     console::run(ProjectInfo::version, ProjectInfo::project_url, ProjectInfo::license);
     return 0;
 }