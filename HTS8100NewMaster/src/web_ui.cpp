#include "web_ui.h"

#include <time.h>

#include "constants.h"
#include "settings.h"
#include "i2c_controller.h"
#include "audio_signal_detector.h"
#include "dbg_msg_macro.h"
#include "timer.h"


void start_signal_detector(HTSNewMaster::audio_signal_detector &sd, const HTSNewMaster::eeprom_data::signal_detector &data);

namespace HTSNewMaster
{
    const std::pair<String, eeprom_data::ESoundMode> sound_mode[] = {
        { "Auto Stereo", eeprom_data::ESoundMode::smAutoStereo }
    ,   { "Multi Channel", eeprom_data::ESoundMode::smMultiCh }
    ,   { "Stereo", eeprom_data::ESoundMode::smStereo }
    };

    const std::pair<String, eeprom_data::EEQMode> eq_modes[] = {
        { "Classic", eeprom_data::EEQMode::eqClassic }
    ,   { "Jazz", eeprom_data::EEQMode::eqJazz }
    ,   { "Rock", eeprom_data::EEQMode::eqRock }
    ,   { "RnB", eeprom_data::EEQMode::eqRnB }
    ,   { "Party", eeprom_data::EEQMode::eqParty }
    ,   { "Lounge", eeprom_data::EEQMode::eqLounge }
    ,   { "Sports", eeprom_data::EEQMode::eqSports }
    ,   { "News", eeprom_data::EEQMode::eqNews }
    };

    const std::pair<String, eeprom_data::EInput> inputs[] = {
        { "None", eeprom_data::EInput::iDisabled }
    ,   { "AUX 1", eeprom_data::EInput::iAUX1 }
    ,   { "AUX 2", eeprom_data::EInput::iAUX2 }
    ,   { "Radio", eeprom_data::EInput::iRadio }
    ,   { "Coaxial", eeprom_data::EInput::iCoaxial }
    };

    const std::pair<String, byte> presets[] = {
        { "Preset 1", 1 }
    ,   { "Preset 2", 2 }
    ,   { "Preset 3", 3 }
    ,   { "Preset 4", 4 }
    ,   { "Preset 5", 5 }
    ,   { "Preset 6", 6 }
    ,   { "Preset 7", 7 }
    ,   { "Preset 8", 8 }
    ,   { "Preset 9", 9 }
    ,   { "Preset 10", 11 }
    ,   { "Preset 11", 11 }
    ,   { "Preset 12", 12 }
    ,   { "Preset 13", 13 }
    ,   { "Preset 14", 14 }
    ,   { "Preset 15", 15 }
    ,   { "Preset 16", 16 }
    ,   { "Preset 17", 17 }
    ,   { "Preset 18", 18 }
    ,   { "Preset 19", 19 }
    ,   { "Preset 20", 20 }
    ,   { "Preset 21", 21 }
    ,   { "Preset 22", 22 }
    ,   { "Preset 23", 23 }
    ,   { "Preset 24", 24 }
    ,   { "Preset 25", 25 }
    ,   { "Preset 26", 26 }
    ,   { "Preset 27", 27 }
    ,   { "Preset 28", 28 }
    ,   { "Preset 29", 29 }
    ,   { "Preset 30", 30 }
    ,   { "Preset 31", 31 }
    ,   { "Preset 32", 32 }
    ,   { "Preset 33", 33 }
    ,   { "Preset 34", 34 }
    ,   { "Preset 35", 35 }
    ,   { "Preset 36", 36 }
    ,   { "Preset 37", 37 }
    ,   { "Preset 38", 38 }
    ,   { "Preset 39", 39 }
    ,   { "Preset 40", 40 }
    };

    template<typename T>
    int get_index(const std::pair<String, T> *seek_in, int len, String value)
    {
        for(int n = 0; n < len; ++n)
        {
            if(seek_in[n].first == value) return n;
        }

        return -1;
    }

    template<typename T>
    int get_index(const std::pair<String, T> *seek_in, int len, T value)
    {
        for(int n = 0; n < len; ++n)
        {
            if(seek_in[n].second == value) return n;
        }

        return -1;
    }

    void UI::begin(eeprom_data *settings, audio_signal_detector *AUX_1, audio_signal_detector *AUX_2, i2c_controller *controller, const char* title, const char* username, const char* password, uint16_t port)
    {
        m_ptr_settings = settings;
        m_ptr_controller = controller;
        m_ptr_AUX1 = AUX_1;
        m_ptr_AUX2 = AUX_2;

        build_ui();
        ESPUI.jsonInitialDocumentSize = 18000;
        ESPUI.begin(title, username, password, port);
    }

    void UI::set_time(time_t time)
    {
        m_time = time;
    }

    void UI::set_input(eeprom_data::EInput input)
    {
        int idx = get_index(inputs, sizeof(inputs)/sizeof(inputs[0]), input);
        ESPUI.getControl(ctrl_input_selector)->value = inputs[idx].first;
        ESPUI.updateControl(ESPUI.getControl(ctrl_input_selector));
    }

