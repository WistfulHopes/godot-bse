/*************************************************************************/
/*  sonic_sound.h                                                        */
/*************************************************************************/

#ifndef SONICSOUND_H
#define SONICSOUND_H

#include "core/object.h"

namespace AcornGlue {
    class SonicSound : public Object {

        GDCLASS(SonicSound, Object);

    public:
        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("resume_sound", "name", "fadeTime"), &SonicSound::resume_sound);
            ClassDB::bind_method(D_METHOD("play_sound", "name", "fadeTime"), &SonicSound::play_sound);
            ClassDB::bind_method(D_METHOD("play_music", "name", "fadeTime"), &SonicSound::play_music);
            ClassDB::bind_method(D_METHOD("pause_sound", "name", "fadeTime"), &SonicSound::pause_sound);
            ClassDB::bind_method(D_METHOD("pause_all", "name", "fadeTime"), &SonicSound::pause_all);
            ClassDB::bind_method(D_METHOD("stop_sound", "name", "fadeTime"), &SonicSound::stop_sound);
            ClassDB::bind_method(D_METHOD("stop_all", "name", "fadeTime"), &SonicSound::stop_all);
        }

        bool resume_sound(String name, float fadeTime) { return true; }
        void play_sound(String name, float fadeTime) {}
        void play_music(String name, float fadeTime) {}
        void pause_sound(String name, float fadeTime) {}
        void pause_all(String name, float fadeTime) {}
        void stop_sound(String name, float fadeTime) {}
        void stop_all(String name, float fadeTime) {}
    };
};

#endif // SONICSOUND_H
