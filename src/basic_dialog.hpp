/* $Id$ */
/*
   Copyright (C) 2006 by Patrick Parker <patrick_x99@hotmail.com>
   wesnoth widget Copyright (C) 2003-5 by David White <davidnwhite@verizon.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef BASIC_DIALOG_H_INCLUDED
#define BASIC_DIALOG_H_INCLUDED

#include "show_dialog.hpp"

#include "widgets/label.hpp"
#include "widgets/textbox.hpp"
#include "widgets/button.hpp"
#include "widgets/menu.hpp"
#include "key.hpp"
#include "sdl_utils.hpp"

namespace gui {

struct style {
	style(std::string const& p, int br) : panel(p), blur_radius(br) {}
	std::string	panel;
	int	blur_radius;
};

struct dialog_process_info
{
public:
	dialog_process_info() : left_button(true), right_button(true), key_down(true),
		first_time(true), double_clicked(false), selection(-1), clear_buttons_(false)
	{}
	void clear_buttons() {
		clear_buttons_ = true;
	}
	void cycle() {
		if(clear_buttons_) {
			left_button = true;
			right_button = true;
			key_down = true;
			clear_buttons_ = false;
		} else {
			left_button = new_left_button;
			right_button = new_right_button;
			key_down = new_key_down;
		}
	}
	CKey key;
	bool left_button, right_button, key_down, first_time, double_clicked;
	bool new_left_button, new_right_button, new_key_down;
	int selection;
private:
	bool clear_buttons_;
};

class dialog_image : public widget {
public:
	dialog_image(label *const caption, CVideo &video, surface img) : widget(video, false),
	  surf_(img), caption_(caption)
	{
		if(!img.null()) {
			set_measurements(img->w, img->h);
		}
	}
	~dialog_image() { delete caption_; }

	//surface surface() const { return surf_; }
	label *caption() const { return caption_; }
	void draw_contents();

	handler_vector handler_members() {
		handler_vector h;
		if(caption_) h.push_back(caption_);
		return h;
	}
private:

	surface surf_;
	label *caption_;
};

class dialog_textbox : public textbox {
public:
	dialog_textbox(label *const label_widget, CVideo &video, int width, const std::string& text="", bool editable=true, size_t max_size = 256, double alpha = 0.4, double alpha_focus = 0.2)
		: textbox(video, width, text, editable, max_size, alpha, alpha_focus, false),
		label_(label_widget)
	{}
	~dialog_textbox() { delete label_; }

	label *get_label() const { return label_; }

	handler_vector handler_members() {
		handler_vector h = textbox::handler_members();
		if(label_) h.push_back(label_);
		return h;
	}
private:

	label *label_;
};

class dialog_button : public button {
public:
	dialog_button(CVideo& video, const std::string& label, TYPE type=TYPE_PRESS,
		int simple_result=CONTINUE_DIALOG, dialog_button_action *handler=NULL)
		: button(video,label,type,"",DEFAULT_SPACE,false), simple_result_(simple_result),
		parent_(NULL), handler_(handler)
	{}
	void set_parent(class basic_dialog *parent) {
		parent_ = parent;
	}
	bool is_option() const {
		return (type_ == TYPE_CHECK);
	}
	virtual int action(dialog_process_info &info);
protected:
	class basic_dialog *dialog() const { return parent_; }
	const int simple_result_;
private:
	class basic_dialog *parent_;
	dialog_button_action *handler_;
};

class standard_dialog_button : public dialog_button {
public:
	standard_dialog_button(CVideo& video, const std::string& label, const int index, const bool is_last)
		: dialog_button(video,label,TYPE_PRESS,index), is_last_(is_last)
	{}
	int action(dialog_process_info &info);
private:
	const bool is_last_;
};


class basic_dialog {
public:
	enum BUTTON_LOCATION { BUTTON_STANDARD, BUTTON_EXTRA, BUTTON_EXTRA_LEFT, BUTTON_CHECKBOX, BUTTON_CHECKBOX_LEFT };
	struct dimension_measurements {
		dimension_measurements();
		int x, y;
		SDL_Rect interior, message, textbox;
		unsigned int menu_width;
		std::map<preview_pane *const, SDL_Rect > panes;
		int label_x, label_y;
		int menu_x, menu_y, menu_height;
		int image_x, image_y, caption_x, caption_y;
		std::map<dialog_button *const, std::pair<int,int> > buttons;
		//use get_frame().get_layout() to check frame dimensions
	};
private:
	typedef std::vector<preview_pane *>::iterator pp_iterator;
	typedef std::vector<preview_pane *>::const_iterator pp_const_iterator;
	typedef std::vector<dialog_button *>::iterator button_iterator;
	typedef std::vector<dialog_button *>::const_iterator button_const_iterator;
	typedef std::vector< std::pair<dialog_button *, BUTTON_LOCATION> >::iterator button_pool_iterator;
	typedef std::vector< std::pair<dialog_button *, BUTTON_LOCATION> >::const_iterator button_pool_const_iterator;

public:

	//Static members
	static const struct style default_style;
	static const struct style message_style;
	static const struct style titlescreen_style;
	static const struct style hotkeys_style;
	static const int message_font_size;
	static const int caption_font_size;
	static const int max_menu_width;
	static const size_t left_padding;
	static const size_t right_padding;
	static const size_t image_h_pad;
	static const size_t top_padding;
	static const size_t bottom_padding;

	//Constructor & destructor
	//dialog - throws button::error() if standard buttons fail to initialize
	//         throws utils::invalid_utf8_exception() if message is invalid
	basic_dialog(display &disp, const std::string& title="", const std::string& message="",
				const DIALOG_TYPE type=MESSAGE, 
				const struct style *dialog_style=&default_style);
	virtual ~basic_dialog();

	//Adding components - the dialog will manage the memory of these widgets,
	//therefore do not attempt to reference its widgets after destroying it
	void set_image(dialog_image *const img) { delete image_; image_ = img; }
	void set_image(surface surf, const std::string &caption="");
	void set_menu(menu *const m) { if(menu_ != empty_menu) delete menu_; menu_ = m; }
	void set_menu(const std::vector<std::string> & menu_items);
	//add_pane - preview panes are not currently memory managed (for backwards compat)

	void add_pane(preview_pane *const pp) { preview_panes_.push_back(pp); }
	void set_textbox(dialog_textbox *const box) {
		delete text_widget_;
		text_widget_ = box;
	}
	void set_textbox(const std::string& text_widget_label="",
				const std::string &text_widget_text="",
				const int text_widget_max_chars = 256,
				const unsigned int text_box_width = font::relative_size(350));
	void add_button(dialog_button *const btn, BUTTON_LOCATION loc);
	void add_button(dialog_button_info btn_info, BUTTON_LOCATION loc=BUTTON_EXTRA);
	void add_option(const std::string& label, bool checked=false, BUTTON_LOCATION loc=BUTTON_CHECKBOX);

	//Specific preparations
	//layout - determines dialog measurements based on all components
	virtual dimension_measurements layout(int xloc=-1, int yloc=-1);
	void set_layout(dimension_measurements &new_dim);
	dimension_measurements get_layout() const { return dim_; }

	//Launching the dialog
	//show - the return value of this method should be the same as result()
	virtual int show(int xloc, int yloc);
	virtual int show();

	//Results
	int result() const { return result_; }
	menu &get_menu();
	bool done() const { return (result_ != CONTINUE_DIALOG); }
	const std::string textbox_text() const { return text_widget_->text();}
	dialog_textbox& get_textbox() const { return *text_widget_; }
	const bool option_checked(unsigned int option_index=0);
	display& get_display() { return disp_; }

protected:
	void set_result(const int result) { result_ = result; }

	//action - invoked at the end of the dialog-processing loop
	virtual void action(dialog_process_info &dp_info);
	//refresh - forces the display to refresh
	void refresh();

	label& get_message() const { return *message_; }
	virtual dialog_frame& get_frame(button *help_button=NULL);

	display &disp_;

	//process - execute a single dialog processing loop 
	//and return the result
	virtual int process(dialog_process_info &info);
	virtual void update_widget_positions();

private:
	void draw_frame();
	void draw_contents();

	//Members
	dialog_image *image_;
	const std::string title_;
 	const struct style *style_;
	label *title_widget_, *message_;
	const DIALOG_TYPE type_;
	gui::menu *menu_;
	std::vector<preview_pane*> preview_panes_;
	std::vector< std::pair<dialog_button*,BUTTON_LOCATION> > button_pool_;
	std::vector<dialog_button*> standard_buttons_;
	std::vector<dialog_button*> extra_buttons_;
	std::vector<button*> frame_buttons_;
	dialog_textbox *text_widget_;
	dialog_frame *frame_;
	surface_restorer *bg_restore_;
	dimension_measurements dim_;
	int result_;
};

typedef Uint32 msecs;
const msecs three_blinks = 300;	// 3 times the 0.1sec human reflex-arc time

class message_dialog : public gui::basic_dialog
{
public:
	message_dialog(display &disp, const std::string& title="", const std::string& message="", const gui::DIALOG_TYPE type=gui::MESSAGE)
		: basic_dialog(disp, title, message, type, &message_style), prevent_misclick_until_(0)
	{}
	~message_dialog();
	int show(msecs minimum_lifetime = three_blinks);
protected:
	void action(gui::dialog_process_info &dp_info);
private:
	msecs prevent_misclick_until_;
};

} //end namespace gui

#endif
