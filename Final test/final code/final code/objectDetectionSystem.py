import Adafruit_BBIO.PWM as PWM
import Adafruit_BBIO.GPIO as GPIO
from threading import Thread
import time

distance = 0 

def servo():
    servoPin="P8_13"
    PWM.start(servoPin,5,50)
    desiredAngle=0
    dutyCycle=1/18*desiredAngle + 2
    PWM.set_duty_cycle(servoPin,dutyCycle)
    while(1):
            desiredAngle = 0
            while(desiredAngle<=180):
                    dutyCycle=1/18*desiredAngle + 2
                    PWM.set_duty_cycle(servoPin,dutyCycle)
                    time.sleep(0.5)
                    desiredAngle =  desiredAngle +5
            desiredAngle = 180
            while(desiredAngle>=0):
                    dutyCycle=1/18*desiredAngle + 2
                    PWM.set_duty_cycle(servoPin,dutyCycle)
                    time.sleep(0.5)
                    desiredAngle =  desiredAngle -5

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
    global distance
    distance = pulseDuration * 17150
    distance = round(distance, 2)
    return distance

def ultra():
    vcc = "5V"
    trigger = "P8_17"
    echo = "P8_16" 
    gnd = "GND"
    GPIO.cleanup()
    time.sleep(2)
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
        distance = distance_measurement(trigger, echo)
    GPIO.cleanup()

def light():
    green = "P8_8"
    red = "P8_7"

    GPIO.setup(red, GPIO.OUT)
    GPIO.setup(green,GPIO.OUT)
    while True:
        if(distance < 30):
            GPIO.output(red, GPIO.HIGH)
            GPIO.output(green,GPIO.LOW)
        else:
            GPIO.output(red, GPIO.LOW)
            GPIO.output(green,GPIO.HIGH)

def buzzer():
    buzzer = "P8_11"
    GPIO.setup(buzzer,GPIO.OUT)
    while(1):
        while(distance < 30):
            GPIO.output(buzzer,GPIO.HIGH)
            time.sleep(0.001)
            GPIO.output(buzzer,GPIO.LOW)
        else:
            GPIO.output(buzzer,GPIO.LOW)

if __name__ == '__main__':
    Thread(target = ultra).start()
    Thread(target = servo).start()
    Thread(target = light).start()
    Thread(target = buzzer).start()
