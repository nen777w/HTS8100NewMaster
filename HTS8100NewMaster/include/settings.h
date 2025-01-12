#ifndef __settings_h__
#define __settings_h__

#include <Arduino.h>
#include <tuple>

#include "constants.h"

namespace HTSNewMaster
{
    const byte current_version = 7;
    const int EEPROM_BUF_SIZE = 1024;

    struct eeprom_data;

    bool load(eeprom_data &data);
    bool save(const eeprom_data &data);
    void set_eeprom_data_dirty();

    struct eeprom_data
    {
        struct signal_detector
        {
            bool enabled;
            unsigned long signal_timeout_s;
            unsigned long silience_timeout_s;
            uint16_t zero_level;
            uint16_t sensivity;
            uint16_t signal_tolerance_precent;
            unsigned long measure_interval_ms;
        };

        enum class ESoundMode : byte { 
              smAutoStereo = 0b00000001
            , smMultiCh = 0b00000010
            , smStereo = 0b00000011
        };
        enum class EEQMode : byte { 
            eqClassic = 0b00000001
          , eqJazz = 0b00000010
          , eqRock = 0b00000011
          , eqRnB = 0b00000100
          , eqParty = 0b00000101
          , eqLounge = 0b00000110
          , eqSports = 0b00000111
          , eqNews = 0b00001000 
        };
        enum class EInput : byte { 
            iDisabled = 0b00000000
          , iCoaxial = 0b00000010
          , iRadio = 0b00000001
          , iAUX1 = 0b00000100
          , iAUX2 = 0b00000101
        };
        enum class EDPriority : byte {
            pAUX1 = 0, pAUX2 = 1
        };

        static void print(const eeprom_data& data);
        static String enum_to_string(ESoundMode mode);
        static String enum_to_string(EEQMode mode);
        static String enum_to_string(EInput mode);
        static String enum_to_string(EDPriority mode);

    private:
        char wifi_ap[25] = {'\0'};
        char wifi_pass[25] = {'\0'};
        
        ESoundMode sound_mode = { ESoundMode::smStereo };

        byte volume = 3;
        byte muted = 1;

        byte bass;
        byte treble;
        EEQMode eq_mode = { EEQMode::eqClassic };
        
        EInput input = { EInput::iDisabled };

        byte audio_sync = 0;

        char ntp_server[35] = {"europe.pool.ntp.org"};
        char ntp_zone[10] = {"UTC-0"};

        char disable_input_from[6] = {"01:00"};
        char disable_input_to[6] = {"06:00"};
        byte disable_input_active = 0;

        signal_detector AUX_1 {
              .enabled = true
            , .signal_timeout_s = 2
            , .silience_timeout_s = 35
            , .zero_level = 1864
            , .sensivity = 15
            , .signal_tolerance_precent = 17
            , .measure_interval_ms = 10
        };

        signal_detector AUX_2 {
              .enabled = true
            , .signal_timeout_s = 2
            , .silience_timeout_s = 35
            , .zero_level = 1864
            , .sensivity = 15
            , .signal_tolerance_precent = 17
            , .measure_interval_ms = 10
        };
        
        EDPriority m_signal_detector_priority = EDPriority::pAUX1;
    
    public:
        const char* get_wifi_ap() const { return wifi_ap; }
        void set_wifi_ap(String value) {
          memcpy(wifi_ap, value.c_str(), std::min(sizeof(wifi_ap)-1, value.length()));
          set_eeprom_data_dirty();
        }

        const char* get_wifi_pass() const { return wifi_pass; }
        void set_wifi_pass(String value) {
          memcpy(wifi_pass, value.c_str(), std::min(sizeof(wifi_pass)-1, value.length()));
          set_eeprom_data_dirty();
        }

        ESoundMode get_sound_mode() const { return sound_mode; }
        void set_sound_mode(ESoundMode value) {
          sound_mode = value;
          set_eeprom_data_dirty();
        }

        byte get_volume() const { return volume; }
        void set_volume(byte value) { 
          volume = value;
          set_eeprom_data_dirty();
        }

        byte get_muted() const { return muted; }
        void set_muted(byte value) { 
          muted = value;
          set_eeprom_data_dirty();
        }

        byte get_bass() const { return bass; }
        void set_bass(byte value) { 
          bass = value;
          set_eeprom_data_dirty();
        }

        byte get_treble() const { return treble; }
        void set_treble(byte value) { 
          treble = value;
          set_eeprom_data_dirty();
        }

        EEQMode get_eq_mode() const { return eq_mode; }
        void set_eq_mode(EEQMode value) {
          eq_mode = value;
          set_eeprom_data_dirty();
        }

        EInput get_input() const { return input; }
        void set_input(EInput value) {
          input = value;
          set_eeprom_data_dirty();
        }

        byte get_audio_sync() const { return audio_sync; }
        void set_audio_sync(byte value) { 
          audio_sync = value;
          set_eeprom_data_dirty();
        }

        const char* get_ntp_server() const { return ntp_server; }
        void set_ntp_server(String value) {
          memcpy(ntp_server, value.c_str(), std::min(sizeof(ntp_server)-1, value.length()));
          set_eeprom_data_dirty();
        }

        const char* get_ntp_zone() const { return ntp_zone; }
        void set_ntp_zone(String value) {
          memcpy(ntp_zone, value.c_str(), std::min(sizeof(ntp_zone)-1, value.length()));
          set_eeprom_data_dirty();
        }

        const char* get_disable_input_from() const { return disable_input_from; }
        void set_disable_input_from(String value) {
          memcpy(disable_input_from, value.c_str(), std::min(sizeof(disable_input_from)-1, value.length()));
          set_eeprom_data_dirty();
        }

        const char* get_disable_input_to() const { return disable_input_to; }
        void set_disable_input_to(String value) {
          memcpy(disable_input_to, value.c_str(), std::min(sizeof(disable_input_to)-1, value.length()));
          set_eeprom_data_dirty();
        }

        byte get_disable_input_active() const { return disable_input_active; }
        void set_disable_input_active(byte value) { 
          disable_input_active = value;
          set_eeprom_data_dirty();
        }

        std::tuple<int, int> get_disable_input_from_tpl() const
        {
            int h = (disable_input_from[1] - '0') + (disable_input_from[0] - '0') * 10;
            int m = (disable_input_from[4] - '0') + (disable_input_from[3] - '0') * 10;
            return {h, m};
        }

        std::tuple<int, int> get_disable_input_to_tpl() const
        {
            int h = (disable_input_to[1] - '0') + (disable_input_to[0] - '0') * 10;
            int m = (disable_input_to[4] - '0') + (disable_input_to[3] - '0') * 10;
            return {h, m};
        }

        const signal_detector & get_AUX_1() const
        {
            return AUX_1;
        }

        void set_AUX_1(const signal_detector & data)
        {
            AUX_1 = data;
            set_eeprom_data_dirty();
        }

        const signal_detector & get_AUX_2() const
        {
            return AUX_2;
        }

        void set_AUX_2(const signal_detector & data)
        {
            AUX_2 = data;
            set_eeprom_data_dirty();
        }

        EDPriority get_signal_detector_priority() const 
        {
            return m_signal_detector_priority;
        }

        void set_signal_detector_priority(EDPriority priority)
        {
            m_signal_detector_priority = priority;
            set_eeprom_data_dirty();
        }
        
    };
}

#endif