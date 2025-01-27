#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>

int main() {
    // Open connection to the X server
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X display\n";
        return -1;
    }

    // Get the default screen
    int screen = DefaultScreen(display);

    // Get the root window of the screen
    Window root = RootWindow(display, screen);

    // Create a simple window
    Window window = XCreateSimpleWindow(
        display, root,
        100, 100,                // x, y position
        800, 600,                // width, height
        1,                       // border width
        BlackPixel(display, screen), // border color
        WhitePixel(display, screen)  // background color
    );

    // Set the title of the window
    XStoreName(display, window, "Move the Black Box");

    // Select events to listen for
    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);

    // Map (show) the window
    XMapWindow(display, window);

    // Initial position of the black box
    int box_x = 20, box_y = 20;
    int box_width = 100, box_height = 100;

    // Event loop
    XEvent event;
    bool running = true;
    while (running) {
        XNextEvent(display, &event);

        switch (event.type) {
            case Expose:
                // Redraw the black box
                XClearWindow(display, window);
                XFillRectangle(display, window, DefaultGC(display, screen), box_x, box_y, box_width, box_height);
                break;

            case KeyPress: {
                // Detect which key was pressed
                KeySym keysym = XLookupKeysym(&event.xkey, 0);

                // Move the box based on arrow keys
                switch (keysym) {
                    case XK_Up:
                        box_y = std::max(0, box_y - 10); // Move up
                        break;
                    case XK_Down:
                        box_y = std::min(600 - box_height, box_y + 10); // Move down
                        break;
                    case XK_Left:
                        box_x = std::max(0, box_x - 10); // Move left
                        break;
                    case XK_Right:
                        box_x = std::min(800 - box_width, box_x + 10); // Move right
                        break;
		    case XK_space:
			box_width++;
			box_height++;
			break;
                    case XK_Escape:
                        running = false; // Exit on Escape key
                        break;
                }

                // Redraw the box at the new position
                XClearWindow(display, window);
                XFillRectangle(display, window, DefaultGC(display, screen), box_x, box_y, box_width, box_height);
                break;
            }

            case DestroyNotify:
                // Handle window destruction
                running = false;
                break;
        }
    }

    // Close the connection to the X server
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
