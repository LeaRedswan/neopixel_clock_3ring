#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#include "DCF77.h" //Alle DCF77 Funktionen laufen ueber diese Bibliothek.
#include "TimeLib.h" //Time.h hat bei mir nie funktioniert, habe TimeLib.h eingebunden, dann gehts.

#define  PIXELS              96             // total number of Neo-Pixels
#define  RINGS                6             // led rings output       

#define  DCF77_PIN            2             // DFC77 pin              
#define  ONE_SECOND_INTERUPT  0             // 1Hz on inturrupt 0    


DCF77 DCF = DCF77(DCF77_PIN, ONE_SECOND_INTERUPT, false);

Adafruit_NeoPixel rings = Adafruit_NeoPixel(PIXELS, RINGS, NEO_GRB + NEO_KHZ800);

boolean       hourOffset  = 0;              // flag for offsetting the hour hand when minutes are between 31 - 59


time_t prevDisplay = 0;

void setup()
{
    Serial.begin(115200);
    rings.begin(); 
    rings.setBrightness(10);
    rings.show(); 
    DCF.Start();
    setTime(10,43,34, 1, 1, 1970);
    setSyncInterval(30);
    setSyncProvider(ReadTimeDate);

    while(timeStatus()== timeNotSet) { 
        // wait until the time is set by the sync provider     
        Serial.print(".");
        delay(2000);
    }
}

void loop()
{
    rings.clear();

    if ( now() != prevDisplay){
        prevDisplay = now();
    }
    displayTime();
    //printouttime(ReadTimeDate());
    delay(300);
}

unsigned long ReadTimeDate(){
    time_t DCFtime = DCF.getTime();
	
    if(DCFtime!=0){          
    }
    return DCFtime;
}

void displayTime() {
    printouttime();

    drawClockFace();
    
    if (minute() > 30) { hourOffset = 1; }         // calc hour hand offset
    else               { hourOffset = 0; }

    // set colors
    rings.setPixelColor  ( second(), rings.Color(   56, 31, 81 ) ); //60
    rings.setPixelColor  ( minute(), rings.Color( 240,  40, 0 ) ); //60
    
    rings.setPixelColor  ( (hourFormat12() * 5 + ( round(minute() / 15) )) , rings.Color( 128, 128, 0 ) ); //60

    rings.setPixelColor  ( (hourFormat12() * 2) + hourOffset + 60  , rings.Color( 128, 128, 0 ) ); //24
    rings.setPixelColor  ( (hourFormat12() + 60 + 24) , rings.Color( 128, 128, 0 ) ); //24
    rings.setPixelColor  ( (hourFormat12() + hourOffset %12) + 60 + 24 , rings.Color( 128, 128, 0 ) ); //12
    
    rings.show();                                // display new time
}


void printouttime( ) {
    Serial.println("");
    Serial.print("Time: ");
    Serial.print(hour());
    Serial.print(":");
    printDigits(minute());
    Serial.print(":");
    printDigits(second());
    Serial.print(" Date: ");
    Serial.print(day());
    Serial.print(".");
    Serial.print(month());
    Serial.print(".");
    Serial.println(year());
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void drawClockFace() {
  byte index;
  
  // draw 12 ring faint red, 24 ring faint magenta, 60 ring faint blue with white tick marks every 5
  for ( index =  0; index < 12; index++ ) { rings.setPixelColor(  index + 24 + 60, rings.Color(  8,  0,  0 ) ); } 
  for ( index =  0; index < 24; index++ ) { rings.setPixelColor(  index + 60 , rings.Color(  8,  0,  8 ) ); }
  for ( index =  0; index < 60; index++ ) { 
    if( index % 5 == 0)                   { rings.setPixelColor(  index , rings.Color( 32, 40, 40 ) ); } 
    else                                  { rings.setPixelColor( index , rings.Color(  0,  0, 8 ) ); } 
  }
}