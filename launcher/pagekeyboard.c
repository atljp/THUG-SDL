#include "pagekeyboard.h"


struct keybinds keybinds;
struct settings settings;

void do_keybind_select(pgui_control* control, keybind_data* data) {
	doKeyBind(data->name, data->target, control);
}

pgui_control* build_keybind_textbox(int x, int y, int w, int h, pgui_control* parent, char* name, SDL_Scancode* target) {
	pgui_control* result = pgui_textbox_create(x, y, w, h, "", parent);

	keybind_data* data = malloc(sizeof(keybind_data));
	data->name = name;
	data->target = target;

	pgui_textbox_set_on_focus_gained(result, do_keybind_select, data);

	return result;
}

void build_keyboard_page(pgui_control* parent) {
	pgui_control* actions_groupbox = pgui_groupbox_create(8, 8, (parent->w / 2) - 8 - 4, (parent->h / 2) - 8 - 4 + 64, "Actions", parent);
	pgui_control* park_editor_groupbox = pgui_groupbox_create(8, (parent->h / 2) + 4 + 64, (parent->w / 2) - 8 - 4, (parent->h / 2) - 8 - 4 - 64, "Park Editor Specific Controls", parent);
	pgui_control* skater_groupbox = pgui_groupbox_create((parent->w / 2) + 4, 8, (parent->w / 2) - 8 - 4, (parent->h / 2) - 8 - 4 - 16, "Skater Controls", parent);
	pgui_control* camera_groupbox = pgui_groupbox_create((parent->w / 2) + 4, (parent->h / 2) + 4 - 16, (parent->w / 2) - 8 - 4, (parent->h / 2) - 8 - 4 + 16, "Camera Controls", parent);

	int label_offset = 4;
	int graphics_v_spacing = 22;
	int park_editor_v_spacing = 23;
	int skater_v_spacing = 28;
	int camera_v_spacing = 28;

	// actions

	pgui_label_create(8, 16 + label_offset, 96, 16, "Ollie (or jump):", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.ollie = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16, 64, 20, actions_groupbox, "Ollie", &(keybinds.ollie));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing), 96, 16, "Grab tricks:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.grab = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing), 64, 20, actions_groupbox, "Grab", &(keybinds.grab));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 2), 96, 16, "Flip tricks:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.flip = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing * 2), 64, 20, actions_groupbox, "Flip", &(keybinds.flip));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 3), 96, 16, "Grind tricks:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.grind = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing * 3), 64, 20, actions_groupbox, "Grind", &(keybinds.grind));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 4), 96, 16, "Nollie (Rotate left):", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.spin_left = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing * 4), 64, 20, actions_groupbox, "Spin Left", &(keybinds.spinLeft));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 5), 120, 16, "Switch (Rotate right):", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.spin_right = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing * 5), 64, 20, actions_groupbox, "Spin Right", &(keybinds.spinRight));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 6), 96, 16, "Nollie*:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.nollie = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing * 6), 64, 20, actions_groupbox, "Nollie", &(keybinds.nollie));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 7), 96, 16, "Switch*:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.switch_revert = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing * 7), 64, 20, actions_groupbox, "Switch", &(keybinds.switchRevert));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 8), 96, 16, "Get off board:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.caveman = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing * 8), 64, 20, actions_groupbox, "Caveman1", &(keybinds.caveman));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 9), 96, 16, "Alt. get off board:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	keyboard_page.caveman2 = build_keybind_textbox(actions_groupbox->w - 8 - 64, 16 + (graphics_v_spacing * 9), 64, 20, actions_groupbox, "Caveman2", &(keybinds.caveman2));

	pgui_label_create_credits(4, (parent->h / 2) + 36, 120, 14, "* Only for Ps2 controls", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);

	// park editor (d-pad)
	pgui_label_create(8, 16 + label_offset, 96, 16, "Item up:", PGUI_LABEL_JUSTIFY_LEFT, park_editor_groupbox);
	keyboard_page.item_up = build_keybind_textbox(park_editor_groupbox->w - 8 - 64, 16, 64, 20, park_editor_groupbox, "Selection Up", &(keybinds.item_up));

	pgui_label_create(8, 16 + label_offset + (park_editor_v_spacing), 96, 16, "Item down:", PGUI_LABEL_JUSTIFY_LEFT, park_editor_groupbox);
	keyboard_page.item_down = build_keybind_textbox(park_editor_groupbox->w - 8 - 64, 16 + (park_editor_v_spacing), 64, 20, park_editor_groupbox, "Selection Down", &(keybinds.item_down));

	pgui_label_create(8, 16 + label_offset + (park_editor_v_spacing * 2), 96, 16, "Item left:", PGUI_LABEL_JUSTIFY_LEFT, park_editor_groupbox);
	keyboard_page.item_left = build_keybind_textbox(park_editor_groupbox->w - 8 - 64, 16 + (park_editor_v_spacing * 2), 64, 20, park_editor_groupbox, "Selection Left", &(keybinds.item_left));

	pgui_label_create(8, 16 + label_offset + (park_editor_v_spacing * 3), 96, 16, "Item right:", PGUI_LABEL_JUSTIFY_LEFT, park_editor_groupbox);
	keyboard_page.item_right = build_keybind_textbox(park_editor_groupbox->w - 8 - 64, 16 + (park_editor_v_spacing * 3), 64, 20, park_editor_groupbox, "Selection Right", &(keybinds.item_right));

	// skater controls
	pgui_label_create(8, 16 + label_offset, 96, 16, "Forward:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	keyboard_page.forward = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16, 64, 20, skater_groupbox, "Forward", &(keybinds.forward));

	pgui_label_create(8, 16 + label_offset + (skater_v_spacing), 96, 16, "Backward:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	keyboard_page.backward = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (skater_v_spacing), 64, 20, skater_groupbox, "Backward", &(keybinds.backward));

	pgui_label_create(8, 16 + label_offset + (skater_v_spacing * 2), 96, 16, "Turn left:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	keyboard_page.left = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (skater_v_spacing * 2), 64, 20, skater_groupbox, "Left", &(keybinds.left));

	pgui_label_create(8, 16 + label_offset + (skater_v_spacing * 3), 96, 16, "Turn right:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	keyboard_page.right = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (skater_v_spacing * 3), 64, 20, skater_groupbox, "Right", &(keybinds.right));

	pgui_label_create(8, 16 + label_offset + (skater_v_spacing * 4), 96, 16, "Feeble (Half stick):", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	keyboard_page.feeble = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (skater_v_spacing * 4), 64, 20, skater_groupbox, "Feeble (Half Stick)", &(keybinds.feeble));

	// camera controls
	pgui_label_create(8, 16 + label_offset, 96, 16, "Camera up:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	keyboard_page.camera_up = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16, 64, 20, camera_groupbox, "Camera Up", &(keybinds.camUp));

	pgui_label_create(8, 16 + label_offset + (camera_v_spacing), 96, 16, "Camera down:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	keyboard_page.camera_down = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (camera_v_spacing), 64, 20, camera_groupbox, "Camera Down", &(keybinds.camDown));

	pgui_label_create(8, 16 + label_offset + (camera_v_spacing * 2), 96, 16, "Camera left:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	keyboard_page.camera_left = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (camera_v_spacing * 2), 64, 20, camera_groupbox, "Camera Left", &(keybinds.camLeft));

	pgui_label_create(8, 16 + label_offset + (camera_v_spacing * 3), 96, 16, "Camera right:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	keyboard_page.camera_right = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (camera_v_spacing * 3), 64, 20, camera_groupbox, "Camera Right", &(keybinds.camRight));

	pgui_label_create(8, 16 + label_offset + (camera_v_spacing * 4), 96, 16, "Camera toggle:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	keyboard_page.view_toggle = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (camera_v_spacing * 4), 64, 20, camera_groupbox, "View Toggle", &(keybinds.viewToggle));

	pgui_label_create(8, 16 + label_offset + (camera_v_spacing * 5), 96, 16, "Swivel lock:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	keyboard_page.swivel_lock = build_keybind_textbox(skater_groupbox->w - 8 - 64, 16 + (camera_v_spacing * 5), 64, 20, camera_groupbox, "Swivel Lock", &(keybinds.swivelLock));
}

void setAllBindText() {
	setBindText(keyboard_page.ollie, keybinds.ollie);
	setBindText(keyboard_page.grab, keybinds.grab);
	setBindText(keyboard_page.flip, keybinds.flip);
	setBindText(keyboard_page.grind, keybinds.grind);
	setBindText(keyboard_page.spin_left, keybinds.spinLeft);
	setBindText(keyboard_page.spin_right, keybinds.spinRight);

	if (controls.ps2_controls) {
		pgui_textbox_set_enabled(keyboard_page.nollie, 1);
		pgui_textbox_set_enabled(keyboard_page.switch_revert, 1);
		pgui_textbox_set_enabled(keyboard_page.caveman, 0);
		pgui_textbox_set_enabled(keyboard_page.caveman2, 0);
	}
	else {
		pgui_textbox_set_enabled(keyboard_page.nollie, 0);
		pgui_textbox_set_enabled(keyboard_page.switch_revert, 0);
		pgui_textbox_set_enabled(keyboard_page.caveman, 1);
		pgui_textbox_set_enabled(keyboard_page.caveman2, 1);

	}
	setBindText(keyboard_page.nollie, keybinds.nollie);
	setBindText(keyboard_page.switch_revert, keybinds.switchRevert);
	setBindText(keyboard_page.caveman, keybinds.caveman);
	setBindText(keyboard_page.caveman2, keybinds.caveman2);

	setBindText(keyboard_page.item_up, keybinds.item_up);
	setBindText(keyboard_page.item_down, keybinds.item_down);
	setBindText(keyboard_page.item_left, keybinds.item_left);
	setBindText(keyboard_page.item_right, keybinds.item_right);

	setBindText(keyboard_page.forward, keybinds.forward);
	setBindText(keyboard_page.backward, keybinds.backward);
	setBindText(keyboard_page.left, keybinds.left);
	setBindText(keyboard_page.right, keybinds.right);
	setBindText(keyboard_page.feeble, keybinds.feeble);

	setBindText(keyboard_page.camera_up, keybinds.camUp);
	setBindText(keyboard_page.camera_down, keybinds.camDown);
	setBindText(keyboard_page.camera_left, keybinds.camLeft);
	setBindText(keyboard_page.camera_right, keybinds.camRight);
	setBindText(keyboard_page.view_toggle, keybinds.viewToggle);
	setBindText(keyboard_page.swivel_lock, keybinds.swivelLock);
}

void set_keybind_settings(struct keybinds* mKeybindsOut) {
	mKeybindsOut->ollie = keybinds.ollie;
	mKeybindsOut->grab = keybinds.grab;
	mKeybindsOut->flip = keybinds.flip;
	mKeybindsOut->grind = keybinds.grind;
	mKeybindsOut->spinLeft = keybinds.spinLeft;
	mKeybindsOut->spinRight = keybinds.spinRight;
	mKeybindsOut->nollie = keybinds.nollie;
	mKeybindsOut->switchRevert = keybinds.switchRevert;
	mKeybindsOut->caveman = keybinds.caveman;
	mKeybindsOut->caveman2 = keybinds.caveman2;

	mKeybindsOut->item_up = keybinds.item_up;
	mKeybindsOut->item_down = keybinds.item_down;
	mKeybindsOut->item_left = keybinds.item_left;
	mKeybindsOut->item_right = keybinds.item_right;

	mKeybindsOut->forward = keybinds.forward;
	mKeybindsOut->backward = keybinds.backward;
	mKeybindsOut->left = keybinds.left;
	mKeybindsOut->right = keybinds.right;
	mKeybindsOut->feeble = keybinds.feeble;

	mKeybindsOut->camUp = keybinds.camUp;
	mKeybindsOut->camDown = keybinds.camDown;
	mKeybindsOut->camLeft = keybinds.camLeft;
	mKeybindsOut->camRight = keybinds.camRight;

	mKeybindsOut->viewToggle = keybinds.viewToggle;
	mKeybindsOut->swivelLock = keybinds.swivelLock;
}