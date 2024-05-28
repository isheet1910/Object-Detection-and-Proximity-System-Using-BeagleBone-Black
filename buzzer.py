import Adafruit_BBIO.GPIO as GPIO
import time

# Set up the GPIO pin
buzzer = "P8_11"
GPIO.setup(buzzer,GPIO.OUT)

while(1):
        GPIO.output(buzzer,GPIO.HIGH)
        time.sleep(0.001)
        GPIO.output(buzzer,GPIO.LOW)


# Cleanup
GPIO.cleanup()
