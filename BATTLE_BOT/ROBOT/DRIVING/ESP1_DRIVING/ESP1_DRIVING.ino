// include libraries
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>

//Iqinetics library
#include <generic_interface.hpp>
#include "bipbuffer.h"
#include "communication_interface.h"
#include "byte_queue.h"
#include "packet_finder.h"
#include "crc_helper.h"
#include "generic_interface.hpp"
#include "multi_turn_angle_control_client.hpp"

//----------------------------------------PIN DEF---------------------------------------
#define servo_pin D2
#define left_motor_PWM_pin D5
#define left_motor_direction1 D6
#define left_motor_direction2 D7
#define right_motor_PWM_pin 10
#define right_motor_direction1 D1
#define right_motor_direction2 D0


//-----------------------------------------VARIABLES-------------------------------------
float final_turret_velocity = 0.0f;
float velocity = 5.0f;
float Kp = 2.0f;
float Ki = 1.0f;
float Kd = 1.0f;

int Data_x;
int Data_y;
int Data_turret_angle;

int F_R_x;
int F_R_y;

int PWM_turret_angle;
int PWM_x;
int PWM_y;

int turret_angle;
int turret_velocity;


//servo object
Servo myservo;

//--------------------------------------------WIFI SET UP----------------------------
// set up wifi const
//const char* ssid = "modlab1";
//const char* password = "ESAP2017";
const char* ssid = "Team2";
const char* password = "wreckandroll";

// set up wifi port to read and packet size
WiFiUDP UDPTestServer;
unsigned int UDPPort = 1340;
const int packetSize = 13;
String ReadData;
byte packetBuffer[packetSize];
//--------------------------------------------IQINETIC MOTR SETUP--------------------
uint8_t communication_buffer[256];
uint8_t communication_length;

unsigned long communication_time_last;

GenericInterface com;

MultiTurnAngleControlClient velocity_ctrl_com(0);

//------------------------------------------------FUNCTION SET UP --------------------

void Turning(int PWM_x, int PWM_y, int F_R_x, int F_R_y);

//------------------------------------------------SETUP-------------------------------
void setup() {
  // set serial communication and pins
  Serial.begin(115200); delay(10);
  pinMode(left_motor_direction1, OUTPUT);
  pinMode(left_motor_direction2, OUTPUT);
  pinMode(right_motor_direction1, OUTPUT);
  pinMode(right_motor_direction2, OUTPUT);

  //attach myservo object to a pin
  myservo.attach(servo_pin);


  Serial.print("Connecting to ");
  Serial.println(ssid);

  //begin wifi connection
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 1, 40),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));

  // wait for wifi status to me connected
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi connected");

  // begin udpport
  UDPTestServer.begin(UDPPort);

  //set and save PID controls to the Iqinetic motors
  velocity_ctrl_com.angle_Kp_.set(com,Kp);
  velocity_ctrl_com.angle_Ki_.set(com,Ki);
  velocity_ctrl_com.angle_Kd_.set(com,Kd);
  velocity_ctrl_com.angle_Kp_.save(com);
  velocity_ctrl_com.angle_Kd_.save(com);
  velocity_ctrl_com.angle_Ki_.save(com);

  // Grab outbound messages in the com queue, store into buffer
  // If it transferred something to communication_buffer...
  if(com.GetTxBytes(communication_buffer,communication_length))
  {
    // Use serial hardware to send messages
    Serial.write(communication_buffer,communication_length);
  }
  delay(100);


}

