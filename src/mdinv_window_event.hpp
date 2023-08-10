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
public:
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
		default:
			break;
		}
		return false;
	}
};

} // namespace mdinv

#endif // __mdinv_src_mdinv_window_event_hpp__

