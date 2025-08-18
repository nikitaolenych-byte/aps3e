// SPDX-License-Identifier: WTFPL

#ifndef APS3E_EMULATOR_H
#define APS3E_EMULATOR_H

#include <android/native_window_jni.h>
#include <string>

namespace ae{
    constexpr int BOOT_TYPE_WITH_PATH=1;
    constexpr int BOOT_TYPE_WITH_FD=2;
    extern int boot_type;

    extern std::string boot_game_path;
    extern int boot_game_fd;

    extern ANativeWindow* window;
    extern int window_width;
    extern int window_height;

    extern void main_thr();
    extern void key_event(int key_code,bool pressed,int value);
    extern bool is_running();
    extern bool is_paused();
    extern void pause();
    extern void resume();
    extern void quit();
}
#endif //APS3E_EMULATOR_H
