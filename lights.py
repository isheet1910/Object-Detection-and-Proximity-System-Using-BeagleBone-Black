import Adafruit_BBIO.GPIO as GPIO
import time

# Set up the GPIO pin
led_pin = "P8_8"
led2 = "P8_7"
buzzer = "P8_11"

GPIO.setup(led_pin, GPIO.OUT)
GPIO.setup(led2,GPIO.OUT)
GPIO.setup(buzzer,GPIO.OUT)

# Turn on the LED
GPIO.output(led_pin, GPIO.HIGH)
GPIO.output(led2,GPIO.HIGH)
GPIO.output(buzzer,GPIO.HIGH)

# Wait for 5 seconds
time.sleep(5)                  

# Turn off the LED
GPIO.output(led_pin, GPIO.LOW)
GPIO.output(led2,GPIO.LOW)
GPIO.output(buzzer,GPIO.LOW)

while(1):
        GPIO.output(buzzer,GPIO.HIGH)
        time.sleep(0.01)
        GPIO.output(buzzer,GPIO.LOW)

# Cleanup
GPIO.cleanup()
