#include "pagecontrol.h"


struct control_page control_page;

char* caveman_options[] = {
	"Default",
	"LB/L1",
	"RB/R1",
	"LT/L2",
	"RT/R2",
	"L1+R1",
	"L2+R2",
	//; PC default: black or white which is R2 or L2 (caveman1 or caveman2 in the launcher), Ps2 default: L1+R1"
};

char* dropdown_options[] = {
	"Default",
	"L1",
	"R1",
	"L2",
	"R2",
	//1 = default, 2 = L1, 3 = R1, 4 = L2, 5 = R2 ; PC default: L1+R1, Ps2 default: R2
};

char* menubutton_options[] = {
	"Default",
	"Ps2 style",
};

char* laddergrab_options[] = {
	"R1",
	"L1",
};

void check_nollieandswitch_keys(pgui_control* control, int value, void* data) {
	if (value) {
		pgui_textbox_set_enabled(keyboard_page.nollie, 1);
		pgui_textbox_set_enabled(keyboard_page.switch_revert, 1);
		pgui_textbox_set_enabled(keyboard_page.caveman, 0);
		pgui_textbox_set_enabled(keyboard_page.caveman2, 0);

		pgui_textbox_set_enabled(gamepad_page.nollie, 1);
		pgui_textbox_set_enabled(gamepad_page.switch_revert, 1);
		pgui_textbox_set_enabled(gamepad_page.caveman, 0);
		pgui_textbox_set_enabled(gamepad_page.caveman2, 0);
	}
	else {
		pgui_textbox_set_enabled(keyboard_page.nollie, 0);
		pgui_textbox_set_enabled(keyboard_page.switch_revert, 0);
		pgui_textbox_set_enabled(keyboard_page.caveman, 1);
		pgui_textbox_set_enabled(keyboard_page.caveman2, 1);

		pgui_textbox_set_enabled(gamepad_page.nollie, 0);
		pgui_textbox_set_enabled(gamepad_page.switch_revert, 0);
		pgui_textbox_set_enabled(gamepad_page.caveman, 1);
		pgui_textbox_set_enabled(gamepad_page.caveman2, 1);
	}
	do_setting_checkbox(control, value, data);
}

