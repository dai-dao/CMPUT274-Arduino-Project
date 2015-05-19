Code Written by Dai Dao and Ben Schreiber, CMPUT 274 A1

Program Screen_communicator is a text and picture messaging system that prints
out messages and aselection of small images (Emoticons) onto the connected TFT
screen, and sends messages between connected arduinos

NOTE: Files cat_cupid.lcd, dog_boxer.lcd, dog_rich.lcd, money.lcd, smiley.lcd,
and suprised.lcd must be loaded onto the SD card in the connected tft screen
prior to program operation.

Wiring Required:
Adafruit TFT screen at A42-A51 on Breadboard (Not Overtop Breadboard)
Joystick at E23-E27 on Breadboard (Overtop Right side of Breadboard)
Potentiometer at J33, H34, J35 on Breadboard
Patch Cable connecting GND on Arduino to GND on other Arduino
Patch Cable connecting TX3 on Arduino to RX3 on other Arduino
Patch Cable connecting RX3 on Arduino to TX3 on other Arduino
Patch Cable connecting 5V on Arduino to Left+ on Breadboard
Patch Cable connecting GND on Arduino to Left- on Breadboard
Patch Cable connecting Left+ on Breadboard to C23 on Breadboard
Patch Cable connecting A0 on Arduino to A24 on Breadboard
Patch Cable connecting A1 on Arduino to A25 on Breadboard
Patch Cable connecting 9 on Arduino to C26 on Breadboard
Patch Cable connecting Left- on Breadboard to A27 on Breadboard
Patch Cable connecting Left- on Breadboard to F33 on Breadboard
Patch Cable connecting A2 on Breadboard to G34 on Breadboard
Patch Cable connecting Left+ on Breadboard to F35 on Breadboard
Patch Cable connecting Left+ on Breadboard to E42 on Breadboard
Patch Cable connecting 50 on Breadboard to C43 on Breadboard
Patch Cable connecting 52 on Breadboard to C44 on Breadboard
Patch Cable connecting 51 on Breadboard to C45 on Breadboard
Patch Cable connecting 6 on Arduino to C46 on Breadboard
Patch Cable connecting 5 on Arduino to C47 on Breadboard
Patch Cable connecting 7 on Arduino to E48 on Breadboard
Patch Cable connecting 8 on Arduino to C49 on Breadboard
Patch Cable connecting Left+ on Breadboard to E50 on Breadboard
Patch Cable connecting Left- on Breadboard to C51 on Breadboard

NOTE: For single unit testing of this program, connect the TX3 and RX3 ports on
the unit together by a patch cable and proceed with regular operation. Messages
will appear twice on the serial-monitor when sent.

Operation:
To use this program, connect two arduinos together, according to the wiring 
above, and to separate computers via their usb ports. Enter the Serial monitors
on both computers. Users can then type into the serial monitor and see the 
messages on the connected tft screen on their arduino, pressing the enter key
into the serial monitor to send. Users can use the attached joystick to choose
and add emoticons to the message, and use the attached potentiometer to change
the color of the inputted characters.

NOTE: It is recommended that each user choose a different color for their 
messages, to allow for easier discrimination of messages, but users are free to
choose between all colors all the time.
