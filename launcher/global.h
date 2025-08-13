#pragma once

#define CONFIG_FILE_NAME "thugsdl.ini"

struct settings {
	int resX;
	int resY;
	int windowed;
	int borderless;
	int savewinpos;
	int screenmode;

	int hq_shadows;
	int antialiasing;
	int distance_clipping;
	int clipping_distance;
	int fog;

	int button_font;
	int disable_blur;
	int disable_fsgamma;

	int intromovies;
	int language;
	int boardscuffs;
	int noadditionalscriptmods;
	int gamerunrespawns;

	int console;
	int writefile;
	int appendlog;
	int exceptionhandler;

	int chatsize;
	int chatwaittime;
	int additionalmods;
	char additionalmods_folder[MAX_PATH];

	int fov;
	int singletapbp;
	int freecamonselect;
	int playernamesize;
};

struct displayMode {
	int width;
	int height;
};

int numDisplayModes;
struct displayMode* displayModeList;
char** displayModeStringList;

struct keybinds {
	SDL_Scancode ollie;
	SDL_Scancode grab;
	SDL_Scancode flip;
	SDL_Scancode grind;
	SDL_Scancode spinLeft;
	SDL_Scancode spinRight;
	SDL_Scancode nollie;
	SDL_Scancode switchRevert;
	SDL_Scancode caveman;
	SDL_Scancode caveman2;

	SDL_Scancode item_up;
	SDL_Scancode item_down;
	SDL_Scancode item_left;
	SDL_Scancode item_right;

	SDL_Scancode forward;
	SDL_Scancode backward;
	SDL_Scancode left;
	SDL_Scancode right;
	SDL_Scancode feeble;

	SDL_Scancode camUp;
	SDL_Scancode camDown;
	SDL_Scancode camLeft;
	SDL_Scancode camRight;
	SDL_Scancode viewToggle;
	SDL_Scancode swivelLock;
};

struct controls {
	int ps2_controls;
	int dropdownkey;
	int laddergrabkey;
	int cavemankey;
	int menubuttons;
	int invertrxp1;
	int invertryp1;
	int disablerxp1;
	int disableryp1;
};

// a recreation of the SDL_GameControllerButton enum, but with the addition of right/left trigger
typedef enum {
	CONTROLLER_UNBOUND = -1,
	CONTROLLER_BUTTON_A = SDL_CONTROLLER_BUTTON_A,
	CONTROLLER_BUTTON_B = SDL_CONTROLLER_BUTTON_B,
	CONTROLLER_BUTTON_X = SDL_CONTROLLER_BUTTON_X,
	CONTROLLER_BUTTON_Y = SDL_CONTROLLER_BUTTON_Y,
	CONTROLLER_BUTTON_BACK = SDL_CONTROLLER_BUTTON_BACK,
	CONTROLLER_BUTTON_GUIDE = SDL_CONTROLLER_BUTTON_GUIDE,
	CONTROLLER_BUTTON_START = SDL_CONTROLLER_BUTTON_START,
	CONTROLLER_BUTTON_LEFTSTICK = SDL_CONTROLLER_BUTTON_LEFTSTICK,
	CONTROLLER_BUTTON_RIGHTSTICK = SDL_CONTROLLER_BUTTON_RIGHTSTICK,
	CONTROLLER_BUTTON_LEFTSHOULDER = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
	CONTROLLER_BUTTON_RIGHTSHOULDER = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
	CONTROLLER_BUTTON_DPAD_UP = SDL_CONTROLLER_BUTTON_DPAD_UP,
	CONTROLLER_BUTTON_DPAD_DOWN = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
	CONTROLLER_BUTTON_DPAD_LEFT = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
	CONTROLLER_BUTTON_DPAD_RIGHT = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
	CONTROLLER_BUTTON_MISC1 = SDL_CONTROLLER_BUTTON_MISC1,
	CONTROLLER_BUTTON_PADDLE1 = SDL_CONTROLLER_BUTTON_PADDLE1,
	CONTROLLER_BUTTON_PADDLE2 = SDL_CONTROLLER_BUTTON_PADDLE2,
	CONTROLLER_BUTTON_PADDLE3 = SDL_CONTROLLER_BUTTON_PADDLE3,
	CONTROLLER_BUTTON_PADDLE4 = SDL_CONTROLLER_BUTTON_PADDLE4,
	CONTROLLER_BUTTON_TOUCHPAD = SDL_CONTROLLER_BUTTON_TOUCHPAD,
	CONTROLLER_BUTTON_RIGHTTRIGGER = 21,
	CONTROLLER_BUTTON_LEFTTRIGGER = 22,
} controllerButton;

