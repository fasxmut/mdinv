//
// Copyright (c) 2023 Fas Xmut (fasxmut AT protonmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef __mdinv_src_mdinv_gui_hpp__
#define __mdinv_src_mdinv_gui_hpp__

#include <mdinv_config.hpp>

#include <nirtcpp.hpp>
#include <utxcpp/core.hpp>

namespace mdinv
{

auto setup_font = [] (nirt::NirtcppDevice * device, nirt::gui::IGUIEnvironment * ngui)
{
	nirt::gui::IGUISkin * skin = ngui->getSkin();
	nirt::gui::IGUIFont * font = nullptr;

	std::string font_name = "Liberation-Mono.1ASC.14-bold.xml";
	for(fs::path path: {".", "..", "../..", "../../..", "../../../.."})
	{
		path /= "share/mdinv/media/fonts";
		path /= font_name;
		if (fs::exists(path))
		{
			font = ngui->getFont(path.string().data());
			skin->setFont(font);
			break;
		}
	}
	if (! font)
		utx::printe("---- Font File Not Found:", font_name, "----");
};

auto create_menu = [](nirt::NirtcppDevice * device, nirt::gui::IGUIEnvironment * ngui)
{
	nirt::gui::IGUIContextMenu * menu_bar = ngui->addMenu(nullptr, -1);
	
	////////////////////////////////////////////////////////////////////////
	
	utx::u32 file_menu_index = menu_bar->addItem(
		L"File", 
		-1,		// id
		true,		// enabled
		true,		// has sub menu
		true,		// checked
		true		// auto checking
	);
	nirt::gui::IGUIContextMenu * file_menu = menu_bar->getSubMenu(file_menu_index);
	
	file_menu->addItem(L"Add Mesh ...", bar_file_add, true, false, false, false);
	file_menu->addItem(L"Close Last Mesh", bar_file_close_last, true, false, false, false);
	file_menu->addItem(L"Close All Mesh", bar_file_close_all, true, false, false, false);
	file_menu->addItem(L"Exit", bar_file_exit, true, false, false, false);
	
	////////////////////////////////////////////////////////////////////////
	
	utx::u32 test_menu_index = menu_bar->addItem(L"Test Menu", -1, true, true, true, true);
	nirt::gui::IGUIContextMenu * test_menu = menu_bar->getSubMenu(test_menu_index);
	const std::wstring tm = L"Item ";
	for (wchar_t i=L'A'; i<=L'Z'; i++)
	{
		test_menu->addItem((tm+std::to_wstring(i)).data(), -1, true, false, false, false);
	}
	
	////////////////////////////////////////////////////////////////////////
	
	utx::u32 help_menu_index = menu_bar->addItem(L"Help", -1, true, true, true, true);
	nirt::gui::IGUIContextMenu * help_menu = menu_bar->getSubMenu(help_menu_index);
	
	help_menu->addItem(L"Help", -1, true, false, false, false);
	help_menu->addItem(L"About", -1, true, false, false, false);
};

auto create_gui = [] (nirt::NirtcppDevice * device, nirt::gui::IGUIEnvironment * ngui)
{
	mdinv::setup_font(device, ngui);
	mdinv::create_menu(device, ngui);
};

} // namespace mdinv

#endif
