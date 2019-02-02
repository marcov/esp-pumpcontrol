/*
 Pump Control
*/
extern "C" {
    #include "mqtt.h"
}

#include "wifi.h"
#include <Ticker.h>
#include "config.h"
#include "webserver.h"
#include "credentials.hh"
#include "irrigation.hh"
#include "webpages.h"

////////////////////////////////////////////////////////////////////////////////

const char *control_topic        = "/pump/control";
const char *status_topic         = "/pump/status";

static unsigned last_btn_check;


char mqtt_pub_buffer[128];

Ticker secondsTimer;
Irrigation irrig;

static void mqtt_set_irrig_cb(unsigned char *payload, unsigned int length);
static const char * my_get_pubmsg_status(void);

mqtt_pubitem_t mqtt_publist[] = {
    {.topic = status_topic, .getmsg=my_get_pubmsg_status, .period_ms = MQTT_PUB_PERIOD_MS},
    {.topic = NULL, .getmsg = NULL, .period_ms = 0},
};

mqtt_subitem_t mqtt_sublist[] = {
    {.topic = control_topic, .cb = mqtt_set_irrig_cb},
    {.topic = NULL, .cb = NULL},
};

const mqtt_cfg_t mqtt_cfg = {
    .server  = Credentials::mqttBroker,
    .publist = mqtt_publist,
    .sublist = mqtt_sublist,
};

/******************************************************************************/


static const char * my_get_pubmsg_status(void)
{
   snprintf (mqtt_pub_buffer, sizeof(mqtt_pub_buffer), (irrig.active ? "1" : "0"));
   return mqtt_pub_buffer;
}



static void mqtt_set_irrig_cb(unsigned char *payload, unsigned int length)
{
    bool mode;

    Serial.print(String(__FUNCTION__) + ": ");

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }

    Serial.println();

    mode = (char)payload[0] == '1';

    if (mode != irrig.active)
    {
        irrig.toggle();
        mqtt_async_publish(status_topic, my_get_pubmsg_status());
    }
}

void btn_check(void)
{
    if (digitalRead(BTN_PIN) == 0)
    {
        irrig.toggle();

        last_btn_check = millis();
    }
}

static void  secondsTimerCB(void)
{
    irrig.updateCounters();

}

void setup(void)
{

    digitalWrite(LED_PIN,  HIGH);
    digitalWrite(VLV1_PIN, LOW);
    digitalWrite(VLV2_PIN, LOW);
    digitalWrite(PMP_PIN,  LOW);

    pinMode(LED_PIN, OUTPUT);
    pinMode(VLV1_PIN, OUTPUT);
    pinMode(VLV2_PIN, OUTPUT);
    pinMode(PMP_PIN, OUTPUT);

    pinMode(BTN_PIN, INPUT);

    irrig.active = false;
    last_btn_check = millis();

    secondsTimer.attach_ms(1000, secondsTimerCB);

    Serial.begin(115200);

    for (unsigned i = 0; i < 10; i++)
    {
        digitalWrite(LED_PIN,  !digitalRead(LED_PIN));
        delay(100);
    }

    digitalWrite(LED_PIN,  HIGH);

    Serial.print("....Started up!");
    Serial.println("");

    irrig.switchOff();

    wifiSetup(false, true, Credentials::wifiSsid, Credentials::wifiPasswd);

#if defined(MQTT_ENABLE)
    mqtt_setup(&mqtt_cfg);
#endif
    webpagesInit();

    Serial.println("setup done");
}


void loop(void)
{
    unsigned now;

    now = millis();

    if (wifiHasConnected()) {
        // flash led to say we have just connected
        unsigned prev_state = digitalRead(LED_PIN);

        digitalWrite(LED_PIN, !prev_state);
        delay(100);
        digitalWrite(LED_PIN, prev_state);
    }

#if defined(MQTT_ENABLE)
    mqtt_check_reconnect();
    mqtt_loop();
#endif

    webserverLoop();

    if (now - last_btn_check > BTN_CHECK_PERIOD_MS)
    {
        btn_check();
    }
}