typedef enum {
	CONTROLLER_STICK_UNBOUND = -1,
	CONTROLLER_STICK_LEFT = 0,
	CONTROLLER_STICK_RIGHT = 1
} controllerStick;

struct controllerbinds {
	controllerButton menu;
	controllerButton cameraToggle;
	controllerButton cameraSwivelLock;

	controllerButton grind;
	controllerButton grab;
	controllerButton ollie;
	controllerButton kick;

	controllerButton leftSpin;
	controllerButton rightSpin;
	controllerButton nollie;
	controllerButton switchRevert;
	controllerButton caveman;
	controllerButton caveman2;

	controllerButton right;
	controllerButton left;
	controllerButton up;
	controllerButton down;
	controllerButton feeble;

	controllerStick movement;
	controllerStick camera;
};

struct gamepad_page {
	pgui_control* ollie;
	pgui_control* grab;
	pgui_control* flip;
	pgui_control* grind;
	pgui_control* spin_left;
	pgui_control* spin_right;
	pgui_control* nollie;
	pgui_control* switch_revert;
	pgui_control* caveman;
	pgui_control* caveman2;
	pgui_control* pause;

	pgui_control* forward;
	pgui_control* backward;
	pgui_control* left;
	pgui_control* right;
	pgui_control* movement_stick;

	pgui_control* camera_stick;
	pgui_control* view_toggle;
	pgui_control* swivel_lock;
};

struct control_page {
	pgui_control* ps2_controls;
	pgui_control* dropdownkey;
	pgui_control* dropdownkey_combobox;
	pgui_control* dropdown_helper;
	pgui_control* cavemankey;
	pgui_control* cavemankey_combobox;
	pgui_control* laddergrabkey;
	pgui_control* laddergrabkey_combobox;
	pgui_control* caveman_helper;
	pgui_control* menubuttons;
	pgui_control* menubuttons_combobox;
	pgui_control* menubuttons_helper;
	pgui_control* ps2_helper;
	pgui_control* invertrxp1;
	pgui_control* invertryp1;
	pgui_control* disablerxp1;
	pgui_control* disableryp1;
};


struct keyboard_page {
	pgui_control* ollie;
	pgui_control* grab;
	pgui_control* flip;
	pgui_control* grind;
	pgui_control* spin_left;
	pgui_control* spin_right;
	pgui_control* nollie;
	pgui_control* switch_revert;
	pgui_control* caveman;
	pgui_control* caveman2;

	pgui_control* item_up;
	pgui_control* item_down;
	pgui_control* item_left;
	pgui_control* item_right;

	pgui_control* forward;
	pgui_control* backward;
	pgui_control* left;
	pgui_control* right;
	pgui_control* feeble;

	pgui_control* camera_up;
	pgui_control* camera_down;
	pgui_control* camera_left;
	pgui_control* camera_right;
	pgui_control* view_toggle;
	pgui_control* swivel_lock;
};


struct general_page {
	pgui_control* resolution_combobox;
	pgui_control* screenmode_combobox;
	pgui_control* custom_resolution;
	pgui_control* custom_res_x_label;
	pgui_control* custom_res_x;
	pgui_control* custom_res_y_label;
	pgui_control* screenmode_label;
	pgui_control* custom_res_y;

	pgui_control* windowed;
	pgui_control* borderless;
	pgui_control* savewinpos;

	pgui_control* antialiasing;
	pgui_control* shadowquality_combobox;
	pgui_control* distance_clipping;
	pgui_control* clipping_distance;
	pgui_control* clipping_distance_near;
	pgui_control* clipping_distance_far;
	pgui_control* fog;

	pgui_control* ps2_controls;
	pgui_control* button_font_label;
	pgui_control* button_font;
	pgui_control* console;
	pgui_control* native_log;
	pgui_control* write_file;
	pgui_control* append_log;
	pgui_control* exception_handler;

	pgui_control* intro_movies;
	pgui_control* language_label;
	pgui_control* language;
	pgui_control* no_additional_script_mods;
	pgui_control* game_run_respawns;
	pgui_control* boardscuffs;

	pgui_control* chat_size_label;
	pgui_control* chat_size;
	pgui_control* chat_wait_time_label;
	pgui_control* chat_wait_time;
	pgui_control* additional_mods_checkbox;
	pgui_control* additional_mods;
	
	pgui_control* disable_fs_gamma;
	pgui_control* disable_blur;
};

typedef struct {
	SDL_Scancode* target;
	char* name;
} keybind_data;


void do_setting_checkbox(pgui_control* control, int value, int* target);
void set_menu_combobox_no_offset(pgui_control* control, int value, int* target);
void set_menu_combobox(pgui_control* control, int value, int* target);