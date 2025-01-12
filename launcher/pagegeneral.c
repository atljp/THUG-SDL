#include "pagegeneral.h"

struct general_page general_page;
struct settings settings;

void set_display_mode(pgui_control* control, int value, void* data) {
	if (value != 0) {
		settings.resX = displayModeList[value - 1].width;
		settings.resY = displayModeList[value - 1].height;
	}
	else {
		settings.resX = 0;
		settings.resY = 0;
	}
}

void do_custom_resolution_textbox(pgui_control* control, int* target) {
	char buf[16];
	pgui_textbox_get_text(control, buf, 16);
	*target = atoi(buf);
}

void do_additional_mod_folder(pgui_control* control, char* target) {
	char buf[MAX_PATH];
	pgui_textbox_get_text(control, buf, MAX_PATH);
	strcpy(target, buf);
}


void check_custom_resolution(pgui_control* control, int value, void* data) {
	if (value) {
		pgui_label_set_enabled(general_page.custom_res_x_label, 1);
		pgui_textbox_set_enabled(general_page.custom_res_x, 1);
		pgui_label_set_enabled(general_page.custom_res_y_label, 1);
		pgui_textbox_set_enabled(general_page.custom_res_y, 1);
		pgui_combobox_set_enabled(general_page.resolution_combobox, 0);

		char buf[16];
		pgui_textbox_get_text(general_page.custom_res_x, buf, 16);
		settings.resX = atoi(buf);

		pgui_textbox_get_text(general_page.custom_res_y, buf, 16);
		settings.resY = atoi(buf);
	}
	else {
		pgui_label_set_enabled(general_page.custom_res_x_label, 0);
		pgui_textbox_set_enabled(general_page.custom_res_x, 0);
		pgui_label_set_enabled(general_page.custom_res_y_label, 0);
		pgui_textbox_set_enabled(general_page.custom_res_y, 0);
		pgui_combobox_set_enabled(general_page.resolution_combobox, 1);

		int res_value = pgui_combobox_get_selection(general_page.resolution_combobox);
		if (res_value != 0) {
			settings.resX = displayModeList[res_value - 1].width;
			settings.resY = displayModeList[res_value - 1].height;
		}
		else {
			settings.resX = 0;
			settings.resY = 0;
		}
	}
}

void check_additional_mods(pgui_control* control, int value, void* data) {
	if (value) {
		pgui_textbox_set_enabled(general_page.additional_mods, 1);
	}
	else {
		pgui_textbox_set_enabled(general_page.additional_mods, 0);
	}
	do_setting_checkbox(control, value, data);
}

void do_setting_trackbar(pgui_control* control, int value, int* target) {
	*target = value;
}

void do_distance_clipping_checkbox(pgui_control* control, int value, int* target) {
	if (value) {
		pgui_trackbar_set_enabled(general_page.clipping_distance, 1);
		pgui_checkbox_set_enabled(general_page.fog, 1);
		pgui_label_set_enabled(general_page.clipping_distance_near, 1);
		pgui_label_set_enabled(general_page.clipping_distance_far, 1);
	}
	else {
		pgui_trackbar_set_enabled(general_page.clipping_distance, 0);
		pgui_checkbox_set_enabled(general_page.fog, 0);
		pgui_label_set_enabled(general_page.clipping_distance_near, 0);
		pgui_label_set_enabled(general_page.clipping_distance_far, 0);
	}

	*target = value;
}

void do_console(pgui_control* control, int value, int* target) {
	if (value) {
		pgui_checkbox_set_enabled(general_page.native_log, 1);
		settings.console = 1;
	}
	else {
		pgui_checkbox_set_enabled(general_page.native_log, 0);
		settings.console = 0;
	}
}

void do_native_log(pgui_control* control, int value, int* target) {
	if (value) {
		settings.console = 2;
	}
	else {
		settings.console = 1;
	}
}

char* uicontrol_options[] = {
	"PC",
	"PS2",
	"XBOX",
	"Gamecube",
};

