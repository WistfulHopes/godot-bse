/*************************************************************************/
/*  movie_player.h                                                       */
/*************************************************************************/

#ifndef MOVIEPLLAYER_H
#define MOVIEPLLAYER_H

#include "core/object.h"
#include "scene/gui/control.h"

namespace AcornGlue {
    class MoviePlayer : public Object {

        GDCLASS(MoviePlayer, Object);

    public:
        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("stop"), &MoviePlayer::stop);
            ClassDB::bind_method(D_METHOD("play", "name", "control"), &MoviePlayer::play);
            ClassDB::bind_method(D_METHOD("draw"), &MoviePlayer::stop);
            ClassDB::bind_method(D_METHOD("is_ended_playing"), &MoviePlayer::is_ended_playing);
        }

        void stop() {}
        void play(const String& name, Control* control) {}
        void draw() {};
        bool is_ended_playing() { return true; }
    };
};

#endif // MOVIEPLLAYER_H
