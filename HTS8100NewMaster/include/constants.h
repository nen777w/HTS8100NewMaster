#ifndef __constants_h__
#define __constants_h__

//Network
#define DNS_PORT 53

//Network - WIFI
#define TIME_TO_CONNECT_S  10
#define TIME_TO_PREFER_INTERNET_NETWORK 1000 * 60
#define TIME_TO_RECONNECT_WIFI 1000 * 60 * 5
#define LOCAL_IP 192,168,1,1
#define AP_NAME "HTS8100"
#define AP ""
#define AP_PASS ""

//NTP Update Interval
#define NTP_UPDATE_INTERVAL 60000
#define CLOCK_UPDATE_INTERVAL 30000
#define STARTUP_SOUND_OFF_TIMER 2

//app
#define INITIAL_DELAY 2000

//strings
#define WEB_UI_TITLE        "HTS8100/12"
#define WEB_UI_AUTH_USER    "HTS8100"
#define WEB_UI_AUTH_PASS    "HTS8100"

//controls
#define WEB_UI_STATUS       "Status"

#define WEB_UI_TAB_SOUND            "Sound"
#define WEB_UI_TAB_EQ               "EQ"
#define WEB_UI_TAB_RADIO            "Radio"
#define WEB_UI_TAB_INPUT            "Input"
#define WEB_UI_TAB_SETTINGS         "Settings"

#define WEB_UI_VOLUME       "Volume"
#define WEB_UI_VOLUME_MIN   0
#define WEB_UI_VOLUME_MAX   40
#define WEB_UI_MUTE         "Mute"
#define WEB_UI_SOUND_MODE   "Sound Mode"
#define WEB_UI_VOLUME_DOWN  "-"
#define WEB_UI_VOLUME_UP    "+"
#define WEB_UI_VOLUME_MINIMUM "MIN"

#define WEB_UI_BASS         "Bass"
#define WEB_UI_TREBLE       "Trible"

#define WEB_UI_BASS_MIN     0
#define WEB_UI_BASS_MAX     9
#define WEB_UI_TREBLE_MIN   0
#define WEB_UI_TREBLE_MAX   9

#define WEB_UI_PRESETS      "Presets"
#define WEB_UI_PRESET       "Preset "

#define WEB_UI_EQ_MODE      "Eqializer presets"
#define WEB_UI_INPUT_SEL    "Input selector"
#define WEB_UI_ASYNC        "Audio Sync (ms)"
#define WEB_UI_INP_DIS_TT   "Disable Input's From <-> To"
#define WEB_UI_INP_DISABLE  "Disable by timer"

#define WEB_UI_ASYNC_MIN    0
#define WEB_UI_ASYNC_MAX    254

#define WEB_UI_AP           "WiFi AP"
#define WEB_UI_AP_PASSW     "WiFi AP Password"
#define WEB_UI_SAVE         "Save"
#define WEB_UI_REBOOT       "Reboot"
#define WEB_UI_I2C_TEST     "I2C Test"
#define WEB_UI_I2C_TEST_SND "Send"
#define WEB_UI_UNPAIR       "Unpair Bluetooth"
#define WEB_UI_NTP_SERVER   "NTP Server"
#define WEB_UI_NTP_ZONE     "NTP Zone"
#define WEB_UI_RESCAN_ALL   "Rescan All"
#define WEB_UI_NXT_PRESET   "  >  "   
#define WEB_UI_PRV_PRESET   "  <  "

#define WEB_UI_AD_AUX1              "Audio Detector: AUX1"
#define WEB_UI_AD_AUX2              "Audio Detector: AUX2"
#define WEB_UI_AD_ENABLED           "Enabled"
#define WEB_UI_SIG_TIMEOUT          "Signal Timeout (sec)"
#define WEB_UI_SIG_TIMEOUT_MIN      2
#define WEB_UI_SIG_TIMEOUT_MAX      5
#define WEB_UI_SIL_TIMEOUT          "Silience Timeout (sec)"
#define WEB_UI_SIL_TIMEOUT_MIN      5
#define WEB_UI_SIL_TIMEOUT_MAX      240
#define WEB_UI_ZERO_LEVEL           "Zero Level (samples 0 - 4095)"
#define WEB_UI_CALIBRATE_ZERO_LEVEL "Calibrate Zero Level"
#define WEB_UI_SENSITIVITY          "Signal Sensivity from zero level (samples)"
#define WEB_UI_SENSITIVITY_MIN      10
#define WEB_UI_SENSITIVITY_MAX      35
#define WEB_UI_TOLERANCE            "Signal Tolerance (%)"
#define WEB_UI_TOLERANCE_MIN        10
#define WEB_UI_TOLERANCE_MAX        60
#define WEB_UI_MEASURE_INTERVAL     "Measure Interval (msec)"
#define WEB_UI_MEASURE_INTERVAL_MIN 10
#define WEB_UI_MEASURE_INTERVAL_MAX 100
#define WEB_UI_SIGNAL_TOLERANCE     "Signal tolerance: "
#define WEB_UI_TRIGGERS_COUNTER     "Triggers counter: "

#define WEB_UI_SD_PRIORITY          "Audio Detector Priority: AUX1 / AUX2"

//I2C Controller
#define I2C_MASTER_ADDRES   0x7C
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22
#define I2C_SPEED           100000
#define BT_UNPAIR_PIN       16

//Signal detector
#define ANALOG_PIN_MIN     0
#define ANALOG_PIN_MAX     4095
#define SD_AUX1_PIN        36
#define SD_AUX2_PIN        34


#endif