char* shadow_quality_options[] = {
	"Default",
	"High",
	"Very High",
};

char* language_options[] = {
	"English",
	"French",
	"German",
};

char* chatsize_options[] = {
	"Tiny",
	"Small",
	"Default",
	"Big",
};

void build_general_page(pgui_control* parent) {
	initResolutionList();

	pgui_control* resolution_groupbox = pgui_groupbox_create(8, 8, (parent->w / 2) - 8 - 4, (parent->h / 2) - 8 - 4, "Resolution", parent);
	pgui_control* graphics_groupbox = pgui_groupbox_create(8, (parent->h / 2) + 4, (parent->w / 2) - 8 - 4, (parent->h / 2) - 8 - 4, "Graphics", parent);
	pgui_control* misc_groupbox = pgui_groupbox_create((parent->w / 2) + 4, 8, (parent->w / 2) - 8 - 4, (parent->h) - 8 - 8, "Miscellaneous", parent);

	// resolution options
	general_page.resolution_combobox = pgui_combobox_create(8, 16, 160, 24, displayModeStringList, numDisplayModes + 1, resolution_groupbox);
	general_page.custom_resolution = pgui_checkbox_create(8, 16 + 24, 128, 24, "Use Custom Resolution", resolution_groupbox);
	general_page.custom_res_x_label = pgui_label_create(8, 16 + (24 * 2) + 4, 48, 24, "Width:", PGUI_LABEL_JUSTIFY_CENTER, resolution_groupbox);
	general_page.custom_res_x = pgui_textbox_create(8 + 48, 16 + (24 * 2), 48, 20, "", resolution_groupbox);
	general_page.custom_res_y_label = pgui_label_create(8, 15 + (24 * 3) + 4, 48, 24, "Height:", PGUI_LABEL_JUSTIFY_CENTER, resolution_groupbox);
	general_page.custom_res_y = pgui_textbox_create(8 + 48, 12 + (24 * 3) + 4, 48, 20, "", resolution_groupbox);
	general_page.windowed = pgui_checkbox_create(8, 16 + (24 * 4), 128, 24, "Windowed", resolution_groupbox);
	general_page.borderless = pgui_checkbox_create(8, 16 + (24 * 5), 128, 24, "Borderless", resolution_groupbox);
	general_page.savewinpos = pgui_checkbox_create(8, 16 + (24 * 6), 128, 24, "Save Window Position", resolution_groupbox);

	// graphics options
	pgui_label_create(8, 16 + 4, 128, 24, "Shadow quality:", PGUI_LABEL_JUSTIFY_LEFT, graphics_groupbox);
	general_page.shadowquality_combobox = pgui_combobox_create(8 + 88, 16 + 2, 80, 24, shadow_quality_options, 3, graphics_groupbox);
	general_page.antialiasing = pgui_checkbox_create(8, 16 + 24, 128, 24, "Anti-Aliasing", graphics_groupbox);
	general_page.distance_clipping = pgui_checkbox_create(8, 16 + (24 * 2), 128, 24, "Distance Clipping", graphics_groupbox);
	general_page.clipping_distance = pgui_trackbar_create(8, 16 + (24 * 3), 160, 24, 0, 100, graphics_groupbox);
	general_page.clipping_distance_near = pgui_label_create(8, 16 + (24 * 4) + 4, 32, 24, "Near", PGUI_LABEL_JUSTIFY_LEFT, graphics_groupbox);
	general_page.clipping_distance_far = pgui_label_create(160 - 32 + 4, 16 + (24 * 4) + 4, 40, 24, "Far", PGUI_LABEL_JUSTIFY_RIGHT, graphics_groupbox);
	general_page.fog = pgui_checkbox_create(8 + 128, 16 + (24 * 2), 40, 24, "Fog", graphics_groupbox);
	general_page.disable_fs_gamma = pgui_checkbox_create(8, 16 + (24 * 5), 136, 24, "Disable fullscreen gamma", graphics_groupbox);
	general_page.disable_blur = pgui_checkbox_with_tooltip_create(8, 16 + (24 * 6), 136, 24, "Disable Blur Effect", graphics_groupbox, "Workaround for severe artifacts seen on many GPU drivers. The effect may work if the game is run through DXVK");

	// miscellaneous options
	general_page.button_font_label = pgui_label_create(8, 16 + 4, 96, 16, "Button Font:", PGUI_LABEL_JUSTIFY_LEFT, misc_groupbox);
	general_page.button_font = pgui_combobox_create(8 + 82, 16, 80, 24, uicontrol_options, 4, misc_groupbox);
	general_page.language_label = pgui_label_create(8, 16 + 24 + 8, 128, 24, "Language:", PGUI_LABEL_JUSTIFY_LEFT, misc_groupbox);
	general_page.language = pgui_combobox_create(8 + 82, 16 + 20 + 8, 80, 24, language_options, 3, misc_groupbox);
	general_page.console = pgui_checkbox_create(8, 24 + (24 * 2), 60, 24, "Console", misc_groupbox);
	general_page.native_log = pgui_checkbox_create(80, 24 + (24 * 2), 80, 24, "Native log", misc_groupbox);
	general_page.write_file = pgui_checkbox_create(8, 24 + (24 * 3), 128, 24, "Write file", misc_groupbox);
	general_page.append_log = pgui_checkbox_create(8, 24 + (24 * 4), 128, 24, "Append log", misc_groupbox);
	general_page.exception_handler = pgui_checkbox_with_tooltip_create(8, 24 + (24 * 5), 136, 24, "Exception Handler", misc_groupbox, "May not work on Windows 11");
	general_page.intro_movies = pgui_checkbox_create(8, 24 + (24 * 6), 128, 24, "Intro movies", misc_groupbox);
	general_page.boardscuffs = pgui_checkbox_create(8, 24 + (24 * 7), 128, 24, "Boardscuffs", misc_groupbox);
	general_page.no_additional_script_mods = pgui_checkbox_create(8, 24 + (24 * 8), 128, 24, "NoAdditionalScriptMods", misc_groupbox);
	general_page.chat_size_label = pgui_label_create(8, 24 + (24 * 9) + 8, 128, 24, "Chat size:", PGUI_LABEL_JUSTIFY_LEFT, misc_groupbox);
	general_page.chat_size = pgui_combobox_create(8 + 82, 24 + (24 * 9) + 4, 80, 24, chatsize_options, 4, misc_groupbox);
	general_page.chat_wait_time_label = pgui_label_with_tooltip_create(8, 24 + (24 * 10) + 11, 128, 24, "Message time (seconds): ", PGUI_LABEL_JUSTIFY_LEFT, misc_groupbox, "Value between 1 and 120");
	general_page.chat_wait_time = pgui_textbox_create(8 + 132, 24 + (24 * 10) + 9, 30, 20, "", misc_groupbox);
	general_page.additional_mods_checkbox = pgui_checkbox_create(8, 24 + (24 * 13) - 2, 128, 24, "Additional mods:", misc_groupbox);
	general_page.additional_mods = pgui_textbox_create(8, 24 + (24 * 14) - 2, (parent->w / 2) - 32, 24, "", misc_groupbox);

	// **************************
	// SET SETTINGS
	//***************************
	pgui_combobox_set_on_select(general_page.button_font, set_menu_combobox, &(settings.button_font));
	pgui_checkbox_set_on_toggle(general_page.console, do_console, &(settings.console));
	pgui_checkbox_set_on_toggle(general_page.native_log, do_native_log, &(settings.console));
	pgui_checkbox_set_on_toggle(general_page.write_file, do_setting_checkbox, &(settings.writefile));
	pgui_checkbox_set_on_toggle(general_page.append_log, do_setting_checkbox, &(settings.appendlog));
	pgui_checkbox_set_on_toggle(general_page.exception_handler, do_setting_checkbox, &(settings.exceptionhandler));
	pgui_checkbox_set_on_toggle(general_page.intro_movies, do_setting_checkbox, &(settings.intromovies));
	pgui_combobox_set_on_select(general_page.language, set_menu_combobox, &(settings.language));
	pgui_checkbox_set_on_toggle(general_page.boardscuffs, do_setting_checkbox, &(settings.boardscuffs));
	pgui_checkbox_set_on_toggle(general_page.no_additional_script_mods, do_setting_checkbox, &(settings.noadditionalscriptmods));
	pgui_combobox_set_on_select(general_page.chat_size, set_menu_combobox, &(settings.chatsize));
	pgui_textbox_set_on_focus_lost(general_page.chat_wait_time, do_custom_resolution_textbox, &(settings.chatwaittime));
	pgui_checkbox_set_on_toggle(general_page.additional_mods_checkbox, check_additional_mods, &(settings.additionalmods));
	pgui_textbox_set_on_focus_lost(general_page.additional_mods, do_additional_mod_folder, &(settings.additionalmods_folder));

	pgui_checkbox_set_on_toggle(general_page.windowed, do_setting_checkbox, &(settings.windowed));
	pgui_checkbox_set_on_toggle(general_page.borderless, do_setting_checkbox, &(settings.borderless));
	pgui_checkbox_set_on_toggle(general_page.savewinpos, do_setting_checkbox, &(settings.savewinpos));
	pgui_checkbox_set_on_toggle(general_page.antialiasing, do_setting_checkbox, &(settings.antialiasing));
	pgui_combobox_set_on_select(general_page.shadowquality_combobox, set_menu_combobox_no_offset, &(settings.hq_shadows));
	pgui_checkbox_set_on_toggle(general_page.distance_clipping, do_distance_clipping_checkbox, &(settings.distance_clipping));
	pgui_trackbar_set_on_changed(general_page.clipping_distance, do_setting_trackbar, &(settings.clipping_distance));
	pgui_checkbox_set_on_toggle(general_page.fog, do_setting_checkbox, &(settings.fog));
	pgui_checkbox_set_on_toggle(general_page.disable_blur, do_setting_checkbox, &(settings.disable_blur));
	pgui_checkbox_set_on_toggle(general_page.disable_fs_gamma, do_setting_checkbox, &(settings.disable_fsgamma));


	
	pgui_combobox_set_on_select(general_page.resolution_combobox, set_display_mode, NULL);
	pgui_checkbox_set_on_toggle(general_page.custom_resolution, check_custom_resolution, NULL);
	pgui_textbox_set_on_focus_lost(general_page.custom_res_x, do_custom_resolution_textbox, &(settings.resX));
	pgui_textbox_set_on_focus_lost(general_page.custom_res_y, do_custom_resolution_textbox, &(settings.resY));
}

