//
// Copyright (c) 2023 Fas Xmut (fasxmut AT protonmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <nirtcpp.hpp>
#include <utxcpp/core.hpp>
#include <mdinv_config.hpp>
#include <mdinv_window_event.hpp>
#include <filesystem>
#include <utxcpp/thread.hpp>

int main()
try
{
	std::unique_lock lock{mdinv::mutex0};
	utx::print("------------------------------------------------------------------------");
	utx::print(mdinv::app_init_info.description, "\n\n", mdinv::app_init_info.license);
	utx::print("------------------------------------------------------------------------");
	utx::print("init resolution:", mdinv::app_init_info.width, 'x', mdinv::app_init_info.height);
	utx::print("update resolution:", mdinv::app_update_info.get_width(), 'x', mdinv::app_update_info.get_height());
	utx::print("------------------------------------------------------------------------");
	lock.unlock();
	nirt::NirtcppDevice * win_device = nirt::createDevice(
		nirt::video::EDT_OPENGL,
		mdinv::app_update_info.get_dimension(),
		32,
		mdinv::app_update_info.get_fullscreen(),
		true,
		true,
		nullptr
	);
	if (! win_device)
		throw std::runtime_error{"can not create Nirtcpp Device!"};
	
	win_device->setWindowCaption(mdinv::app_update_info.get_title().data());

	nirt::video::IVideoDriver * win_driver = win_device->getVideoDriver();
	nirt::scene::ISceneManager * win_smgr = win_device->getSceneManager();
	nirt::gui::IGUIEnvironment * win_gui = win_device->getGUIEnvironment();

	while (win_device->run())
	{
		if (win_device->isWindowActive())
		{
			win_driver->beginScene(true, true, nirt::video::SColor{0xff335774});
			win_smgr->drawAll();
			win_gui->drawAll();
			mdinv::app_update_info.update_dimension(win_driver->getScreenSize());
			win_driver->endScene();
		}
		else
		{
			win_device->yield();
		}
	}

	win_device->drop();

	lock.lock();
	utx::print("------------------------------------------------------------------------");
	utx::print(
		"Window is closed.", '\n', 
		"Last window resolution:", mdinv::app_update_info.get_width(), 'x', mdinv::app_update_info.get_height()
	);
	utx::print("------------------------------------------------------------------------");
	lock.unlock();
}
catch (const std::exception & err)
{
	utx::printe("---- c++ standard exception ----");
	utx::printe(err.what());
	return 1;
}
catch (...)
{
	utx::printe("---- c++ unknown exception ----");
	return 2;
}

