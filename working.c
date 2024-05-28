#include <stdio.h>  //standard input output header
#include <stdlib.h> //standard header for memory allocation
#include <stdbool.h>
#include <unistd.h> //declares miscelleneous files
#include <sys/mman.h>
#include <fcntl.h>       //read write functions header file
#include <errno.h>       //header file for error conditions
#include <string.h>      //for string hanfling header
#include <time.h>        //for functions like sleep and wait
#include <sys/utsname.h> //for uts sysinformation
#include <pthread.h>     //pthread library for threads

#include <pru_cfg.h>   //This header file contains configuration options for the PRU subsystem, such as clock speed and memory layout.
#include <pru_ctrl.h>  //This header file provides access to the control registers of the PRU, which are used to configure various aspects of the PRU operation.
#include <pru_intc.h>  // This header file provides access to the PRU Interrupt Controller (INTC), which is used to manage interrupts on the PRU.
#include <pru_rpmsg.h> //This header file provides support for the Remote Proc Messaging (RPMsg) framework, which is used to facilitate communication between the PRU and the host processor.
#include <pru_uart.h>  //This header file provides support for UART (Universal Asynchronous Receiver/Transmitter) communication on the PRU.

struct utsname sysInfo; // access utsname class for functions like machine name node name and systemname

#define GPIO_PATH "/sys/class/gpio" // storage location on beag$
#define redPin1 "gpio66"            // variable redPin1 for red led
#define greenPin1 "gpio67"          // variable green pin 1 for green led

#define BUZZER_PIN "/sys/class/gpio/gpio45/value" // variable buzzer pin for  buzzer

#define PWM_PATH "/sys/class/pwm/pwmchip0/"   // PWM pin declaration for using in servo motor EHRPWM2B in gpio setup
#define PWM_EXPORT_PATH PWM_PATH "export"     // to exprort the PWM pin
#define PWM_UNEXPORT_PATH PWM_PATH "unexport" // to unexprort the PWM pin

#define PWM_PIN "P8_13"     // control pin conncted to the P8_13 of the beaglebone to control the servo motor
#define PWM_PERIOD 20000000 // 20ms period

#define MIN_DUTY_CYCLE 600000  // 0 degrees
#define MAX_DUTY_CYCLE 2400000 // 180 degrees

#define GPIO_EXPORT_PATH GPIO_PATH "export"     // to exprort the GPIO pin
#define GPIO_UNEXPORT_PATH GPIO_PATH "unexport" // to unexprort the GPIO pin

#define TRIGGER_PIN "P8_17" // trig pin of ultrasonic sensor connected to the p8_17 pin
#define ECHO_PIN "P8_16"    // echo pin of ultrasonic sensor connected to the p8_17 pin

float distance;        // distance variabvle golobal which will be a mutex to use for various variables
pthread_mutex_t mutex; // mutex declaration

pthread_t tid1[2], tid2[2], tid3[2], tid4[2], tid5[2]; // declare thread identifiers for each signal

// writeToFile function opens the required pin file on the beaglebone for examp$
void writeToFile(const char *filename, const char *value)
{
    int fd = open(filename, O_WRONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to open file %s: %s\n", filename, strerror(errno));
        exit(1);
    }

    if (write(fd, value, strlen(value)) < 0)
    {
        fprintf(stderr, "Failed to write to file %s: %s\n", filename, strerror(errno));
        exit(1);
    }

    close(fd);
}

// setPinMode function sets the particular pin to write access in the beagle bon$
void setPinMode(char *pin, const char *mode)
{
    char path[50];
    snprintf(path, sizeof(path), GPIO_PATH "/%s/direction", pin);

    writeToFile(path, mode);
}

// setPinValue function sets the value of the pin (LED) to on or off in the beag$
void setPinValue(char *pin, const char *value)
{
    char path[50];
    snprintf(path, sizeof(path), GPIO_PATH "/%s/value", pin);
    writeToFile(path, value);
}

