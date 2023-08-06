//include NewPing library to get distance using ultrasonic sensor
#include <NewPing.h>

// include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>

// Include dependant SPI Library
#include <SPI.h>

// Create Amplitude Shift Keying Object
RH_ASK rf_driver;

#define ECHO_PIN1 8
#define ECHO_PIN2 11
#define TRIG_PIN1 9
#define TRIG_PIN2 10

int num_of_persons = 0;
int check1 = 0;
int check2 = 0;
bool going_inside = false;
bool going_outside = false;

NewPing Sensor1(TRIG_PIN1,ECHO_PIN1,2000);
NewPing Sensor2(TRIG_PIN2,ECHO_PIN2,2000);

unsigned int dis_1;
unsigned int dis_2;

void get_battery_level(){
  // put your main code here, to run repeatedly:
  int volt = analogRead(A0);
  double voltage = map(volt, 0, 1023, 0, 2500);

  voltage /= 100;
  return voltage;
  }
  
void setup() {
  pinMode(ECHO_PIN1, INPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  Serial.begin(9600);
  rf_driver.init();

}


void loop() {

  unsigned long start_time = millis();
 

  while (millis()- start_time <= 1000){

    if (check1 != 1){
      unsigned int dis=0;
      //Serial.print("dis1");Serial.print(" ");
      for (int i=0;i<=5;i++){
        dis = dis + Sensor1.ping_cm();
        //Serial.print(" ");Serial.print(dis);Serial.print(" ");
        }
       dis_1 = dis/3;
       //Serial.print(dis_1);

    if (dis_1 < 70 ) {
      if (check2 == 1) {
        going_outside = true;
        check2 = 0;
      } else {
        check1 = 1;
        //delay(200);
      }
  
    }
    }
    
    if (check2 != 1){
      unsigned int dis = 0;
      //Serial.print("dis2");Serial.print(" ");
      for (int i=0;i<=5;i++){
        dis = dis + Sensor2.ping_cm();
        //Serial.print(dis);Serial.print(" ");
        }
      dis_2 = dis/3;
      
      //Serial.println(dis_2);
    if (dis_2 < 70) {
      if (check1 == 1) {
        going_inside = true;
        check1 = 0;
      } else {
        check2 = 1;
        //delay(200);
      }
    }
      }
    }

  if (going_inside && !going_outside){
    num_of_persons ++;
    going_inside = false;
    }else if (going_outside && !going_inside){
      num_of_persons--;
      going_outside = false;
      }
          
  Serial.println(num_of_persons);
//  Serial.print(" ");
//  Serial.print(check1);Serial.print(" ");
//  Serial.println(check2);
  
  going_inside = false;
  going_outside = false;
  check1 = 0;check2 = 0;
  
  const char *msg;
  if (num_of_persons > 0){msg = "1";}
  else{msg = "0";}
  //Serial.println(msg);
  rf_driver.send((uint8_t *)msg,strlen(msg));
  rf_driver.waitPacketSent();
  delay(10);

}
