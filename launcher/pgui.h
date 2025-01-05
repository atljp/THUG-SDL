#pragma once

#include <stdio.h>

#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <uxtheme.h>

#include <SDL2/SDL.h>
#include "Resources.h"

typedef void (*control_callback)(struct pgui_control*, void*);
typedef void (*control_value_callback)(struct pgui_control*, int, void*);
//typedef void (*control_string_callback)(char *, void *);


typedef enum {
	PGUI_CONTROL_TYPE_WINDOW,
	PGUI_CONTROL_TYPE_EMPTY,
	PGUI_CONTROL_TYPE_BUTTON,
	PGUI_CONTROL_TYPE_CHECKBOX,
	PGUI_CONTROL_TYPE_COMBOBOX,
	PGUI_CONTROL_TYPE_GROUPBOX,
	PGUI_CONTROL_TYPE_TABS,
	PGUI_CONTROL_TYPE_TEXTBOX,
	PGUI_CONTROL_TYPE_LABEL,
	PGUI_CONTROL_TYPE_TRACKBAR,
} pgui_controltype;

typedef enum {
	PGUI_LABEL_JUSTIFY_LEFT,
	PGUI_LABEL_JUSTIFY_CENTER,
	PGUI_LABEL_JUSTIFY_RIGHT,
} pgui_label_justify;

typedef struct {
	int current_id;
} pgui_control_window;

typedef struct {
	control_callback onPressed;
	void* onPressedData;
} pgui_control_button;

typedef struct {
	control_value_callback on_toggle;
	void* on_toggle_data;
} pgui_control_checkbox;

typedef struct {
	control_value_callback on_select;
	void* on_select_data;
} pgui_control_combobox;

typedef struct {
	int num_tabs;
	int current_tab;
	HBRUSH brush;	// used for drawing; brush with the tab's visuals
} pgui_control_tabs;

typedef struct {
	control_callback on_focus_lost;
	void* on_focus_lost_data;
	control_callback on_focus_gained;
	void* on_focus_gained_data;
} pgui_control_textbox;

typedef struct {
	control_value_callback on_changed;
	void* on_changed_data;
} pgui_control_trackbar;

typedef struct pgui_control {
	pgui_controltype type;
	int id;
	HWND hwnd;
	size_t num_children;
	size_t children_size;

	int x;
	int y;
	int w;
	int h;

	int hidden;

	struct pgui_control** children;
	struct pgui_control* parent;
	union {
		pgui_control_window window;
		pgui_control_button button;
		pgui_control_checkbox checkbox;
		pgui_control_combobox combobox;
		pgui_control_tabs tabs;
		pgui_control_textbox textbox;
		pgui_control_trackbar trackbar;
	};
} pgui_control;

HINSTANCE hinst;
HFONT hfont;
HFONT hfont_cursive;
HFONT hfont_cursive_gray;

LRESULT pgui_control_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void pgui_add_child(pgui_control* parent, pgui_control* child);
void pgui_control_hide(pgui_control* control, int hidden);
void pgui_control_set_hidden(pgui_control* control, int hidden);
void pgui_get_hierarchy_position(pgui_control* control, int* x, int* y);
pgui_control* pgui_create_control(int x, int y, int w, int h, pgui_control* parent);
pgui_control* findControl(pgui_control* control, HWND target);
LRESULT CALLBACK pgui_wndproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
pgui_control* pgui_window_create(int width, int height, char* title);
void pgui_window_show(pgui_control* control);
pgui_control* pgui_empty_create(int x, int y, int w, int h, pgui_control* parent);
LRESULT pgui_button_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
pgui_control* pgui_button_create(int x, int y, int w, int h, char* label, pgui_control* parent);
void pgui_button_set_on_press(pgui_control* control, control_callback on_pressed, void* data);
void pgui_button_set_enabled(pgui_control* control, int enabled);
LRESULT pgui_checkbox_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
pgui_control* pgui_checkbox_create(int x, int y, int w, int h, char* label, pgui_control* parent);
pgui_control* pgui_checkbox_with_tooltip_create(int x, int y, int w, int h, char* label, pgui_control* parent, char* tooltip_text);
void pgui_checkbox_set_on_toggle(pgui_control* control, control_value_callback on_toggle, void* data);
void pgui_checkbox_set_checked(pgui_control* control, int checked);
void pgui_checkbox_set_enabled(pgui_control* control, int enabled);
pgui_control* pgui_label_create(int x, int y, int w, int h, char* label, pgui_label_justify justify, pgui_control* parent);
pgui_control* pgui_label_create_credits(int x, int y, int w, int h, char* label, pgui_label_justify justify, pgui_control* parent);
pgui_control* pgui_label_with_tooltip_create(int x, int y, int w, int h, char* label, pgui_label_justify justify, pgui_control* parent, char* tooltip_text);
void pgui_label_set_enabled(pgui_control* control, int enabled);
pgui_control* pgui_groupbox_create(int x, int y, int w, int h, char* label, pgui_control* parent);
pgui_control* pgui_textbox_create(int x, int y, int w, int h, char* text, pgui_control* parent);
void pgui_textbox_set_on_focus_gained(pgui_control* control, control_value_callback callback, void* data);
void pgui_textbox_set_on_focus_lost(pgui_control* control, control_value_callback callback, void* data);
void pgui_textbox_set_text(pgui_control* control, char* text);
void pgui_textbox_get_text(pgui_control* control, char* output_buffer, size_t buffer_size);
void pgui_textbox_set_enabled(pgui_control* control, int enabled);
LRESULT pgui_textbox_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
pgui_control* pgui_combobox_create(int x, int y, int w, int h, char** options, size_t num_options, pgui_control* parent);
LRESULT pgui_combobox_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void pgui_combobox_set_on_select(pgui_control* control, control_value_callback on_select, void* data);
void pgui_combobox_set_selection(pgui_control* control, int value);
int pgui_combobox_get_selection(pgui_control* control);
void pgui_combobox_set_enabled(pgui_control* control, int enabled);
pgui_control* pgui_trackbar_create(int x, int y, int w, int h, int range_min, int range_max, pgui_control* parent);
LRESULT pgui_trackbar_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void pgui_trackbar_set_on_changed(pgui_control* control, control_value_callback on_changed, void* data);
void pgui_trackbar_set_pos(pgui_control* control, int value);
void pgui_trackbar_set_enabled(pgui_control* control, int enabled);
pgui_control* pgui_tabs_create(int x, int y, int w, int h, char** options, size_t num_options, pgui_control* parent);
LRESULT pgui_tabs_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT pgui_control_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
