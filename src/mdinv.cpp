//
// Copyright (c) 2023 Fas Xmut (fasxmut AT protonmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#include <mdinv_config.hpp>
#include <mdinv_gui.hpp>
#include <mdinv_window_event.hpp>

#include <nirtcpp.hpp>

#include <utxcpp/core.hpp>
#include <utxcpp/thread.hpp>

#include <filesystem>

int main()
try
{
	std::unique_lock lock{utx::mutex0};
	utx::print("------------------------------------------------------------------------");
	utx::print(mdinv::app_init_info.description(), "\n\n", mdinv::app_init_info.license());
	utx::print("------------------------------------------------------------------------");
	utx::print("init resolution:", mdinv::app_init_info.width(), 'x', mdinv::app_init_info.height());
	utx::print("update resolution:", mdinv::app_update_info.width(), 'x', mdinv::app_update_info.height());
	utx::print("------------------------------------------------------------------------");
	lock.unlock();
	nirt::NirtcppDevice * win_device = nirt::createDevice(
		nirt::video::EDT_OPENGL,
		mdinv::app_update_info.dimension(),
		32,
		mdinv::app_update_info.fullscreen(),
		true,
		true,
		nullptr
	);
	if (! win_device)
		throw std::runtime_error{"can not create Nirtcpp Device!"};
	
	win_device->setWindowCaption(mdinv::app_update_info.title().data());

	nirt::video::IVideoDriver * win_driver = win_device->getVideoDriver();
	nirt::scene::ISceneManager * win_smgr = win_device->getSceneManager();
	nirt::gui::IGUIEnvironment * win_gui = win_device->getGUIEnvironment();
	
	mdinv::create_gui(win_device, win_gui);
	
	nirt::scene::ICameraSceneNode * camera = 
		win_smgr->addCameraSceneNodeFPS(nullptr, 35.0f, 0.05f, -1, nullptr, 0, false, 5.0f, true, false);
	camera->setPosition({10.0f, 10.0f, -500.0f});
	camera->setTarget({0,0,0});
	
	nirt::gui::ICursorControl * cursor = win_device->getCursorControl();
	cursor->setVisible(true);
		
	mdinv::window_event win_event{win_device};
	win_device->setEventReceiver(&win_event);

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
		"Last window resolution:", mdinv::app_update_info.width(), 'x', mdinv::app_update_info.height()
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

