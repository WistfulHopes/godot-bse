/*************************************************************************/
/*  sonic.h                                                              */
/*************************************************************************/

#ifndef SONIC_H
#define SONIC_H

#include "core/object.h"
#include "scene/main/node.h"

namespace AcornGlue {
    class Sonic : public Object {

        GDCLASS(Sonic, Object);

    private:
        bool invincible;
        bool demigod;
        bool debugcam;

    public:
        enum EntitlementID {
            EID_AmericasRetailer,
            EID_AsiaRetailer,
            EID_EuropeRetailer,
            EID_MovieFXCosmetic,
            EID_MoviePlayersIcons,
            EID_PlayerIconPack,
            EID_SilverAndGold,
            EID_UltimateCosmeticsBundle,
            EID_UltimateMusicPack,
        };

        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("give_infinite_wisppower"), &Sonic::give_infinite_wisppower);
            ClassDB::bind_method(D_METHOD("give_infinite_boost"), &Sonic::give_infinite_boost);
            ClassDB::bind_method(D_METHOD("give_rings", "count"), &Sonic::give_rings);
            ClassDB::bind_method(D_METHOD("apply_customization", "root", "isPreview"), &Sonic::apply_customization);
            ClassDB::bind_method(D_METHOD("cleanup_customization", "root"), &Sonic::cleanup_customization);
            ClassDB::bind_method(D_METHOD("toggle_boost_preview"), &Sonic::toggle_boost_preview);
            ClassDB::bind_method(D_METHOD("return_to_title"), &Sonic::return_to_title);

            ClassDB::bind_method(D_METHOD("set_invincible"), &Sonic::set_invincible);
            ClassDB::bind_method(D_METHOD("set_demigod"), &Sonic::set_demigod);
            ClassDB::bind_method(D_METHOD("set_debugcam"), &Sonic::set_debugcam);

        	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "invincible"), "set_invincible", "");
        	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "demigod"), "set_demigod", "");
        	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debugcam"), "set_debugcam", "");

            BIND_ENUM_CONSTANT(EID_AmericasRetailer);
            BIND_ENUM_CONSTANT(EID_AsiaRetailer);
            BIND_ENUM_CONSTANT(EID_EuropeRetailer);
            BIND_ENUM_CONSTANT(EID_MovieFXCosmetic);
            BIND_ENUM_CONSTANT(EID_MoviePlayersIcons);
            BIND_ENUM_CONSTANT(EID_PlayerIconPack);
            BIND_ENUM_CONSTANT(EID_SilverAndGold);
            BIND_ENUM_CONSTANT(EID_UltimateCosmeticsBundle);
            BIND_ENUM_CONSTANT(EID_UltimateMusicPack);
        }

        void set_invincible(bool is_invincible) { invincible = is_invincible; }
        void set_demigod(bool is_demigod) { demigod = is_demigod; }
        void set_debugcam(bool is_debugcam) { debugcam = is_debugcam; }
        void give_infinite_wisppower() {}
        void give_infinite_boost() {}
        void give_rings(int count) {}
        void apply_customization(Node* root, bool isPreview) {}
        void cleanup_customization(Node* root) {}
        void toggle_boost_preview() {}
        void return_to_title() {}
    };
};

VARIANT_ENUM_CAST(AcornGlue::Sonic::EntitlementID);

#endif // SONIC_H