void update_general_page() {
	// find resolution in list
	int foundResolution = 0;
	if (settings.resX == 0 && settings.resY == 0) {
		foundResolution = 1;

		pgui_combobox_set_selection(general_page.resolution_combobox, 0);

		pgui_checkbox_set_checked(general_page.custom_resolution, 0);
		pgui_combobox_set_enabled(general_page.resolution_combobox, 1);
		pgui_label_set_enabled(general_page.custom_res_x_label, 0);
		pgui_textbox_set_enabled(general_page.custom_res_x, 0);
		pgui_label_set_enabled(general_page.custom_res_y_label, 0);
		pgui_textbox_set_enabled(general_page.custom_res_y, 0);
	}
	else {
		for (int i = 0; i < numDisplayModes; i++) {
			if (displayModeList[i].width == settings.resX && displayModeList[i].height == settings.resY) {
				foundResolution = 1;
				pgui_combobox_set_selection(general_page.resolution_combobox, i + 1);

				pgui_checkbox_set_checked(general_page.custom_resolution, 0);
				pgui_combobox_set_enabled(general_page.resolution_combobox, 1);
				pgui_label_set_enabled(general_page.custom_res_x_label, 0);
				pgui_textbox_set_enabled(general_page.custom_res_x, 0);
				pgui_label_set_enabled(general_page.custom_res_y_label, 0);
				pgui_textbox_set_enabled(general_page.custom_res_y, 0);

				break;
			}
		}
	}

	if (!foundResolution) {
		char buf[16];
		itoa(settings.resX, buf, 10);
		pgui_textbox_set_text(general_page.custom_res_x, buf);
		itoa(settings.resY, buf, 10);
		pgui_textbox_set_text(general_page.custom_res_y, buf);

		pgui_checkbox_set_checked(general_page.custom_resolution, 1);
		pgui_combobox_set_enabled(general_page.resolution_combobox, 0);
		pgui_label_set_enabled(general_page.custom_res_x_label, 1);
		pgui_textbox_set_enabled(general_page.custom_res_x, 1);
		pgui_label_set_enabled(general_page.custom_res_y_label, 1);
		pgui_textbox_set_enabled(general_page.custom_res_y, 1);
	}

	pgui_checkbox_set_checked(general_page.windowed, settings.windowed);
	pgui_checkbox_set_checked(general_page.borderless, settings.borderless);
	pgui_checkbox_set_checked(general_page.savewinpos, settings.savewinpos);
	pgui_checkbox_set_checked(general_page.antialiasing, settings.antialiasing);
	pgui_combobox_set_selection(general_page.shadowquality_combobox, settings.hq_shadows);
	pgui_checkbox_set_checked(general_page.distance_clipping, settings.distance_clipping);
	if (settings.distance_clipping) {
		pgui_trackbar_set_enabled(general_page.clipping_distance, 1);
		pgui_checkbox_set_enabled(general_page.fog, 1);
		pgui_label_set_enabled(general_page.clipping_distance_near, 1);
		pgui_label_set_enabled(general_page.clipping_distance_far, 1);
	}
	else {
		pgui_trackbar_set_enabled(general_page.clipping_distance, 0);
		pgui_checkbox_set_enabled(general_page.fog, 0);
		pgui_label_set_enabled(general_page.clipping_distance_near, 0);
		pgui_label_set_enabled(general_page.clipping_distance_far, 0);
	}
	pgui_trackbar_set_pos(general_page.clipping_distance, settings.clipping_distance);
	pgui_checkbox_set_checked(general_page.fog, settings.fog);

	if (settings.additionalmods) {
		pgui_textbox_set_enabled(general_page.additional_mods, 1);
	}
	else {
		pgui_textbox_set_enabled(general_page.additional_mods, 0);
	}
	pgui_checkbox_set_checked(general_page.additional_mods_checkbox, settings.additionalmods);
	pgui_textbox_set_text(general_page.additional_mods, settings.additionalmods_folder);
	
	pgui_combobox_set_selection(general_page.button_font, settings.button_font-1);
	pgui_combobox_set_selection(general_page.language, settings.language-1);

	pgui_checkbox_set_checked(general_page.console, settings.console);
	if (settings.console)
		pgui_checkbox_set_enabled(general_page.native_log, 1);
	else
		pgui_checkbox_set_enabled(general_page.native_log, 0);
	if (settings.console == 2)
		pgui_checkbox_set_checked(general_page.native_log, settings.console);

	pgui_checkbox_set_checked(general_page.write_file, settings.writefile);
	pgui_checkbox_set_checked(general_page.append_log, settings.appendlog);
	pgui_checkbox_set_checked(general_page.exception_handler, settings.exceptionhandler);
	pgui_checkbox_set_checked(general_page.intro_movies, settings.intromovies);
	pgui_checkbox_set_checked(general_page.boardscuffs, settings.boardscuffs);
	pgui_checkbox_set_checked(general_page.no_additional_script_mods, settings.noadditionalscriptmods);
	pgui_combobox_set_selection(general_page.chat_size, settings.chatsize-1);

	char buf_chat_wait_time[16];
	itoa(settings.chatwaittime, buf_chat_wait_time, 10);
	pgui_textbox_set_text(general_page.chat_wait_time, buf_chat_wait_time);

	pgui_checkbox_set_checked(general_page.disable_blur, settings.disable_blur);
	pgui_checkbox_set_checked(general_page.disable_fs_gamma, settings.disable_fsgamma);
}

