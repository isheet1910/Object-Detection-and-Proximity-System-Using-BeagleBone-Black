import Adafruit_BBIO.GPIO as GPIO

# Define the pin numbers
button1_pin = "P8_12"

# Set the pin mode
GPIO.setup(button1_pin, GPIO.IN)

# Define a callback function for button presses
def button_callback(pin):
    if pin == button1_pin:
        print("Button 1 pressed")

# Add the button_callback function to both buttons
GPIO.add_event_detect(button1_pin, GPIO.RISING, callback=button_callback)

# Keep the program running to continue detecting button presses
while True:
    pass
