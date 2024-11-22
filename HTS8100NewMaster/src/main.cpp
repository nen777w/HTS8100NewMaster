#include <Arduino.h>
#include <ArduinoOTA.h>
//constants
#include "constants.h"
//Network
#include <DNSServer.h>
//wifiman
#include <wifi_man.h>
//NTP
#include <NTPClient.h>
//debug
#include "dbg_msg_macro.h"
//web ui
#include "web_ui.h"
//settings
#include "settings.h"
//i2c
#include "i2c_controller.h"
//tools
#include "timer.h"


DNSServer dnsServer;
devices::wifi_man wifi(IPAddress(LOCAL_IP), TIME_TO_CONNECT_S, AP_NAME, AP, AP_PASS, TIME_TO_PREFER_INTERNET_NETWORK, TIME_TO_RECONNECT_WIFI);
//settings
HTSNewMaster::eeprom_data settings;
//eeprom store timer
unsigned long last_eeprom_store_timestamp = millis();
const unsigned long interval_eeprom_store_ms = 10000; //10 sec
//controller
HTSNewMaster::i2c_controller controller(I2C_MASTER_ADDRES);
//NTP
WiFiUDP ntpUDP;
NTPClient time_client(ntpUDP);
//timer
tools::timer clock_timer(CLOCK_UPDATE_INTERVAL);


void setup() 
{
  delay(INITIAL_DELAY);
  pinMode(BT_UNPAIR_PIN, OUTPUT);
  digitalWrite(BT_UNPAIR_PIN, LOW);

  DBG_INIT();
  DBGNL(DBG_INFO, ("--------------------- Session start ---------------------"));

  //HTSNewMaster::set_eeprom_data_dirty();
  ///bool b = HTSNewMaster::save(settings);
  //delay(100);
  //DBGNL(DBG_DEBUG, ("Save: %d")(b));

  if(!HTSNewMaster::load(settings))
  {
    DBGNL(DBG_ERROR, ("Load settings from EEPROM - ERROR"));
    HTSNewMaster::set_eeprom_data_dirty();
  }
  else
  {
    DBGNL(DBG_INFO, ("Load settings from EEPROM - SUCCESS"));
  }

  HTSNewMaster::eeprom_data::print(settings);

  //NTP Client
  time_client.setPoolServerName(settings.get_ntp_server());
  time_client.setTimeOffset(0);
  time_client.setUpdateInterval(NTP_UPDATE_INTERVAL);
  setenv("TZ", settings.get_ntp_zone(), 1);
  tzset();

  //WiFi
  wifi.setup(AP_NAME, settings.get_wifi_ap(), settings.get_wifi_pass());
  wifi.on_connect_to_internet = [&]()
  {
    DBGNL(DBG_INFO, ("WiFi connected"));
    time_client.end();
    time_client.begin();
  };
  wifi.start();
  wifi.prefer_internet_mode(true);
  wifi.connect();
  dnsServer.start(DNS_PORT, "*", wifi.apIP());

  //OTA
  ArduinoOTA
    .onStart([]() 
    {
      if (ArduinoOTA.getCommand() == U_FLASH)
      {
        DBGNL(DBG_INFO, ("Start updating sketch"));
      }
      else
      {
        DBGNL(DBG_INFO, ("Start updating filesystem"));
      }
    })
    .onEnd([]() 
    {
      DBGNL(DBG_INFO, ("OTA END"));
      DBGNL(DBG_INFO, ("Save Settings to EEPROM"));
      HTSNewMaster::save(settings);

      ESP.restart();      
    })
    .onProgress([](unsigned int progress, unsigned int total) 
    {
      DBGNL(DBG_INFO, ("Progress: %u%%\r")(progress / (total / 100)));
    })
    .onError([](ota_error_t error) 
    {
      switch(error)
      {
        case OTA_AUTH_ERROR:
        {
          DBGNL(DBG_ERROR, ("Error Auth Failed"));
        } break;
        case OTA_BEGIN_ERROR:
        {
          DBGNL(DBG_ERROR, ("Error Begin Failed"));
        } break;
        case OTA_CONNECT_ERROR:
        {
          DBGNL(DBG_ERROR, ("Error Connect Failed"));
        } break;
        case OTA_RECEIVE_ERROR:
        {
          DBGNL(DBG_ERROR, ("Error Receive Failed"));
        } break;
        case OTA_END_ERROR:
        {
          DBGNL(DBG_ERROR, ("Error End Failed"));
        } break;
      }
    });

  ArduinoOTA.begin();

  HTSNewMaster::UI::get().begin(&settings, &controller, WEB_UI_TITLE/*, WEB_UI_AUTH_USER, WEB_UI_AUTH_PASS*/);
  
  controller.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_SPEED);

  //setup device according to settings
  controller.sound_mode_set(settings.get_sound_mode());
  controller.volume_set(settings.get_volume());
  settings.get_muted() ? controller.volume_mute() : controller.volume_unmute();
  controller.eq_bass_set(settings.get_bass());
  controller.eq_treble_set(settings.get_treble());
  controller.eq_mode_set(settings.get_eq_mode());
  controller.input_set(settings.get_input());
  controller.audio_sync_set(settings.get_audio_sync());

  //setup timer
  clock_timer.set_callback(
    [](void*)
    {
        HTSNewMaster::UI::get().set_time(time_client.getEpochTime());

        using namespace HTSNewMaster;

        if(eeprom_data::EInput::iDisabled != settings.get_input() && settings.get_disable_input_active())
        {
            enum {
                Hour = 0,
                Minute = 1
            };

            auto disable_from = settings.get_disable_input_from_tpl();
            auto disable_to = settings.get_disable_input_to_tpl();

            int current_time = time_client.getHours() * 60 + time_client.getMinutes();
            int time_from = std::get<Hour>(disable_from) * 60 + std::get<Minute>(disable_from);
            int time_to = std::get<Hour>(disable_to) * 60 + std::get<Minute>(disable_to);

            // Check if current time is within the disabled input range
            bool is_within_range = (time_from <= time_to)
                ? (current_time >= time_from && current_time <= time_to)
                : !(current_time >= time_to && current_time <= time_from);

            if (is_within_range) {
                settings.set_input(eeprom_data::EInput::iDisabled);
                controller.input_set(eeprom_data::EInput::iDisabled);
                HTSNewMaster::UI::get().disable_input();
            }
        }

        HTSNewMaster::UI::get().update();

    }, nullptr
  );
  clock_timer.start();

  
  //DBGNL(DBG_INFO, ("%d : %d")(std::get<0>(mh))(std::get<1>(mh)));
}

void loop() 
{
  wifi.update();
  dnsServer.processNextRequest();

  controller.update();

  if(millis() - last_eeprom_store_timestamp >= interval_eeprom_store_ms)
  {
    if(HTSNewMaster::save(settings)) {
      DBGNL(DBG_INFO, ("Save Settings to EEPROM"));
    }
    last_eeprom_store_timestamp = millis();
  }

  time_client.update();
  clock_timer.update();

  //OTA
  ArduinoOTA.handle();
}
