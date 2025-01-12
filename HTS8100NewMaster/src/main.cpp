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
//signal detector
#include "audio_signal_detector.h"


DNSServer dnsServer;
devices::wifi_man wifi(IPAddress(LOCAL_IP), TIME_TO_CONNECT_S, AP_NAME, AP, AP_PASS, TIME_TO_PREFER_INTERNET_NETWORK, TIME_TO_RECONNECT_WIFI);
//settings
HTSNewMaster::eeprom_data settings;
//eeprom store timer
unsigned long last_eeprom_store_timestamp = millis();
const unsigned long interval_eeprom_store_ms = tools::timer::sec_to_msec(10);
//controller
HTSNewMaster::i2c_controller controller(I2C_MASTER_ADDRES);
//NTP
WiFiUDP ntpUDP;
NTPClient time_client(ntpUDP);
//timer
tools::timer clock_timer(CLOCK_UPDATE_INTERVAL);

//............................................................
//AUDIO DETECTORS.............................................
//............................................................

void set_active_input(HTSNewMaster::eeprom_data::EInput input)
{
    settings.set_input(input);
    controller.input_set(input);
    HTSNewMaster::UI::get().set_input(input);
    HTSNewMaster::UI::get().update_status();
}

void start_signal_detector(HTSNewMaster::audio_signal_detector &sd, const HTSNewMaster::eeprom_data::signal_detector &data)
{
    if(data.enabled) {
        sd.start( 
              tools::timer::sec_to_msec(data.signal_timeout_s)
            , tools::timer::sec_to_msec(data.silience_timeout_s)
            , data.zero_level
            , data.sensivity
            , data.signal_tolerance_precent
            , data.measure_interval_ms
        );
    } else {
        sd.stop();
    }
}

void try_input_change(HTSNewMaster::eeprom_data::EDPriority call_from_ad, HTSNewMaster::eeprom_data::EInput change_to);

//audio signal detector AUX 1
HTSNewMaster::audio_signal_detector sd_AUX1(
      SD_AUX1_PIN
    , [&]()
        {
            static int c = 0;
            DBGNL(DBG_DEBUG, ("-================ AUX1 Signal (%d) ================-")(c++));

            using EInput = HTSNewMaster::eeprom_data::EInput;
            using EDPriority = HTSNewMaster::eeprom_data::EDPriority;

            try_input_change(EDPriority::pAUX1, EInput::iAUX1);
        }
    , [&]()
        {
            static int c = 0;
            DBGNL(DBG_DEBUG, ("-================ AUX1 Silience (%d) ================-")(c++));

            using EInput = HTSNewMaster::eeprom_data::EInput;
            using EDPriority = HTSNewMaster::eeprom_data::EDPriority;

            try_input_change(EDPriority::pAUX1, EInput::iDisabled);
        }
    , [&](uint16_t tolerance)
        {
            DBGNL(DBG_DEBUG, ("AUX1 Tolerance %d")(tolerance));

            HTSNewMaster::UI::get().set_AUX1_tolerance(tolerance);
        }
);

//............................................................

//audio signal detector AUX 2
HTSNewMaster::audio_signal_detector sd_AUX2(
      SD_AUX2_PIN
    , [&]()
        {
            static int c = 0;
            DBGNL(DBG_DEBUG, ("-================ AUX2 Signal (%d) ================-")(c++));

            using EInput = HTSNewMaster::eeprom_data::EInput;
            using EDPriority = HTSNewMaster::eeprom_data::EDPriority;

            try_input_change(EDPriority::pAUX2, EInput::iAUX2);
        }
    , [&]()
        {
            static int c = 0;
            DBGNL(DBG_DEBUG, ("-================ AUX2 Silience (%d) ================-")(c++));

            using EInput = HTSNewMaster::eeprom_data::EInput;
            using EDPriority = HTSNewMaster::eeprom_data::EDPriority;

            try_input_change(EDPriority::pAUX2, EInput::iDisabled);
        }
    , [](uint16_t tolerance)
        {
            DBGNL(DBG_DEBUG, ("AUX2 Tolerance %d")(tolerance));

            HTSNewMaster::UI::get().set_AUX2_tolerance(tolerance);
        }
);

HTSNewMaster::eeprom_data::EInput map(HTSNewMaster::eeprom_data::EDPriority from)
{
    using EInput = HTSNewMaster::eeprom_data::EInput;
    using EDPriority = HTSNewMaster::eeprom_data::EDPriority;

    switch(from)
    {
        case EDPriority::pAUX1: return EInput::iAUX1;
        case EDPriority::pAUX2: return EInput::iAUX2;
    }

    throw("New priority value?");
}

void smart_switch(HTSNewMaster::eeprom_data::EDPriority call_from_ad
                , HTSNewMaster::eeprom_data::EInput change_to
                , HTSNewMaster::eeprom_data::EDPriority opposite_ad
                , HTSNewMaster::audio_signal_detector::EState opposite_state)
{
    using EInput = HTSNewMaster::eeprom_data::EInput;
    using EDPriority = HTSNewMaster::eeprom_data::EDPriority;
    using EState = HTSNewMaster::audio_signal_detector::EState;

    if(EInput::iDisabled != change_to)
    {
        if(    EInput::iDisabled == settings.get_input() 
            || EInput::iRadio == settings.get_input()
            || EInput::iCoaxial == settings.get_input()
            || call_from_ad == settings.get_signal_detector_priority())
        {
            set_active_input(map(call_from_ad));
        }
    }
    else
    {
        auto active_input = settings.get_input();
        if(map(call_from_ad) == active_input)
        {
            if(EState::eSignal == opposite_state)
            {
                set_active_input(map(opposite_ad));
            }
            else
            {
                set_active_input(EInput::iDisabled);
            }
        }
    }
}

void try_input_change(HTSNewMaster::eeprom_data::EDPriority call_from_ad, HTSNewMaster::eeprom_data::EInput change_to)
{
    using EInput = HTSNewMaster::eeprom_data::EInput;
    using EDPriority = HTSNewMaster::eeprom_data::EDPriority;
    using EState = HTSNewMaster::audio_signal_detector::EState;

    switch(call_from_ad)
    {
        case EDPriority::pAUX1:
        {
            smart_switch(call_from_ad, change_to, EDPriority::pAUX2, sd_AUX2.state());
        } break;

        case EDPriority::pAUX2:
        {
            smart_switch(call_from_ad, change_to, EDPriority::pAUX1, sd_AUX1.state());
        } break;
    }
}

//............................................................

void setup() 
{
  delay(INITIAL_DELAY);
  pinMode(BT_UNPAIR_PIN, OUTPUT);
  digitalWrite(BT_UNPAIR_PIN, LOW);

  DBG_INIT();
  DBGNL(DBG_INFO, ("--------------------- Session start ---------------------"));

  //HTSNewMaster::set_eeprom_data_dirty();
  //bool b = HTSNewMaster::save(settings);
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

  HTSNewMaster::UI::get().begin(&settings, &sd_AUX1, &sd_AUX2, &controller, WEB_UI_TITLE/*, WEB_UI_AUTH_USER, WEB_UI_AUTH_PASS*/);
  
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
                set_active_input(eeprom_data::EInput::iDisabled);
            }
        }

        HTSNewMaster::UI::get().update_status();

    }, nullptr
  );
  clock_timer.start();

  start_signal_detector(sd_AUX1, settings.get_AUX_1());
  start_signal_detector(sd_AUX2, settings.get_AUX_2());
  
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

  sd_AUX1.update();
  sd_AUX2.update();

  //OTA
  ArduinoOTA.handle();
}
