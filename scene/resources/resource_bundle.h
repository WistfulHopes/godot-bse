/*************************************************************************/
/*  resource_bundle.h                                                    */
/*************************************************************************/

#ifndef RESOURCEBUNDLE_H
#define RESOURCEBUNDLE_H

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"

class ResourceBundle : public Resource {

	GDCLASS(ResourceBundle, Resource);

private:
	Array _resources;

public:
	static void _bind_methods();
	void clear();
	Array get_all_item_resources() const;
	void set_all_item_resources(const Array& p_in);
};

#endif // RESOURCEBUNDLE_H
