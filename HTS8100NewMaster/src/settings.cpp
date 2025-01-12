#include "settings.h"
//EEPROM
#include <EEPROM.h>
//debug
#include "dbg_msg_macro.h"

//dirty flag
bool eeprom_data_dirty = false;

namespace HTSNewMaster
{
    void eeprom_data::print(const eeprom_data& data)
    {
        DBGNL(DBG_DEBUG, ("---------------------------------------------"));
        DBGNL(DBG_DEBUG, ("Settings"));
        DBGNL(DBG_DEBUG, ("---------------------------------------------"));

        DBGNL(DBG_DEBUG, ("WiFi AP: %s")(data.wifi_ap));
        DBGNL(DBG_DEBUG, ("WiFi Pass: %s")(data.wifi_pass));
        DBGNL(DBG_DEBUG, ("Sound mode: %s")(enum_to_string(data.sound_mode)));
        DBGNL(DBG_DEBUG, ("Volume: %d")(data.volume));
        DBGNL(DBG_DEBUG, ("Muted: %d")(data.muted));
        DBGNL(DBG_DEBUG, ("Bass: %d")(data.bass));
        DBGNL(DBG_DEBUG, ("Treble: %d")(data.treble));
        DBGNL(DBG_DEBUG, ("EQ Mode: %s")(enum_to_string(data.eq_mode)));
        DBGNL(DBG_DEBUG, ("Input: %s")(enum_to_string(data.input)));
        DBGNL(DBG_DEBUG, ("Audio Sync: %d")(data.audio_sync));
        DBGNL(DBG_DEBUG, ("NTP Server: %s")(data.ntp_server));
        DBGNL(DBG_DEBUG, ("NTP Zone: %s")(data.ntp_zone));
        DBGNL(DBG_DEBUG, ("Disable Input From: %s")(data.disable_input_from));
        DBGNL(DBG_DEBUG, ("Disable Input To: %s")(data.disable_input_to));
        DBGNL(DBG_DEBUG, ("Disable Input: %d")(data.disable_input_active));

        const signal_detector * p_sd = nullptr;
        for(int n = 0; n < 2; ++n)
        {
            switch(n)
            {
                case 0: p_sd = &data.AUX_1; break;
                case 1: p_sd = &data.AUX_2; break;
            }

            DBGNL(DBG_DEBUG, ("............................................."));
            DBGNL(DBG_DEBUG, ("AUX%d, Enabled: %d")(n+1)(p_sd->enabled));
            DBGNL(DBG_DEBUG, ("AUX%d, Signal Timeout: %d s")(n+1)(p_sd->signal_timeout_s));
            DBGNL(DBG_DEBUG, ("AUX%d, Silience Timeout: %d s")(n+1)(p_sd->silience_timeout_s));
            DBGNL(DBG_DEBUG, ("AUX%d, Zero Level: %d")(n+1)(p_sd->zero_level));
            DBGNL(DBG_DEBUG, ("AUX%d, Sensivity: %d")(n+1)(p_sd->sensivity));
            DBGNL(DBG_DEBUG, ("AUX%d, Signal Tolerance: %d %")(n+1)(p_sd->signal_tolerance_precent));
            DBGNL(DBG_DEBUG, ("AUX%d, Measure Interval: %d ms")(n+1)(p_sd->measure_interval_ms));
            DBGNL(DBG_DEBUG, ("............................................."));
        }

        DBGNL(DBG_DEBUG, ("Signal Detector Priority: %s")(enum_to_string(data.m_signal_detector_priority)));

        DBGNL(DBG_DEBUG, ("---------------------------------------------"));
    }

    String eeprom_data::enum_to_string(ESoundMode mode)
    {
        switch(mode)
        {
        case ESoundMode::smAutoStereo:
            return "Auto Stereo";
        case ESoundMode::smMultiCh:
            return "Multi Channel";
        case ESoundMode::smStereo:
            return "Stereo";
        }

        return "Unknown";
    }

    String eeprom_data::enum_to_string(EEQMode mode)
    {
        switch(mode)
        {
        case EEQMode::eqClassic:
            return "Classic";
        case EEQMode::eqJazz:
            return "Jazz";
        case EEQMode::eqRock:
            return "Rock";
        case EEQMode::eqRnB:
            return "RnB";
        case EEQMode::eqParty:
            return "Party";
        case EEQMode::eqLounge:
            return "Lounge";
        case EEQMode::eqSports:
            return "Sports";
        case EEQMode::eqNews:
            return "News";
        }

        return "Unknown";
    }

    String eeprom_data::enum_to_string(EInput mode)
    {
        switch(mode)
        {
        case EInput::iDisabled:
            return "Off";
        case EInput::iCoaxial:
            return "Coaxial";
        case EInput::iRadio:
            return "Radio";
        case EInput::iAUX1:
            return "AUX1";
        case EInput::iAUX2:
            return "AUX2";
        }

        return "Unknown";
    }

    String eeprom_data::enum_to_string(EDPriority mode)
    {
        switch(mode)
        {
        case EDPriority::pAUX1:
            return "AUX1";
        case EDPriority::pAUX2:
            return "AUX2";
        }

        return "Unknown";
    }

    bool load(eeprom_data &data)
    {
        if(!EEPROM.begin(EEPROM_BUF_SIZE)) {
            return false;
        }

        byte *p = reinterpret_cast<byte*>(&data);

        byte version = EEPROM.read(0);

        DBGNL(DBG_DEBUG, ("EEPROM Ver: %d, Current Ver: %d, EEPROM Len: %d")(current_version)(version)(EEPROM.length()));

        if(current_version != version) {
            return false;
        }

        for(int n = 1; n <= sizeof(eeprom_data) + 1; ++n)
        {
            p[n-1] = EEPROM.read(n);
        }

        EEPROM.end();

        return true;
    }

    bool save(const eeprom_data &data)
    {
        if(!eeprom_data_dirty) return false;

        if(!EEPROM.begin(EEPROM_BUF_SIZE)) {
            return false;
        }

        const byte *p = reinterpret_cast<const byte*>(&data);

        EEPROM.write(0, current_version);

        for(int n = 1; n < sizeof(eeprom_data) + 1; ++n)
        {
            EEPROM.write(n, p[n-1]);
        }

        EEPROM.commit();
        EEPROM.end();

        eeprom_data_dirty = false;

        return true;
    }

    void set_eeprom_data_dirty()
    {
        eeprom_data_dirty = true;
    }
}