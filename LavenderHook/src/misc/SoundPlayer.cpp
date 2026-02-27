#include "SoundPlayer.h"
#include <windows.h>
#include <vector>
#define MINIAUDIO_IMPLEMENTATION
#include "../miniaudio/miniaudio.h"
#include "../assets/resources/resource.h"
#include "../misc/Globals.h"

namespace LavenderHook {
    namespace Audio {

        struct SoundData {
            std::vector<uint8_t> raw; // full WAV file bytes
        };

        static ma_engine s_engine;
        static bool s_engineInited = false;
        static ma_sound s_soundOn;
        static ma_sound s_soundOff;
        static ma_sound s_soundFail;
        static ma_sound s_soundHide;
        static ma_decoder s_decoderOn;
        static ma_decoder s_decoderOff;
        static ma_decoder s_decoderFail;
        static ma_decoder s_decoderHide;
        static bool s_onLoaded = false;
        static bool s_offLoaded = false;
        static bool s_failLoaded = false;
        static bool s_hideLoaded = false;
        static SoundData s_onData;
        static SoundData s_offData;
        static SoundData s_failData;
        static SoundData s_hideData;

        static void InitSoundFromData(const SoundData& data, ma_decoder* decoder, ma_sound* sound, bool& loaded)
        {
            if (data.raw.empty()) return;
            ma_decoder_config dcfg = ma_decoder_config_init(ma_format_unknown, 0, 0);
            if (ma_decoder_init_memory(data.raw.data(), data.raw.size(), &dcfg, decoder) == MA_SUCCESS) {
                if (ma_sound_init_from_data_source(&s_engine, (ma_data_source*)decoder, MA_SOUND_FLAG_DECODE, NULL, sound) == MA_SUCCESS) {
                    loaded = true;
                } else {
                    ma_decoder_uninit(decoder);
                }
            }
        }

        static bool LoadWavFromResource(UINT id, SoundData& out)
        {
            HMODULE mod = NULL;
            if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)&LoadWavFromResource, &mod))
                mod = GetModuleHandle(NULL);

            HRSRC rc = FindResourceW(mod, MAKEINTRESOURCEW(id), L"WAVE");
            if (!rc) return false;
            HGLOBAL hg = LoadResource(mod, rc);
            if (!hg) return false;
            void* ptr = LockResource(hg);
            DWORD sz = SizeofResource(mod, rc);
            if (!ptr || sz == 0) return false;

            uint8_t* p = (uint8_t*)ptr;
            out.raw.resize(sz);
            memcpy(out.raw.data(), p, sz);
            return true;
        }

        static void EnsureInit()
        {
            if (s_engineInited) return;
            if (ma_engine_init(NULL, &s_engine) != MA_SUCCESS) return;
            s_engineInited = true;

            LoadWavFromResource(TOGGLE_ON, s_onData);
            LoadWavFromResource(TOGGLE_OFF, s_offData);
            LoadWavFromResource(STOP_ON_FAIL, s_failData);
            LoadWavFromResource(HIDE_NOTIF, s_hideData);

            InitSoundFromData(s_onData,   &s_decoderOn,   &s_soundOn,   s_onLoaded);
            InitSoundFromData(s_offData,  &s_decoderOff,  &s_soundOff,  s_offLoaded);
            InitSoundFromData(s_failData, &s_decoderFail, &s_soundFail, s_failLoaded);
            InitSoundFromData(s_hideData, &s_decoderHide, &s_soundHide, s_hideLoaded);

            SetVolumePercent(LavenderHook::Globals::sound_volume);
        }

        void PlayFailSound()
        {
            EnsureInit();
            if (!s_engineInited) return;

            // Only play fail sound if stop_on_fail is enabled and the fail sound is not muted
            if (!LavenderHook::Globals::stop_on_fail) return;
            if (LavenderHook::Globals::mute_fail) return;

            float vol = (float)LavenderHook::Globals::sound_volume / 100.0f;
            if (!s_failLoaded) return;
            ma_sound_set_volume(&s_soundFail, vol);
            ma_sound_start(&s_soundFail);
        }

        void PlayToggleSound(bool on)
        {
            EnsureInit();
            if (!s_engineInited) return;
            // Respect global mute for button sounds
            if (LavenderHook::Globals::mute_buttons)
                return;

            float vol = (float)LavenderHook::Globals::sound_volume / 100.0f;
            if (on) {
                if (!s_onLoaded) return;
                ma_sound_set_volume(&s_soundOn, vol);
                ma_sound_start(&s_soundOn);
            } else {
                if (!s_offLoaded) return;
                ma_sound_set_volume(&s_soundOff, vol);
                ma_sound_start(&s_soundOff);
            }
        }

        void PlayHideWindowSound()
        {
            EnsureInit();
            if (!s_engineInited) return;
            if (LavenderHook::Globals::mute_buttons) return;
            if (!s_hideLoaded) return;
            float vol = (float)LavenderHook::Globals::sound_volume / 100.0f;
            ma_sound_set_volume(&s_soundHide, vol);
            ma_sound_start(&s_soundHide);
        }

        void SetVolumePercent(int pct) {
            if (pct < 0) pct = 0; if (pct > 100) pct = 100;
            LavenderHook::Globals::sound_volume = pct;
            float vol = (float)pct / 100.0f;
            if (s_onLoaded)   ma_sound_set_volume(&s_soundOn,   vol);
            if (s_offLoaded)  ma_sound_set_volume(&s_soundOff,  vol);
            if (s_failLoaded) ma_sound_set_volume(&s_soundFail, vol);
            if (s_hideLoaded) ma_sound_set_volume(&s_soundHide, vol);
        }

        int GetVolumePercent() { return LavenderHook::Globals::sound_volume; }

    }
}