    void UI::set_AUX1_tolerance(uint16_t tolerance)
    {
        String value = WEB_UI_SIGNAL_TOLERANCE;
        value += tolerance;

        ESPUI.getControl(ctrl_tolerance_AUX_12[0])->value = value;
        ESPUI.updateControl(ESPUI.getControl(ctrl_tolerance_AUX_12[0]));

    }

    void UI::set_AUX2_tolerance(uint16_t tolerance)
    {
        String value = WEB_UI_SIGNAL_TOLERANCE;
        value += tolerance;

        ESPUI.getControl(ctrl_tolerance_AUX_12[1])->value = value;
        ESPUI.updateControl(ESPUI.getControl(ctrl_tolerance_AUX_12[1]));
    }

    void UI::update_status()
    {
        //Time
        char strftime_buf[64];
        struct tm timeinfo;
        localtime_r(&m_time, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%F %T", &timeinfo);

        String time = strftime_buf;
        String input;

        int idx = get_index(inputs, sizeof(inputs)/sizeof(inputs[0]), m_ptr_settings->get_input());
        input = inputs[idx].first;

        m_str_time = 
        "<style>"
        "table, th, td {"
            "border:0px solid black;"
            "text-align: left;"
        "}"
        "</style>"
        "<table>"
        "<tr>"
            "<td>Time: </td>"
            "<td>" + time + "</td>"
        "</tr>"
        "<tr>"
            "<td>Active input: </td>"
            "<td>" + input + "</td>"
        "</tr>"
        "</table>";

        //ESPUI.ui_title = m_str_time.c_str();

        ESPUI.updateControlValue(ctrl_status, m_str_time.c_str());
    }

    void UI::build_ui()
    {
        ctrl_status = ESPUI.addControl(ControlType::Label, WEB_UI_STATUS, "", ControlColor::Dark);

        //-------------------------------------------------------------------------------------------------------------------
        // Sound
        //-------------------------------------------------------------------------------------------------------------------
        ctrl_tab_volume = ESPUI.addControl(ControlType::Tab, WEB_UI_TAB_SOUND, WEB_UI_TAB_SOUND);
        
        ctrl_volume = ESPUI.addControl(ControlType::Slider, WEB_UI_VOLUME, String(m_ptr_settings->get_volume()), ControlColor::Wetasphalt, ctrl_tab_volume,
            [this](Control *sender, int, void*) {
                m_ptr_settings->set_volume(sender->value.toInt());
                m_ptr_controller->volume_set(m_ptr_settings->get_volume());
            }, this);
            ESPUI.addControl(ControlType::Min, WEB_UI_VOLUME, String(WEB_UI_VOLUME_MIN), ControlColor::None, ctrl_volume);
            ESPUI.addControl(ControlType::Max, WEB_UI_VOLUME, String(WEB_UI_VOLUME_MAX), ControlColor::None, ctrl_volume);
        ESPUI.setElementStyle(ctrl_volume, "background: linear-gradient(to right, blue, red);");

        ctrl_volume_min = ESPUI.addControl(ControlType::Button, WEB_UI_VOLUME_MINIMUM, WEB_UI_VOLUME_MINIMUM, ControlColor::Wetasphalt, ctrl_volume,
            [this](Control*, int type, void*) {
                if(B_UP == type) {
                    m_ptr_settings->set_volume(WEB_UI_VOLUME_MIN);
                    m_ptr_controller->volume_set(m_ptr_settings->get_volume());
                    ESPUI.getControl(ctrl_volume)->value = String(m_ptr_settings->get_volume());
                    ESPUI.updateControl(ESPUI.getControl(ctrl_volume));
                }
            }, this); 

        ctrl_volume_down = ESPUI.addControl(ControlType::Button, WEB_UI_VOLUME_DOWN, WEB_UI_VOLUME_DOWN, ControlColor::Wetasphalt, ctrl_volume,
            [this](Control*, int type, void*) {
                if(B_UP == type) {
                    m_ptr_settings->set_volume(WEB_UI_VOLUME_MIN == m_ptr_settings->get_volume() ? WEB_UI_VOLUME_MIN : m_ptr_settings->get_volume()-1);
                    m_ptr_controller->volume_set(m_ptr_settings->get_volume());
                    ESPUI.getControl(ctrl_volume)->value = String(m_ptr_settings->get_volume());
                    ESPUI.updateControl(ESPUI.getControl(ctrl_volume));
                }
            }, this);

        ctrl_volume_up = ESPUI.addControl(ControlType::Button, WEB_UI_VOLUME_UP, WEB_UI_VOLUME_UP, ControlColor::Wetasphalt, ctrl_volume,
            [this](Control*, int type, void*) {
                if(B_UP == type) {
                    m_ptr_settings->set_volume(WEB_UI_VOLUME_MAX == m_ptr_settings->get_volume() ? WEB_UI_VOLUME_MAX : m_ptr_settings->get_volume()+1);
                    m_ptr_controller->volume_set(m_ptr_settings->get_volume());
                    ESPUI.getControl(ctrl_volume)->value = String(m_ptr_settings->get_volume());
                    ESPUI.updateControl(ESPUI.getControl(ctrl_volume));
                }
            }, this);

        ctrl_mute = ESPUI.addControl(ControlType::Switcher, WEB_UI_MUTE, "", ControlColor::Wetasphalt, ctrl_tab_volume,
            [this](Control*, int value, void*) {
                m_ptr_settings->set_muted(S_ACTIVE == value ? true : false);
                S_ACTIVE == value ? m_ptr_controller->volume_mute() : m_ptr_controller->volume_unmute();
            }, this);
        ESPUI.updateControlValue(ctrl_mute, m_ptr_settings->get_muted() ? "1" : "0");

        ctrl_sound_mode = ESPUI.addControl(ControlType::Select, WEB_UI_SOUND_MODE, "", ControlColor::Wetasphalt, ctrl_tab_volume,
            [this](Control *sender, int, void*) {
                int idx = get_index(sound_mode, sizeof(sound_mode)/sizeof(sound_mode[0]), sender->value);
                m_ptr_settings->set_sound_mode(sound_mode[idx].second);
                m_ptr_controller->sound_mode_set(m_ptr_settings->get_sound_mode());
            }, this);
            int cIdx = -1;
            for(int n = 0; n < sizeof(sound_mode)/sizeof(sound_mode[0]); ++n)
            {
                ESPUI.addControl(ControlType::Option, sound_mode[n].first.c_str(), sound_mode[n].first, ControlColor::Wetasphalt, ctrl_sound_mode);
                if(-1 == cIdx && sound_mode[n].second == m_ptr_settings->get_sound_mode()) 
                {
                    cIdx = n;
                }
            }
        ESPUI.updateSelect(ctrl_sound_mode, sound_mode[cIdx].first);
        
        //-------------------------------------------------------------------------------------------------------------------
        // Eq
        //-------------------------------------------------------------------------------------------------------------------
        ctrl_tab_eq = ESPUI.addControl(ControlType::Tab, WEB_UI_TAB_EQ, WEB_UI_TAB_EQ);
        ctrl_bass = ESPUI.addControl(ControlType::Slider, WEB_UI_BASS, String(m_ptr_settings->get_bass()), ControlColor::Wetasphalt, ctrl_tab_eq,
            [this](Control *sender, int, void*) {
                m_ptr_settings->set_bass(sender->value.toInt());
                m_ptr_controller->eq_bass_set(m_ptr_settings->get_bass());
            }, this);
            ESPUI.addControl(ControlType::Min, WEB_UI_BASS, String(WEB_UI_BASS_MIN), ControlColor::None, ctrl_bass);
            ESPUI.addControl(ControlType::Max, WEB_UI_BASS, String(WEB_UI_BASS_MAX), ControlColor::None, ctrl_bass);
        
        ctrl_treble = ESPUI.addControl(ControlType::Slider, WEB_UI_TREBLE, String(m_ptr_settings->get_treble()), ControlColor::Wetasphalt, ctrl_tab_eq,
            [this](Control *sender, int, void*) {
                m_ptr_settings->set_treble(sender->value.toInt());
                m_ptr_controller->eq_treble_set(m_ptr_settings->get_treble());
            }, this);
            ESPUI.addControl(ControlType::Min, WEB_UI_TREBLE, String(WEB_UI_TREBLE_MIN), ControlColor::None, ctrl_treble);
            ESPUI.addControl(ControlType::Max, WEB_UI_TREBLE, String(WEB_UI_TREBLE_MAX), ControlColor::None, ctrl_treble);

        ctrl_eq_mode = ESPUI.addControl(ControlType::Select, WEB_UI_EQ_MODE, "", ControlColor::Wetasphalt, ctrl_tab_eq,
            [this](Control *sender, int, void*) {
                int idx = get_index(eq_modes, sizeof(eq_modes)/sizeof(eq_modes[0]), sender->value);
                m_ptr_settings->set_eq_mode(eq_modes[idx].second);
                m_ptr_controller->eq_mode_set(m_ptr_settings->get_eq_mode());
            }, this);
            cIdx = -1;
            for(int n = 0; n < sizeof(eq_modes)/sizeof(eq_modes[0]); ++n)
            {
                ESPUI.addControl(ControlType::Option, eq_modes[n].first.c_str(), eq_modes[n].first, ControlColor::Wetasphalt, ctrl_eq_mode);
                if(-1 == cIdx && eq_modes[n].second == m_ptr_settings->get_eq_mode()) 
                {
                    cIdx = n;
                }
            }
        ESPUI.updateSelect(ctrl_eq_mode, eq_modes[cIdx].first);

        //-------------------------------------------------------------------------------------------------------------------
        // Radio
        //-------------------------------------------------------------------------------------------------------------------
        ctrl_tab_radio = ESPUI.addControl(ControlType::Tab, WEB_UI_TAB_RADIO, WEB_UI_TAB_RADIO);
        ctrl_radio_presets = ESPUI.addControl(ControlType::Select, WEB_UI_PRESETS, "", ControlColor::Wetasphalt, ctrl_tab_radio,
            [this](Control *sender, int, void*) {
                int idx = get_index(presets, sizeof(presets)/sizeof(presets[0]), sender->value);
                m_ptr_controller->radio_preset_select(presets[idx].second);

                //switch to Radio
                idx = get_index(inputs, sizeof(inputs)/sizeof(inputs[0]), eeprom_data::EInput::iRadio);
                ESPUI.getControl(ctrl_input_selector)->value = inputs[idx].first;
                ESPUI.updateControl(ESPUI.getControl(ctrl_input_selector));

            }, this);
            for(int n = 0; n < sizeof(presets)/sizeof(presets[1]); ++n) {
                ESPUI.addControl(ControlType::Option, presets[n].first.c_str(), presets[n].first, ControlColor::Wetasphalt, ctrl_radio_presets);
            }
            ESPUI.getControl(ctrl_radio_presets)->value = presets[0].first;
            ESPUI.updateControl(ctrl_radio_presets);

        ctrl_radio_prev_preset = ESPUI.addControl(ControlType::Button, WEB_UI_PRV_PRESET, WEB_UI_PRV_PRESET, ControlColor::Wetasphalt, ctrl_radio_presets,
            [this](Control*, int type, void*) {
                if(B_UP == type) {
                    String cur_preset = ESPUI.getControl(ctrl_radio_presets)->value;
                    int idx = get_index(presets, sizeof(presets)/sizeof(presets[0]), cur_preset);
                    if(0 != idx)
                    {
                        idx--;
                        ESPUI.getControl(ctrl_radio_presets)->value = presets[idx].first;
                        ESPUI.updateControl(ctrl_radio_presets);

                        m_ptr_controller->radio_preset_select(presets[idx].second);

                        //switch to Radio
                        idx = get_index(inputs, sizeof(inputs)/sizeof(inputs[0]), eeprom_data::EInput::iRadio);
                        ESPUI.getControl(ctrl_input_selector)->value = inputs[idx].first;
                        ESPUI.updateControl(ESPUI.getControl(ctrl_input_selector));
                    }
                }
            }, this); 

        ctrl_radio_next_preset = ESPUI.addControl(ControlType::Button, WEB_UI_NXT_PRESET, WEB_UI_NXT_PRESET, ControlColor::Wetasphalt, ctrl_radio_presets,
            [this](Control*, int type, void*) {
                if(B_UP == type) {
                    String cur_preset = ESPUI.getControl(ctrl_radio_presets)->value;
                    int idx = get_index(presets, sizeof(presets)/sizeof(presets[0]), cur_preset);
                    if(idx < sizeof(presets)/sizeof(presets[0]) - 1)
                    {
                        idx++;
                        ESPUI.getControl(ctrl_radio_presets)->value = presets[idx].first;
                        ESPUI.updateControl(ctrl_radio_presets);

                        m_ptr_controller->radio_preset_select(presets[idx].second);

                        //switch to Radio
                        idx = get_index(inputs, sizeof(inputs)/sizeof(inputs[0]), eeprom_data::EInput::iRadio);
                        ESPUI.getControl(ctrl_input_selector)->value = inputs[idx].first;
                        ESPUI.updateControl(ESPUI.getControl(ctrl_input_selector));
                    }                    
                }
            }, this); 

         ctrl_radio_full_rescan = ESPUI.addControl(ControlType::Button, WEB_UI_RESCAN_ALL, WEB_UI_RESCAN_ALL, ControlColor::Wetasphalt, ctrl_tab_radio,
            [this](Control*, int type, void*) {
                if(B_UP == type) {
                    m_ptr_controller->radio_full_rescan();
                    //switch to Radio
                    int idx = get_index(inputs, sizeof(inputs)/sizeof(inputs[0]), eeprom_data::EInput::iRadio);
                    ESPUI.getControl(ctrl_input_selector)->value = inputs[idx].first;
                    ESPUI.updateControl(ESPUI.getControl(ctrl_input_selector));
                }
            }, this); 

        //-------------------------------------------------------------------------------------------------------------------
        // Inputs
        //-------------------------------------------------------------------------------------------------------------------
        ctrl_tab_input = ESPUI.addControl(ControlType::Tab, WEB_UI_TAB_INPUT, WEB_UI_TAB_INPUT);
        ctrl_input_selector = ESPUI.addControl(ControlType::Select, WEB_UI_INPUT_SEL, "", ControlColor::Wetasphalt, ctrl_tab_input,
            [this](Control *sender, int, void*) {
                int idx = get_index(inputs, sizeof(inputs)/sizeof(inputs[0]), sender->value);
                m_ptr_settings->set_input(inputs[idx].second);
                m_ptr_controller->input_set(m_ptr_settings->get_input());
                update_status();
            }, this);
        cIdx = -1;
        for(int n = 0; n < sizeof(inputs)/sizeof(inputs[0]); ++n)
        {
            ESPUI.addControl(ControlType::Option, inputs[n].first.c_str(), inputs[n].first, ControlColor::Wetasphalt, ctrl_input_selector);
            if(-1 == cIdx && inputs[n].second == m_ptr_settings->get_input()) 
            {
                cIdx = n;
            }
        }
        ESPUI.updateSelect(ctrl_input_selector, inputs[cIdx].first);

        ctrl_audio_sync = ESPUI.addControl(ControlType::Slider, WEB_UI_ASYNC, String(m_ptr_settings->get_audio_sync()-1), ControlColor::Wetasphalt, ctrl_tab_input,
            [this](Control *sender, int, void*) {
                m_ptr_settings->set_audio_sync(sender->value.toInt()+1);
                m_ptr_controller->audio_sync_set(m_ptr_settings->get_audio_sync());
            }, this);
            ESPUI.addControl(ControlType::Min, WEB_UI_ASYNC, String(WEB_UI_ASYNC_MIN), ControlColor::None, ctrl_audio_sync);
            ESPUI.addControl(ControlType::Max, WEB_UI_ASYNC, String(WEB_UI_ASYNC_MAX), ControlColor::None, ctrl_audio_sync);

        //-------------------------------------------------------------------------------------------------------------------
        // Settings
        //-------------------------------------------------------------------------------------------------------------------
        ctrl_tab_settings = ESPUI.addControl(ControlType::Tab, WEB_UI_TAB_SETTINGS, WEB_UI_TAB_SETTINGS);

        ctrl_wifi_ap = ESPUI.addControl(ControlType::Text, WEB_UI_AP, m_ptr_settings->get_wifi_ap(), ControlColor::Wetasphalt, ctrl_tab_settings,
            [](Control*, int, void*) {

            }, this);

        ctrl_wifi_ap_pass = ESPUI.addControl(ControlType::Text, WEB_UI_AP_PASSW, m_ptr_settings->get_wifi_pass(), ControlColor::Wetasphalt, ctrl_wifi_ap,
            [](Control*, int, void*) {

            }, this);

        ctrl_ntp_server = ESPUI.addControl(ControlType::Text, WEB_UI_NTP_SERVER, m_ptr_settings->get_ntp_server(), ControlColor::Wetasphalt, ctrl_tab_settings,
            [](Control*, int, void*) {

            }, this);

        ctrl_ntp_zone = ESPUI.addControl(ControlType::Text, WEB_UI_NTP_ZONE, m_ptr_settings->get_ntp_zone(), ControlColor::Wetasphalt, ctrl_ntp_server,
            [](Control*, int, void*) {

            }, this);

        ctrl_save = ESPUI.addControl(ControlType::Button, WEB_UI_SAVE, WEB_UI_SAVE, ControlColor::Wetasphalt, ctrl_tab_settings,
            [this](Control*, int type, void*) {
                if(B_UP == type) {
                    m_ptr_settings->set_wifi_ap(ESPUI.getControl(ctrl_wifi_ap)->value);
                    m_ptr_settings->set_wifi_pass(ESPUI.getControl(ctrl_wifi_ap_pass)->value);
                    m_ptr_settings->set_ntp_server(ESPUI.getControl(ctrl_ntp_server)->value);
                    m_ptr_settings->set_ntp_zone(ESPUI.getControl(ctrl_ntp_zone)->value);
                    HTSNewMaster::save(*m_ptr_settings);
                }
            }, this); 

        ctrl_unpair = ESPUI.addControl(ControlType::Button, WEB_UI_UNPAIR, WEB_UI_UNPAIR, ControlColor::Wetasphalt, ctrl_tab_settings,
            [](Control*, int type, void*) {
                if(B_UP == type) {
                    digitalWrite(BT_UNPAIR_PIN, HIGH);
                    delay(800);
                    digitalWrite(BT_UNPAIR_PIN, LOW);
                }
            }, this);

        //...

        for(int n = 0; n < 2; ++n)
        {
            using EDPriority = eeprom_data::EDPriority;

            EDPriority sd { 0 == n ? EDPriority::pAUX1 : EDPriority::pAUX2 };
            //...............................................................
            // Audio Detector AUX x
            //...............................................................

            std::function<const eeprom_data::signal_detector &()> get_data { EDPriority::pAUX1 == sd
                ? std::bind(&eeprom_data::get_AUX_1, m_ptr_settings)
                : std::bind(&eeprom_data::get_AUX_2, m_ptr_settings) };

            std::function<void(const eeprom_data::signal_detector &)> set_data {EDPriority::pAUX1 == sd
                ? std::bind(&eeprom_data::set_AUX_1, m_ptr_settings, std::placeholders::_1)
                : std::bind(&eeprom_data::set_AUX_2, m_ptr_settings, std::placeholders::_1) };

            int group_ag_aux = ESPUI.addControl(ControlType::Switcher
                , EDPriority::pAUX1 == sd ? WEB_UI_AD_AUX1 : WEB_UI_AD_AUX2, ""
                , EDPriority::pAUX1 == sd ? ControlColor::Peterriver : ControlColor::Turquoise, ctrl_tab_settings
                , [this, get_data, set_data, sd](Control*, int value, void*) {
                    auto ads = get_data();
                    ads.enabled = S_ACTIVE == value;
                    set_data(ads);

                    switch(sd)
                    {
                        case EDPriority::pAUX1:
                            start_signal_detector(*m_ptr_AUX1, m_ptr_settings->get_AUX_1());
                            break;

                        case EDPriority::pAUX2:
                            start_signal_detector(*m_ptr_AUX2, m_ptr_settings->get_AUX_2());
                            break;
                    }
                }, this);
            ESPUI.updateControlValue(group_ag_aux, get_data().enabled ? "1" : "0");
            ESPUI.setElementStyle(ESPUI.addControl(Label, "", WEB_UI_AD_ENABLED, None, group_ag_aux),  "background-color: unset; width: 100%;");

            int ctrl_slider_sig_timeout = ESPUI.addControl(ControlType::Slider, "", String(get_data().signal_timeout_s), ControlColor::None, group_ag_aux
                , [this, get_data, set_data, sd](Control *sender, int type, void*)  {
                    auto ads = get_data();
                    ads.signal_timeout_s = sender->value.toInt();
                    set_data(ads);

                    switch(sd)
                    {
                        case EDPriority::pAUX1:
                            start_signal_detector(*m_ptr_AUX1, m_ptr_settings->get_AUX_1());
                            break;

                        case EDPriority::pAUX2:
                            start_signal_detector(*m_ptr_AUX2, m_ptr_settings->get_AUX_2());
                            break;
                    }
                }, this);
            ESPUI.addControl(ControlType::Min, "", String(WEB_UI_SIG_TIMEOUT_MIN), ControlColor::None, ctrl_slider_sig_timeout);
            ESPUI.addControl(ControlType::Max, "", String(WEB_UI_SIG_TIMEOUT_MAX), ControlColor::None, ctrl_slider_sig_timeout);
            ESPUI.setElementStyle(ESPUI.addControl(Label, "", WEB_UI_SIG_TIMEOUT, ControlColor::None, group_ag_aux),  "background-color: unset; width: 100%;");

            int ctrl_slider_sil_timeout = ESPUI.addControl(ControlType::Slider, "", String(get_data().silience_timeout_s), ControlColor::None, group_ag_aux
                , [this, get_data, set_data, sd](Control *sender, int type, void*) 
                {
                    auto ads = get_data();
                    ads.silience_timeout_s = sender->value.toInt();
                    set_data(ads);

                    switch(sd)
                    {
                        case EDPriority::pAUX1:
                            start_signal_detector(*m_ptr_AUX1, m_ptr_settings->get_AUX_1());
                            break;

                        case EDPriority::pAUX2:
                            start_signal_detector(*m_ptr_AUX2, m_ptr_settings->get_AUX_2());
                            break;
                    }
                }, this);
            ESPUI.addControl(ControlType::Min, "", String(WEB_UI_SIL_TIMEOUT_MIN), ControlColor::None, ctrl_slider_sil_timeout);
            ESPUI.addControl(ControlType::Max, "", String(WEB_UI_SIL_TIMEOUT_MAX), ControlColor::None, ctrl_slider_sil_timeout);
            ESPUI.setElementStyle(ESPUI.addControl(Label, "", WEB_UI_SIL_TIMEOUT, ControlColor::None, group_ag_aux),  "background-color: unset; width: 100%;");

            int ctrl_zero_level = ESPUI.addControl(ControlType::Text, "", String(get_data().zero_level), ControlColor::None, group_ag_aux
                , [this, get_data, set_data, sd](Control *sender, int type, void*) 
                {
                    auto ads = get_data();
                    ads.zero_level = sender->value.toInt();
                    set_data(ads);

                    switch(sd)
                    {
                        case EDPriority::pAUX1:
                            start_signal_detector(*m_ptr_AUX1, m_ptr_settings->get_AUX_1());
                            break;

                        case EDPriority::pAUX2:
                            start_signal_detector(*m_ptr_AUX2, m_ptr_settings->get_AUX_2());
                            break;
                    }
                }, this);
            ESPUI.setElementStyle(ESPUI.addControl(Label, "", WEB_UI_ZERO_LEVEL, ControlColor::None, group_ag_aux),  "background-color: unset; width: 100%;");

            ESPUI.addControl(ControlType::Button, "", WEB_UI_CALIBRATE_ZERO_LEVEL, ControlColor::None, group_ag_aux
                , [this, sd, ctrl_zero_level](Control*, int, void*) 
                {
                    switch(sd)
                    {
                        case EDPriority::pAUX1:
                        {
                            m_ptr_AUX1->silience_calibrate(tools::timer::sec_to_msec(10), [this, ctrl_zero_level](uint16_t sig)
                                {
                                    ESPUI.updateControlValue(ctrl_zero_level, String(sig));
                                    
                                    auto ads = m_ptr_settings->get_AUX_1();
                                    ads.zero_level = sig;
                                    m_ptr_settings->set_AUX_1(ads);

                                    start_signal_detector(*m_ptr_AUX1, m_ptr_settings->get_AUX_1());
                                });
                        } break;

                        case EDPriority::pAUX2:
                        {
                            m_ptr_AUX2->silience_calibrate(tools::timer::sec_to_msec(10), [this, ctrl_zero_level](uint16_t sig)
                                {
                                    ESPUI.updateControlValue(ctrl_zero_level, String(sig));

                                    auto ads = m_ptr_settings->get_AUX_2();
                                    ads.zero_level = sig;
                                    m_ptr_settings->set_AUX_2(ads);

                                    start_signal_detector(*m_ptr_AUX2, m_ptr_settings->get_AUX_2());
                                });
                        } break;
                    }
                }, this);

            auto ctrl_slider_sensivity = ESPUI.addControl(ControlType::Slider, "", String(get_data().sensivity), ControlColor::None, group_ag_aux
                , [this, get_data, set_data, sd](Control *sender, int type, void*) 
                {
                    auto ads = get_data();
                    ads.sensivity = sender->value.toInt();
                    set_data(ads);

                    switch(sd)
                    {
                        case EDPriority::pAUX1:
                            start_signal_detector(*m_ptr_AUX1, m_ptr_settings->get_AUX_1());
                            break;

                        case EDPriority::pAUX2:
                            start_signal_detector(*m_ptr_AUX2, m_ptr_settings->get_AUX_2());
                            break;
                    }
                }, this);
            ESPUI.addControl(ControlType::Min, "", String(WEB_UI_SENSITIVITY_MIN), ControlColor::None, ctrl_slider_sensivity);
            ESPUI.addControl(ControlType::Max, "", String(WEB_UI_SENSITIVITY_MAX), ControlColor::None, ctrl_slider_sensivity);
            ESPUI.setElementStyle(ESPUI.addControl(Label, "", WEB_UI_SENSITIVITY, ControlColor::None, group_ag_aux),  "background-color: unset; width: 100%;");

            auto ctrl_slider_sig_tolerance = ESPUI.addControl(ControlType::Slider, "", String(get_data().signal_tolerance_precent), ControlColor::None, group_ag_aux
                , [this, get_data, set_data, sd](Control *sender, int type, void*) 
                {
                    auto ads = get_data();
                    ads.signal_tolerance_precent = sender->value.toInt();
                    set_data(ads);

                    switch(sd)
                    {
                        case EDPriority::pAUX1:
                            start_signal_detector(*m_ptr_AUX1, m_ptr_settings->get_AUX_1());
                            break;

                        case EDPriority::pAUX2:
                            start_signal_detector(*m_ptr_AUX2, m_ptr_settings->get_AUX_2());
                            break;
                    }
                }, this);
            ESPUI.addControl(ControlType::Min, "", String(WEB_UI_TOLERANCE_MIN), ControlColor::None, ctrl_slider_sig_tolerance);
            ESPUI.addControl(ControlType::Max, "", String(WEB_UI_TOLERANCE_MAX), ControlColor::None, ctrl_slider_sig_tolerance);
            ESPUI.setElementStyle(ESPUI.addControl(Label, "", WEB_UI_TOLERANCE, ControlColor::None, group_ag_aux),  "background-color: unset; width: 100%;");

            int ctrl_slider_measure_interval = ESPUI.addControl(ControlType::Slider, "", String(get_data().measure_interval_ms), ControlColor::None, group_ag_aux, 
                [this, get_data, set_data, sd](Control *sender, int type, void*) 
                {
                    auto ads = get_data();
                    ads.measure_interval_ms = sender->value.toInt();
                    set_data(ads);

                    switch(sd)
                    {
                        case EDPriority::pAUX1:
                            start_signal_detector(*m_ptr_AUX1, m_ptr_settings->get_AUX_1());
                            break;

                        case EDPriority::pAUX2:
                            start_signal_detector(*m_ptr_AUX2, m_ptr_settings->get_AUX_2());
                            break;
                    }
                }, this);
            ESPUI.addControl(ControlType::Min, "", String(WEB_UI_MEASURE_INTERVAL_MIN), ControlColor::None, ctrl_slider_measure_interval);
            ESPUI.addControl(ControlType::Max, "", String(WEB_UI_MEASURE_INTERVAL_MAX), ControlColor::None, ctrl_slider_measure_interval);
            ESPUI.setElementStyle(ESPUI.addControl(Label, "", WEB_UI_MEASURE_INTERVAL, ControlColor::None, group_ag_aux), "background-color: unset; width: 100%;");
            
            int ctrl_id = ESPUI.addControl(Label, "", WEB_UI_SIGNAL_TOLERANCE, ControlColor::None, group_ag_aux);
            ESPUI.setElementStyle(ctrl_id, "background-color: unset; width: 100%;");

            switch(sd)
            {
                case EDPriority::pAUX1:
                    ctrl_tolerance_AUX_12[0] = ctrl_id;
                    break;

                case EDPriority::pAUX2:
                    ctrl_tolerance_AUX_12[1] = ctrl_id;
                    break;
            }           
        
        } // for

        ESPUI.addControl(ControlType::Switcher, WEB_UI_SD_PRIORITY, String((int)m_ptr_settings->get_signal_detector_priority()), ControlColor::Wetasphalt, ctrl_tab_settings
                , [this](Control*, int value, void*) {
                    m_ptr_settings->set_signal_detector_priority(S_ACTIVE == value ? eeprom_data::EDPriority::pAUX2 : eeprom_data::EDPriority::pAUX1);
                }, this);

       ctrl_input_off_timer_from = ESPUI.addControl(ControlType::Text, WEB_UI_INP_DIS_TT, "", ControlColor::Turquoise, ctrl_tab_settings,
            [this](Control *sender, int, void*) {
                m_ptr_settings->set_disable_input_from(ESPUI.getControl(ctrl_input_off_timer_from)->value);
            }, this);
        ESPUI.setInputType(ctrl_input_off_timer_from, "time");
        ESPUI.updateControlValue(ctrl_input_off_timer_from, m_ptr_settings->get_disable_input_from());

        ctrl_input_off_timer_to = ESPUI.addControl(ControlType::Text, WEB_UI_INP_DIS_TT, "", ControlColor::Turquoise, ctrl_input_off_timer_from,
            [this](Control *sender, int, void*) {
                m_ptr_settings->set_disable_input_to(ESPUI.getControl(ctrl_input_off_timer_to)->value);
            }, this);
        ESPUI.setInputType(ctrl_input_off_timer_to, "time");
        ESPUI.updateControlValue(ctrl_input_off_timer_to, m_ptr_settings->get_disable_input_to());

        ctrl_input_off_switch = ESPUI.addControl(ControlType::Switcher, WEB_UI_INP_DISABLE, "", ControlColor::Turquoise, ctrl_input_off_timer_from,
            [this](Control*, int value, void*) {
                m_ptr_settings->set_disable_input_active(S_ACTIVE == value ? true : false);
            }, this);
        ESPUI.updateControlValue(ctrl_input_off_switch, m_ptr_settings->get_disable_input_active() ? "1" : "0");

        //...
        
        ctrl_i2c_test = ESPUI.addControl(ControlType::Text, WEB_UI_I2C_TEST, "C:00000000,D:00000000", ControlColor::Emerald, ctrl_tab_settings,
            [](Control*, int type, void*) {
                
            }, this);

        ctrl_i2c_test_send = ESPUI.addControl(ControlType::Button, WEB_UI_I2C_TEST_SND, WEB_UI_I2C_TEST_SND, ControlColor::Sunflower, ctrl_i2c_test,
            [this](Control*, int type, void*) {
                if(B_UP == type) 
                {
                    bool success = true;
                    String text = ESPUI.getControl(ctrl_i2c_test)->value;
                    if('C' == text[0] && ':' == text[1])
                    {
                        byte cmd = 0;
                        for(int n = 2, k = 7; n < 2+8; ++n, --k)
                        {
                            char c = text[n];
                            if('1' != c && '0' != c)
                            {
                                success = false;
                                goto l_exit;                            
                            }

                            c -= '0';
                            cmd += c * (1 << k);
                        }
                        
                        byte val = 0;
                        if('D' == text[11] && ':' == text[12])
                        {
                            for(int n = 13, k = 7; n < 13+8; ++n, --k)
                            {
                                char c = text[n];
                                if('1' != c && '0' != c)
                                {
                                    success = false;
                                    goto l_exit;                           
                                }

                                c -= '0';
                                val += c * (1 << k);
                            }

                            m_ptr_controller->custom_commend(cmd, val);
                        }
                        else
                        {
                            success = false;
                            goto l_exit;    
                        }
                    }
                    else
                    {
                        success = false;
                        goto l_exit;    
                    }
                
                l_exit:
                    if(success)
                    {
                        ESPUI.getControl(ctrl_i2c_test)->color = ControlColor::Emerald;
                    }
                    else
                    {
                        ESPUI.getControl(ctrl_i2c_test)->color = ControlColor::Dark;
                    }

                    ESPUI.updateControl(ctrl_i2c_test);
                }
            }, this);


        ctrl_reboot = ESPUI.addControl(ControlType::Button, WEB_UI_REBOOT, WEB_UI_REBOOT, ControlColor::Carrot, ctrl_tab_settings,
            [](Control*, int type, void*) {
                if(B_UP == type) {
                    ESP.restart();
                }
            }, this);
    }
}