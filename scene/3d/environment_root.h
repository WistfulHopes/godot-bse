/****************************************************************************/
/*  environment_root.h                                                      */
/****************************************************************************/
/*                           !!!!   NOTICE   !!!!                           */
/* This type is a copy of the WorldEnvironment type                         */
/* This type is not a valid implementation of the BSE EnvironmentRoot type  */
/****************************************************************************/

#ifndef SCENARIO_FX_H2
#define SCENARIO_FX_H2

#include "scene/3d/spatial.h"

class EnvironmentRoot : public Node {

	GDCLASS(EnvironmentRoot, Node);

	Ref<Environment> environment;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_environment(const Ref<Environment> &p_environment);
	Ref<Environment> get_environment() const;

	String get_configuration_warning() const;

	EnvironmentRoot();
};

#endif
