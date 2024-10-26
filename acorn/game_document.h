/*************************************************************************/
/*  game_document.h                                                      */
/*************************************************************************/

#ifndef GAMEDOCUMENT_H
#define GAMEDOCUMENT_H

#include "core/engine.h"
#include "core/object.h"
#include "acorn/sonic.h"
#include "acorn/sonic_sound.h"
#include "acorn/sonic_entitlements.h"

namespace AcornGlue {
    class GameDocument : public Object {

        GDCLASS(GameDocument, Object);

    private:
        static GameDocument* singleton;
        Sonic* sonic;
        SonicSound* sound;
        SonicEntitlements* entitlements;
        int extra_item_ID;
        float gamespeed;

    public:
        GameDocument() {
            singleton = this;
            sonic = memnew(Sonic);
            sound = memnew(SonicSound);
            entitlements = memnew(SonicEntitlements);
        }

        ~GameDocument() {
            memdelete(sonic);
            memdelete(sound);
            memdelete(entitlements);
        }

        static GameDocument* get_singleton() {
            return singleton;
        }

        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("is_zone_act_clear", "zone", "act", "unk"), &GameDocument::is_zone_act_clear);
            ClassDB::bind_method(D_METHOD("check_node_pos", "idx"), &GameDocument::check_node_pos);
            ClassDB::bind_method(D_METHOD("get_completed_rival_rush"), &GameDocument::get_completed_rival_rush);
            ClassDB::bind_method(D_METHOD("GenerateRivalRushItemCostText", "unk"), &GameDocument::GenerateRivalRushItemCostText);

            ClassDB::bind_method(D_METHOD("get_sonic"), &GameDocument::get_sonic);
            ClassDB::bind_method(D_METHOD("get_sonic_sound"), &GameDocument::get_sonic_sound);
            ClassDB::bind_method(D_METHOD("get_sonic_entitlements"), &GameDocument::get_sonic_entitlements);
            ClassDB::bind_method(D_METHOD("get_extra_item_ID"), &GameDocument::get_extra_item_ID);
            ClassDB::bind_method(D_METHOD("set_gamespeed"), &GameDocument::set_gamespeed);

        	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "sonic"), "", "get_sonic");
        	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "sound"), "", "get_sonic_sound");
        	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "entitlements"), "", "get_sonic_entitlements");
        	ADD_PROPERTY(PropertyInfo(Variant::INT, "extra_item_ID"), "", "get_extra_item_ID");
        	ADD_PROPERTY(PropertyInfo(Variant::REAL, "gamespeed"), "set_gamespeed", "");
        }

        Sonic* get_sonic() { return sonic; }
        SonicSound* get_sonic_sound() { return sound; }
        SonicEntitlements* get_sonic_entitlements() { return entitlements; }
        int get_extra_item_ID() { return extra_item_ID; }
        void set_gamespeed(float in_gamespeed) { gamespeed = in_gamespeed; }
        bool is_zone_act_clear(int zone, int act, bool unk) { return true; }
        void check_node_pos(int idx) {}
        int get_completed_rival_rush() { return 0; }
        String GenerateRivalRushItemCostText(String unk) { return "0"; }
    };
};

#endif // GAMEDOCUMENT_H
