#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      40

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#include <SoftwareSerial.h>
SoftwareSerial mySerial(11, 10); // RX, TX
int myValues[5];
String concatStr;
void setup() {
  // put your setup code here, to run once:
  mySerial.begin(115200);
  //Serial.begin(115200);
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(255);
  pixels.show(); // This initializes the NeoPixel library.
}

void loop() {
  // put your main code here, to run repeatedly:
  lights();
  delay(10);
}

//function for lights with 4 parameters that reads the message from MT7688 and makes the Atmega32U4 to act acordingly
int x, y, z, w;
void lights() {
  byte counter = 0;
  while (  mySerial.available())
  {
    char c = mySerial.read();
    if ( c == ',' )
    {
      if ( counter == 5 ) //Array size is set to 10, when full, reset and redo
      { memset(myValues, 0, sizeof(myValues));
        counter = 0;
      //  Serial.println("_____________________");
      }

      myValues[counter] = concatStr.toInt();
    //Serial.print("Added ");
    //     Serial.println(counter);
    //     Serial.println(myValues[counter]);


      counter++;
      concatStr = "";
    }
    else
    {
      concatStr += c;
    }

   /* Serial.println(myValues[0]);
      Serial.println(myValues[1]);
      Serial.println(myValues[2]);
      Serial.println(myValues[3]);*/
  }
  if (myValues[0] == 10) {
    Serial.println(myValues[0]);
    Serial.println(myValues[1]);
    Serial.println(myValues[2]);
    Serial.println(myValues[3]);
    culoare(myValues[1], myValues[3], myValues[2]);
  }



  myValues[0] = 0;
  myValues[1] = 0;
  myValues[2] = 0;
   //Serial.println(myValues[0]);
  //Serial.println(myValues[1]);

}

void culoare (byte r, byte g, byte b)
{
  for (int i = 0; i < NUMPIXELS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r, g, b)); // Moderately bright green color.
    pixels.setBrightness(255);
    pixels.show(); // This sends the updated pixel color to the hardware.

    // delay(delayval); // Delay for a period of time (in milliseconds).

  }
}
