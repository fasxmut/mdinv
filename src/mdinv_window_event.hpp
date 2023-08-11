//
// Copyright (c) 2023 Fas Xmut (fasxmut AT protonmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef __mdinv_src_mdinv_window_event_hpp__
#define __mdinv_src_mdinv_window_event_hpp__

#include <nirtcpp.hpp>
#include <utxcpp/core.hpp>

namespace mdinv
{

class window_event: public nirt::IEventReceiver
{
protected:
	nirt::NirtcppDevice * device;
	nirt::gui::IGUIEnvironment * ngui;
	nirt::scene::ISceneManager * smgr;
	std::vector<nirt::scene::IAnimatedMeshSceneNode *> added_mesh_list;
public:
	window_event(nirt::NirtcppDevice * device):
		device{device}
	{
		ngui = device->getGUIEnvironment();
		smgr = device->getSceneManager();
	}
	bool OnEvent(const nirt::SEvent & event) override
	{
		this->gui_event(event);
		return false;
	}
	bool gui_event(const nirt::SEvent & event)
	{
		if (event.EventType != nirt::EET_GUI_EVENT)
			return false;
		switch (event.GUIEvent.EventType)
		{
		case nirt::gui::EGET_MENU_ITEM_SELECTED:
			this->process_eget_menu_item_selected(event);
			break;
		case nirt::gui::EGET_FILE_SELECTED:
			this->process_eget_file_selected(event);
			break;
		default:
			break;
		}
		return false;
	}
	bool process_eget_menu_item_selected(const nirt::SEvent & event)
	{
		nirt::gui::IGUIElement * caller = event.GUIEvent.Caller;
		auto * menu = std::bit_cast<nirt::gui::IGUIContextMenu *>(caller);
		utx::i32 id = menu->getItemCommandId(menu->getSelectedItem());
		
		switch (id)
		{
		case bar_file_add:
			this->add_mesh();
			break;
		case bar_file_close_last:
			this->close_last_mesh();
			break;
		case bar_file_close_all:
			utx::print("-------------------------------- Close All Mesh Called ----");
			break;
		case bar_file_exit:
			device->closeDevice();
			break;
		default:
			break;
		}
		
		return false;
	}
	
	void add_mesh()
	{
		ngui->addFileOpenDialog(
			L"Choose Mesh(es):",
			true,		// Modal
			nullptr,		// parent
			dialog_add_mesh,		// id
			true,		// restore cwd ?
			nullptr		// start dir
		);
	}
	bool process_eget_file_selected(const nirt::SEvent & event)
	{
		nirt::gui::IGUIElement * caller = event.GUIEvent.Caller;
		auto * dialog = std::bit_cast<nirt::gui::IGUIFileOpenDialog *>(caller);
		const std::wstring filename = dialog->getFileName();
		switch (dialog->getID())
		{
		case dialog_add_mesh:
			this->try_load_mesh(filename);
			break;
		default:
			break;
		}
		return false;
	}
	
	void try_load_mesh(const std::wstring_view filename)
	{
		utx::printnl("try loading mesh ....");
		std::wcout << filename << '\n';
		try
		{
			auto * node = smgr->addAnimatedMeshSceneNode(
				smgr->getMesh(filename.data()),	
				nullptr,
				-1,
				nirt::core::vector3df{0},
				nirt::core::vector3df{0},
				nirt::core::vector3df{1},
				false
			);
			if (! node)
				throw std::runtime_error{"Load Mesh Error!"};
			node->setMaterialFlag(nirt::video::EMF_LIGHTING, false);
			node->setPosition(nirt::core::vector3df{0});
			{// experimental
				nirt::core::aabbox3df box = node->getBoundingBox();
				auto * camera = smgr->getActiveCamera();

				nirt::core::vector3df target = box.getCenter();
				camera->setTarget(target);

				utx::f32 slide = (box.MaxEdge-box.MinEdge).getLength()/2;
				auto pos = target;
				pos.X += slide/4;
				pos.Y += slide/2;
				pos.Z -= slide*2.5f;
				camera->setPosition(pos);
			}
			this->added_mesh_list.push_back(node);
		}
		catch (const std::exception & err)
		{
			ngui->addMessageBox(
				L"Load Mesh Error!",
				filename.data(),
				true, // modal
				nirt::gui::EMBF_OK,
				nullptr, // parent
				-1, // id
				nullptr // texture
			);
		}
	}

	void close_last_mesh()
	{
		if (this->added_mesh_list.empty())
			return;
		auto itr = this->added_mesh_list.end();
		itr--;
		//(*itr)->drop();
		(*itr)->getParent()->removeChild(*itr);
		this->added_mesh_list.erase(itr);
	}
};

} // namespace mdinv

#endif // __mdinv_src_mdinv_window_event_hpp__

