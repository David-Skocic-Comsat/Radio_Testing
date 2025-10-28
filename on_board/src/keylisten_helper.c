#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>

void enable_raw_mode() {
    struct termios tty;

    tcgetattr(STDIN_FILENO, &tty);            // Get current settings
    tty.c_lflag &= ~(ICANON | ECHO);          // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);   // Apply new settings
}

void disable_raw_mode() {
    struct termios tty;

    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ICANON | ECHO;             // Re-enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}
