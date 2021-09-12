#ifndef DIR_ACCESS_HYBRID_H
#define DIR_ACCESS_HYBRID_H

#include "core/list.h"
#include "core/os/dir_access.h"

class DirAccessHybrid : public DirAccess {

private:
	DirAccess *dir_access_os;
	DirAccess *dir_access_pack;

	Error err_os;
	Error err_pack;

	List<String> list_dirs;
	List<String> list_files;

	bool cdir;

public:
	Error list_dir_begin() override;
	String get_next() override;
	bool current_is_dir() const override;
	bool current_is_hidden() const override;
	void list_dir_end() override;
	int get_drive_count() override;
	String get_drive(int p_drive) override;
	Error change_dir(String p_dir) override;
	String get_current_dir() override;
	Error make_dir(String p_dir) override;
	bool file_exists(String p_file) override;
	bool dir_exists(String p_dir) override;
	size_t get_space_left() override;
	Error rename(String p_from, String p_to) override;
	Error remove(String p_name) override;
	String get_filesystem_type() const override;

	DirAccessHybrid();
	~DirAccessHybrid() override;
};

#endif
