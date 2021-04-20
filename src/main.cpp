#include "EspMQTTClient.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

TaskHandle_t neopixelTask;
//TaskHandle_t Task2;

#define LED_PIN 15
#define LED_COUNT 25

String identifier = "3dfa";
String client_name = "codye_neonm_" + identifier;
String color_topic = "neonm/" + identifier + "/color";

int r = 0;
int g = 255;
int b = 0;

int doOnce = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

EspMQTTClient client(
  "PLACEHOLDER",
  "PLACEHOLDER",
  "PLACEHOLDER",  // MQTT Broker server ip
  "PLACEHOLDER",   // Can be omitted if not needed
  "PLACEHOLDER",   // Can be omitted if not needed
  "PLACEHOLDER",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<=strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void neopixel( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  colorWipe(strip.Color(r,   g,   b), 50);
  vTaskDelete( NULL );
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(color_topic, [](const String & payload) {
    Serial.println("From " + color_topic + ": " + payload);
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    JsonObject obj = doc.as<JsonObject>();
    r = obj["r"];
    g = obj["g"];
    b = obj["b"];
      xTaskCreatePinnedToCore(
                    neopixel,   /* Task function. */
                    "neopixelTask",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &neopixelTask,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */
    //vTaskSuspend( neopixelTask );
  });

  // Publish a message to "mytopic/test"
  client.publish("mytopic/test", "This is a message"); // You can activate the retain flag by setting the third parameter to true

  // Execute delayed instructions
  client.executeDelayed(5 * 1000, []() {
    client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  });
}

void setup()
{
  Serial.begin(115200);
  Serial.print(client_name);
  Serial.print("Setup core:");
  Serial.println(xPortGetCoreID());

  // Optionnal functionnalities of EspMQTTClient : 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  //client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true

  // xTaskCreatePinnedToCore(
  //                   neopixel,   /* Task function. */
  //                   "neopixelTask",     /* name of task. */
  //                   10000,       /* Stack size of task */
  //                   NULL,        /* parameter of the task */
  //                   1,           /* priority of the task */
  //                   &neopixelTask,      /* Task handle to keep track of created task */
  //                   0);          /* pin task to core 0 */ 

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop()
{
  if ( doOnce == 0 ) {
    Serial.print("Loop core:");
    Serial.println(xPortGetCoreID());
    doOnce = 1;
  }
  client.loop();
}