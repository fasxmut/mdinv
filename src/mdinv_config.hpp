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

class application_init_info
{
public:
	utx::u32 best_win_width = 1280;
	utx::u32 best_win_height = 720;
	utx::f32 best_ratio = static_cast<utx::f32>(best_win_height)/best_win_width;
	utx::u32 width;
	utx::u32 height;
	bool fullscreen = false;
	std::wstring_view title = L"Mdinv 3D Viewer";

	std::string_view description = 
R"(Mdinv 3D Viewer is a 3D mesh viewer for irrlicht and nirtcpp mesh formats.
It will help you develop 3D applications using irrlicht or nirtcpp.)";

	std::string_view license =
R"(Copyright (c) 2023 Fas Xmut (fasxmut at protonmail.com)

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt))";

public:
	application_init_info()
	{
		nirt::NirtcppDevice * device = nirt::createDevice(
			nirt::video::EDT_BURNINGSVIDEO,
			nirt::core::dimension2du{300,300}
		);
		nirt::video::IVideoModeList * vm_list = device->getVideoModeList();
		const nirt::core::dimension2du & dim = vm_list->getDesktopResolution();

		// too big desktop resolution.
		if (dim.Width > this->best_win_width*2 && dim.Height > this->best_win_height*2)
		{
			this->width = dim.Width/2;
			this->height = static_cast<utx::u32>((this->width)*(this->best_ratio));
			device->drop();
			return;
		}

		// else

		if (dim.Width > this->best_win_width && dim.Height > this->best_win_height)
		{
			this->width = this->best_win_width;
			this->height = this->best_win_height;
			device->drop();
			return;
		}
		
		// else

		utx::f32 desk_ratio = static_cast<utx::f32>(dim.Height)/dim.Width;

		if (desk_ratio > (this->best_ratio))
		{
			this->width = dim.Width;
			this->height = static_cast<utx::u32>(dim.Width * (this->best_ratio));
			device->drop();
			return;
		}

		// else

		this->height = dim.Height;
		this->width = static_cast<utx::u32>(dim.Height / (this->best_ratio));
		device->drop();

		return;
	}
};

class application_update_info
{
protected:
// private members
	utx::u32 width;
	utx::u32 height;
	bool fullscreen;
	std::wstring title;
	constexpr static std::string_view information_saved_path = "/tmp/mdinv-3d-viewer-information-saved-path.txt";
protected:
// private member functions
	void save_information() const
	{
		fs::path path{this->information_saved_path};
		std::ofstream file{path, std::ios::trunc};
		file << this->width << ' ' << this->height << ' ' << this->fullscreen << std::endl;
	}
public:
// destructor
	virtual ~application_update_info()
	{
		this->save_information();
	}
public:
// constructors
	application_update_info() = delete;
	application_update_info(utx::u32 width, utx::u32 height, bool fullscreen, std::wstring_view title):
		width{width},
		height{height},
		fullscreen{fullscreen},
		title{title}
	{
		if (fs::exists(this->information_saved_path))
			this->read_information(true);
	}
public:
// update
	void update(utx::u32 width, utx::u32 height, bool fullscreen, std::wstring_view title)
	{
		this->width = width;
		this->height = height;
		this->fullscreen = fullscreen;
		this->title = title;
	}
	void update_dimension(utx::u32 width, utx::u32 height)
	{
		this->width = width;
		this->height = height;
	}
	void update_dimension(const nirt::core::dimension2du & dim)
	{
		this->width = dim.Width;
		this->height = dim.Height;
	}
	void update_fullscreen(bool fullscreen)
	{
		this->fullscreen = fullscreen;
	}
	void update_title(const std::wstring_view title)
	{
		this->title = title;
	}
public:
// get
	utx::u32 get_width() const
	{
		return width;
	}
	utx::u32 get_height() const
	{
		return height;
	}
	nirt::core::dimension2du get_dimension() const
	{
		return {width, height};
	}
	bool get_fullscreen() const
	{
		return fullscreen;
	}
	std::wstring_view get_title() const
	{
		return title;
	}
public:
	// Read information from information saved path.
	// overrite: overrite this->width and this->height
	bool read_information(bool overwrite)
	{
		if (! overwrite)
			return false;
		try
		{
			fs::path path{this->information_saved_path};
			std::ifstream file{path};
			utx::u32 w{0}, h{0}, full{false};
			file >> w >> h >> full;
			// If the size read is too small or too big, it will be ignored.
			if (w<=320 || w>=10000 || h<=180 || h>=10000)
				return false;
			this->width = w;
			this->height = h;
			if (full)
				this->fullscreen = true;
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
};

static const auto app_init_info = application_init_info{};
static auto app_update_info = application_update_info{
	app_init_info.width,
	app_init_info.height,
	app_init_info.fullscreen,
	app_init_info.title
};

std::mutex mutex0;
std::mutex mutex1;

} // namespace mdinv

#endif // __mdinv_src_mdinv_config_hpp__

