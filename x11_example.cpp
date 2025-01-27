#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <sys/select.h>
#include <unistd.h>  // For select() and usleep()
#include <queue>
#include <algorithm>

struct box
{
    int BOX_HEIGHT;
    int BOX_WIDTH;
    int box_x;
    int box_y;    
};

int main() {
    // Globals 
    const int BOX_WIDTH = 50;
    const int BOX_HEIGHT = 50;
    const int DISPLAY_WIDTH = 800;
    const int DISPLAY_HEIGHT = 600;


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
        DISPLAY_WIDTH, DISPLAY_HEIGHT,  // width, height
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
    box initBox;
    initBox.BOX_HEIGHT = 50;
    initBox.BOX_WIDTH = 50;
    initBox.box_x = 0;
    initBox.box_y = 0;
    
    int direction = 0; //0 right, 1 down, 2 left, 3 up
    std::deque<box> snake;
    snake.push_back(initBox);
    // Event loop
    XEvent event;
    bool running = true;
    bool firstloop = true;
    box nextSegment;
    nextSegment.box_x = 150;
    nextSegment.box_y = 150;
    while (running) {
        // Set up the timeout (e.g., 1 second timeout)

        if (XPending(display) > 0 || firstloop) {
            XNextEvent(display, &event);
            firstloop = false;
            // std::cout << "event" << std::endl;
            switch (event.type) {
                
                // case Expose:
                //     // Redraw the black box
                //     XClearWindow(display, window);
                //     XFillRectangle(display, window, DefaultGC(display, screen), box_x, box_y, BOX_WIDTH, BOX_HEIGHT);
                //     break;

                case KeyPress: {
                    // Detect which key was pressed
                    KeySym keysym = XLookupKeysym(&event.xkey, 0);

                    // Move the box based on arrow keys
                    switch (keysym) {
                        case XK_Up:
                            direction = 3;
                            break;
                        case XK_Down:
                            direction = 1;
                            break;
                        case XK_Left:
                            direction = 2;
                            break;
                        case XK_Right:
                            direction = 0;
                            break;
                        case XK_space:
                            
                            break;
                        case XK_Escape:
                            running = false; // Exit on Escape key
                            break;
                    }
                    break;
                }

                case DestroyNotify:
                    // Handle window destruction
                    running = false;
                    break;
            }
        }
        
        // std::cout << "update loc" << std::endl;
        box currBox;
        box lastBox = snake.front();
        switch (direction)
        {
        case 0:
            currBox.box_x = (lastBox.box_x >= DISPLAY_WIDTH - BOX_WIDTH) ? 0 : lastBox.box_x + BOX_WIDTH; // Move right
            break;
        case 1:
            currBox.box_y = (lastBox.box_y >= DISPLAY_HEIGHT - BOX_HEIGHT) ? 0 : lastBox.box_y + BOX_HEIGHT; // Move down
            break;
        case 2:
            currBox.box_x = (lastBox.box_x < BOX_WIDTH) ? DISPLAY_WIDTH - BOX_WIDTH : lastBox.box_x - BOX_WIDTH; // Move left
            break;
        case 3:
            currBox.box_y = (lastBox.box_y < BOX_HEIGHT) ? DISPLAY_HEIGHT - BOX_HEIGHT : lastBox.box_y - BOX_HEIGHT; // Move up
            break;
        }

        snake.push_front(currBox);
        if (currBox.box_x != nextSegment.box_x || currBox.box_y != nextSegment.box_y) {
            snake.pop_back();
        }
        

        XClearWindow(display, window);
        for (box b : snake) {
            XFillRectangle(display, window, DefaultGC(display, screen), b.box_x, b.box_y, BOX_WIDTH, BOX_HEIGHT);
        }
       
        XFillRectangle(display, window, DefaultGC(display, screen), nextSegment.box_x, nextSegment.box_y, BOX_WIDTH, BOX_HEIGHT);
        usleep(150000);
    }

    // Close the connection to the X server
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
