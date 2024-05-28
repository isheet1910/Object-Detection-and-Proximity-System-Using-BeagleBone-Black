import Adafruit_BBIO.GPIO as GPIO
import time

# HC-SR04 connection
# red wire
vcc = "5V"

# white wire
trigger = "P8_17"

# blue wire using resistor
echo = "P8_16" #echo = "GPIO1_17"

# black wire
gnd = "GND"


GPIO.cleanup()
time.sleep(2)

def distance_measurement(TRIG,ECHO):
    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)
    pulseStart = time.time()
    pulseEnd = time.time()
    counter = 0
    while GPIO.input(ECHO) == 0:
        pulseStart = time.time()
        # print("going in 0")
        counter += 1
    while GPIO.input(ECHO) == 1:
        # print("going in 1")
        pulseEnd = time.time()
    pulseDuration = pulseEnd - pulseStart
    distance = pulseDuration * 17150
    distance = round(distance, 2)
    return distance


# Configuration
print("trigger: [{}]".format(trigger))
GPIO.setup(trigger, GPIO.OUT) #Trigger
print("echo: [{}]".format(echo))
GPIO.setup(echo, GPIO.IN)  #Echo
GPIO.output(trigger, False)
print("Setup completed!")

# Security
GPIO.output(trigger, False)
time.sleep(0.5)

distance = distance_measurement(trigger, echo)
while True:
    print("Distance: [{}] cm.".format(distance))
    time.sleep(2)
    if distance <= 5:
        print("Too close! Exiting...")
        break
    else:
        distance = distance_measurement(trigger, echo)
GPIO.cleanup()