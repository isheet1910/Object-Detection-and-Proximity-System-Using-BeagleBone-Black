import Adafruit_BBIO.PWM as PWM
import time
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
                