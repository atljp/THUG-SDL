#pragma once
#include "pgui.h"

// gui library

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


int pgui_initialized = 0;
size_t num_windows = 0;
size_t window_list_size = 0;
pgui_control** window_list = NULL;

struct pgui_control;


void pgui_add_child(pgui_control* parent, pgui_control* child) {
	if (!parent->children) {
		parent->children = malloc(sizeof(pgui_control*));

		parent->children_size = 1;
	}
	else if (parent->children_size <= parent->num_children) {
		parent->children = realloc(parent->children, sizeof(pgui_control*) * (parent->children_size + 1));

		parent->children_size += 1;
	}

	parent->children[parent->num_children] = child;

	parent->num_children++;
}

void pgui_control_hide(pgui_control* control, int hidden) {
	if (!hidden || !control->hidden) {
		ShowWindow(control->hwnd, !hidden);
	}
	else {
		ShowWindow(control->hwnd, 0);
	}

	for (int i = 0; i < control->num_children; i++) {
		pgui_control_hide(control->children[i], hidden);
	}
}

void pgui_control_set_hidden(pgui_control* control, int hidden) {
	control->hidden = hidden;

	pgui_control_hide(control, hidden);
}

void pgui_get_hierarchy_position(pgui_control* control, int* x, int* y) {
	if (control->parent) {
		*x += control->parent->x;
		*y += control->parent->y;
		pgui_get_hierarchy_position(control->parent, x, y);
	}
}

pgui_control* pgui_create_control(int x, int y, int w, int h, pgui_control* parent) {
	pgui_control* result = malloc(sizeof(pgui_control));
	result->num_children = 0;
	result->children = NULL;
	result->parent = parent;

	pgui_add_child(parent, result);

	result->x = x;
	result->y = y;
	result->w = w;
	result->h = h;

	return result;
}

pgui_control* findControl(pgui_control* control, HWND target) {
	if (control->hwnd == target) {
		return control;
	}
	else if (control->num_children == 0) {
		return NULL;
	}

	for (int i = 0; i < control->num_children; i++) {
		pgui_control* result = findControl(control->children[i], target);
		if (result) {
			return result;
		}
	}

	return NULL;
}