//----------------------------------------------------MAIN LOOP--------------------------------------------
void loop() {
  //start reading data from Wifi
  yield();
  
  handleUDPServer();
  //String ReadData = "12012105066";
  Serial.println("ReadData");
  Serial.println(ReadData);
  //converts first 4 char of ReadData to Int and seperate them in new variables
  Data_y = ReadData[1] - '0';
  Data_x = ReadData[0] - '0';

  // store value for turret angle and velocity from incoming string
  Data_turret_angle = ReadData[4] - '0';
  turret_velocity = ReadData[5] - '0';

  // store direction of x/y axis of the joystick from string
  F_R_y = ReadData[3] - '0';
  F_R_x = ReadData[2] - '0';


  //checks how many char we need to read for pwm ( 1 or 2);
  // and stores the value of each joystick into a new variable
  if (Data_x == 1){
    PWM_x = ReadData[6] - '0';

    if (Data_y ==1){
      PWM_y = ReadData[7] - '0';

      if(Data_turret_angle == 1){
        PWM_turret_angle = ReadData[8]- '0';
      }else if(Data_turret_angle == 2){
        PWM_turret_angle = ReadData.substring(8,10).toInt();
      }

    }else if (Data_y ==2){
      PWM_y = ReadData.substring(7,9).toInt();

      if(Data_turret_angle == 1){
        PWM_turret_angle = ReadData[9]- '0';
      }else if(Data_turret_angle == 2){
        PWM_turret_angle = ReadData.substring(9,11).toInt();
      }
    }

  }else if(Data_x == 2){
    PWM_x = ReadData.substring(6,8).toInt();

    if (Data_y ==1){
      PWM_y = ReadData[8] - '0';

      if(Data_turret_angle == 1){
        PWM_turret_angle = ReadData[9]- '0';
      }else if(Data_turret_angle == 2){
        PWM_turret_angle = ReadData.substring(9,11).toInt();
      }

    }else if (Data_y==2){
      PWM_y = ReadData.substring(8,10).toInt();

      if(Data_turret_angle == 1){
        PWM_turret_angle = ReadData[10]- '0';
      }else if(Data_turret_angle == 2){
        PWM_turret_angle = ReadData.substring(10,12).toInt();
      }
    }
  }
//  Serial.println("PWM_x");
//  Serial.println(PWM_x);
//  Serial.println("PWM_y");
//  Serial.println(PWM_y);

  //maps pwm from 0 to 1020 for analogWrite()
  PWM_y = PWM_y * 10.22;
  PWM_x = PWM_x * 10.22;

  //transfrom PWM to deisired velocity for the turret
  if (turret_velocity == 0){
    final_turret_velocity = 0.0f;

  }else if (turret_velocity == 1){
    final_turret_velocity = velocity;

  }else if(turret_velocity == 2){
    final_turret_velocity = -velocity;
  }

  //creates a ceiling for driving motors
  if (PWM_y > 950){
    PWM_y = 1020;
  }

  if (PWM_x > 950){
    PWM_x = 1020;
  }

  // maps PWM of the turret_angle from 0-180
  turret_angle = map(PWM_turret_angle, 0, 100, 0, 180);


//  Serial.println("F_R_x");
//  Serial.println(F_R_x);
//  Serial.println("F_R_y");
//  Serial.println(F_R_y);
//  Serial.println("turret_angle");
//  Serial.println(PWM_turret_angle);
//  Serial.println("final_turret_velocity");
//  Serial.println(final_turret_velocity);
//  
  

  // activates driving motors
  Turning(PWM_x, PWM_y, F_R_x, F_R_y);

  //sends desired angle to the Servo
  myservo.write(turret_angle);

  // set velocity for the turret
  velocity_ctrl_com.ctrl_velocity_.set(com, final_turret_velocity);

  // Grab outbound messages in the com queue, store into buffer
  // If it transferred something to communication_buffer...
  if(com.GetTxBytes(communication_buffer,communication_length))
  {
    // Use serial hardware to send messages
    Serial.write(communication_buffer,communication_length);
  }
  

}

//---------------------------------------------------------FUNCTIONS DEF------------------------------------------

void Turning(int PWM_x, int PWM_y, int F_R_x, int F_R_y){
  //turning left
  if (F_R_x ==0){
    // subtract X value From Y value of the left wheel
    int PWM_LEFT = PWM_y - PWM_x;
    // creates a floor
    if (PWM_LEFT <0){
      PWM_LEFT = 0;
    }
    //output values to the motor
    //Serial.println(PWM_LEFT);
    analogWrite(left_motor_PWM_pin, PWM_LEFT); // left motor
    analogWrite(right_motor_PWM_pin, PWM_y); // right motor

    // changes direction of the motor depending on Y sign
    //reverse
    if (F_R_y == 0){
      digitalWrite(left_motor_direction1, LOW);
      digitalWrite(left_motor_direction2, HIGH);

      digitalWrite(right_motor_direction1, LOW);
      digitalWrite(right_motor_direction2, HIGH);

    //foward
    }else if (F_R_y == 1){
      //Serial.println("hello");
      digitalWrite(left_motor_direction1, HIGH);
      digitalWrite(left_motor_direction2, LOW);

      digitalWrite(right_motor_direction1, HIGH);
      digitalWrite(right_motor_direction2, LOW);
    }


  //turning Right
  } else if (F_R_x == 1 ){
    // subtract X value From Y value of the right wheel
    int PWM_RIGHT = PWM_y - PWM_x;

    //creates a floor value
    if (PWM_RIGHT <0){
      PWM_RIGHT = 0;
    }

    //sends PWM to motors
    analogWrite(left_motor_PWM_pin, PWM_y); // left motor
    analogWrite(right_motor_PWM_pin, PWM_RIGHT); // right motor

    // change direction of motor depending of sign of Y
    //reverse
    if (F_R_y == 0){
      digitalWrite(left_motor_direction1, LOW);
      digitalWrite(left_motor_direction2, HIGH);

      digitalWrite(right_motor_direction1, LOW);
      digitalWrite(right_motor_direction2, HIGH);

    //foward
    }else if(F_R_y==1){

      digitalWrite(left_motor_direction1, HIGH);
      digitalWrite(left_motor_direction2, LOW);

      digitalWrite(right_motor_direction1, HIGH);
      digitalWrite(right_motor_direction2, LOW);
    }
  }

}


// this function listen at the wifi port and imports packet into a string
void handleUDPServer(){
  yield();

  int c = UDPTestServer.parsePacket();


  if (c) {
    UDPTestServer.read(packetBuffer, packetSize);
    ReadData = "";
    for(int i = 0; i < packetSize; i++){
      ReadData += (char)packetBuffer[i];
    }
  }
}