void initResolutionList() {
	DEVMODE deviceMode;

	numDisplayModes = 0;
	displayModeList = NULL;

	int i = 0;

	while (EnumDisplaySettings(NULL, i, &deviceMode)) {
		struct displayMode displayMode = { deviceMode.dmPelsWidth, deviceMode.dmPelsHeight };

		// insert into list
		if (displayModeList == NULL) {
			displayModeList = malloc(sizeof(struct displayMode));
			displayModeList[0] = displayMode;
			numDisplayModes = 1;
		}
		else {
			// search list for duplicate or larger resolution
			int j;
			int isDuplicate = 0;
			for (j = 0; j < numDisplayModes; j++) {
				if (displayModeList[j].width == displayMode.width && displayModeList[j].height == displayMode.height) {
					isDuplicate = 1;
					break;
				}
				else if (displayModeList[j].width >= displayMode.width && displayModeList[j].height >= displayMode.height) {
					break;
				}
			}

			if (!isDuplicate) {
				displayModeList = realloc(displayModeList, sizeof(struct displayMode) * (numDisplayModes + 1));

				// move existing elements
				for (int k = numDisplayModes; k > j; k--) {
					displayModeList[k] = displayModeList[k - 1];
				}

				displayModeList[j] = displayMode;
				numDisplayModes++;
			}
		}

		i++;
	}

	displayModeStringList = malloc(sizeof(char*) * (numDisplayModes + 1));
	displayModeStringList[0] = "Default Desktop Resolution";

	for (i = 0; i < numDisplayModes; i++) {
		//printf("OUTPUT DISPLAY MODE %d: %d x %d\n", i, displayModeList[i].width, displayModeList[i].height);
		char resolutionString[64];
		sprintf(resolutionString, "%dx%d", displayModeList[i].width, displayModeList[i].height);

		displayModeStringList[i + 1] = calloc(strlen(resolutionString) + 1, 1);
		strcpy(displayModeStringList[i + 1], resolutionString);
	}
}

