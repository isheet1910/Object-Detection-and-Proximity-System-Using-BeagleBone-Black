#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#define PWM_PATH "/sys/class/pwm/pwmchip0/"
#define PWM_EXPORT_PATH PWM_PATH "export"
#define PWM_UNEXPORT_PATH PWM_PATH "unexport"

#define PWM_PIN "P8_13"
#define PWM_PERIOD 20000000 // 20ms period

#define MIN_DUTY_CYCLE 600000  // 0 degrees
#define MAX_DUTY_CYCLE 2400000 // 180 degrees

void pwm_export(const char *pin)
{
    int fd = open(PWM_EXPORT_PATH, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open PWM export file");
        exit(EXIT_FAILURE);
    }

    if (write(fd, pin, strlen(pin)) < 0)
    {
        perror("Failed to export PWM pin");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void pwm_unexport(const char *pin)
{
    int fd = open(PWM_UNEXPORT_PATH, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open PWM unexport file");
        exit(EXIT_FAILURE);
    }

    if (write(fd, pin, strlen(pin)) < 0)
    {
        perror("Failed to unexport PWM pin");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void pwm_set_period(const char *pin, int period_ns)
{
    char path[64];
    snprintf(path, sizeof(path), PWM_PATH "pwm%s/period", pin);

    int fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open PWM period file");
        exit(EXIT_FAILURE);
    }

    char period_str[16];
    snprintf(period_str, sizeof(period_str), "%d", period_ns);

    if (write(fd, period_str, strlen(period_str)) < 0)
    {
        perror("Failed to set PWM period");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void pwm_set_duty_cycle(const char *pin, int duty_ns)
{
    char path[64];
    snprintf(path, sizeof(path), PWM_PATH "pwm%s/duty_cycle", pin);

    int fd = open(path, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open PWM duty cycle file");
        exit(EXIT_FAILURE);
    }

    char duty_str[16];
    snprintf(duty_str, sizeof(duty_str), "%d", duty_ns);

    if (write(fd, duty_str, strlen(duty_str)) < 0)
    {
        perror("Failed to set PWM duty cycle");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

int main(int argc, char **argv)
{
    // Export PWM pin
    pwm_export(PWM_PIN);

    // Set PWM period
    pwm_set_period(PWM_PIN, PWM_PERIOD);

    // Set initial duty cycle to 0 degrees
    pwm_set_duty_cycle(PWM_PIN, MIN_DUTY_CYCLE);

    while (1)
    {
        // Sweep from 0 to 180 degrees
        for (int angle = 0; angle <= 180; angle += 5)
        {
            // Calculate duty cycle for current angle
            int duty_ns = MIN_DUTY_CYCLE + (MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) * angle / 180;
            // Set duty cycle
            pwm_set_duty_cycle(PWM_PIN, duty_ns);

            // Wait for 100ms
            usleep(100000);
        }
        for (int angle = 180; angle >= 0; angle -= 5)
        {
            // Calculate duty cycle for current angle
            int duty_ns = MIN_DUTY_CYCLE + (MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) * angle / 180;
            // Set duty cycle
            pwm_set_duty_cycle(PWM_PIN, duty_ns);

            // Wait for 100ms
            usleep(100000);
        }
    }

    // Unexport PWM pin
    pwm_unexport(PWM_PIN);

    return 0;
}