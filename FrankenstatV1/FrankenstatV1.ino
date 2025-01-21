#include <ESP32Servo.h>



/*
  LED

  This example creates a Bluetooth® Low Energy peripheral with service that contains a
  characteristic to control an LED.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

Servo myServo;
int pos =0;


BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED servo Characteristic - custom 128-bit UUID, read and writable by central
BLEIntCharacteristic servoCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int ledPin = LED_BUILTIN; // pin to use for the LED

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Frankenstat");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(servoCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  servoCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");

  myServo.attach(27);
}

void loop() {
  long int tempPos=0;
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (servoCharacteristic.written()) {
        if (servoCharacteristic.value()) {   // any value other than 0
          Serial.println("moving to ");
          servoCharacteristic.readValue(tempPos);
          Serial.print(tempPos);
          digitalWrite(ledPin, HIGH);         // will turn the LED on
          if(tempPos > 180){
            pos=180;
          }else if (tempPos< 0){
            pos =0;
          } else{
            pos = tempPos;
          }
          myServo.write(pos);
        } else {                              // a 0 value
          Serial.println(F("Error: Line 80"));        // will turn the LED off
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
