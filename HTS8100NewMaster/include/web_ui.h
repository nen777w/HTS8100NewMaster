#ifndef __ui_h__
#define __ui_h__

#include <LittleFS.h>
#include <ESPUI.h>

namespace HTSNewMaster
{
    //fw
    struct eeprom_data;
    class i2c_controller;

    class UI
    {
    private:
        UI() {}

    public:
        void begin(eeprom_data *settings, i2c_controller *controller, const char* title, const char* username = nullptr, const char* password = nullptr, uint16_t port = 80);
        void update();
        
        void set_time(time_t time);
        void disable_input();

    private:
        void build_ui();

    public:
        static UI & get()
        {
            static UI instance;
            return instance;
        }

    private:
        eeprom_data *m_ptr_settings;
        i2c_controller *m_ptr_controller;
    private:
        time_t  m_time;
        String  m_str_time;
    private:
        int ctrl_status;
        int ctrl_tab_volume;
        int ctrl_tab_eq;
        int ctrl_tab_input;
        int ctrl_tab_settings;
        int ctrl_tab_radio;
        int ctrl_volume;
        int ctrl_volume_up;
        int ctrl_volume_down;
        int ctrl_volume_min;
        int ctrl_mute;
        int ctrl_sound_mode;
        int ctrl_bass;
        int ctrl_treble;
        int ctrl_eq_mode;
        int ctrl_radio_presets;
        int ctrl_radio_next_preset;
        int ctrl_radio_prev_preset;
        int ctrl_radio_full_rescan;
        int ctrl_input_selector;
        int ctrl_audio_sync;
        int ctrl_input_off_timer_from;
        int ctrl_input_off_timer_to;
        int ctrl_input_off_switch;
        int ctrl_wifi_ap;
        int ctrl_wifi_ap_pass;
        int ctrl_save;
        int ctrl_reboot;
        int ctrl_i2c_test;
        int ctrl_i2c_test_send;
        int ctrl_unpair;
        int ctrl_ntp_server;
        int ctrl_ntp_zone;
    };
}

#endif