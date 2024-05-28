#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define BUZZER_PIN "/sys/class/gpio/gpio45/value"

int main(int argc, char *argv[])
{
    // Set up the buzzer pin
    int fd, len;
    char value[2];

    fd = open(BUZZER_PIN, O_WRONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to open %s: %s\n", BUZZER_PIN, strerror(errno));
        exit(1);
    }

    // Continuously toggle the buzzer on and off
    while (1)
    {
        // Turn on the buzzer
        len = write(fd, "1", 1);
        if (len < 0)
        {
            fprintf(stderr, "Failed to write buzzer value: %s\n", strerror(errno));
            exit(1);
        }

        // Wait for a short delay
        usleep(1000);

        // Turn off the buzzer
        len = write(fd, "0", 1);
        if (len < 0)
        {
            fprintf(stderr, "Failed to write buzzer value: %s\n", strerror(errno));
            exit(1);
        }

        // Wait for a short delay
        usleep(1000);
    }

    // Cleanup
    close(fd);
    return 0;
}
