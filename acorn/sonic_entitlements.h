/*************************************************************************/
/*  sonic_entitlements.h                                                 */
/*************************************************************************/

#ifndef SONICENTITLEMENTS_H
#define SONICENTITLEMENTS_H

#include "core/object.h"
#include "acorn/sonic.h"

namespace AcornGlue {
    class SonicEntitlements : public Object {

        GDCLASS(SonicEntitlements, Object);

    public:
        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("is_unlocked", "id"), &SonicEntitlements::is_unlocked);
            ClassDB::bind_method(D_METHOD("open_store", "id"), &SonicEntitlements::open_store);
        }

        bool is_unlocked(Sonic::EntitlementID id) { return true; }
        void open_store(Sonic::EntitlementID id) {}
    };
};

#endif // SONICENTITLEMENTS_H
