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
            LoadWavFromResource(HIDE_WINDOW, s_hideData);

            if (!s_onData.raw.empty()) {
                ma_decoder_config dcfg = ma_decoder_config_init(ma_format_unknown, 0, 0);
                if (ma_decoder_init_memory(s_onData.raw.data(), s_onData.raw.size(), &dcfg, &s_decoderOn) == MA_SUCCESS) {
                    if (ma_sound_init_from_data_source(&s_engine, (ma_data_source*)&s_decoderOn, MA_SOUND_FLAG_DECODE, NULL, &s_soundOn) == MA_SUCCESS) {
                        s_onLoaded = true;
                    } else {
                        ma_decoder_uninit(&s_decoderOn);
                    }
                }
            }

            if (!s_offData.raw.empty()) {
                ma_decoder_config dcfg = ma_decoder_config_init(ma_format_unknown, 0, 0);
                if (ma_decoder_init_memory(s_offData.raw.data(), s_offData.raw.size(), &dcfg, &s_decoderOff) == MA_SUCCESS) {
                    if (ma_sound_init_from_data_source(&s_engine, (ma_data_source*)&s_decoderOff, MA_SOUND_FLAG_DECODE, NULL, &s_soundOff) == MA_SUCCESS) {
                        s_offLoaded = true;
                    } else {
                        ma_decoder_uninit(&s_decoderOff);
                    }
                }
            }

            if (!s_failData.raw.empty()) {
                ma_decoder_config dcfg = ma_decoder_config_init(ma_format_unknown, 0, 0);
                if (ma_decoder_init_memory(s_failData.raw.data(), s_failData.raw.size(), &dcfg, &s_decoderFail) == MA_SUCCESS) {
                    if (ma_sound_init_from_data_source(&s_engine, (ma_data_source*)&s_decoderFail, MA_SOUND_FLAG_DECODE, NULL, &s_soundFail) == MA_SUCCESS) {
                        s_failLoaded = true;
                    } else {
                        ma_decoder_uninit(&s_decoderFail);
                    }
                }
            }

            if (!s_hideData.raw.empty()) {
                ma_decoder_config dcfg = ma_decoder_config_init(ma_format_unknown, 0, 0);
                if (ma_decoder_init_memory(s_hideData.raw.data(), s_hideData.raw.size(), &dcfg, &s_decoderHide) == MA_SUCCESS) {
                    if (ma_sound_init_from_data_source(&s_engine, (ma_data_source*)&s_decoderHide, MA_SOUND_FLAG_DECODE, NULL, &s_soundHide) == MA_SUCCESS) {
                        s_hideLoaded = true;
                    } else {
                        ma_decoder_uninit(&s_decoderHide);
                    }
                }
            }

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
