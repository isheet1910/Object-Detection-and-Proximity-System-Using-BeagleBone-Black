#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define BUTTON1_PIN "/sys/class/gpio/gpio44/value"

int main(int argc, char *argv[])
{
    int button1_fd;
    struct pollfd button1_poll;
    char button1_value;

    // Export GPIO pins and set their direction to input
    system("config-pin P8_12 gpio");
    system("echo in > /sys/class/gpio/gpio44/direction");
    // Open file descriptors for both buttons
    button1_fd = open(BUTTON1_PIN, O_RDONLY);

    // Set up the pollfd structs for polling button events
    button1_poll.fd = button1_fd;
    button1_poll.events = POLLPRI;

    // Clear any initial button state
    read(button1_fd, &button1_value, sizeof(button1_value));

    // Wait for button events and print messages when they occur
    while (1)
    {
        lseek(button1_fd, 0, SEEK_SET);
        poll(&button1_poll, 1, -1);

        if (button1_poll.revents & POLLPRI)
        {
            read(button1_fd, &button1_value, sizeof(button1_value));
            printf("Button 1 pressed\n");
        }

        usleep(100000); // Pause for a short period to debounce buttons
    }

    // Clean up by closing file descriptors and unexporting GPIO pins
    close(button1_fd);
    system("echo 44 > /sys/class/gpio/unexport");

    return 0;
}
