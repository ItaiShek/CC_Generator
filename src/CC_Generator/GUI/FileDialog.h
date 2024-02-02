#pragma once
#include <string>
#include "GLFW//glfw3.h"

namespace gui
{
	namespace FileDialog
	{
		std::string OpenFile(const char* filter, GLFWwindow*& GLFWwindow);
		std::string SaveFile(const char* filter, GLFWwindow*& GLFWwindow);
	}
}