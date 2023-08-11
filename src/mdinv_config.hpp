//
// Copyright (c) 2023 Fas Xmut (fasxmut AT protonmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef __mdinv_src_mdinv_config_hpp__
#define __mdinv_src_mdinv_config_hpp__

#include <utxcpp/core.hpp>
#include <utxcpp/thread.hpp>
#include <nirtcpp.hpp>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace mdinv
{

////////////////////////////////////////////////////////////////////////
// class application_init_info

class application_init_info
{
protected:
// data
	utx::u32 __best_win_width = 1280;
	utx::u32 __best_win_height = 720;
	utx::f32 __best_ratio = static_cast<utx::f32>(__best_win_height)/__best_win_width;
	utx::u32 __width;
	utx::u32 __height;
	bool __fullscreen = false;
	std::wstring_view __title = L"Mdinv 3D Viewer";

	utx::u32 __splitx = 2;
	utx::u32 __splity = 2;

	std::string_view __description = 
R"(Mdinv 3D Viewer is a 3D mesh viewer for irrlicht and nirtcpp mesh formats.
It will help you develop 3D applications using irrlicht or nirtcpp.)";

	std::string_view __license =
R"(Copyright (c) 2023 Fas Xmut (fasxmut at protonmail.com)

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt))";

public:
// destructor
	virtual ~application_init_info() {}
	
protected:
// constructor
	application_init_info()
	{
		nirt::NirtcppDevice * device = nullptr;
		nirt::video::IVideoModeList * vm_list = nullptr;
		nirt::core::dimension2du dim;
		
		try
		{
			device = nirt::createDevice(nirt::video::EDT_BURNINGSVIDEO, nirt::core::dimension2du{300,300});
			if (! device)
				throw std::runtime_error{""};
			vm_list = device->getVideoModeList();
			if (! vm_list)
				throw std::runtime_error{""};
			dim = vm_list->getDesktopResolution();
		}
		catch (...)
		{
			this->__width = this->__best_win_width;
			this->__height = this->__best_win_height;
			if (device)
				device->drop();
			return;
		}

		// too big desktop resolution.
		if (dim.Width > this->__best_win_width*2 && dim.Height > this->__best_win_height*2)
		{
			this->__width = dim.Width/2;
			this->__height = static_cast<utx::u32>((this->__width)*(this->__best_ratio));
			device->drop();
			return;
		}

		// else

		if (dim.Width > this->__best_win_width && dim.Height > this->__best_win_height)
		{
			this->__width = this->__best_win_width;
			this->__height = this->__best_win_height;
			device->drop();
			return;
		}		
		
		// else

		utx::f32 desk_ratio = static_cast<utx::f32>(dim.Height)/dim.Width;

		if (desk_ratio > (this->__best_ratio))
		{
			this->__width = dim.Width;
			this->__height = static_cast<utx::u32>(dim.Width * (this->__best_ratio));
			device->drop();
			return;
		}

		// else

		this->__height = dim.Height;
		this->__width = static_cast<utx::u32>(dim.Height / (this->__best_ratio));
		device->drop();

		return;
	}

protected:
// Removed
	application_init_info(const application_init_info &) = delete;
	application_init_info(const application_init_info &&) = delete;
	
	application_init_info & operator=(const application_init_info &) = delete;
	application_init_info & operator=(const application_init_info &&) = delete;
	
protected:
	static const application_init_info __instance;
	
public:
	static const application_init_info & instance()
	{
		return __instance;
	}
public:
// get
	utx::u32 width() const {return __width;}
	utx::u32 height() const {return __height;}
	utx::u32 splitx() const {return __splitx;}
	utx::u32 splity() const {return __splity;}
	bool fullscreen() const {return __fullscreen;}
	std::wstring_view title() const {return __title;}
	std::string_view description() const {return __description;}
	std::string_view license() const {return __license;}
}; // class application_init_info
const application_init_info application_init_info::__instance{};

// app_init_info
static const application_init_info & app_init_info = application_init_info::instance();

////////////////////////////////////////////////////////////////////////
// class application_update_info

class application_update_info
{
protected:
// private members
	utx::u32 __width;
	utx::u32 __height;
	bool __fullscreen;
	std::wstring __title;
	constexpr static std::string_view __information_saved_path = "/tmp/mdinv-3d-viewer-information-saved-path.txt";
	
protected:
// private member functions
	void save_information() const
	{
		fs::path path{this->__information_saved_path};
		std::ofstream file{path, std::ios::trunc};
		file << this->__width << ' ' << this->__height << ' ' << this->__fullscreen << std::endl;
	}
	
public:
// destructor
	virtual ~application_update_info()
	{
		this->save_information();
	}
	
protected:
// constructor
	application_update_info(utx::u32 width, utx::u32 height, bool fullscreen, std::wstring_view title):
		__width{width},
		__height{height},
		__fullscreen{fullscreen},
		__title{title}
	{
		if (fs::exists(this->__information_saved_path))
			this->read_information(true);
	}

protected:
	static application_update_info __instance;

public:
	static application_update_info & instance()
	{
		return __instance;
	}

protected:
// Removed
	application_update_info() = delete;
	application_update_info(const application_update_info &) = delete;
	application_update_info(const application_update_info &&) = delete;
	
	application_update_info & operator=(const application_update_info &) = delete;
	application_update_info & operator=(const application_update_info &&) = delete;
	
public:
// update
	void update(utx::u32 width, utx::u32 height, bool fullscreen, std::wstring_view title)
	{
		this->__width = width;
		this->__height = height;
		this->__fullscreen = fullscreen;
		this->__title = title;
	}
	void update_dimension(utx::u32 width, utx::u32 height)
	{
		this->__width = width;
		this->__height = height;
	}
	void update_dimension(const nirt::core::dimension2du & dim)
	{
		this->__width = dim.Width;
		this->__height = dim.Height;
	}
	void update_fullscreen(bool fullscreen)
	{
		this->__fullscreen = fullscreen;
	}
	void update_title(const std::wstring_view title)
	{
		this->__title = title;
	}
	
public:
// get
	utx::u32 width() const
	{
		return __width;
	}
	utx::u32 height() const
	{
		return __height;
	}
	nirt::core::dimension2du dimension() const
	{
		return {__width, __height};
	}
	bool fullscreen() const
	{
		return __fullscreen;
	}
	std::wstring_view title() const
	{
		return __title;
	}
	
public:
	// Read information from information saved path.
	// overrite: overrite this->__width and this->__height
	bool read_information(bool overwrite)
	{
		if (! overwrite)
			return false;
		try
		{
			fs::path path{this->__information_saved_path};
			std::ifstream file{path};
			utx::u32 w{0}, h{0}, full{false};
			file >> w >> h >> full;
			// If the size read is too small or too big, it will be ignored.
			if (w<=320 || w>=10000 || h<=180 || h>=10000)
				return false;
			this->__width = w;
			this->__height = h;
			if (full)
				this->__fullscreen = true;
			return true;
		}
		catch (const std::exception &)
		{
		}
		catch (...)
		{
		}
		return false;
	}
}; // application_update_info

application_update_info application_update_info::__instance{
	app_init_info.width(),
	app_init_info.height(),
	app_init_info.fullscreen(),
	app_init_info.title()
};

// app_update_info
static application_update_info & app_update_info = application_update_info::instance();

enum gui_menu_id
{
	bar_file_add,
	dialog_add_mesh,
	bar_file_close_last,
	bar_file_close_all,
	bar_file_exit
};

} // namespace mdinv

#endif // __mdinv_src_mdinv_config_hpp__

