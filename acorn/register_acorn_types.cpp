/*************************************************************************/
/*  register_acorn_types.cpp                                             */
/*************************************************************************/

#include "register_acorn_types.h"
#include "acorn/game_document.h"
#include "acorn/sonic.h"
#include "acorn/sonic_sound.h"
#include "acorn/sonic_entitlements.h"
#include "acorn/movie_player.h"

static AcornGlue::GameDocument* game_document;

void register_acorn_types() {
	ClassDB::register_class<AcornGlue::GameDocument>();
	ClassDB::register_class<AcornGlue::Sonic>();
	ClassDB::register_class<AcornGlue::SonicSound>();
	ClassDB::register_class<AcornGlue::SonicEntitlements>();
	ClassDB::register_class<AcornGlue::MoviePlayer>();

	game_document = memnew(AcornGlue::GameDocument);
	Engine::get_singleton()->add_singleton(Engine::Singleton("GameDocument", AcornGlue::GameDocument::get_singleton()));
}

void unregister_acorn_types() {
	memdelete(game_document);
}