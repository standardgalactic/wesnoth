/*
   Copyright (C) 2014 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_DIALOGS_LOBBY_SOUNDS_OPTIONS_HPP_INCLUDED
#define GUI_DIALOGS_LOBBY_SOUNDS_OPTIONS_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"

namespace gui2
{
class tlabel;

class tlobby_sounds_options : public tdialog
{
public:
	/** Constructor. */
	tlobby_sounds_options();

	/**
	 * The display function.
	 *
	 * See @ref tdialog for more information.
	 */
	static void display(CVideo& video)
	{
		tlobby_sounds_options().show(video);
	}

private:
	/** Inherited from tdialog, implemented by REGISTER_DIALOG. */
	virtual const std::string& window_id() const;

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);
};

}

#endif