void build_control_page(pgui_control* parent) {
	pgui_control* general_groupbox = pgui_groupbox_create(8, 8, (parent->w / 2) - 8 - 4, parent->h - 8 - 110, "General control settings", parent);
	pgui_control* buttons_groupbox = pgui_groupbox_create((parent->w / 2) + 4, 8, (parent->w / 2) - 8 - 4, parent->h - 8 - 110, "Button actions", parent);

	int label_offset = 4;
	int graphics_v_spacing = 34;
	int skater_v_spacing = 39;
	int camera_v_spacing = 39;
	int box_width = 80;

	control_page.ps2_controls = pgui_checkbox_create(8, 16, 140, 24, "Use PS2 Controls*", general_groupbox);
	control_page.invertrxp1 = pgui_checkbox_create(8, 16 + (24 *1), 140, 24, "Invert right stick X axis", general_groupbox);
	control_page.invertryp1 = pgui_checkbox_create(8, 16 + (24 * 2), 140, 24, "Invert right stick Y axis", general_groupbox);
	control_page.disablerxp1 = pgui_checkbox_create(8, 16 + (24 * 3), 140, 24, "Disable right stick X axis", general_groupbox);
	control_page.disableryp1 = pgui_checkbox_create(8, 16 + (24 * 4), 140, 24, "Disable right stick Y axis", general_groupbox);
	control_page.menubuttons = pgui_label_create(8, 16 + (24 * 5) + 11, 140, 24, "Menu buttons:", PGUI_LABEL_JUSTIFY_LEFT, general_groupbox);
	control_page.menubuttons_combobox = pgui_combobox_create(8 + 88, 16 + (24 * 5) + 8, 80, 24, menubutton_options, 2, general_groupbox);
	control_page.menubuttons_helper = pgui_label_create(8, 16 + (24 * 7) - 12, 170, 80, "- Sets the back button for menus\n- PC default: ESC or return\n- Xbox default: Circle (grab)\n- Ps2 style: Triangle (grind)", PGUI_LABEL_JUSTIFY_LEFT, general_groupbox);

	control_page.cavemankey = pgui_label_create(8, 16, 70, 32, "Caveman button(s):", PGUI_LABEL_JUSTIFY_CENTER, buttons_groupbox);
	control_page.cavemankey_combobox = pgui_combobox_create(8 + 88, 16 + 4, 80, 24, caveman_options, 7, buttons_groupbox);
	control_page.caveman_helper = pgui_label_create(8, (24 * 2), 180, 160, "- PC and Xbox default: R1 or L1\n-- Black or White on Xbox (Duke)\n-- RB or LB on Xbox\n-- Caveman1 or Caveman2 on PC\n- Ps2 default: L1+R1", PGUI_LABEL_JUSTIFY_LEFT, buttons_groupbox);

	control_page.dropdownkey = pgui_label_create(8, 16 + (24 * 5), 70, 32, "Dropdown button(s):", PGUI_LABEL_JUSTIFY_CENTER, buttons_groupbox);
	control_page.dropdownkey_combobox = pgui_combobox_create(8 + 88, 16 + 4 + (24 * 5), 80, 24, dropdown_options, 5, buttons_groupbox);
	control_page.dropdown_helper = pgui_label_create(8, (24 * 7), 180, 80, "- PC and Xbox default: R2+L2\n--Spin right and Spin left on PC\n--RT and LT on Xbox\n- Ps2 default: R2", PGUI_LABEL_JUSTIFY_LEFT, buttons_groupbox);

	control_page.laddergrabkey = pgui_label_create(8, 24 + (24 * 9), 70, 32, "Ladder grab button:", PGUI_LABEL_JUSTIFY_CENTER, buttons_groupbox);
	control_page.laddergrabkey_combobox = pgui_combobox_create(8 + 88, 24 + 4 + (24 * 9), 80, 24, laddergrab_options, 2, buttons_groupbox);

	control_page.ps2_helper = pgui_label_create(8, 16 + (24 * 12) + 16 , (parent->w) + - 24 , 90, "* Using Ps2 controls will set getting of board to R1 + L1\!\n   Not using Ps2 controls will merge SpinRight + Switch and SpinLeft + Nollie onto    the spin keys!\n   Make sure to reset the pad bindings after chaning your preset!", PGUI_LABEL_JUSTIFY_LEFT, parent);
	
	// **************************
	// SET SETTINGS
	//***************************
	pgui_checkbox_set_on_toggle(control_page.ps2_controls, check_nollieandswitch_keys, &(controls.ps2_controls));
	pgui_checkbox_set_on_toggle(control_page.invertrxp1, do_setting_checkbox, &(controls.invertrxp1));
	pgui_checkbox_set_on_toggle(control_page.invertryp1, do_setting_checkbox, &(controls.invertryp1));
	pgui_checkbox_set_on_toggle(control_page.disablerxp1, do_setting_checkbox, &(controls.disablerxp1));
	pgui_checkbox_set_on_toggle(control_page.disableryp1, do_setting_checkbox, &(controls.disableryp1));

	pgui_combobox_set_on_select(control_page.menubuttons_combobox, set_menu_combobox, &(controls.menubuttons));
	pgui_combobox_set_on_select(control_page.cavemankey_combobox, set_menu_combobox, &(controls.cavemankey));
	pgui_combobox_set_on_select(control_page.dropdownkey_combobox, set_menu_combobox, &(controls.dropdownkey));
	pgui_combobox_set_on_select(control_page.laddergrabkey_combobox, set_menu_combobox, &(controls.laddergrabkey));


}


void update_control_page() {
	pgui_checkbox_set_checked(control_page.ps2_controls, controls.ps2_controls);
	pgui_checkbox_set_checked(control_page.invertrxp1, controls.invertrxp1);
	pgui_checkbox_set_checked(control_page.invertryp1, controls.invertryp1);
	pgui_checkbox_set_checked(control_page.disablerxp1, controls.disablerxp1);
	pgui_checkbox_set_checked(control_page.disableryp1, controls.disableryp1);
	pgui_combobox_set_selection(control_page.menubuttons_combobox, controls.menubuttons-1);
	pgui_combobox_set_selection(control_page.cavemankey_combobox, controls.cavemankey-1);
	pgui_combobox_set_selection(control_page.dropdownkey_combobox, controls.dropdownkey-1);
	pgui_combobox_set_selection(control_page.laddergrabkey_combobox, controls.laddergrabkey-1);
}

void set_control_settings(struct controls* mControlsOut) {

	mControlsOut->ps2_controls = controls.ps2_controls;
	mControlsOut->dropdownkey = controls.dropdownkey;
	mControlsOut->laddergrabkey = controls.laddergrabkey;
	mControlsOut->cavemankey = controls.cavemankey;
	mControlsOut->menubuttons = controls.menubuttons;
	mControlsOut->invertrxp1 = controls.invertrxp1;
	mControlsOut->invertryp1 = controls.invertryp1;
	mControlsOut->disablerxp1 = controls.disablerxp1;
	mControlsOut->disableryp1 = controls.disableryp1;
}