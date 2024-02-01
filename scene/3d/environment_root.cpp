/****************************************************************************/
/*  environment_root.cpp                                                    */
/****************************************************************************/
/*                           !!!!   NOTICE   !!!!                           */
/* This type is a copy of the WorldEnvironment type                         */
/* This type is not a valid implementation of the BSE EnvironmentRoot type  */
/****************************************************************************/

#include "environment_root.h"
#include "scene/main/viewport.h"

void EnvironmentRoot::_notification(int p_what) {

	if (p_what == Spatial::NOTIFICATION_ENTER_WORLD || p_what == Spatial::NOTIFICATION_ENTER_TREE) {

		if (environment.is_valid()) {
			if (get_viewport()->find_world()->get_environment().is_valid()) {
				WARN_PRINT("World already has an environment (Another EnvironmentRoot?), overriding.");
			}
			get_viewport()->find_world()->set_environment(environment);
			add_to_group("_environment_root_" + itos(get_viewport()->find_world()->get_scenario().get_id()));
		}

	} else if (p_what == Spatial::NOTIFICATION_EXIT_WORLD || p_what == Spatial::NOTIFICATION_EXIT_TREE) {

		if (environment.is_valid() && get_viewport()->find_world()->get_environment() == environment) {
			get_viewport()->find_world()->set_environment(Ref<Environment>());
			remove_from_group("_environment_root_" + itos(get_viewport()->find_world()->get_scenario().get_id()));
		}
	}
}

void EnvironmentRoot::set_environment(const Ref<Environment> &p_environment) {

	if (is_inside_tree() && environment.is_valid() && get_viewport()->find_world()->get_environment() == environment) {
		get_viewport()->find_world()->set_environment(Ref<Environment>());
		remove_from_group("_environment_root_" + itos(get_viewport()->find_world()->get_scenario().get_id()));
		//clean up
	}

	environment = p_environment;
	if (is_inside_tree() && environment.is_valid()) {
		if (get_viewport()->find_world()->get_environment().is_valid()) {
			WARN_PRINT("World already has an environment (Another EnvironmentRoot?), overriding.");
		}
		get_viewport()->find_world()->set_environment(environment);
		add_to_group("_environment_root_" + itos(get_viewport()->find_world()->get_scenario().get_id()));
	}

	update_configuration_warning();
}

Ref<Environment> EnvironmentRoot::get_environment() const {

	return environment;
}

String EnvironmentRoot::get_configuration_warning() const {

	String warning = Node::get_configuration_warning();
	if (!environment.is_valid()) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("EnvironmentRoot requires its \"Environment\" property to contain an Environment to have a visible effect.");
		return warning;
	}

	if (/*!is_visible_in_tree() ||*/ !is_inside_tree())
		return String();

	List<Node *> nodes;
	get_tree()->get_nodes_in_group("_environment_root_" + itos(get_viewport()->find_world()->get_scenario().get_id()), &nodes);

	if (nodes.size() > 1) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("Only one EnvironmentRoot is allowed per scene (or set of instanced scenes).");
	}

	// Commenting this warning for now, I think it makes no sense. If anyone can figure out what its supposed to do, feedback welcome. Else it should be deprecated.
	//if (environment.is_valid() && get_viewport() && !get_viewport()->get_camera() && environment->get_background() != Environment::BG_CANVAS) {
	//	return TTR("This EnvironmentRoot is ignored. Either add a Camera (for 3D scenes) or set this environment's Background Mode to Canvas (for 2D scenes).");
	//}

	return warning;
}

void EnvironmentRoot::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_environment", "env"), &EnvironmentRoot::set_environment);
	ClassDB::bind_method(D_METHOD("get_environment"), &EnvironmentRoot::get_environment);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "environment", PROPERTY_HINT_RESOURCE_TYPE, "Environment"), "set_environment", "get_environment");
}

EnvironmentRoot::EnvironmentRoot() {
}
