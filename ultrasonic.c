#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_intc.h>
#include <pru_rpmsg.h>
#include <pru_uart.h>

#define GPIO_PATH "/sys/class/gpio/"
#define GPIO_EXPORT_PATH GPIO_PATH "export"
#define GPIO_UNEXPORT_PATH GPIO_PATH "unexport"

#define TRIGGER_PIN "P8_17"
#define ECHO_PIN "P8_16"

// This function sets up the specified GPIO pin
// and returns a file descriptor for the corresponding sysfs file.
int gpio_setup(const char *pin, const char *direction)
{
    // Export GPIO pin
    int fd_export = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd_export < 0)
    {
        perror("Failed to open GPIO export file");
        exit(EXIT_FAILURE);
    }

    if (write(fd_export, pin, strlen(pin)) < 0)
    {
        perror("Failed to export GPIO pin");
        exit(EXIT_FAILURE);
    }

    close(fd_export);

    // Set GPIO direction
    char path[64];
    snprintf(path, sizeof(path), GPIO_PATH "gpio%s/direction", pin);

    int fd_direction = open(path, O_WRONLY);
    if (fd_direction < 0)
    {
        perror("Failed to open GPIO direction file");
        exit(EXIT_FAILURE);
    }

    if (write(fd_direction, direction, strlen(direction)) < 0)
    {
        perror("Failed to set GPIO direction");
        exit(EXIT_FAILURE);
    }

    close(fd_direction);

    // Open GPIO value file and return file descriptor
    snprintf(path, sizeof(path), GPIO_PATH "gpio%s/value", pin);

    int fd_value = open(path, O_RDWR);
    if (fd_value < 0)
    {
        perror("Failed to open GPIO value file");
        exit(EXIT_FAILURE);
    }

    return fd_value;
}

// This function sets the trigger pin to high for a short duration
void trigger_pulse(int fd_trigger)
{
    // Send pulse
    write(fd_trigger, "1", 1);
    usleep(10);
    write(fd_trigger, "0", 1);
}

// This function measures the distance using the HC-SR04 ultrasonic sensor
float distance_measurement(int fd_trigger, int fd_echo)
{
    float distance;

    // Send trigger pulse
    trigger_pulse(fd_trigger);

    // Wait for echo pulse
    struct timespec pulse_start, pulse_end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &pulse_start);

    while (1)
    {
        char value;
        if (pread(fd_echo, &value, 1, 0) < 0)
        {
            perror("Failed to read GPIO value file");
            exit(EXIT_FAILURE);
        }

        if (value == '1')
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &pulse_end);
            break;
        }
    }

    // Calculate distance
    float pulse_duration = (pulse_end.tv_sec - pulse_start.tv_sec) * 1000000.0f +
                           (pulse_end.tv_nsec - pulse_start.tv_nsec) / 1000.0f;

    distance = pulse_duration * 0.034 / 2.0f;

    return distance;
}

int main(int argc, char **argv)
{
    // Setup trigger pin as output
    int fd_trigger = gpio_setup(TRIGGER_PIN, "out");

    // Setup echo pin as input
    int fd_echo = gpio_setup(ECHO_PIN, "in");
    // Print setup completed message
    printf("Setup completed!\n");

    // Wait for sensor to settle
    usleep(500000);

    float distance = distance_measurement(fd_trigger, fd_echo);

    while (1)
    {
        // Measure distance
        distance = distance_measurement(fd_trigger, fd_echo);

        // Print distance
        printf("Distance: %0.2f cm.\n", distance);

        // Check if distance is too close
        if (distance <= 5.0f)
        {
            printf("Too close! Exiting...\n");
            break;
        }

        // Wait before next measurement
        usleep(2000000);
    }

    // Clean up
    close(fd_trigger);
    close(fd_echo);

    // Unexport GPIO pins
    int fd_unexport = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd_unexport < 0)
    {
        perror("Failed to open GPIO unexport file");
        exit(EXIT_FAILURE);
    }

    if (write(fd_unexport, TRIGGER_PIN, strlen(TRIGGER_PIN)) < 0)
    {
        perror("Failed to unexport trigger pin");
        exit(EXIT_FAILURE);
    }

    if (write(fd_unexport, ECHO_PIN, strlen(ECHO_PIN)) < 0)
    {
        perror("Failed to unexport echo pin");
        exit(EXIT_FAILURE);
    }

    close(fd_unexport);

    return 0;
}
