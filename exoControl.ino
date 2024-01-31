#include <VescUart.h>
#include <SoftwareSerial.h>

// Objects/variables (potentiometer, duty-cycle, and vesc)
SoftwareSerial SerialVesc(12, 13); //RX, TX
SoftwareSerial SerialBluetooth(10, 11); //Rx, Tx
const int FSRpin1 = A0;
const int FSRpin2 = A1;
VescUart vesc;

// CPM Parameters
float resistanceLevel = 0.04;
int minsDuration = 5;
int romDegrees = 90;
int romTime = 0;
int romCount = 0;
int repCount = 0;
//int romStatus = 0;

// Motor Parameters
unsigned long timeIncrement = 125UL;
unsigned long timeLast = 0;
float pressureFeedback = 0.0;
float gearboxRatio = 90.0;
float RPM = 3000.0;//240.0;
bool isRunning = false;
bool feedbackMode = false; //New feature

// Device/Port initializations
void setup() {
  Serial.begin(19200);
  SerialVesc.begin(19200);
  SerialBluetooth.begin(9600);
  vesc.setSerialPort(&SerialVesc);
  delay(1000);

  Serial.println("Goodnight moon!");
  //romTime = (romDegrees / ((/*RPM*/ 480 / 60) * 4)) * 1000; // in millis
  romTime = (((romDegrees * gearboxRatio) / 360) / (RPM / 60)) * 1000; //ms 
}

void loop() { // Super-Loop for CPM functionality

  // Setting forward/reverse duty cycle using FSR values
  pressureFeedback = (analogRead(FSRpin1)/1023.0)-(analogRead(FSRpin2)/1023.0);

  // Running motor
  if (abs(pressureFeedback) > resistanceLevel || isRunning == false) {
    vesc.setDuty(0.0); // stop motor
    //vesc.setDuty(pressureFeedback);

  } else { vesc.setRPM(RPM); }
  
  // Increment count if 125ms has passed
  if (millis() - timeLast > timeIncrement) {
    timeLast += timeIncrement;
    romCount++;
    Serial.println("Time incremented.");
  } 

  // Change direction of motor and restart count
  if (romCount >= romTime/timeIncrement) {
    vesc.setDuty(0.0);
    delay(1000);

    RPM = RPM * (-1);
    romCount = 0;
    timeLast = millis();

    Serial.print("New RPM: ");
    Serial.println(RPM, DEC);
    vesc.setRPM(RPM);
  }

    // If arm is opening, we can increment romTime by a unit of timeIncrement. 
    // This will increase the ROM experienced
    if (RPM > 1 && repcount >= 5) {
      repCount = 0;
      romTime+= timeIncrement;
    }
  }
  //TODO: make a function to start arm at 75% romDegrees, and slowly increment until at 100% over the minsDuration of operation
  // New instruction from app
  if (SerialBluetooth.available()) {
    
    char message[8];
    for (int i = 0; i < 8; i++) message[i] = SerialBluetooth.read();
    Serial.println(message);
    
    // Handle message[] instruction
    switch (message[0]) {
      
      case '1': // Start/Stop
        if (message[1] == '1') isRunning = true;
        else isRunning = false;
        Serial.print("Motor status: ");
        Serial.println(isRunning);
      break;
      
      case '2': // set ROM
        romDegrees = atoi(message[1])*100 
                  + atoi(message[2])*10 
                  + atoi(message[3]);
        romTime = (((romDegrees * gearboxRatio) / 360) 
                  / (RPM / 60)) * 1000; //ms
        //(romDegrees / ((RPM / 60) * 4)) * 1000; // (Theta / ()
        Serial.print("NEW ROM: ");
        Serial.println(romDegrees, DEC);
        Serial.print("Movement duration (ms): ");
        Serial.println(romTime, DEC);
      break;
      
      case '3': // set Resistance
        resistanceLevel = float(atoi(message[2])/10 
                          + atoi(message[3]))/100;
        Serial.print("NEW Resistance: ");
        Serial.println(resistanceLevel, DEC);
      break;
      
      case '4': // set Duration
        minsDuration = atoi(message[1])*100 
                      + atoi(message[2])*10 
                      + atoi(message[3]);
        Serial.print("NEW Duration: ");
        Serial.println(minsDuration, DEC);
      break;
      
      case '5': // get ROM status
        Serial.print("RomCount sent:");
        Serial.println(romCount/(romTime/timeIncrement));
        SerialBluetooth.print(romCount);
      break;

      //This setting allows the user to move the arm based on FSR data
      case '6': 
        
      break;

      default: break;
    } 
  }
  // Refresh rate for motor instructions
  delay(25);
}