int getIniBool(char* section, char* key, int def, char* file) {
	int result = GetPrivateProfileInt(section, key, def, file);
	if (result) {
		return 1;
	}
	else {
		return 0;
	}
}

void set_general_settings(struct settings* mSettingsOut) {

	mSettingsOut->resX = settings.resX;
	mSettingsOut->resY = settings.resY;
	mSettingsOut->windowed = settings.windowed;
	mSettingsOut->borderless = settings.borderless;
	mSettingsOut->hq_shadows = settings.hq_shadows;
	mSettingsOut->savewinpos = settings.savewinpos;

	mSettingsOut->antialiasing = settings.antialiasing;
	mSettingsOut->distance_clipping = settings.distance_clipping;
	mSettingsOut->clipping_distance = settings.clipping_distance;
	mSettingsOut->fog = settings.fog;
	mSettingsOut->button_font = settings.button_font;
	mSettingsOut->console = settings.console;
	mSettingsOut->writefile = settings.writefile;
	mSettingsOut->appendlog = settings.appendlog;
	mSettingsOut->exceptionhandler = settings.exceptionhandler;
	mSettingsOut->intromovies = settings.intromovies;
	mSettingsOut->language = settings.language;
	mSettingsOut->boardscuffs = settings.boardscuffs;
	mSettingsOut->noadditionalscriptmods = settings.noadditionalscriptmods;
	mSettingsOut->disable_fsgamma = settings.disable_fsgamma;
	mSettingsOut->chatsize = settings.chatsize;
	if (settings.chatwaittime < 1 || settings.chatwaittime > 120) settings.chatwaittime = 30;
	mSettingsOut->chatwaittime = settings.chatwaittime;
	mSettingsOut->additionalmods = settings.additionalmods;
	strcpy(mSettingsOut->additionalmods_folder, settings.additionalmods_folder);
}