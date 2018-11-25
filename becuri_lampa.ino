
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define NUMPIXELS     40

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
//#include "WemoSwitch.h"
//#include "WemoManager.h"
#include "CallbackFunction.h"

#include "fauxmoESP.h"

fauxmoESP fauxmo;

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

int r;
int g;
int b;

int o_y = 255;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "4237217983da48d1b12233e2f2e9050e";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "hPop";
char pass[] = "pg1981camsabRAZ";
bool isFirstConnect = true;

/*WemoManager wemoManager;
  WemoSwitch *lamp = NULL;
  WemoSwitch *neon = NULL;
*/

void neonOn();
void neonOff();

void lampaOn();
void lampaOff();

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  neonOff();
  lampaOff();
  //Serial.println("Booting");
  Blynk.begin(auth, ssid, pass);
  //Alarm.timerRepeat(10, syncBlynk); // check every 10 SECONDS if schedule should run today
  Alarm.timerRepeat(30, reconnectBlynk);  // check every 30s if still connected to server

  /* wemoManager.begin();
    lamp = new WemoSwitch("desktop lamp", 80, lampaOn, lampaOff);
    neon = new WemoSwitch("desktop second lamp", 81, neonOn ,neonOff  );
    wemoManager.addDevice(*lamp);
    wemoManager.addDevice(*neon);*/

  fauxmo.enable(true);
  fauxmo.enable(false);
  fauxmo.enable(true);

  fauxmo.addDevice("desktop lamp");
  fauxmo.addDevice("desktop second lamp");
  fauxmo.addDevice("ambient light");


  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {

    if (device_id == 0) {
      state ? lampaOn() : lampaOff();
      int y = map(value, 0, 255, 0, 1023);
      if (o_y != y) {
        analogWrite(12, y);
        o_y = y;
      }

    }
    if (device_id == 1) {
      state ? neonOn() : neonOff();
    }
    if (device_id == 2) {
     // 
      ambient(value);
      if (state == false) {ambient(50);}
      
    }

    //Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
    // digitalWrite(LED, !state);
  });


  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("hPop Lampa");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");


}

BLYNK_CONNECTED() {
  if (isFirstConnect) {
    Blynk.syncAll();
    isFirstConnect = false;
  }
}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 0) {
    neonOff();
  }
  else if (pinValue == 1) {
    neonOn();
  }

  // process received value
}

BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 0) {
    lampaOff();
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (pinValue == 1) {
    lampaOn();
    digitalWrite(LED_BUILTIN, LOW);
  }

  // process received value
}


BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  int y = map(pinValue, 0, 100, 0, 1023);
  analogWrite(12, y);
}

BLYNK_WRITE(V2)
{
  // get a RED channel value
  r = param[0].asInt();
  g = param[1].asInt();
  b = param[2].asInt();
  String tmp = "";
  tmp = String(tmp + "10");
  tmp = String(tmp + ",");
  tmp = String(tmp + r);
  tmp = String(tmp + ",");
  tmp = String(tmp + g);
  tmp = String(tmp + ",");
  tmp = String(tmp + b);
  tmp = String(tmp + ",");
  Serial.println(tmp);
  delay(100);
  Serial.flush();
}

void ambient(int i) {
  String tmp = "";
  switch (i) {
    case 0:

      tmp = String(tmp + "10");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      Serial.println(tmp);
      delay(100);
      Serial.flush();
      break;
    case 2:

      tmp = String(tmp + "10");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      Serial.println(tmp);
      delay(100);
      Serial.flush();
      break;
    case 5:

      tmp = String(tmp + "10");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      Serial.println(tmp);
      delay(100);
      Serial.flush();
      break;
    case 7:

      tmp = String(tmp + "10");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      Serial.println(tmp);
      delay(100);
      Serial.flush();
      break;
    case 10:

      tmp = String(tmp + "10");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      Serial.println(tmp);
      delay(100);
      Serial.flush();
      break;
    case 12:

      tmp = String(tmp + "10");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      Serial.println(tmp);
      delay(100);
      Serial.flush();
      break;
    case 15:

      tmp = String(tmp + "10");
      tmp = String(tmp + ",");
      tmp = String(tmp + "150");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      tmp = String(tmp + "255");
      tmp = String(tmp + ",");
      Serial.println(tmp);
      delay(100);
      Serial.flush();
      break;

    default:
      tmp = String(tmp + "10");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      tmp = String(tmp + "0");
      tmp = String(tmp + ",");
      Serial.println(tmp);
      delay(100);
      Serial.flush();
      break;
  }


}


void loop() {
  if (Blynk.connected()) {
    Blynk.run();
  }
  // wemoManager.serverLoop();
  fauxmo.handle();

  ArduinoOTA.handle();
  Alarm.delay(5); // wait a little for the alarms to work
}


void reconnectBlynk() {
  if (!Blynk.connected()) {
    if (Blynk.connect()) {
      BLYNK_LOG("Reconnected");
    } else {
      BLYNK_LOG("Not reconnected");
    }
  }
}

void neonOn()
{
  digitalWrite(5, HIGH);
  Blynk.virtualWrite(V1, 1);
}
void neonOff()
{
  digitalWrite(5, LOW);
  Blynk.virtualWrite(V1, 0);
}

void lampaOn()
{
  digitalWrite(12, HIGH);
  Blynk.virtualWrite(V0, 1);
}
void lampaOff()
{
  digitalWrite(12, LOW);
  Blynk.virtualWrite(V0, 0);
}
