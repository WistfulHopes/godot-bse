/*************************************************************************/
/*  resource_bundle.cpp                                                  */
/*************************************************************************/

#include "resource_bundle.h"

void ResourceBundle::_bind_methods() {
	ClassDB::bind_method(D_METHOD("clear"), &ResourceBundle::clear);
	ClassDB::bind_method(D_METHOD("get_all_item_resources"), &ResourceBundle::get_all_item_resources);
	ClassDB::bind_method(D_METHOD("set_all_item_resources", "in"), &ResourceBundle::set_all_item_resources);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "_resources"), "set_all_item_resources", "get_all_item_resources");
}

void ResourceBundle::clear() {
	_resources.clear();
	notify_change_to_owners();
	emit_changed();
}

Array ResourceBundle::get_all_item_resources() const {
	return _resources;
}

void ResourceBundle::set_all_item_resources(const Array& p_in) {
	_resources = p_in;
}