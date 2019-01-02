# Mechatronics Final Project

![robot image](https://github.com/dskart/Mechatronics/blob/master/Images/robot.jpg)

This repos contains all the code used for the final project for the Mechactronics class at Upenn.
The aim of this project was to create a "battle bot" that could go around different terrain (inclines, tight turns) while hitting opponents and pushing objects. The robot also needed the ability to detect its health and user commands from a server. A multitude of extra features were present such as a OLED screen to display the health and team number, a phototransistor to detect and filter out frequency from "healing beams", and sensors to register hits while sending back the information to the server


## Getting Started

This repo contains all the code we used on the robot and on our home made controllers. We used C for the ATmega32U4 and arduino code for the esp8211.
I was using a multitude of teensy 2.0 as boards for the ATmega32U4 and esp8211 as wifi modules.
This project is very big and requires a lot of specific electronics, manufacturing and components to work, I still believe that this code might be interesting for someone but it would be near impossible to make it run without recreating exactly the same robot as I made. Unfortunatly at this time I am only sharing the code and not the build/electronics for the robot.

Here is a diagram explaining how the robot system worked

![robot diagram](https://github.com/dskart/Mechatronics/blob/master/Images/diagram.png)

### Prerequisites

You will need the arduino IDE with the esp add-on and the teensy loader if you are using the ATmega32U4 on the teensy 2.0
[teensy loader](https://www.pjrc.com/teensy/loader.html)


### Installing
 
Run make to generate the .hex files for the ATmega32U4 and the arduino IDE to upload to the esp8211.


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

