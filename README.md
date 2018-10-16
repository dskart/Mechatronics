This repos contains all the code used for the final project for the Mechactronics class at Upenn

This code contains C code for the ATmega32U4 and arduino code for the esp8211 board

The aim of this project was to create a "battle bot" that could go around different terrain (inclines, tight turns) while hitting opponents and pushing objects. The robot also needed the ability to detect its health and user commands from a server (this is why we used the esp8211). 

A multitude of extra features were present such as a OLED screen to display the health and team number, a phototransistor to detect and filter out frequency from "healing beams", and sensors to register hits while sending back the information to the server