// export the PWM pin
void pwm_export(int pin)
{
    int fd = open(PWM_EXPORT_PATH, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open PWM export file");
        exit(EXIT_FAILURE);
    }

    char pin_str[3];
    snprintf(pin_str, sizeof(pin_str), "%d", pin);

    if (write(fd, pin_str, strlen(pin_str)) < 0)
    {
        perror("Failed to export PWM pin");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

// unexport the PWM pin
void pwm_unexport(int pin)
{
    int fd = open(PWM_UNEXPORT_PATH, O_WRONLY);
    if (fd < 0)
    {
        perror("Failed to open PWM unexport file");
        exit(EXIT_FAILURE);
    }

    char pin_str[3];
    snprintf(pin_str, sizeof(pin_str), "%d", pin);

    if (write(fd, pin_str, strlen(pin_str)) < 0)
    {
        perror("Failed to unexport PWM pin");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

// set period to use for the servo motor to rotate at intervals
void pwm_set_period(int pin, int period_ns)
{
    char path[64];
    snprintf(path, sizeof(path), PWM_PATH "pwm%d/period", pin);

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

// set angle to use for the servo motor to rotate at inetervals
void pwm_set_duty_cycle(int pin, int duty_ns)
{
    char path[64];
    snprintf(path, sizeof(path), PWM_PATH "pwm%d/duty_cycle", pin);

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

// gpio set up for the ultrasonic and the trig pins
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

// initiate the trigger pin once
void trigger_pulse(int fd_trigger)
{
    // Send pulse
    write(fd_trigger, "1", 1);
    usleep(10);
    write(fd_trigger, "0", 1);
}

// measure the distance from the ultrasonic sensor using the echo pin
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

    pthread_mutex_lock(&mutex);               // initiaise lock as other threads wont be able to access or change it
    distance = pulse_duration * 0.034 / 2.0f; // clculate the distance
    pthread_mutex_unlock(&mutex);             // unlock it so other threads and functions will be  able to access or change it
    return distance;
}

void *thread1(void *arg)
{
    int fd_trigger = gpio_setup(TRIGGER_PIN, "out");

    // Setup echo pin as input
    int fd_echo = gpio_setup(ECHO_PIN, "in");
    // Print setup completed message
    printf("Setup completed!\n");

    // Wait for sensor to settle
    usleep(500000);
    float localdistance = 0;
    while (1)
    {
        // Measure distance
        pthread_mutex_lock(&mutex);                                // initiaise lock as other threads wont be able to access or change it
        localdistance = distance_measurement(fd_trigger, fd_echo); // get the distacnce in a local variable
        pthread_mutex_unlock(&mutex);                              // unlock it so other threads and functions will be  able to access or change it

        // Print distance
        printf("Distance: %0.2f cm.\n", localdistance);
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
}

void *thread2(void *arg)
{
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
    return NULL;
}

// thread to control the LEDs red and green to indicate obstacle near 30 cm or not
void *thread3(void *arg)
{
    float localdistance = 0;
    setPinMode(redPin1, "out");   // set the pin $
    setPinMode(greenPin1, "out"); // set the pin $
    while (1)
    {
        pthread_mutex_lock(&mutex);   // initiaise lock as other threads wont be able to access or change it
        localdistance = distance;     // transfer distance into local variable
        pthread_mutex_unlock(&mutex); // unlock it so other threads and functions will be  able to access or change it
        if (localdistance < 30.0)
        {
            setPinValue(redPin1, "1");
            setPinValue(greenPin1, "0");
        }
        else
        {
            setPinValue(redPin1, "0");
            setPinValue(greenPin1, "1");
        }
    }
    return NULL;
}

// thread to control the buzzer on and off to indicate obstacle near 30 cm or not
void *thread4(void *arg)
{
    int fd, len;
    char value[2];
    float localdistance = 0;

    fd = open(BUZZER_PIN, O_WRONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to open %s: %s\n", BUZZER_PIN, strerror(errno));
        exit(1);
    }
    while (1)
    {
        pthread_mutex_lock(&mutex);   // initiaise lock as other threads wont be able to access or change it
        localdistance = distance;     // transfer distance into local variable
        pthread_mutex_unlock(&mutex); // unlock it so other threads and functions will be  able to access or change it
        if (localdistance < 30.0)
        {
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
        else
        {
            // Keep buzzer off
            len = write(fd, "0", 1);
            if (len < 0)
            {
                fprintf(stderr, "Failed to write buzzer value: %s\n", strerror(errno));
                exit(1);
            }
        }
    }
    close(fd);
    return NULL;
}

int main()
{
    printf("Machine : %s \n", sysInfo.machine);                                        // machine name prints
    printf("System name : %s \n", sysInfo.sysname);                                    // System name prints
    printf("Node Name: %s \n", sysInfo.nodename);                                      // Node name prints
    printf("Group 14 Student Name: Isheet Shetty, Neeharika Rangisetty and Bhavana "); // group members name

    pthread_t tid1, tid2, tid3, tid4; // declare 4 threads for ultrasonic sensor , servo motor , LEDS and Buzzers

    pthread_mutex_init(&mutex, NULL); // Mutex declaration for distance variable

    pthread_create(&tid1, NULL, thread1, NULL); // create thread 1 for ultrasonic sensor
    pthread_create(&tid2, NULL, thread2, NULL); // create thread 2 for servo motor
    pthread_create(&tid3, NULL, thread3, NULL); // create thread 3 LEDs red and green
    pthread_create(&tid4, NULL, thread4, NULL); // create thread 4 Buzzer

    pthread_join(tid1, NULL); // initiating thread 1
    pthread_join(tid2, NULL); // initiating thread 2
    pthread_join(tid3, NULL); // initiating thread 3
    pthread_join(tid4, NULL); // initiating thread 4

    // pthread_mutex_destroy(&mutex);     /for destroying the mutex if we even want to end but this program is oto be run always in infinite loop

    return 0;
}