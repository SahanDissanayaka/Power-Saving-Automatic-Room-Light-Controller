//include NewPing library to get distance using ultrasonic sensor
#include <NewPing.h>

// include RadioHead Amplitude Shift Keying Library
#include <VirtualWire.h>

// Include dependant SPI Library
#include <SPI.h>

// Create Amplitude Shift Keying Object



#define ECHO_PIN1 8
#define ECHO_PIN2 11
#define TRIG_PIN1 9
#define TRIG_PIN2 10

int num_of_persons = 0;
int check1 = 0;
int check2 = 0;
bool going_inside = false;
bool going_outside = false;

NewPing Sensor1(TRIG_PIN1, ECHO_PIN1, 2000);
NewPing Sensor2(TRIG_PIN2, ECHO_PIN2, 2000);

unsigned int dis_1;
unsigned int dis_2;

double get_battery_level() {
  // put your main code here, to run repeatedly:
  int volt = analogRead(A0);
  double voltage = map(volt, 0, 1023, 0, 2500);

  voltage = voltage/7.4;
  return voltage;
}


#include <SoftwareSerial.h>
#define RX 2
#define TX 3
String AP = "SLT-4G-74C6C8";       // AP NAME
String PASS = "7K7743775D"; // AP PASSWORD
String API = "tPmAT5Ab3j7F9";   // Write API KEY
String HOST = "192.168.1.240";
String PORT = "80";

int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial esp8266(RX,TX); 


int compId=102;



void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK");
  

  vw_set_tx_pin(12); // Set the pin used for transmitting data
  vw_setup(2000);                   // Bits per second (baud rate)
  
  pinMode(ECHO_PIN1, INPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(TRIG_PIN2, OUTPUT);

  
 

  

}

int counter=0;

void loop() {
  
  counter++;
  if(counter==5){
    sendData();
    counter=0;
  }



  unsigned long start_time = millis();


  while (millis() - start_time <= 1000) {

    if (check1 != 1) {
      unsigned int dis = 0;
      
      for (int i = 0; i <= 5; i++) {
        dis = dis + Sensor1.ping_cm();
        
      }
      dis_1 = dis / 3;
      

      if (dis_1 < 70 ) {
        if (check2 == 1) {
          going_outside = true;
          check2 = 0;
        } else {
          check1 = 1;
          
        }

      }
    }

    if (check2 != 1) {
      unsigned int dis = 0;
      
      for (int i = 0; i <= 5; i++) {
        dis = dis + Sensor2.ping_cm();
        
      }
      dis_2 = dis / 3;

      if (dis_2 < 70) {
        if (check1 == 1) {
          going_inside = true;
          check1 = 0;
        } else {
          check2 = 1;
          
        }
      }
    }
  }

  if (going_inside && !going_outside) {
    num_of_persons ++;
    going_inside = false;
  } else if (going_outside && !going_inside) {
    if(num_of_persons > 0){num_of_persons--;}
    else{num_of_persons = 0;}
    going_outside = false;
  }

  Serial.println(num_of_persons);
  Serial.println(get_battery_level());
  

  going_inside = false;
  going_outside = false;
  check1 = 0; check2 = 0;

  if (num_of_persons > 0) {
    sendRadioData("1",1);
  }
  else {
    sendRadioData("0",1);
  }
}

void sendRadioData(const uint8_t* data, uint8_t len) {
  vw_send(data, len);
  vw_wait_tx(); // Wait until the whole message is gone
}



void sendData() {
 
  String num_people = String(num_of_persons);
  String battery_level = String(get_battery_level());
  String comp_id = String(compId);
  String getData = "GET /technoBeasts/postData/postEspData.php?method=data&api_key=tPmAT5Ab3j7F9&num_people=" + num_people + "&battery_level=" + battery_level + "&comp_id=" + comp_id;
  
  // Add the necessary headers
  String headers = "Host: " + HOST + "\r\n" +
                  "Connection: close\r\n" +  // Close the connection after the request is complete
                  "User-Agent: Arduino/1.0\r\n";  // You can specify your desired user agent

  int headersLength = headers.length();
  int requestLength = getData.length() + headersLength + 2; // Add 2 for the additional \r\n at the end

  sendCommand("AT+CIPMUX=1", 1, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 1, "OK");
  sendCommand("AT+CIPSEND=0," + String(requestLength), 1, ">");

  esp8266.print(getData); // Send the request line
  esp8266.print("\r\n"); // New line after the request line
  esp8266.print(headers); // Send the headers
  esp8266.print("\r\n"); // New line after the headers

  delay(500); 
  countTrueCommand++;
  // sendCommand("AT+CIPCLOSE=0", 0.5, "OK");
}



void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }


 
