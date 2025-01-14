#pragma once

#include "pagegamepad.h"

struct controllerbinds padbinds;

char* gamepad_stick_values[3] = {
	"Unbound",
	"Left Stick",
	"Right Stick"
};

char* gamepad_bind_values[22] = {
	"Unbound",
	"A/Cross",
	"B/Circle",
	"X/Square",
	"Y/Triangle",
	"D-Pad Up",
	"D-Pad Down",
	"D-Pad Left",
	"D-Pad Right",
	"LB/L1",
	"RB/R1",
	"LT/L2",
	"RT/R2",
	"Left Stick/L3",
	"Right Stick/R3",
	"Select/Back",
	"Start/Options",
	"Touchpad",
	"Paddle 1",
	"Paddle 2",
	"Paddle 3",
	"Paddle 4"
};

controllerButton stickLUT[] = {
	CONTROLLER_STICK_UNBOUND,
	CONTROLLER_STICK_LEFT,
	CONTROLLER_STICK_RIGHT,
};

controllerButton buttonLUT[] = {
	CONTROLLER_UNBOUND,
	CONTROLLER_BUTTON_A,
	CONTROLLER_BUTTON_B,
	CONTROLLER_BUTTON_X,
	CONTROLLER_BUTTON_Y,
	CONTROLLER_BUTTON_DPAD_UP,
	CONTROLLER_BUTTON_DPAD_DOWN,
	CONTROLLER_BUTTON_DPAD_LEFT,
	CONTROLLER_BUTTON_DPAD_RIGHT,
	CONTROLLER_BUTTON_LEFTSHOULDER,
	CONTROLLER_BUTTON_RIGHTSHOULDER,
	CONTROLLER_BUTTON_LEFTTRIGGER,
	CONTROLLER_BUTTON_RIGHTTRIGGER,
	CONTROLLER_BUTTON_LEFTSTICK,
	CONTROLLER_BUTTON_RIGHTSTICK,
	CONTROLLER_BUTTON_BACK,
	CONTROLLER_BUTTON_START,
	CONTROLLER_BUTTON_TOUCHPAD,
	CONTROLLER_BUTTON_PADDLE1,
	CONTROLLER_BUTTON_PADDLE2,
	CONTROLLER_BUTTON_PADDLE3,
	CONTROLLER_BUTTON_PADDLE4,
};

void do_button_select(pgui_control* control, int value, controllerButton* target) {
	*target = buttonLUT[value];
	printf("Set button to %d\n", buttonLUT[value]);
}

void do_stick_select(pgui_control* control, int value, controllerStick* target) {
	*target = stickLUT[value];
}

pgui_control* build_button_combobox(int x, int y, int w, int h, pgui_control* parent, controllerButton* target) {
	pgui_control* result = pgui_combobox_create(x, y, w, h, gamepad_bind_values, 22, parent);
	pgui_combobox_set_on_select(result, do_button_select, target);

	return result;
}

pgui_control* build_stick_combobox(int x, int y, int w, int h, pgui_control* parent, controllerStick* target) {
	pgui_control* result = pgui_combobox_create(x, y, w, h, gamepad_stick_values, 3, parent);
	pgui_combobox_set_on_select(result, do_stick_select, target);

	return result;
}

void setButtonBindBox(pgui_control* control, controllerButton bind) {
	int sel = 0;

	switch (bind) {
	case CONTROLLER_UNBOUND:
		sel = 0;
		break;
	case CONTROLLER_BUTTON_A:
		sel = 1;
		break;
	case CONTROLLER_BUTTON_B:
		sel = 2;
		break;
	case CONTROLLER_BUTTON_X:
		sel = 3;
		break;
	case CONTROLLER_BUTTON_Y:
		sel = 4;
		break;
	case CONTROLLER_BUTTON_DPAD_UP:
		sel = 5;
		break;
	case CONTROLLER_BUTTON_DPAD_DOWN:
		sel = 6;
		break;
	case CONTROLLER_BUTTON_DPAD_LEFT:
		sel = 7;
		break;
	case CONTROLLER_BUTTON_DPAD_RIGHT:
		sel = 8;
		break;
	case CONTROLLER_BUTTON_LEFTSHOULDER:
		sel = 9;
		break;
	case CONTROLLER_BUTTON_RIGHTSHOULDER:
		sel = 10;
		break;
	case CONTROLLER_BUTTON_LEFTTRIGGER:
		sel = 11;
		break;
	case CONTROLLER_BUTTON_RIGHTTRIGGER:
		sel = 12;
		break;
	case CONTROLLER_BUTTON_LEFTSTICK:
		sel = 13;
		break;
	case CONTROLLER_BUTTON_RIGHTSTICK:
		sel = 14;
		break;
	case CONTROLLER_BUTTON_BACK:
		sel = 15;
		break;
	case CONTROLLER_BUTTON_START:
		sel = 16;
		break;
	case CONTROLLER_BUTTON_TOUCHPAD:
		sel = 17;
		break;
	case CONTROLLER_BUTTON_PADDLE1:
		sel = 18;
		break;
	case CONTROLLER_BUTTON_PADDLE2:
		sel = 19;
		break;
	case CONTROLLER_BUTTON_PADDLE3:
		sel = 20;
		break;
	case CONTROLLER_BUTTON_PADDLE4:
		sel = 21;
		break;
	}

	pgui_combobox_set_selection(control, sel);
}

