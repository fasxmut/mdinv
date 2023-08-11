//
// Copyright (c) 2023 Fas Xmut (fasxmut AT protonmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef __mdinv_src_mdinv_window_event_hpp__
#define __mdinv_src_mdinv_window_event_hpp__

#include <mdinv_config.hpp>
#include <nirtcpp.hpp>
#include <utxcpp/core.hpp>

using namespace std::string_literals;

namespace mdinv
{

class window_event: public nirt::IEventReceiver
{
protected:
	nirt::NirtcppDevice * device;
	nirt::gui::IGUIEnvironment * ngui;
	nirt::scene::ISceneManager * smgr;

	utx::f32 box_slide; // box_slide = box-edge/2
	std::vector<nirt::core::vector3df> vp_centers; // center of every View Port
	std::vector<nirt::scene::ICameraSceneNode *> cameras;

	std::vector<nirt::scene::IAnimatedMeshSceneNode *> added_mesh_list;
public:
	window_event(nirt::NirtcppDevice * device, utx::f32 box_slide):
		device{device},
		box_slide{box_slide}
	{
		ngui = device->getGUIEnvironment();
		smgr = device->getSceneManager();

		this->setup_vp_centers();
		this->setup_cameras();
	}
protected:
	void setup_vp_centers()
	{
		utx::f32 total_side_x = box_slide*2 * mdinv::app_init_info.splitx();
		utx::f32 total_side_y = box_slide*2 * mdinv::app_init_info.splity();

		auto start_pos = nirt::core::vector3df{-total_side_x/2, total_side_y/2, 0}; // UpperLeft
		[[maybe_unused]]
		auto end_pos = -start_pos; // RightBottom

		nirt::core::vector3df pos = start_pos;
		pos.X += box_slide;
		pos.Y -= box_slide;

		for (utx::u32 j=0; j<mdinv::app_init_info.splity(); j++)
		{
			for (utx::u32 i=0; i<mdinv::app_init_info.splitx(); i++)
			{
				//BOOST_ASSERT(( pos.Z == 0 ));
				vp_centers.push_back(pos);
				pos.X += box_slide*2;
			}
			pos.Y -= box_slide*2;
		}
	}
	void setup_cameras()
	{
		[[maybe_unused]]
		utx::u32 camera_count = mdinv::app_init_info.splity() * mdinv::app_init_info.splitx();
		for (utx::u32 j=0; j<mdinv::app_init_info.splity(); j++)
		{
			for (utx::u32 i=0; i<mdinv::app_init_info.splitx(); i++)
			{
				utx::u32 index = j*mdinv::app_init_info.splitx()+i;
				nirt::scene::ICameraSceneNode * camera = smgr->addCameraSceneNode(
					nullptr,
					vp_centers[index]+nirt::core::vector3df{5, 5, box_slide}, // will be ignored.
					vp_centers[index], // will not be ignored.
					-1
				);
				cameras.push_back(camera);
			}
		}
	}
public:
	nirt::scene::ICameraSceneNode * & camera(utx::u32 index)
	{
		return cameras[index];
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
			this->close_all_mesh();
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
			if (this->added_mesh_list.size() >= this->cameras.size())
				throw std::runtime_error{"Added meshes are full!"};
			if (this->vp_centers.size() != this->cameras.size())
				throw std::runtime_error{"vp_centers and cameras size violation!"};
			utx::u32 vp_index = added_mesh_list.size();

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
				throw std::runtime_error{"Loading Mesh Error!"};
			node->setMaterialFlag(nirt::video::EMF_LIGHTING, false);

			node->setPosition(vp_centers[vp_index]);

			const nirt::core::aabbox3df & aabb = node->getBoundingBox();
			const utx::f32 aabb_radius = (aabb.MaxEdge - aabb.MinEdge).getLength() / 2;
			const utx::f32 disy = aabb_radius*3.2f;
			nirt::core::vector3df dirvec{0, 0, disy};

			dirvec = vp_centers[vp_index] - dirvec;

			cameras[vp_index]->setPosition(dirvec);

			this->added_mesh_list.push_back(node);
		}
		catch (const std::exception & err)
		{
			ngui->addMessageBox(
				L"Loading Mesh Error!",
				(utx::s2w("msg: "s + err.what())+L", when loading "+filename.data()).data(),
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
		{
			utx::printe("No mesh to close!");
			return;
		}
		auto itr = this->added_mesh_list.end();
		itr--;
		//(*itr)->drop();
		(*itr)->getParent()->removeChild(*itr);
		this->added_mesh_list.erase(itr);
	}
	void close_all_mesh()
	{
		while (! this->added_mesh_list.empty())
			this->close_last_mesh();
	}
};

} // namespace mdinv

#endif // __mdinv_src_mdinv_window_event_hpp__

