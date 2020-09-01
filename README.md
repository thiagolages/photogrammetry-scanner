# Arduino controlled Photogrammetry Wi-Fi 3D-scanner using ESP8266

*This is the Wi-Fi version of [Brian Brocken's original work](https://hackaday.io/project/168301-arduino-controlled-photogrammetry-3d-scanner), so thanks to him for the original project.

*Work in progress*

-------------------------------------------
Video of an result example
-------------------------------------------
[![Watch the video](black-panther.gif)](https://www.youtube.com/watch?v=YgXV4LTF02c)

### Description

The description of the project, given by Brian Brocken himself is as follows:

"It uses an Arduino UNO to control a 1602 LCD screen, a stepper motor and a 9g servo motor. The turntable can be used as a 3D-scanner by taking photos of an object 360 degrees around it. These photos can be converted later on into a 3D-model using photogrammetry software. The amount of photos that have to be taken can be determined by the user, ranging from 2 photos to 200 photo's (can be easily adjusted in the arduino program). The turntable can also be used to take cinematic shots. In this mode the turntable rotates at a constant speed chosen by the user. The last menu enables the user to manually move the turntable."

### This Wi-Fi version uses:

- ***An ESP8266 (ESP-01 module) connected to the same Wi-Fi network as a smartphone running the IP Webcam Android app***. Of course any app would work, it just needs to make your phone's camera available through the web. Just note that you would need to change the code and make the right HTTP request in order to obtain pictures from your camera. ***The ESP-01 module and the Arduino communicate via an I2C protocol, and exchange messages on whether the picture was taken or not, and whether the table should rotate or not.***

- [The IP Webcam app](https://play.google.com/store/apps/details?id=com.pas.webcam&hl=en) installed on any compatible Android phone.

Instead of using a servo to presses a button on a bluetooth remote, which is imprecise, slow, noisy and not truly reliable, I felt like creating a more robust and electronically-controlled way to take pictures. 



## Usage
### Initial Setup
0. Build the Camera Turtable as described [here](https://hackaday.io/project/168301-arduino-controlled-photogrammetry-3d-scanner), except for the bluetooth remote and servo.
1. Connect the ESP8266 and the Arduino via an I2C communication.
2. Edit the `ESP8266-side.ino` code and insert your Wi-Fi's SSID and password. Also insert your phone's IP address inside your network. Remember they have to be connected to the same Wi-Fi network in order for this to work.
3. Upload the `ESP8266-side.ino` code on the ESP8266
4. Upload the `turntable-side.ino` code on the Arduino inside the turntable

### Daily usage
1. When you turn your table on, go once to the left and select "Connect to Wi-Fi". Wait a few seconds and it should write "CONNECTED" and go back to the initial screen.
2. Select "Photogrammetry"
3. Select the number of photos
4. Enjoy!

***For the other modes "Cinematic" and "Manual Control", it shouldn't make any difference from the original work.***

### Improvements from original version

This version includes:

- New "Connect to Wi-Fi" menu entry
- New Joystick Auto Calibration at start-up
- New #define LOG_SERIAL option to output debugging messages on the Serial Monitor when setting up the Arduino and ESP8266 separately*
- Better 'Motor Speed' and 'Nr. of turns' selection
- More readable and easy-to-change code

*Note that leaving #define LOG_SERIAL uncommented and connecting both devices WILL MOST LIKELY NOT WORK. Uncomment when you're finished debugging so it works smoothly


---------------------------------------
To-do List
---------------------------------------

- Include Electrical Diagram 
- Explain how communication takes place between Arduino <-> ESP8266
- Explain what each device does
