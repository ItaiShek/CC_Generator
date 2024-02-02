#define GLFW_EXPOSE_NATIVE_X11

#include "FileDialog.h"
#include "GLFW/glfw3native.h"
/*
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <cstring>
#include <string>

#include <gtk/gtk.h>
extern "C" {
}

#include "nfd.h"
*/


namespace gui
{
	std::string FileDialog::OpenFile(const char* filter, GLFWwindow*& GLFWwindow)
	{
		return std::string{};
	}

	/*
	std::string FileDialog::OpenFile(const char* filter, GLFWwindow*& window) {
		int argc = 0;
		char **argv{};
		gtk_init(&argc, &argv);
	GtkWidget *dialog;
GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
gint res;
		std::string selectedFilePath{};

dialog = gtk_file_chooser_dialog_new ("Open File",
										NULL,
                                      //parent_window,
                                      action,
                                      "_Cancel",
                                      GTK_RESPONSE_CANCEL,
                                      "_Open",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

res = gtk_dialog_run (GTK_DIALOG (dialog));
if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
    filename = gtk_file_chooser_get_filename (chooser);
    
		 selectedFilePath=filename;
    g_free (filename);
  }

gtk_widget_destroy (dialog);
return selectedFilePath;
	}
	*/

/*
	std::string FileDialog::OpenFile(const char* filter, GLFWwindow*& GLFWwindow) {
		
    gtk_init(0, nullptr); // Initialize GTK+ if it's not initialized already

		std::string selectedFilePath;

		Window test_window = glfwGetX11Window(GLFWwindow);
		GtkWidget *file_chooser_dialog = gtk_file_chooser_dialog_new("Open File",
																	NULL,
																	GTK_FILE_CHOOSER_ACTION_OPEN,
																	"Cancel",
																	GTK_RESPONSE_CANCEL,
																	"Open",
																	GTK_RESPONSE_ACCEPT,
																	NULL);


    gtk_window_set_transient_for(GTK_WINDOW(file_chooser_dialog), GDK_WINDOW_XID(parentWindow));
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser_dialog), g_get_home_dir());

		// Set filter if provided
		if (filter != nullptr) {
			GtkFileFilter *file_filter = gtk_file_filter_new();
			gtk_file_filter_set_name(file_filter, filter);
			gtk_file_filter_add_pattern(file_filter, filter);
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser_dialog), file_filter);
		}

		if (gtk_dialog_run(GTK_DIALOG(file_chooser_dialog)) == GTK_RESPONSE_ACCEPT) {
			char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_dialog));
			selectedFilePath = filename;
			g_free(filename);
		}

		gtk_widget_destroy(file_chooser_dialog);

		return selectedFilePath;
}
*/
	/*
	*/

	/*
	std::string FileDialog::OpenFile(const char* filter, GLFWwindow*& GLFWwindow)
	{
		Display* display = XOpenDisplay(NULL);
		if (display==0)
		{
			// Handle error: Unable to open X display
			return std::string();
		}

		//Window root = DefaultRootWindow(display);
		
		Window  root = glfwGetX11Window(GLFWwindow);

		XEvent event;
		memset(&event, 0, sizeof(event));

		Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
		Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", 0);

		Window dialog = XCreateSimpleWindow(display, root, 0, 0, 400, 200, 1, BlackPixel(display, DefaultScreen(display)), WhitePixel(display, DefaultScreen(display)));
		XSetWMProtocols(display, dialog, &wm_delete_window, 1);

		XTextProperty windowName;
		const char* dialogTitle = "Open";
		XStringListToTextProperty(const_cast<char**>(&dialogTitle), 1, &windowName);

		XSetWMName(display, dialog, &windowName);
		XFree(windowName.value);

		XMapWindow(display, dialog);
		XSelectInput(display, dialog, ExposureMask | KeyPressMask | StructureNotifyMask);

		bool windowClosed = false;
		std::string selectedFile;

		while (!windowClosed)
		{
			XNextEvent(display, &event);

			if (event.type == Expose && event.xexpose.count == 0)
			{
				   XDrawString(display, dialog, DefaultGC(display, root), 10, 50, "Select a file:", strlen("Select a file:"));
           
				// Redraw the window content here
			}
			else if (event.type == KeyPress)
			{
				// Handle key events if needed
			}
			else if (event.type == ClientMessage)
			{
				if (event.xclient.message_type == wm_protocols &&
					static_cast<Atom>(event.xclient.data.l[0]) == wm_delete_window)
				{
					windowClosed = true;
				}
			}
		}

		XDestroyWindow(display, dialog);
		XCloseDisplay(display);

		return selectedFile;
	}

	*/

	/*
	std::string FileDialog::OpenFile(const char* filter, GLFWwindow*& GLFWwindow)
	{
		Window test_window = glfwGetX11Window(GLFWwindow);
        return "Open file";
		return std::string();
	}
	*/

	std::string FileDialog::SaveFile(const char* filter, GLFWwindow*& GLFWwindow)
	{
		Window test_window = glfwGetX11Window(GLFWwindow);
		return "Save file";
		return std::string();
	}
}