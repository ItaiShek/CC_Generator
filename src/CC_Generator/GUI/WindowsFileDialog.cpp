#define GLFW_EXPOSE_NATIVE_WIN32

#include <Windows.h>
#include "FileDialog.h"
#include "GLFW/glfw3native.h"

namespace gui
{
	std::string FileDialog::OpenFile(const char* filter, GLFWwindow*& GLFWwindow)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };
		CHAR currentDir[MAX_PATH] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(GLFWwindow);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		if (GetCurrentDirectory(MAX_PATH, currentDir))
		{
			ofn.lpstrInitialDir = currentDir;
		}

		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}

		return std::string();
	}

	std::string FileDialog::SaveFile(const char* filter, GLFWwindow*& GLFWwindow)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };
		CHAR currentDir[MAX_PATH] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(GLFWwindow);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		if (GetCurrentDirectory(MAX_PATH, currentDir))
		{
			ofn.lpstrInitialDir = currentDir;
		}

		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}

		return std::string();
	}
}