LRESULT CALLBACK pgui_wndproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	// find hwnd in window list
	pgui_control* self = NULL;

	for (int i = 0; i < num_windows; i++) {
		//printf("hwnd in = 0x%08x, checking 0x%08x\n", hwnd, window_list[i].hwnd);
		if (window_list[i]->hwnd == hwnd) {
			//printf("window found!");
			self = window_list[i];
			break;
		}
	}

	switch (uMsg) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

		EndPaint(hwnd, &ps);

		return 0;
	}
	case WM_CTLCOLOR:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC: {
		if (self) {
			// find this control
			pgui_control* child_control = findControl(self, (HWND)lParam);

			if (child_control) {
				// find parent tab if it exists
				pgui_control* parent_tab = child_control->parent;
				while (parent_tab && parent_tab->type != PGUI_CONTROL_TYPE_TABS) {
					parent_tab = parent_tab->parent;
				}

				if (parent_tab) {
					// create a brush that copies the tab's body 
					if (!parent_tab->tabs.brush) {
						RECT rc;

						GetWindowRect(parent_tab->hwnd, &rc);
						HDC hdc = GetDC(parent_tab->hwnd);
						HDC hdc_new = CreateCompatibleDC(hdc);	// create a new device context to draw our tab into
						HBITMAP hbmp = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);	// create a new bitmap to draw the tab into
						HBITMAP hbmp_old = (HBITMAP)(SelectObject(hdc_new, hbmp));	// replace the device context's bitmap with our new bitmap

						SendMessage(parent_tab->hwnd, WM_PRINTCLIENT, hdc_new, (LPARAM)(PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT));	// draw the tab into our bitmap
						parent_tab->tabs.brush = CreatePatternBrush(hbmp);	// create a brush from the bitmap
						SelectObject(hdc_new, hbmp_old);	// replace the bitmap in the device context

						DeleteObject(hbmp);
						DeleteDC(hdc_new);
						ReleaseDC(parent_tab->hwnd, hdc);
					}

					// use our previously created brush as a background for the control
					RECT rc2;

					HDC hEdit = (HDC)wParam;
					SetBkMode(hEdit, TRANSPARENT);

					GetWindowRect((HWND)lParam, &rc2);	// get control's position
					MapWindowPoints(NULL, parent_tab->hwnd, (LPPOINT)(&rc2), 2);	// convert coordinates into tab's space
					SetBrushOrgEx(hEdit, -rc2.left, -rc2.top, NULL);	// set brush origin to our control's position

					return parent_tab->tabs.brush;
				}
			}
		}
	}
	}

	if (self) {
		pgui_control_wndproc(self, hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

pgui_control* pgui_window_create(int width, int height, char* title) {	// TODO: window styling
	pgui_control* result = malloc(sizeof(pgui_control));
	result->type = PGUI_CONTROL_TYPE_WINDOW;
	result->id = 0;
	result->num_children = 0;
	result->children = NULL;
	result->parent = NULL;
	result->window.current_id = 0x8800;
	result->x = 0;
	result->y = 0;
	result->w = width;
	result->h = height;

	HINSTANCE hinst = GetModuleHandle(NULL);

	const wchar_t CLASS_NAME[] = L"PGUI Window Class";
	HICON hIconSmall = LoadIcon(hinst, MAKEINTRESOURCE(IDI_THUGICO));

	if (!pgui_initialized) {
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_TAB_CLASSES;
		InitCommonControlsEx(&icex);

		LOGFONT lf;
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		hfont = CreateFont(lf.lfHeight, lf.lfWidth,
			lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
			lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
			lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
			lf.lfPitchAndFamily, lf.lfFaceName);
		lf.lfItalic = TRUE;
		hfont_cursive = CreateFont(lf.lfHeight, lf.lfWidth,
			lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
			lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
			lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
			lf.lfPitchAndFamily, lf.lfFaceName);

		WNDCLASS wc = { 0,
			pgui_wndproc,
			0,
			0,
			hinst,
			hIconSmall,
			NULL,
			NULL,
			NULL,
			CLASS_NAME
		};
		RegisterClass(&wc);
	}

	result->hwnd = CreateWindow(CLASS_NAME, title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);
	SendMessage(result->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
	SendMessage(result->hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconSmall);

	RECT rc_client, rc_window;
	POINT pt_diff;
	GetClientRect(result->hwnd, &rc_client);
	GetWindowRect(result->hwnd, &rc_window);
	pt_diff.x = (rc_window.right - rc_window.left) - rc_client.right;
	pt_diff.y = (rc_window.bottom - rc_window.top) - rc_client.bottom;
	MoveWindow(result->hwnd, rc_window.left, rc_window.top, width + pt_diff.x, height + pt_diff.y, TRUE);

	// todo: add window to window list, return window
	if (!window_list) {
		window_list = malloc(sizeof(pgui_control*));

		window_list_size = 1;
	}
	else if (window_list_size == num_windows) {
		window_list = realloc(window_list, sizeof(pgui_control*) * (window_list_size + 1));

		window_list_size += 1;
	}

	window_list[num_windows] = result;
	num_windows += 1;

	return result;
}

void pgui_window_show(pgui_control* control) {
	ShowWindow(control->hwnd, SW_NORMAL);
}

pgui_control* pgui_empty_create(int x, int y, int w, int h, pgui_control* parent) {	// TODO: window styling
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_EMPTY;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->id = 0;

	result->hwnd = NULL;

	// reorder parent(s) probably?  need to remember how win32 orders things

	return result;
}

LRESULT pgui_button_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND: {
		//printf("COMMAND CHILD!!! LO: %d, HI: %d, L: %d\n", LOWORD(wParam), HIWORD(wParam), lParam);
		int controlId = LOWORD(wParam);

		if (controlId == control->id && control->button.onPressed) {
			control->button.onPressed(control, control->button.onPressedData);	// maybe pass in self?
		}

		return 0;
	}
	}
}

pgui_control* pgui_button_create(int x, int y, int w, int h, char* label, pgui_control* parent) {	// TODO: window styling
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_BUTTON;

	result->button.onPressed = NULL;
	result->button.onPressedData = NULL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->id = node->window.current_id;

	result->hwnd = CreateWindow(WC_BUTTON, label, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, x, y, w, h, node->hwnd, node->window.current_id, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	// reorder parent(s) probably?  need to remember how win32 orders things

	node->window.current_id += 1;

	return result;
}

void pgui_button_set_on_press(pgui_control* control, control_callback on_pressed, void* data) {
	control->button.onPressed = on_pressed;
	control->button.onPressedData = data;
}

void pgui_button_set_enabled(pgui_control* control, int enabled) {
	Button_Enable(control->hwnd, enabled);
}

LRESULT pgui_checkbox_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND: {
		//printf("COMMAND CHILD!!! LO: %d, HI: %d, L: %d\n", LOWORD(wParam), HIWORD(wParam), lParam);
		int controlId = LOWORD(wParam);

		if (controlId == control->id) {
			int checkstate = SendMessage(control->hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;

			if (checkstate) {
				SendMessage(control->hwnd, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			else {
				SendMessage(control->hwnd, BM_SETCHECK, BST_CHECKED, 0);
			}

			if (control->checkbox.on_toggle) {
				control->checkbox.on_toggle(control, !checkstate, control->checkbox.on_toggle_data);	// maybe pass in self?
			}
		}

		return 0;
	}
	}
}

pgui_control* pgui_checkbox_create(int x, int y, int w, int h, char* label, pgui_control* parent) {	// TODO: window styling
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_CHECKBOX;

	result->checkbox.on_toggle = NULL;
	result->checkbox.on_toggle_data = NULL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->id = node->window.current_id;

	result->hwnd = CreateWindow(WC_BUTTON, label, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX, x, y, w, h, node->hwnd, node->window.current_id, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	// reorder parent(s) probably?  need to remember how win32 orders things

	node->window.current_id += 1;

	return result;
}

pgui_control* pgui_checkbox_with_tooltip_create(int x, int y, int w, int h, char* label, pgui_control* parent, char* tooltip_text) {	// TODO: window styling
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_CHECKBOX;

	result->checkbox.on_toggle = NULL;
	result->checkbox.on_toggle_data = NULL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->id = node->window.current_id;

	result->hwnd = CreateWindow(WC_BUTTON, label, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_LEFT, x, y, w, h, node->hwnd, node->window.current_id, hinst, NULL);
	HWND hwndTooltip = CreateWindow(TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, node->hwnd, NULL, hinst, NULL);

	// Associate the tooltip with the checkbox 
	TOOLINFO ti; 
	ti.cbSize = sizeof(TOOLINFO); 
	ti.uFlags = TTF_SUBCLASS; 
	ti.hwnd = result->hwnd;
	ti.hinst = hinst; 
	ti.uId = 0; 
	ti.lpszText = (LPCSTR)tooltip_text;
	GetClientRect(result->hwnd, &ti.rect);
	
	SendMessage(hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont_cursive, TRUE);

	// reorder parent(s) probably?  need to remember how win32 orders things

	node->window.current_id += 1;

	return result;
}

void pgui_checkbox_set_on_toggle(pgui_control* control, control_value_callback on_toggle, void* data) {
	control->checkbox.on_toggle = on_toggle;
	control->checkbox.on_toggle_data = data;
}

void pgui_checkbox_set_checked(pgui_control* control, int checked) {
	if (!checked) {
		SendMessage(control->hwnd, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	else {
		SendMessage(control->hwnd, BM_SETCHECK, BST_CHECKED, 0);
	}
}

void pgui_checkbox_set_enabled(pgui_control* control, int enabled) {
	Button_Enable(control->hwnd, enabled);
}

pgui_control* pgui_label_with_tooltip_create(int x, int y, int w, int h, char* label, pgui_label_justify justify, pgui_control* parent, char* tooltip_text) {	// todo: positioning (center, left, etc)
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_LABEL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	int justify_flag = 0;
	if (justify == PGUI_LABEL_JUSTIFY_CENTER) {
		justify_flag = SS_CENTER;
	}
	else if (justify == PGUI_LABEL_JUSTIFY_RIGHT) {
		justify_flag = SS_RIGHT;
	}

	result->hwnd = CreateWindow(WC_STATIC, label, WS_CHILD | WS_VISIBLE | justify_flag, x, y, w, h, node->hwnd, NULL, hinst, NULL);
	HWND hwndTooltip = CreateWindow(TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, node->hwnd, NULL, hinst, NULL);

	// Associate the tooltip with the label
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = result->hwnd;
	ti.hinst = hinst;
	ti.uId = 0;
	ti.lpszText = (LPCSTR)tooltip_text;
	GetClientRect(result->hwnd, &ti.rect);

	SendMessage(hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	// reorder parent(s) probably?  need to remember how win32 orders things

	return result;
}

pgui_control* pgui_label_create(int x, int y, int w, int h, char* label, pgui_label_justify justify, pgui_control* parent) {	// todo: positioning (center, left, etc)
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_LABEL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	int justify_flag = 0;
	if (justify == PGUI_LABEL_JUSTIFY_CENTER) {
		justify_flag = SS_CENTER;
	}
	else if (justify == PGUI_LABEL_JUSTIFY_RIGHT) {
		justify_flag = SS_RIGHT;
	}

	result->hwnd = CreateWindow(WC_STATIC, label, WS_CHILD | WS_VISIBLE | justify_flag, x, y, w, h, node->hwnd, NULL, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	// reorder parent(s) probably?  need to remember how win32 orders things

	return result;
}

pgui_control* pgui_label_create_credits(int x, int y, int w, int h, char* label, pgui_label_justify justify, pgui_control* parent) {	// todo: positioning (center, left, etc)
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_LABEL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	int justify_flag = 0;
	if (justify == PGUI_LABEL_JUSTIFY_CENTER) {
		justify_flag = SS_CENTER;
	}
	else if (justify == PGUI_LABEL_JUSTIFY_RIGHT) {
		justify_flag = SS_RIGHT;
	}

	result->hwnd = CreateWindow(WC_STATIC, label, WS_CHILD | WS_VISIBLE | justify_flag, x, y, w, h, node->hwnd, NULL, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont_cursive, TRUE);

	// reorder parent(s) probably?  need to remember how win32 orders things

	return result;
}



void pgui_label_set_enabled(pgui_control* control, int enabled) {
	Static_Enable(control->hwnd, enabled);
}

pgui_control* pgui_groupbox_create(int x, int y, int w, int h, char* label, pgui_control* parent) {	// todo: positioning (center, left, etc)
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_GROUPBOX;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->hwnd = CreateWindow(WC_BUTTON, label, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_GROUPBOX, x, y, w, h, node->hwnd, NULL, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	// reorder parent(s) probably?  need to remember how win32 orders things

	return result;
}

pgui_control* pgui_textbox_create(int x, int y, int w, int h, char* text, pgui_control* parent) {	// todo: positioning (center, left, etc)
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_TEXTBOX;

	result->textbox.on_focus_gained = NULL;
	result->textbox.on_focus_gained_data = NULL;
	result->textbox.on_focus_lost = NULL;
	result->textbox.on_focus_lost_data = NULL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->id = node->window.current_id;

	result->hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, text, WS_CHILD | WS_VISIBLE, x, y, w, h, node->hwnd, (HMENU)node->window.current_id, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	// reorder parent(s) probably?  need to remember how win32 orders things

	node->window.current_id += 1;

	return result;
}

void pgui_textbox_set_on_focus_gained(pgui_control* control, control_value_callback callback, void* data) {
	control->textbox.on_focus_gained = callback;
	control->textbox.on_focus_gained_data = data;
}

void pgui_textbox_set_on_focus_lost(pgui_control* control, control_value_callback callback, void* data) {
	control->textbox.on_focus_lost = callback;
	control->textbox.on_focus_lost_data = data;
}

void pgui_textbox_set_text(pgui_control* control, char* text) {
	Edit_SetText(control->hwnd, text);
}

void pgui_textbox_get_text(pgui_control* control, char* output_buffer, size_t buffer_size) {
	Edit_GetText(control->hwnd, output_buffer, buffer_size);
}

void pgui_textbox_set_enabled(pgui_control* control, int enabled) {
	Edit_Enable(control->hwnd, enabled);
}

LRESULT pgui_textbox_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND: {
		int controlId = LOWORD(wParam);
		int controlCode = HIWORD(wParam);

		if (controlId == control->id) {
			if (controlCode == EN_SETFOCUS) {
				if (control->textbox.on_focus_gained) {
					control->textbox.on_focus_gained(control, control->textbox.on_focus_gained_data);
				}
			}
			if (controlCode == EN_KILLFOCUS) {
				if (control->textbox.on_focus_lost) {
					control->textbox.on_focus_lost(control, control->textbox.on_focus_lost_data);
				}
			}
		}

		return 0;
	}
	}
}

pgui_control* pgui_combobox_create(int x, int y, int w, int h, char** options, size_t num_options, pgui_control* parent) {	// todo: positioning (center, left, etc)
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_COMBOBOX;

	result->combobox.on_select = NULL;
	result->combobox.on_select_data = NULL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->id = node->window.current_id;

	result->hwnd = CreateWindow(WC_COMBOBOX, "", WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, x, y, w, h, node->hwnd, node->window.current_id, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	for (int i = 0; i < num_options; i++) {
		ComboBox_AddString(result->hwnd, options[i]);
	}

	ComboBox_SetCurSel(result->hwnd, 0);

	// reorder parent(s) probably?  need to remember how win32 orders things

	node->window.current_id += 1;

	return result;
}

LRESULT pgui_combobox_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND: {
		int controlId = LOWORD(wParam);
		int controlCode = HIWORD(wParam);

		if (controlId == control->id) {
			if (controlCode == 1) {
				int idx = ComboBox_GetCurSel(lParam);
				if (control->combobox.on_select) {
					control->combobox.on_select(control, idx, control->combobox.on_select_data);
				}
			}
		}

		return 0;
	}
	}
}



void pgui_combobox_set_on_select(pgui_control* control, control_value_callback on_select, void* data) {
	control->combobox.on_select = on_select;
	control->combobox.on_select_data = data;
}

void pgui_combobox_set_selection(pgui_control* control, int value) {
	ComboBox_SetCurSel(control->hwnd, value);
}

int pgui_combobox_get_selection(pgui_control* control) {
	return ComboBox_GetCurSel(control->hwnd);
}

void pgui_combobox_set_enabled(pgui_control* control, int enabled) {
	ComboBox_Enable(control->hwnd, enabled);
}

pgui_control* pgui_trackbar_create(int x, int y, int w, int h, int range_min, int range_max, pgui_control* parent) {
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_TRACKBAR;

	result->trackbar.on_changed = NULL;
	result->trackbar.on_changed_data = NULL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->id = node->window.current_id;

	result->hwnd = CreateWindow(TRACKBAR_CLASS, "", WS_CHILD | WS_VISIBLE | TBS_HORZ, x, y, w, h, node->hwnd, node->window.current_id, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);
	SendMessage(result->hwnd, TBM_SETRANGEMIN, (WPARAM)TRUE, (LPARAM)range_min);
	SendMessage(result->hwnd, TBM_SETRANGEMAX, (WPARAM)TRUE, (LPARAM)range_max);

	node->window.current_id += 1;

	return result;
}

LRESULT pgui_trackbar_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_HSCROLL: {
		int controlCode = LOWORD(wParam);
		int pos = HIWORD(wParam);
		HWND controlHwnd = (HWND)lParam;

		if (controlHwnd == control->hwnd) {
			if (controlCode == TB_THUMBPOSITION || controlCode == TB_THUMBTRACK) {
				if (control->trackbar.on_changed) {
					control->trackbar.on_changed(control, pos, control->trackbar.on_changed_data);
				}
			}
			if (controlCode == TB_BOTTOM || controlCode == TB_ENDTRACK || controlCode == TB_LINEDOWN || controlCode == TB_LINEUP ||
				controlCode == TB_PAGEDOWN || controlCode == TB_PAGEUP || controlCode == TB_TOP) {
				pos = SendMessage(control->hwnd, TBM_GETPOS, NULL, NULL);
				if (control->trackbar.on_changed) {
					control->trackbar.on_changed(control, pos, control->trackbar.on_changed_data);
				}
			}
		}

		return 0;
	}
	}
}

void pgui_trackbar_set_on_changed(pgui_control* control, control_value_callback on_changed, void* data) {
	control->trackbar.on_changed = on_changed;
	control->trackbar.on_changed_data = data;
}

void pgui_trackbar_set_pos(pgui_control* control, int value) {
	SendMessage(control->hwnd, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)value);
}

void pgui_trackbar_set_enabled(pgui_control* control, int enabled) {
	EnableWindow(control->hwnd, enabled);
}

pgui_control* pgui_tabs_create(int x, int y, int w, int h, char** options, size_t num_options, pgui_control* parent) {	// todo: positioning (center, left, etc)
	pgui_control* result = pgui_create_control(x, y, w, h, parent);
	result->type = PGUI_CONTROL_TYPE_TABS;

	result->tabs.current_tab = 0;
	result->tabs.num_tabs = num_options;
	result->tabs.brush = NULL;

	pgui_get_hierarchy_position(result, &x, &y);

	// get window hwnd
	pgui_control* node = parent;
	while (node->parent) {
		node = node->parent;
	}

	result->id = node->window.current_id;

	result->hwnd = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE, x, y, w, h, node->hwnd, node->window.current_id, hinst, NULL);
	SendMessage(result->hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	for (int i = 0; i < num_options; i++) {
		TCITEM item;
		item.mask = TCIF_TEXT | TCIF_IMAGE;
		item.iImage = -1;

		item.pszText = options[i];
		TabCtrl_InsertItem(result->hwnd, i, &item);
	}

	ComboBox_SetCurSel(result->hwnd, 0);

	RECT tabRect;
	GetClientRect(result->hwnd, &tabRect);
	TabCtrl_AdjustRect(result->hwnd, FALSE, &tabRect);

	// reorder parent(s) probably?  need to remember how win32 orders things

	for (int i = 0; i < num_options; i++) {
		pgui_empty_create(tabRect.left, tabRect.top, tabRect.right - tabRect.left, tabRect.bottom - tabRect.top, result);
	}

	for (int i = 1; i < num_options; i++) {
		pgui_control_set_hidden(result->children[i], 1);
	}

	node->window.current_id += 1;

	pgui_label_create_credits((parent->w) - 160, 12, 190, 14, "Original code by PARTYMANX", PGUI_LABEL_JUSTIFY_LEFT, parent);

	return result;
}

LRESULT pgui_tabs_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_NOTIFY: {
		int controlCode = ((LPNMHDR)lParam)->code;
		int controlId = ((LPNMHDR)lParam)->idFrom;

		if (controlId == control->id) {
			if (controlCode == TCN_SELCHANGE) {
				int tab = TabCtrl_GetCurSel(((LPNMHDR)lParam)->hwndFrom);

				// hide all children on current tab
				pgui_control_set_hidden(control->children[control->tabs.current_tab], 1);

				// show all children on new tab
				pgui_control_set_hidden(control->children[tab], 0);

				// set current tab to new tab
				control->tabs.current_tab = tab;
			}
		}

		return 0;
	}
	}
}

LRESULT pgui_control_wndproc(pgui_control* control, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (control->type) {
	case PGUI_CONTROL_TYPE_BUTTON:
		pgui_button_wndproc(control, hwnd, uMsg, wParam, lParam);
		break;
	case PGUI_CONTROL_TYPE_CHECKBOX:
		pgui_checkbox_wndproc(control, hwnd, uMsg, wParam, lParam);
		break;
	case PGUI_CONTROL_TYPE_TEXTBOX:
		pgui_textbox_wndproc(control, hwnd, uMsg, wParam, lParam);
		break;
	case PGUI_CONTROL_TYPE_COMBOBOX:
		pgui_combobox_wndproc(control, hwnd, uMsg, wParam, lParam);
		break;
	case PGUI_CONTROL_TYPE_TABS:
		pgui_tabs_wndproc(control, hwnd, uMsg, wParam, lParam);
		break;
	case PGUI_CONTROL_TYPE_TRACKBAR:
		pgui_trackbar_wndproc(control, hwnd, uMsg, wParam, lParam);
		break;
	default:
		break;
	}

	for (int i = 0; i < control->num_children; i++) {
		pgui_control_wndproc(control->children[i], hwnd, uMsg, wParam, lParam);
	}

	return 0;
}