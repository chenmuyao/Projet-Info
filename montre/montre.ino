/* 
 *  @Project Name: Watch'INT
 *  @author: CHEN Muyao, FIS Victor
 *  @date: 13/03/2017
 *  @discription: Prototype d'une montre connectée
 *  
 *  This prototype is aimed at showing the time on the OLED screen by synchronizing the time via Bluetooth
 *  Time-sychronizing messages consists of the letter T followed by ten digit time (as seconds since Jan 1 1970)
 *  For example, we can send the text on the next line using Bluetooth Terminal to set the clock to noon Jan 1 2013: T1357041600  
 *  
 *  Bluetooth TX-10(RX)
 *  Bluetooth RX-11(TX)
 *  Screen SDA_PIN-A4
 *  Screen SCL_PIN-A5
 */ 
 
#include <TimeLib.h>    // Library of Time
#include <Adafruit_ssd1306syp.h>    // Library of the screen
#include <SoftwareSerial.h>    // Library of software serial communication

// Define the pins for RX, TX on the Arduino board respectively to connect with Bluetooth
SoftwareSerial mySerial(10, 11);    

// Define the pins for the I²C Bus to communicate with OLED
#define SDA_PIN A4    
#define SCL_PIN A5
// Define the objet "display" of OLED
Adafruit_ssd1306syp display(SDA_PIN,SCL_PIN);

#define TIME_HEADER  "T"    // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

void setup()  {
  // Initialisation of Bluetooth communication module
  mySerial.begin(38400);    // Set the baud of port to 38400 in order to communicate with Bluetooth
  pinMode(13, OUTPUT);    // Define the indicator Led PIN 13 as output
  setSyncProvider(requestSync);    // Set function to call when sync required
  mySerial.println("Waiting for sync message");    // Show the sync require message on the bluetooth terminal
  delay(1000);    // Wait for 1s
  
  // Initialisation of OLED Screen module 
  display.initialize();    // Initialize the screen
  display.setTextSize(2);    // set the text size
  display.setTextColor(WHITE);    // set the text color as white
  display.setCursor(10,20);    // set the cursor
  display.print("WATCH'INT");    // show the welcome page in waiting for the sync
  display.update();    // update all the changes to the screen


}

void loop(){    
  // if the bluetooth serial communication is established, then process the synchronisation
  if (mySerial.available()) {
    processSyncMessage();
  }
 
  // if time status is not set(false),then 
  if (timeStatus()!= timeNotSet) {
    display.clear();
    digitalClockDisplay();  
  }
  if (timeStatus() == timeSet) {
    digitalWrite(13, HIGH); // LED on if synced
  } else {
    digitalWrite(13, LOW);  // LED off if needs refresh
  }
  delay(1000);
}

void digitalClockDisplay(){
  // digital clock display of the time
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15,20);
  display.print(hour());
  printDigits(minute());
  printDigits(second());
  display.setCursor(15,40);
 
  //display.print(" ");
  display.print(day());
  display.print("/");
  display.print(month());
  display.print("/");
  display.print(year()); 
  display.update();
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  display.print(":");
  if(digits < 10)
    display.print('0');
  display.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(mySerial.find(TIME_HEADER)) {
     pctime = mySerial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync()
{
  mySerial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}


