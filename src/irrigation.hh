#ifndef __IRRIGATION_HH__
#define __IRRIGATION_HH__

#include <Arduino.h>
#include "config.h"

class Irrigation {

private:
    void valve_pump_mode(bool on)
    {
        if (on)
        {
            Serial.println("pump on and valve open");
            digitalWrite(LED_PIN, LOW);

            valve_open();
            delay(VALVE_2_PUMP_DELAY_MS);
            digitalWrite(PMP_PIN, HIGH);
        }
        else
        {
            Serial.println("valve close and pump off");
            digitalWrite(LED_PIN, HIGH);

            digitalWrite(PMP_PIN, LOW);
            delay(VALVE_2_PUMP_DELAY_MS);
            valve_close();
        }

        active = on;
    }
    inline void valve_open(void)
    {
        digitalWrite(VLV1_PIN, HIGH);
        delay(VALVE_PULSE_MS);
        digitalWrite(VLV1_PIN, LOW);
    }

    inline void valve_close(void)
    {
        digitalWrite(VLV2_PIN, HIGH);
        delay(VALVE_PULSE_MS);
        digitalWrite(VLV2_PIN, LOW);
    }

public:
    unsigned uptime;
    unsigned timeOn;
    bool active;

    Irrigation() : uptime(0), timeOn(0), active(false) {}

    void switchOn() {
        valve_pump_mode(true);
    }

    void switchOff() {
        valve_pump_mode(false);
    }

    void toggle() {

        if (!active) switchOn();
        else         switchOff();

    }

    void updateCounters() {
        uptime++;

        if (active)    timeOn++;
        else                 timeOn = 0;

        if (timeOn > SWITCH_OFF_FAILSAFE_S)
        {
            // fail-safe switch off
            valve_pump_mode(false);
        }
    }
};

extern Irrigation irrig;

#endif /*  #ifndef __IRRIGATION_HH__ */
