# import Adafruit_BBIO.GPIO as GPIO
# import time

# # HC-SR04 connection
# # red wire
# vcc = "5V"

# # white wire
# trigger = "P8_17"

# # blue wire using resistor
# echo = "P8_18" #echo = "GPIO1_17"

# # black wire
# gnd = "GND"


# GPIO.cleanup()
# time.sleep(2)

# def distance_measurement(TRIG,ECHO):
#     GPIO.output(TRIG, True)
#     time.sleep(0.00001)
#     GPIO.output(TRIG, False)
#     pulseStart = time.time()
#     pulseEnd = time.time()
#     counter = 0
#     while GPIO.input(ECHO) == 0:
#         pulseStart = time.time()
#         counter += 1
#     while GPIO.input(ECHO) == 1:
#         pulseEnd = time.time()
#     pulseDuration = pulseEnd - pulseStart
#     distance = pulseDuration * 17150
#     distance = round(distance, 2)
#     return distance


# # Configuration
# print("trigger: [{}]".format(trigger))
# GPIO.setup(trigger, GPIO.OUT) #Trigger
# print("echo: [{}]".format(echo))
# GPIO.setup(echo, GPIO.IN)  #Echo
# GPIO.output(trigger, False)
# print("Setup completed!")

# # Security
# GPIO.output(trigger, False)
# time.sleep(0.5)

# distance = distance_measurement(trigger, echo)
# while True:
#     print("Distance: [{}] cm.".format(distance))
#     time.sleep(2)
#     if distance <= 0.5:
#         print("Too close! Exiting...")
#         break
#     else:
#         distance = distance_measurement(trigger, echo)

# GPIO.cleanup()


import Adafruit_BBIO.GPIO as GPIO
import time

def distanceMeasurement(TRIG,ECHO):

    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)

    while GPIO.input(ECHO) == 0:
        pulseStart = time.time()
    while GPIO.input(ECHO) == 1:
        pulseEnd = time.time()

    pulseDuration = pulseEnd - pulseStart
    distance = pulseDuration * 2    
    distance = round(distance, 2)
    return distance

#Configuration
GPIO.setup("P8_17",GPIO.OUT) #Trigger
GPIO.setup("P8_18",GPIO.IN)  #Echo
# GPIO.setup("P9_15",GPIO.OUT) #Trigger
# GPIO.setup("P9_12",GPIO.IN)  #Echo
# GPIO.setup("P9_11",GPIO.OUT)
# GPIO.setup("P9_13",GPIO.IN)

#Security
# GPIO.output("P9_17", False)
# GPIO.output("P9_15", False)
time.sleep(0.5)

#main Loop
try:
    while True:
        recoveredDIstance = distanceMeasurement("P8_17","P8_18")
        print("Distance: ",recoveredDIstance,"cm")
        time.sleep(1)
except KeyboardInterrupt:
    print("Measurement stopped by user")
    GPIO.cleanup()
    
    #    for i in range(2):
    #        if i == 0:
    #            recoveredDistance = distanceMeasurement("P9_11","P9_13")
    #            print "Distance1: ",recoveredDistance,"cm"
    #        elif i == 1: