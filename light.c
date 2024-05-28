#include <stdio.h>       //standard input output header
#include <stdlib.h>      //standard header for memory allocation
#include <unistd.h>      //declares miscelleneous files
#include <fcntl.h>       //read write functions header file
#include <errno.h>       //header file for error conditions
#include <string.h>      //for string hanfling header
#include <time.h>        //for functions like sleep and wait
#include <sys/utsname.h> //for uts sysinformation

struct utsname sysInfo; // access utsname class for functions like machin$

#define GPIO_PATH "/sys/class/gpio" // storage location on beag$
#define redPin1 "gpio44"            // variable redPin1 for$
#define yellowPin1 "gpio68"         // variable yellowPi$
#define greenPin1 "gpio67"          // variable greenPin1$
#define redPin2 "gpio45"            // variable redPin2 for$
#define yellowPin2 "gpio69"         // variable yellowPi$
#define greenPin2 "gpio66"          // variable greenPin2$

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

int main()
{

    uname(&sysInfo);                            // declare sys$
    printf("Machine : %s \n", sysInfo.machine); // machine nam$

    time_t rawtime;      // time_t fun$
    struct tm *timeinfo; // to access $

    // const char *redPin1 = "gpio44";                                 //variab$
    // const char *yellowPin1 = "gpio68";                                 //var$
    // const char *greenPin1 = "gpio67";                                 //vari$
    // const char *redPin2 = "gpio45";                                 //variab$
    // const char *yellowPin2 = "gpio69";                                 //var$
    // const char *greenPin2 = "gpio66";                                 //vari$

    setPinMode(redPin1, "out");    // set the pin $
    setPinMode(yellowPin1, "out"); // set the pin $
    setPinMode(greenPin1, "out");  // set the pin $
    setPinMode(redPin2, "out");    // set the pin mo$
    setPinMode(yellowPin2, "out"); // set the pin$
    setPinMode(greenPin2, "out");  // set the pin $

    while (1)
    { // infinite loop

        setPinValue(redPin1, "1");
        setPinValue(yellowPin1, "1");
        setPinValue(greenPin1, "1");
        setPinValue(redPin2, "1");
        setPinValue(yellowPin2, "1");
        setPinValue(greenPin2, "1");
    }

    return 0;
}