void setStickBindBox(pgui_control* control, controllerButton bind) {
	int sel = 0;

	switch (bind) {
	case CONTROLLER_STICK_UNBOUND:
		sel = 0;
		break;
	case CONTROLLER_STICK_LEFT:
		sel = 1;
		break;
	case CONTROLLER_STICK_RIGHT:
		sel = 2;
		break;
	}

	pgui_combobox_set_selection(control, sel);
}

void build_gamepad_page(pgui_control* parent) {
	pgui_control* actions_groupbox = pgui_groupbox_create(8, 8, (parent->w / 2) - 8 - 4, parent->h - 8 - 8, "Actions", parent);
	pgui_control* skater_groupbox = pgui_groupbox_create((parent->w / 2) + 4, 8, (parent->w / 2) - 8 - 4, (parent->h / 2) - 8 - 4 + 32, "Skater Controls", parent);
	pgui_control* camera_groupbox = pgui_groupbox_create((parent->w / 2) + 4, (parent->h / 2) + 4 + 32, (parent->w / 2) - 8 - 4, (parent->h / 2) - 8 - 4 - 32, "Camera Controls", parent);

	int label_offset = 4;
	int graphics_v_spacing = 32;
	int skater_v_spacing = 39;
	int camera_v_spacing = 39;
	int box_width = 80;

	// actions

	pgui_label_create(8, 16 + label_offset, 96, 16, "Ollie:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.ollie = build_button_combobox(actions_groupbox->w - 8 - box_width, 16, box_width, 20, actions_groupbox, &(padbinds.ollie));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing), 96, 16, "Grab:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.grab = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing), box_width, 20, actions_groupbox, &(padbinds.grab));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 2), 96, 16, "Flip:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.flip = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing * 2), box_width, 20, actions_groupbox, &(padbinds.kick));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 3), 96, 16, "Grind:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.grind = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing * 3), box_width, 20, actions_groupbox, &(padbinds.grind));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 4), 96, 16, "Spin Left:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.spin_left = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing * 4), box_width, 20, actions_groupbox, &(padbinds.leftSpin));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 5), 96, 16, "Spin Right:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.spin_right = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing * 5), box_width, 20, actions_groupbox, &(padbinds.rightSpin));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 6), 96, 16, "Nollie¹:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.nollie = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing * 6), box_width, 20, actions_groupbox, &(padbinds.nollie));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 7), 96, 16, "Switch¹:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.switch_revert = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing * 7), box_width, 20, actions_groupbox, &(padbinds.switchRevert));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 8), 96, 16, "Caveman²:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.caveman = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing * 8), box_width, 20, actions_groupbox, &(padbinds.caveman));

	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 9), 90, 16, "Alt Caveman²:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.caveman2 = build_button_combobox(actions_groupbox->w - 9 - box_width, 16 + (graphics_v_spacing * 9), box_width, 20, actions_groupbox, &(padbinds.caveman2));
	
	pgui_label_create(8, 16 + label_offset + (graphics_v_spacing * 10), 96, 16, "Pause:", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);
	gamepad_page.pause = build_button_combobox(actions_groupbox->w - 8 - box_width, 16 + (graphics_v_spacing * 10), box_width, 20, actions_groupbox, &(padbinds.menu));

	pgui_label_create_credits(4, parent->h - 36, 180, 14, "¹ Ps2 controls          ² Xbox controls", PGUI_LABEL_JUSTIFY_LEFT, actions_groupbox);

	// skater controls
	pgui_label_create(8, 16 + label_offset, 96, 16, "Forward:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	gamepad_page.forward = build_button_combobox(skater_groupbox->w - 8 - box_width, 16, box_width, 20, skater_groupbox, &(padbinds.up));

	pgui_label_create(8, 16 + label_offset + (skater_v_spacing), 96, 16, "Backward:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	gamepad_page.backward = build_button_combobox(skater_groupbox->w - 8 - box_width, 16 + (skater_v_spacing), box_width, 20, skater_groupbox, &(padbinds.down));

	pgui_label_create(8, 16 + label_offset + (skater_v_spacing * 2), 96, 16, "Left:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	gamepad_page.left = build_button_combobox(skater_groupbox->w - 8 - box_width, 16 + (skater_v_spacing * 2), box_width, 20, skater_groupbox, &(padbinds.left));

	pgui_label_create(8, 16 + label_offset + (skater_v_spacing * 3), 96, 16, "Right:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	gamepad_page.right = build_button_combobox(skater_groupbox->w - 8 - box_width, 16 + (skater_v_spacing * 3), box_width, 20, skater_groupbox, &(padbinds.right));

	pgui_label_create(8, 16 + label_offset + (skater_v_spacing * 4), 96, 16, "Move Stick:", PGUI_LABEL_JUSTIFY_LEFT, skater_groupbox);
	gamepad_page.movement_stick = build_stick_combobox(skater_groupbox->w - 8 - box_width, 16 + (skater_v_spacing * 4), box_width, 20, skater_groupbox, &(padbinds.movement));

	// camera controls
	pgui_label_create(8, 16 + label_offset, 96, 16, "Camera Stick:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	gamepad_page.camera_stick = build_stick_combobox(skater_groupbox->w - 8 - box_width, 16, box_width, 20, camera_groupbox, &(padbinds.camera));

	pgui_label_create(8, 16 + label_offset + (camera_v_spacing), 96, 16, "View Toggle:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	gamepad_page.view_toggle = build_button_combobox(skater_groupbox->w - 8 - box_width, 16 + (camera_v_spacing), box_width, 20, camera_groupbox, &(padbinds.cameraToggle));

	pgui_label_create(8, 16 + label_offset + (camera_v_spacing * 2), 96, 16, "Swivel Lock:", PGUI_LABEL_JUSTIFY_LEFT, camera_groupbox);
	gamepad_page.swivel_lock = build_button_combobox(skater_groupbox->w - 8 - box_width, 16 + (camera_v_spacing * 2), box_width, 20, camera_groupbox, &(padbinds.cameraSwivelLock));
}

void setAllPadBindText() {
	setButtonBindBox(gamepad_page.ollie, padbinds.ollie);
	setButtonBindBox(gamepad_page.grab, padbinds.grab);
	setButtonBindBox(gamepad_page.flip, padbinds.kick);
	setButtonBindBox(gamepad_page.grind, padbinds.grind);
	setButtonBindBox(gamepad_page.spin_left, padbinds.leftSpin);
	setButtonBindBox(gamepad_page.spin_right, padbinds.rightSpin);

	if (controls.ps2_controls) {
		pgui_textbox_set_enabled(gamepad_page.nollie, 1);
		pgui_textbox_set_enabled(gamepad_page.switch_revert, 1);
		pgui_textbox_set_enabled(gamepad_page.caveman, 0);
		pgui_textbox_set_enabled(gamepad_page.caveman2, 0);
	}
	else {
		pgui_textbox_set_enabled(gamepad_page.nollie, 0);
		pgui_textbox_set_enabled(gamepad_page.switch_revert, 0);
		pgui_textbox_set_enabled(gamepad_page.caveman, 1);
		pgui_textbox_set_enabled(gamepad_page.caveman2, 1);

	}
	setButtonBindBox(gamepad_page.nollie, padbinds.nollie);
	setButtonBindBox(gamepad_page.switch_revert, padbinds.switchRevert);
	setButtonBindBox(gamepad_page.caveman, padbinds.caveman);
	setButtonBindBox(gamepad_page.caveman2, padbinds.caveman2);
	setButtonBindBox(gamepad_page.pause, padbinds.menu);

	setButtonBindBox(gamepad_page.forward, padbinds.up);
	setButtonBindBox(gamepad_page.backward, padbinds.down);
	setButtonBindBox(gamepad_page.left, padbinds.left);
	setButtonBindBox(gamepad_page.right, padbinds.right);
	setStickBindBox(gamepad_page.movement_stick, padbinds.movement);

	setStickBindBox(gamepad_page.camera_stick, padbinds.camera);
	setButtonBindBox(gamepad_page.view_toggle, padbinds.cameraToggle);
	setButtonBindBox(gamepad_page.swivel_lock, padbinds.cameraSwivelLock);
}


void set_padbind_settings(struct controllerbinds* mPadbindsOut) {
	mPadbindsOut->ollie = padbinds.ollie;
	mPadbindsOut->grab = padbinds.grab;
	mPadbindsOut->kick = padbinds.kick;
	mPadbindsOut->grind = padbinds.grind;
	mPadbindsOut->leftSpin = padbinds.leftSpin;
	mPadbindsOut->rightSpin = padbinds.rightSpin;
	mPadbindsOut->nollie = padbinds.nollie;
	mPadbindsOut->switchRevert = padbinds.switchRevert;
	mPadbindsOut->caveman = padbinds.caveman;
	mPadbindsOut->caveman2 = padbinds.caveman2;
	mPadbindsOut->menu = padbinds.menu;
	mPadbindsOut->up = padbinds.up;
	mPadbindsOut->down = padbinds.down;
	mPadbindsOut->left = padbinds.left;
	mPadbindsOut->right = padbinds.right;
	mPadbindsOut->movement = padbinds.movement;
	mPadbindsOut->camera = padbinds.camera;
	mPadbindsOut->cameraToggle = padbinds.cameraToggle;
	mPadbindsOut->cameraSwivelLock = padbinds.cameraSwivelLock;
}