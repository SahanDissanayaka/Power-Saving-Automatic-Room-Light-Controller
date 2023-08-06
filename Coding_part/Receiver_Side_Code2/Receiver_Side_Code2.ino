#include <VirtualWire.h>

#define RECEIVE_PIN 11  // Replace this with the pin connected to the RF receiver data pin
#define BUFFER_SIZE 32  // Adjust this to the maximum expected message length

char receivedMsg[BUFFER_SIZE];  // Buffer to hold the received message
uint8_t msgLength = BUFFER_SIZE;

void setup() {
  Serial.begin(9600);        // Initialize Serial for debugging (optional)
  vw_set_rx_pin(RECEIVE_PIN); // Set the pin used for receiving data
  vw_setup(2000);             // Bits per second (baud rate)
  vw_rx_start();              // Start the receiver
  pinMode(6,OUTPUT);
}

void loop() {
  if (vw_get_message((uint8_t*)receivedMsg, &msgLength)) {
    receivedMsg[msgLength] = '\0'; // Null-terminate the received message
    Serial.println(receivedMsg);   // Print the received message
    // Process the receivedMsg as needed
    int result = strcmp(receivedMsg, "1");
    if (!result){
        digitalWrite(6,HIGH);
        }else{
          digitalWrite(6,LOW);
          }   
  }

  // Your other loop code
}