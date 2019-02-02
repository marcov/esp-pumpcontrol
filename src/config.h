#ifndef __CONFIG_H__
#define __CONFIG_H__

#define LED_PIN             2
#define VLV1_PIN            4
#define VLV2_PIN            5
#define PMP_PIN             14
#define BTN_PIN             12

#define FW_VERSION          "2.0"

#define MQTT_PUB_PERIOD_MS      5000
#define BTN_CHECK_PERIOD_MS     1500
#define VALVE_PULSE_MS          50
#define VALVE_2_PUMP_DELAY_MS   500

#define SWITCH_OFF_FAILSAFE_S   (20 * 60)

#endif /* #ifndef __CONFIG_H__ */
