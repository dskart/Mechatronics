//include libraries
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//set variables
#define X_value D7
#define Y_value D0
#define Turret_v D8
#define Turret_angle D5

#define X_direction D1
#define Y_direction D2

char myData[13] ="0";

unsigned long Pwm_Time1;// x_ joystick
unsigned long Pwm_Time2;// y_ joystick
unsigned long Pwm_Time3;// turret_velocity
unsigned long Pwm_Time4;// turret_angle


int Direction1; // x sign value
int Direction2; // y sign value

int PWM1; // x_ joystick
int PWM2; // y_ joystick
int PWM3; // turret_velocity
int PWM4; // turret_angle

int turret_velocity;

//set const
const char* ssid = "modlab1";
const char* password = "ESAP2017";
//const char* ssid = "Team2";
//const char* password = "wreckandroll";
IPAddress ipSendto(192, 168, 1, 40);

//initialize wifi to send packets
unsigned int udpRemotePort=1340;
unsigned int udplocalPort=1324;
const int UDP_PACKET_SIZE = 13;
char udpBuffer[UDP_PACKET_SIZE];
WiFiUDP udp;


void setup() {
   // start serial communication
  Serial.begin(115200); delay(10);
  
  //setting pwm read pins (joystick abs value)
  pinMode(X_value,INPUT); // left joystick
  pinMode(Y_value,INPUT); // right joystick
  pinMode(Turret_v,INPUT); // 
  pinMode(Turret_angle,INPUT); //x angle


  //setting direction read pins
  pinMode(X_direction,INPUT);// right joystick
  pinMode(Y_direction,INPUT);// left joystick

  
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //begin wifi
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 1, 76),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));
  // wait to be connected to wifi and start receiving/sending packets to ports
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi connected");

  udp.begin(udplocalPort);    // SEND
        
  
}

void loop() {

  //read PWM time (joystick abs value) of each pin and save them in variable, pwm runs at 1Khz (fast read)
  Pwm_Time1 = pulseIn(X_value, HIGH, 2000); // right
  Pwm_Time2 = pulseIn(Y_value, HIGH, 2000); // left
  Pwm_Time3 = pulseIn(Turret_v, HIGH, 2000); // v
  Pwm_Time4 = pulseIn(Turret_angle, HIGH, 2000); // angle


  // checks if pin is high or low (sign of each joystick)
  Direction1 = digitalRead(X_direction); // right
  Direction2 = digitalRead(Y_direction); // left


  //map abs value of joysticks from 0 to 100
  PWM1 = ((Pwm_Time1 / 1000.0 )*100);
  PWM2 = ((Pwm_Time2 / 1000.0 )*100);
  PWM3 = ((Pwm_Time3 / 1000.0 )*100);
  PWM4 = ((Pwm_Time4 / 1000.0 )*100);

  // creates a floor for wrong reads
  if (PWM2 <=2){
    PWM2 =0;
  }
  if (PWM1 <=2){
    PWM1 =0;
  }
  
  //converts PWM of turret to corresponding velcoity for the turret
  if(PWM3 < 10){
    turret_velocity = 0; //no velocity
  }else if( 40 < PWM3 && PWM3 <60){
    turret_velocity = 2; // clockwise (-)
  }else if( PWM3 > 80){
    turret_velocity = 1; // counter clockwise (+)
  }
  
  //saves all the data in a string
  String myData_String = String(String(PWM1).length()) + String(String(PWM2).length()) + //[0;1] [x_length, y_length] 
                         String(Direction1) + String(Direction2) +                       //[2;3] [x_direction; y_direction]  
                         String(String(PWM4).length()) + String(turret_velocity) +       //[4;5] [turret_angle_length; turret_v]
                         String(PWM1) + String(PWM2) + String(PWM4);                      //[6;7;8;9;10;11] [x_value; y_value; turrent_angle_value]
  
  // serial print the data string                       
  Serial.println(myData_String);
  
  //stores string in a char of array
  for (int n = 0; n < 6+sizeof(String(PWM1))+sizeof(String(PWM2))+sizeof(String(PWM4)); n+=1){
          myData[n] = myData_String[n];
        } 

  // sends Char of Array of our data through wifi
  fncUdpSend(myData);
  delay(100);
  
}


//send data to the port as a array of char
void fncUdpSend(char myData[13]){
  
  strcpy(udpBuffer, myData);
  // send what ever you want upto 48 char
  udp.beginPacket(ipSendto, udpRemotePort);
  udp.write(udpBuffer, sizeof(udpBuffer));
  udp.endPacket();

  }
