/*
 * HelTec Automation(TM) WIFI_LoRa_32 factory test code, witch includ
 * follow functions:
 *
 * - Basic OLED function test;
 *
 * - Basic serial port test(in baud rate 115200);
 *
 * - Basic LED test;
 *
 * - WIFI join and scan test;
 *
 * - ArduinoOTA By Wifi;
 *
 * - Timer test and some other Arduino basic functions.
 *
 * by lxyzn from HelTec AutoMation, ChengDu, China
 * 成都惠利特自动化科技有限公司
 * www.heltec.cn
 *
 * this project also realess in GitHub:
 * https://github.com/HelTecAutomation/Heltec_ESP32
*/


#include <ArduinoOTA.h>
#include <WiFi.h>
#include <Wire.h>
#include "heltec.h"
#include <PubSubClient.h>


/**********************************************  WIFI Client 注意编译时要设置此值 *********************************
 * wifi client
 */
const char* ssid = "PLACEHOLDER"; //replace "xxxxxx" with your WIFI's ssid
const char* password = "PLACEHOLDER"; //replace "xxxxxx" with your WIFI's password

/************************************************  注意编译时要设置此值 *********************************
 * 是否使用静态IP
 */
#define USE_STATIC_IP false
#if USE_STATIC_IP
  IPAddress staticIP(192,168,86,42);
  IPAddress gateway(192,168,86,1);
  IPAddress subnet(255,255,255,0);
  IPAddress dns1(1, 1, 1, 1);
  IPAddress dns2(8,8,8,8);
#endif

/* MQTT Config */
IPAddress server(172, 16, 0, 2);

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.println("callbacked");
}

//PubSubClient client(server, 1883, callback, ethClient);

/*******************************************************************
 * OLED Arguments
 */
//#define RST_OLED 16                     //OLED Reset引脚，需要手动Reset，否则不显示
#define OLED_UPDATE_INTERVAL 500        //OLED屏幕刷新间隔ms
//SSD1306 display(0x3C, 4, 15);           //引脚4，15是绑定在Kit 32的主板上的，不能做其它用


/*********************************************************************
 * setup wifi
 */
void setupWIFI()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Connecting...");
  Heltec.display->drawString(0, 10, String(ssid));
  Heltec.display->display();

  //连接WiFi，删除旧的配置，关闭WIFI，准备重新配置
  WiFi.disconnect(true);
  delay(1000);

  WiFi.mode(WIFI_STA);
  //WiFi.onEvent(WiFiEvent);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);    //断开WiFi后自动重新连接,ESP32不可用
  //WiFi.setHostname(HOSTNAME);
  WiFi.begin(ssid, password);
#if USE_STATIC_IP
  WiFi.config(staticIP, gateway, subnet);
#endif

  //等待5000ms，如果没有连接上，就继续往下
  //不然基本功能不可用
  byte count = 0;
  while(WiFi.status() != WL_CONNECTED && count < 10)
  {
    count ++;
    delay(500);
    Serial.print(".");
  }

  Heltec.display->clear();
  if(WiFi.status() == WL_CONNECTED)
    Heltec.display->drawString(0, 0, "Connected");
  else
    Heltec.display->drawString(0, 0, "Connect False");
  Heltec.display->display();
}

/******************************************************
 * arduino setup
 */
void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  pinMode(25, OUTPUT);
  digitalWrite(25,HIGH);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Initialize...");

  setupWIFI();
}

/******************************************************
 * arduino loop
 */
void loop()
{
  unsigned long ms = millis();
  if(ms % 5000 == 0)
  {
    Serial.print(".");
  }
}

/****************************************************
 * [通用函数]ESP32 WiFi Kit 32事件处理
 */
void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event)
    {
        case SYSTEM_EVENT_WIFI_READY:               /**< ESP32 WiFi ready */
            break;
        case SYSTEM_EVENT_SCAN_DONE:                /**< ESP32 finish scanning AP */
            break;

        case SYSTEM_EVENT_STA_START:                /**< ESP32 station start */
            break;
        case SYSTEM_EVENT_STA_STOP:                 /**< ESP32 station stop */
            break;

        case SYSTEM_EVENT_STA_CONNECTED:            /**< ESP32 station connected to AP */
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:         /**< ESP32 station disconnected from AP */
            break;

        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:      /**< the auth mode of AP connected by ESP32 station changed */
            break;

        case SYSTEM_EVENT_STA_GOT_IP:               /**< ESP32 station got IP from connected AP */
        case SYSTEM_EVENT_STA_LOST_IP:              /**< ESP32 station lost IP and the IP is reset to 0 */
            break;

        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:       /**< ESP32 station wps succeeds in enrollee mode */
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:        /**< ESP32 station wps fails in enrollee mode */
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:       /**< ESP32 station wps timeout in enrollee mode */
        case SYSTEM_EVENT_STA_WPS_ER_PIN:           /**< ESP32 station wps pin code in enrollee mode */
        case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP:
            break;

        case SYSTEM_EVENT_AP_START:                 /**< ESP32 soft-AP start */
        case SYSTEM_EVENT_AP_STOP:                  /**< ESP32 soft-AP stop */
        case SYSTEM_EVENT_AP_STACONNECTED:          /**< a station connected to ESP32 soft-AP */
        case SYSTEM_EVENT_AP_STADISCONNECTED:       /**< a station disconnected from ESP32 soft-AP */
        case SYSTEM_EVENT_AP_PROBEREQRECVED:        /**< Receive probe request packet in soft-AP interface */
        case SYSTEM_EVENT_AP_STA_GOT_IP6:           /**< ESP32 station or ap interface v6IP addr is preferred */
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            break;

        case SYSTEM_EVENT_ETH_START:                /**< ESP32 ethernet start */
        case SYSTEM_EVENT_ETH_STOP:                 /**< ESP32 ethernet stop */
        case SYSTEM_EVENT_ETH_CONNECTED:            /**< ESP32 ethernet phy link up */
        case SYSTEM_EVENT_ETH_DISCONNECTED:         /**< ESP32 ethernet phy link down */
        case SYSTEM_EVENT_ETH_GOT_IP:               /**< ESP32 ethernet got IP from connected AP */
        case SYSTEM_EVENT_MAX:
            break;
    }
}