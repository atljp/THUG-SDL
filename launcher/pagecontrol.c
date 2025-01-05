#include "pagecontrol.h"


struct control_page control_page;
struct controls controls;

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

void build_control_page(pgui_control* parent) {
	pgui_control* general_groupbox = pgui_groupbox_create(8, 8, (parent->w / 2) - 8 - 4, parent->h - 8 - 64, "General control settings", parent);
	pgui_control* buttons_groupbox = pgui_groupbox_create((parent->w / 2) + 4, 8, (parent->w / 2) - 8 - 4, parent->h - 8 - 64, "Button actions", parent);

	int label_offset = 4;
	int graphics_v_spacing = 34;
	int skater_v_spacing = 39;
	int camera_v_spacing = 39;
	int box_width = 80;

	control_page.ps2_controls = pgui_checkbox_create(8, 16, 140, 24, "Use PS2 Controls", general_groupbox);
	control_page.invertrxp1 = pgui_checkbox_create(8, 16 + (24 *1), 140, 24, "Invert right stick X axis", general_groupbox);
	control_page.invertryp1 = pgui_checkbox_create(8, 16 + (24 * 2), 140, 24, "Invert right stick Y axis", general_groupbox);
	control_page.disablerxp1 = pgui_checkbox_create(8, 16 + (24 * 3), 140, 24, "Disable right stick X axis", general_groupbox);
	control_page.disableryp1 = pgui_checkbox_create(8, 16 + (24 * 4), 140, 24, "Disable right stick Y axis", general_groupbox);
	control_page.menubuttons = pgui_label_create(8, 16 + (24 * 5) + 11, 140, 24, "Menu buttons:", PGUI_LABEL_JUSTIFY_LEFT, general_groupbox);
	control_page.menubuttons_combobox = pgui_combobox_create(8 + 88, 16 + (24 * 5) + 8, 80, 24, menubutton_options, 2, general_groupbox);
	control_page.menubuttons_helper = pgui_label_create(8, 16 + (24 * 7) - 12, 170, 200, "- Sets the back button for menus\n- PC default: ESC or return\n- Xbox default: Circle (grab)\n- Ps2 style: Triangle (grind)", PGUI_LABEL_JUSTIFY_LEFT, general_groupbox);

	control_page.cavemankey = pgui_label_create(8, 16, 70, 32, "Caveman button(s):", PGUI_LABEL_JUSTIFY_CENTER, buttons_groupbox);
	control_page.cavemankey_combobox = pgui_combobox_create(8 + 88, 16 + 4, 80, 24, caveman_options, 7, buttons_groupbox);
	control_page.caveman_helper = pgui_label_create(8, (24 * 2), 180, 160, "- PC and XBOX default: R2 or L2\n- A.k.a. Black or White on OG Xbox\n- A.k.a. Caveman1 or Caveman2\n- Ps2 default: R2", PGUI_LABEL_JUSTIFY_LEFT, buttons_groupbox);

	control_page.dropdownkey = pgui_label_create(8, 16 + (24 * 5), 70, 32, "Dropdown button(s):", PGUI_LABEL_JUSTIFY_CENTER, buttons_groupbox);
	control_page.dropdownkey_combobox = pgui_combobox_create(8 + 88, 16 + 4 + (24 * 5), 80, 24, dropdown_options, 5, buttons_groupbox);
	control_page.dropdown_helper = pgui_label_create(8, (24 * 7), 180, 160, "- PC and XBOX default: L1+R1\n- A.k.a. Spin left and Spin right\n- Ps2 default: L1+R1", PGUI_LABEL_JUSTIFY_LEFT, buttons_groupbox);

	control_page.laddergrabkey = pgui_label_create(8, 16 + (24 * 8), 70, 32, "Ladder grab button(s):", PGUI_LABEL_JUSTIFY_CENTER, buttons_groupbox);
	control_page.laddergrabkey_combobox = pgui_combobox_create(8 + 88, 16 + 4 + (24 * 8), 80, 24, laddergrab_options, 2, buttons_groupbox);


	//Set
	pgui_checkbox_set_on_toggle(control_page.ps2_controls, do_setting_checkbox, &(control_page.ps2_controls));


}


void update_control_page() {

	pgui_checkbox_set_checked(control_page.ps2_controls, controls.ps2_controls);

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