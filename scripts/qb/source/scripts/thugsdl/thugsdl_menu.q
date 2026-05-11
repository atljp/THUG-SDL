
console_wait_time = 30
restart_exists = 0
camera_fov_value = 72
camera_fov_def = 72
camera_fov_min = 21
camera_fov_max = 133
chat_duration_min = 1
chat_duration_max = 120
m_gamerunrespawns = 1
m_singletapbp = 0
m_wpinput = 0
m_chat_scale = 3 // 1: extra small / 2: small / 3: default / 4: big
m_chat_scale_real = 0.80000001192 // 1: extra small / 2: small / 3: default / 4: big
m_chat_padding_real = 0.64999997616
m_playername_scale = 3
m_playername_scale_real = 1.0
m_freecam_select = 0
m_using_custom_pre = 0
m_enable_bscounter = 0
m_enable_landpivots = 0
m_dd_buttons = 0
m_dd_sping_lag = 0
m_directional_dd = 0
m_boardscuffs = 0
m_screenmode = 0
m_restore_original_double_taps = 0

// -----------------------------------------
// Initialize all values from ini
// -----------------------------------------

SCRIPT M_InitializeMod
	Change m_using_custom_pre = 1
	// GAMERUNRESPAWNS
	M_GetINIValue section = "Multiplayer" key = "GameRunRespawns" default = 1
	IF NOT IsTrue <value> 
		Change m_gamerunrespawns = 0
	ENDIF	
	// BOOSTPLANTS
	M_GetINIValue section = "Miscellaneous" key = "SingleTapBP" default = 0
	IF IsTrue <value> 
		change m_singletapbp = 1
	ENDIF
	// WALLPLANT INPUT
	M_GetINIValue section = "Controls" key = "WallplantInput" default = 0
	IF IsTrue <value> 
		change m_wpinput = 1
	ENDIF
	set_wallplantinput
	// FOV
	M_GetINIValue section = "Graphics" key = "FOV" default = 72
	IF ( ( <value> ) < ( camera_fov_min ) )
		Change camera_fov_value = camera_fov_min
	ELSE
		IF ( ( <value> ) > ( camera_fov_max ) )
			Change camera_fov_value = camera_fov_max
		ELSE
			Change camera_fov_value = <value>
		ENDIF
	ENDIF
	M_SetFOV fov_value = camera_fov_value
	// TEXT CHAT SIZE
	M_GetINIValue section = "Chat" key = "ChatSize" default = 3
	IF ( ( <value> ) < 1 )
		Change m_chat_scale = 1
	ELSE
		IF ( ( <value> ) > 4 )
			Change m_chat_scale = 4
		ELSE
			Change m_chat_scale = <value>
		ENDIF
	ENDIF
	set_chatsize
	// PLAYER NAME SIZE
	M_GetINIValue section = "Multiplayer" key = "PlayerNameSize" default = 3
	IF ( ( <value> ) < 1 )
		Change m_playername_scale = 1
	ELSE
		IF ( ( <value> ) > 4 )
			Change m_playername_scale = 4
		ELSE
			Change m_playername_scale = <value>
		ENDIF
	ENDIF	
	set_playernamesize
	// TEXT CHAT DURATION
	M_GetINIValue section = "Chat" key = "ChatWaitTime" default = 30
	IF ( ( <value> ) < ( chat_duration_min ) )
		Change console_wait_time = 1
	ELSE
		IF ( ( <value> ) > ( chat_duration_max ) )
			Change console_wait_time = 30
		ELSE
			Change console_wait_time = <value>
		ENDIF
	ENDIF
	// FREE CAM SELECT
	M_GetINIValue section = "Miscellaneous" key = "FreeCamOnSelect" default = 0
	IF IsTrue <value> 
		change m_freecam_select = 1
	ENDIF
	// BSCOUNTER
	M_GetINIValue section = "Miscellaneous" key = "BSCounter" default = 0
	IF IsTrue <value> 
		change m_enable_bscounter = 1
	ENDIF
	// BHRA
	M_GetINIValue section = "Miscellaneous" key = "BHRA" default = 0
	IF IsTrue <value> 
		change m_enable_landpivots = 1
	ENDIF
	// DROP DOWNS
	M_GetINIValue section = "Controls" key = "DropDownControl" default = 0
	IF ( ( <value> ) > 5 )
		change m_dd_buttons = <value>
		change m_directional_dd = 1
	ENDIF
	M_GetINIValue section = "Controls" key = "DDSpinLag" default = 0
	IF IsTrue <value>
		change m_dd_sping_lag = 1
	ENDIF
	// ORIGINAL DOUBLE TAPS
	M_GetINIValue section = "Debug" key = "OriginalDoubleTapGrinds" default = 0
	IF IsTrue <value> 
		change m_restore_original_double_taps = 1
	ENDIF
	// BOARDSCUFFS
	M_GetINIValue section = "Miscellaneous" key = "Boardscuffs" default = 1
	IF IsTrue <value>
		change m_boardscuffs = 1
	ENDIF
	// ASPECT RATIO
	M_GetINIValue section = "Graphics" key = "ScreenMode" default = 0
	IF ( ( <value> ) < 1 )
		Change m_screenmode = 0
	ELSE
		IF ( ( <value> ) > 5 )
			Change m_screenmode = 5
		ELSE
			Change m_screenmode = <value>
		ENDIF
	ENDIF
ENDSCRIPT

// -----------------------------------------
// Launch Mod Options menu
// -----------------------------------------

SCRIPT launch_mod_menu
	FormatText ChecksumName = title_icon "%i_special" i = ( THEME_PREFIXES [ current_theme_prefix ] ) 
	make_new_themed_sub_menu title = "THUG SDL OPTIONS" title_icon = <title_icon> 
	create_helper_text { helper_text_elements = [ { text = "\\b7/\\b4 = Select" } 
			{ text = "\\bn = Back" } 
			{ text = "\\b6/\\b5 = Change Item" } 
		] 
	} 
	SetScreenElementProps { id = sub_menu 
		event_handlers = [ 
			{ pad_back generic_menu_pad_back params = { callback = create_pause_menu } } 
		] 
	} 
	// FOV
	theme_menu_add_item { text = "Field Of View:" 
		id = menu_fov
		focus_script = menu_fov_focus 
		unfocus_script = menu_fov_unfocus 
	}
	// SCREENMODE
	theme_menu_add_item { text = "Aspect Ratio:" 
		id = menu_screenmode
		focus_script = menu_sdl_generic_focus
		unfocus_script = menu_sdl_generic_unfocus
	}
	// TH4 BOOSTPLANT INPUT
	theme_menu_add_item { text = "THPS4 Boostplant Input:" 
		id = menu_singletapbp
		focus_script = menu_sdl_generic_focus 
		unfocus_script = menu_sdl_generic_unfocus
	}
	// WALLPLANT INPUT
	theme_menu_add_item { text = "Wallplant Input:" 
		id = menu_wpinput
		focus_script = menu_sdl_generic_focus 
		unfocus_script = menu_sdl_generic_unfocus
	}
	// BOARDSCUFFS
	theme_menu_add_item { text = "Boardscuffs:" 
		id = menu_boardscuffs
		focus_script = menu_sdl_generic_focus 
		unfocus_script = menu_sdl_generic_unfocus
	}	
	IF GotParam NetGame
		// RESPAWN ON NEW RUN
		theme_menu_add_item { text = "Respawn on new run:" 
			id = menu_gamerunrespawns
			focus_script = menu_sdl_generic_focus
			unfocus_script = menu_sdl_generic_unfocus
		}
		// CHAT MESSAGE SIZE
		theme_menu_add_item { text = "Chat text size:" 
			id = menu_chatsize
			focus_script = menu_sdl_generic_focus
			unfocus_script = menu_sdl_generic_unfocus
		}
		// PLAYER NAME SIZE
		theme_menu_add_item { text = "Player name size:" 
			id = menu_playernamesize
			focus_script = menu_sdl_generic_focus
			unfocus_script = menu_sdl_generic_unfocus
		}
	ELSE 
		// SINGLEPLAYER OPTIONS
		printf "Mod menu: No Single player options"
	ENDIF 
    // BUTTSLAP COUNTER
    theme_menu_add_item { text = "Buttslap Counter:" 
        id = menu_bscounter
        focus_script = menu_sdl_generic_focus
		unfocus_script = menu_sdl_generic_unfocus
    }
    // BHRA
    theme_menu_add_item { text = "Land Pivots:" 
        id = menu_bhra
        focus_script = menu_sdl_generic_focus
		unfocus_script = menu_sdl_generic_unfocus
    }
	// FREE CAM SELECT
	theme_menu_add_item { text = "Select Button:" 
		id = menu_selectbutton
		focus_script = menu_sdl_generic_focus
		unfocus_script = menu_sdl_generic_unfocus
	}
	// SET AND GOTO RESTART
	IF ( restart_exists = 0 ) 
		theme_menu_add_item text = "Set Restart" id = set_restart_custom pad_choose_script = m_set_custom_restart last_menu_item = 1
	ELSE 
		theme_menu_add_item text = "Set Restart" id = set_restart_custom pad_choose_script = m_set_custom_restart
		theme_menu_add_item text = "Go to Restart" id = goto_restart_custom pad_choose_script = m_skip_to_custom_restart last_menu_item = 1
	ENDIF
	
	// Actually show the values of our entries
	fov_show_value
	screenmode_show_value
	singletapbp_show_value
	wpinput_show_value
	boardscuffs_show_value
	bscounter_show_value
	bhra_show_value
	freecamselect_show_value
	IF GotParam NetGame
		gamerunrespawns_show_value
		chatsize_show_value
		playername_size_show_value
	ELSE
	ENDIF
	finish_themed_sub_menu 
ENDSCRIPT

// -----------------------------------------
// For boolean toggles
// -----------------------------------------

SCRIPT toggle_gameitem
	GetTags
	IF GotParam singletapbp
		IF IsTrue m_singletapbp 
			Change m_singletapbp = 0
		ELSE 	
			Change m_singletapbp = 1
		ENDIF
		singletapbp_get_string
		SetScreenElementProps { id = { menu_singletapbp child = 4 } text = <text> }
		M_SetINIValue section = "Miscellaneous" key = "SingleTapBP" value = (m_singletapbp)	
	ENDIF
	IF GotParam wpinput
		IF IsTrue m_wpinput
			Change m_wpinput = 0
		ELSE
			Change m_wpinput = 1
		ENDIF
		wpinput_get_string
		SetScreenElementProps { id = { menu_wpinput child = 4 } text = <text> }
		M_SetINIValue section = "Controls" key = "WallplantInput" value = (m_wpinput)
		set_wallplantinput
	ENDIF
	IF GotParam boardscuffs
        IF IsTrue m_boardscuffs
			Change m_boardscuffs = 0
		ELSE
			Change m_boardscuffs = 1
		ENDIF
		boardscuffs_get_string
		SetScreenElementProps { id = { menu_boardscuffs child = 4 } text = <text> }
        M_SetINIValue section = "Miscellaneous" key = "Boardscuffs" value = (m_boardscuffs)		
    ENDIF
	IF GotParam gamerunrespawns
		IF IsTrue m_gamerunrespawns
			Change m_gamerunrespawns = 0
		ELSE
			Change m_gamerunrespawns = 1
		ENDIF
		gamerunrespawns_get_string
		SetScreenElementProps { id = { menu_gamerunrespawns child = 4 } text = <text> }
		M_SetINIValue section = "Multiplayer" key = "GameRunRespawns" value = (m_gamerunrespawns)
	ENDIF
    IF GotParam bscounter
		IF IsTrue m_enable_bscounter
			Change m_enable_bscounter = 0
		ELSE
			Change m_enable_bscounter = 1
		ENDIF
		bscounter_get_string
        SetScreenElementProps { id = { menu_bscounter child = 4 } text = <text> }
		M_SetINIValue section = "Miscellaneous" key = "BSCounter" value = (m_enable_bscounter)
    ENDIF
    IF GotParam bhra
		IF IsTrue m_enable_landpivots
			Change m_enable_landpivots = 0
		ELSE
			Change m_enable_landpivots = 1
		ENDIF
		bhra_get_string
        SetScreenElementProps { id = { menu_bhra child = 4 } text = <text> }
        M_SetINIValue section = "Miscellaneous" key = "BHRA" value = (m_enable_landpivots)
    ENDIF
	IF GotParam freecamselect
		IF IsTrue m_freecam_select
			Change m_freecam_select = 0
		ELSE
			Change m_freecam_select = 1
		ENDIF
		freecamselect_get_string
		SetScreenElementProps { id = { menu_selectbutton child = 4 } text = <text> }
		M_SetINIValue section = "Miscellaneous" key = "FreeCamOnSelect" value = (m_freecam_select)
	ENDIF
	
	IF GotParam left
			menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
	ENDIF
	IF GotParam right
		menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
	ENDIF
	PlaySound MenuUp
ENDSCRIPT

// -----------------------------------------
// Helper
// -----------------------------------------

SCRIPT menu_sdl_generic_focus
GetTags 
	FormatText ChecksumName = arrow_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] ) 
	main_theme_focus 
	SetScreenElementProps { id = { <id> child = 5 } rgba = <arrow_color> } 
	SetScreenElementProps { id = { <id> child = 6 } rgba = <arrow_color> } 
ENDSCRIPT

SCRIPT menu_sdl_generic_unfocus
	GetTags 
	main_theme_unfocus 
	SetScreenElementProps { id = { <id> child = 5 } rgba = [ 128 128 128 0 ] } 
	SetScreenElementProps { id = { <id> child = 6 } rgba = [ 128 128 128 0 ] } 
ENDSCRIPT

// -----------------------------------------
// Chat options
// -----------------------------------------

SCRIPT chat_size_get_string
	SWITCH m_chat_scale
	CASE 1
		FormatText TextName = chat_size_text "Extra small"
	CASE 2
		FormatText TextName = chat_size_text "Small"
	CASE 3
		FormatText TextName = chat_size_text "Default"
	CASE 4
		FormatText TextName = chat_size_text "Big"
	ENDSWITCH
	RETURN text = <chat_size_text> 
ENDSCRIPT

SCRIPT chatsize_show_value
	chat_size_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = chat_size_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_chatsize
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <chat_size_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_chatsize 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_chatsize 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_chatsize 
		event_handlers = [ 
			{ pad_left change_chatsize params = { left } } 
			{ pad_right change_chatsize params = { right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

SCRIPT change_chatsize
	GetTags
	IF GotParam left
		SWITCH m_chat_scale
			CASE 1
				Change m_chat_scale = 4
				Change m_chat_scale_real = 1.0
				Change m_chat_padding_real = 0.85
			CASE 2
				Change m_chat_scale = 1
				Change m_chat_scale_real = 0.3
				Change m_chat_padding_real = 0.25
			CASE 3
				Change m_chat_scale = 2
				Change m_chat_scale_real = 0.5
				Change m_chat_padding_real = 0.45
			CASE 4
				Change m_chat_scale = 3
				Change m_chat_scale_real = 0.80000001192
				Change m_chat_padding_real = 0.64999997616
		ENDSWITCH
		menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
	ENDIF
	IF GotParam right
		SWITCH m_chat_scale
			CASE 1
				Change m_chat_scale = 2
				Change m_chat_scale_real = 0.5
				Change m_chat_padding_real = 0.45
			CASE 2
				Change m_chat_scale = 3
				Change m_chat_scale_real = 0.80000001192
				Change m_chat_padding_real = 0.64999997616
			CASE 3
				Change m_chat_scale = 4
				Change m_chat_scale_real = 1.0
				Change m_chat_padding_real = 0.85
			CASE 4
				Change m_chat_scale = 1
				Change m_chat_scale_real = 0.3
				Change m_chat_padding_real = 0.25
		ENDSWITCH
		menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
	ENDIF
	chat_size_get_string
	SetScreenElementProps { id = { menu_chatsize child = 4 } text = <text> }
	M_SetINIValue section = "Chat" key = "ChatSize" value = (m_chat_scale)
	PlaySound MenuUp
ENDSCRIPT

SCRIPT set_chatsize
	SWITCH m_chat_scale
		CASE 1
			Change m_chat_scale_real = 0.3
			Change m_chat_padding_real = 0.25
		CASE 2
			Change m_chat_scale_real = 0.5
			Change m_chat_padding_real = 0.45
		CASE 3
			Change m_chat_scale_real = 0.80000001192
			Change m_chat_padding_real = 0.64999997616
		CASE 4
			Change m_chat_scale_real = 1.0
			Change m_chat_padding_real = 0.85
	ENDSWITCH
ENDSCRIPT

// -----------------------------------------
// Player name options
// -----------------------------------------

SCRIPT playername_size_get_string
	SWITCH m_playername_scale
		CASE 0
			FormatText TextName = playername_size_text "Off"
		CASE 1
			FormatText TextName = playername_size_text "Extra small"
		CASE 2
			FormatText TextName = playername_size_text "Small"
		CASE 3
			FormatText TextName = playername_size_text "Default"
		CASE 4
			FormatText TextName = playername_size_text "Big"
	ENDSWITCH
	RETURN text = <playername_size_text> 
ENDSCRIPT

SCRIPT playername_size_show_value
	playername_size_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = playername_size_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_playernamesize
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <playername_size_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_playernamesize 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_playernamesize 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_playernamesize 
		event_handlers = [ 
			{ pad_left change_playernamesize params = { left } } 
			{ pad_right change_playernamesize params = { right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

SCRIPT change_playernamesize
	GetTags
	IF GotParam left
		SWITCH m_playername_scale
			CASE 0
				Change m_playername_scale = 4
				Change m_playername_scale_real = 1.5
			CASE 1
				Change m_playername_scale = 0
				Change m_playername_scale_real = 0.0
			CASE 2
				Change m_playername_scale = 1
				Change m_playername_scale_real = 0.3
			CASE 3
				Change m_playername_scale = 2
				Change m_playername_scale_real = 0.5
			CASE 4
				Change m_playername_scale = 3
				Change m_playername_scale_real = 1.0
		ENDSWITCH
		menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
	ENDIF
	IF GotParam right
		SWITCH m_playername_scale
			CASE 0
				Change m_playername_scale = 1
				Change m_playername_scale_real = 0.3
			CASE 1
				Change m_playername_scale = 2
				Change m_playername_scale_real = 0.5
			CASE 2
				Change m_playername_scale = 3
				Change m_playername_scale_real = 1.0
			CASE 3
				Change m_playername_scale = 4
				Change m_playername_scale_real = 1.5
			CASE 4
				Change m_playername_scale = 0
				Change m_playername_scale_real = 0.0
		ENDSWITCH
		menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
	ENDIF
	playername_size_get_string
	SetScreenElementProps { id = { menu_playernamesize child = 4 } text = <text> }
	M_SetINIValue section = "Multiplayer" key = "PlayerNameSize" value = (m_playername_scale)
	PlaySound MenuUp
	update_all_player_names
ENDSCRIPT

SCRIPT set_playernamesize
	SWITCH m_playername_scale
		CASE 0
			Change m_playername_scale_real = 0.0
		CASE 1
			Change m_playername_scale_real = 0.3
		CASE 2
			Change m_playername_scale_real = 0.5
		CASE 3
			Change m_playername_scale_real = 1.0
		CASE 4
			Change m_playername_scale_real = 1.5
	ENDSWITCH
	update_all_player_names
ENDSCRIPT

// -----------------------------------------
// FOV
// -----------------------------------------

SCRIPT menu_camera_fov_get_string 
	FormatText TextName = fov_text "%a" a = ( camera_fov_value ) 
	RETURN text = <fov_text> 
ENDSCRIPT

SCRIPT menu_fov_focus
	GetTags 
	FormatText ChecksumName = arrow_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] ) 
	main_theme_focus 
	IF ( ( camera_fov_value - 1 ) = ( camera_fov_min ) ) 
		SetScreenElementProps { id = { <id> child = 5 } rgba = [ 128 128 128 0 ] } 
	ELSE 
		SetScreenElementProps { id = { <id> child = 5 } rgba = <arrow_color> } 
	ENDIF 
	IF ( ( camera_fov_value + 1 ) = ( camera_fov_max ) ) 
		SetScreenElementProps { id = { <id> child = 6 } rgba = [ 128 128 128 0 ] } 
	ELSE 
		SetScreenElementProps { id = { <id> child = 6 } rgba = <arrow_color> } 
	ENDIF 
ENDSCRIPT

SCRIPT menu_fov_unfocus
	GetTags 
	main_theme_unfocus 
	SetScreenElementProps { id = { <id> child = 5 } rgba = [ 128 128 128 0 ] } 
	SetScreenElementProps { id = { <id> child = 6 } rgba = [ 128 128 128 0 ] } 
ENDSCRIPT

SCRIPT fov_show_value
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = fov_text "%v" v = camera_fov_value
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_fov 
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <fov_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_fov 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_fov 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_fov 
		event_handlers = [ 
			{ pad_left change_fov params = { dec } } 
			{ pad_right change_fov params = { inc } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

SCRIPT change_fov
	GetTags
	IF GotParam dec
		IF ( ( ( camera_fov_value ) - 1 ) > ( camera_fov_min ) )
			SetScreenElementProps id = { <id> child = 6 } rgba = [ 128 128 128 128 ]
			Change camera_fov_value = ( ( camera_fov_value ) - 1 )
			M_SetINIValue section = "Graphics" key = "FOV" value = (camera_fov_value)
			M_SetFOV fov_value = camera_fov_value	
			menu_camera_fov_get_string
			SetScreenElementProps { 
				id = { menu_fov child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
			PlaySound MenuUp
		ENDIF
		IF ( ( ( camera_fov_value ) - 1 ) = ( camera_fov_min ) )
			SetScreenElementProps id = { <id> child = 5 } rgba = [ 128 128 128 0 ] 
		ENDIF
	ENDIF
	IF GotParam inc
		IF ( ( ( camera_fov_value ) + 1 ) < ( camera_fov_max ) )
			SetScreenElementProps id = { <id> child = 5 } rgba = [ 128 128 128 128 ]
			Change camera_fov_value = ( ( camera_fov_value ) + 1 )
			M_SetINIValue section = "Graphics" key = "FOV" value = (camera_fov_value)
			M_SetFOV fov_value = camera_fov_value	
			menu_camera_fov_get_string
			SetScreenElementProps { 
				id = { menu_fov child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
			PlaySound MenuUp
		ENDIF
		IF ( ( ( camera_fov_value ) + 1 ) = ( camera_fov_max ) )
			SetScreenElementProps id = { <id> child = 6 } rgba = [ 128 128 128 0 ] 
		ENDIF
	ENDIF	
ENDSCRIPT

// -----------------------------------------
// Screen mode
// -----------------------------------------

SCRIPT screenmode_get_string
	SWITCH m_screenmode
	CASE 0
		FormatText TextName = m_screenmode_text "Auto"
	CASE 1
		FormatText TextName = m_screenmode_text "4:3"
	CASE 2
		FormatText TextName = m_screenmode_text "16:9"
	CASE 3
		FormatText TextName = m_screenmode_text "16:10"
	CASE 4
		FormatText TextName = m_screenmode_text "21:9"
	CASE 5
		FormatText TextName = m_screenmode_text "21:10"
	DEFAULT
		FormatText TextName = m_screenmode_text "\\c2ERROR!"
	ENDSWITCH
	RETURN text = <m_screenmode_text>
ENDSCRIPT

SCRIPT screenmode_show_value
	screenmode_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = screenmode_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_screenmode
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <screenmode_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_screenmode 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_screenmode 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_screenmode 
		event_handlers = [ 
			{ pad_left change_screenmode params = { left } } 
			{ pad_right change_screenmode params = { right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

SCRIPT change_screenmode
	GetTags
	IF GotParam left
		SWITCH m_screenmode
		CASE 0
			Change m_screenmode = 5
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
			PlaySound MenuUp
		CASE 1
			Change m_screenmode = 0
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
			PlaySound MenuUp
		CASE 2
			Change m_screenmode = 1
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
			PlaySound MenuUp
		CASE 3
			Change m_screenmode = 2
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
			PlaySound MenuUp
		CASE 4
			Change m_screenmode = 3
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
			PlaySound MenuUp
		CASE 5
			Change m_screenmode = 4
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 5 } 
			PlaySound MenuUp
		DEFAULT
			printf "Invalid Screenmode setting!"
		ENDSWITCH
	ENDIF
	IF GotParam right
		SWITCH m_screenmode
		CASE 0
			Change m_screenmode = 1
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
			PlaySound MenuUp
		CASE 1
			Change m_screenmode = 2
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
			PlaySound MenuUp
		CASE 2
			Change m_screenmode = 3
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
			PlaySound MenuUp
		CASE 3
			Change m_screenmode = 4
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
			PlaySound MenuUp
		CASE 4
			Change m_screenmode = 5
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
			PlaySound MenuUp
		CASE 5
			Change m_screenmode = 0
			M_SetINIValue section = "Graphics" key = "ScreenMode" value = (m_screenmode)
			screenmode_get_string
			SetScreenElementProps { 
				id = { menu_screenmode child = 4 } 
				text = <text> 
			}
			menu_horiz_blink_arrow arrow_id = { <id> child = 6 } 
			PlaySound MenuUp
		DEFAULT
			printf "Invalid Screenmode setting!"
		ENDSWITCH
	ENDIF 
	M_SetAspectRatio value = (m_screenmode)
	M_SetFOV fov_value = camera_fov_value
ENDSCRIPT

// -----------------------------------------
// TH4 Boostplant input
// -----------------------------------------

SCRIPT singletapbp_get_string
	IF IsTrue m_singletapbp
		FormatText TextName = singletapbp_text "Single Tap"
	ELSE
		FormatText TextName = singletapbp_text "Double Tap"
	ENDIF
	RETURN text = <singletapbp_text> 
ENDSCRIPT

SCRIPT singletapbp_show_value
	singletapbp_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = singletapbp_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_singletapbp
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <singletapbp_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_singletapbp 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_singletapbp 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_singletapbp 
		event_handlers = [ 
			{ pad_left toggle_gameitem params = { singletapbp left } } 
			{ pad_right toggle_gameitem params = { singletapbp right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

// -----------------------------------------
// Wallplant input
// -----------------------------------------

SCRIPT wpinput_get_string
	IF IsTrue m_wpinput
		FormatText TextName = wpinput_text "\\b3"
	ELSE
		FormatText TextName = wpinput_text "\\b3 + \\b4"
	ENDIF
	RETURN text = <wpinput_text> 
ENDSCRIPT

SCRIPT wpinput_show_value
	wpinput_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = wpinput_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_wpinput
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <wpinput_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_wpinput 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_wpinput 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_wpinput 
		event_handlers = [ 
			{ pad_left toggle_gameitem params = { wpinput left } } 
			{ pad_right toggle_gameitem params = { wpinput right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

SCRIPT set_wallplantinput
	IF IsTrue m_wpinput
		M_ToggleWallplantInput Ollie
	ELSE
		M_ToggleWallplantInput DownAndOllie
	ENDIF
ENDSCRIPT

// -----------------------------------------
// Boardscuffs
// -----------------------------------------

SCRIPT boardscuffs_get_string
	IF IsTrue m_boardscuffs
		FormatText TextName = boardscuffs_text "On"
	ELSE
		FormatText TextName = boardscuffs_text "Off"
	ENDIF
	RETURN text = <boardscuffs_text> 
ENDSCRIPT

SCRIPT boardscuffs_show_value
	boardscuffs_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = boardscuffs_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_boardscuffs
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <boardscuffs_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_boardscuffs 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_boardscuffs 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_boardscuffs 
		event_handlers = [ 
			{ pad_left toggle_gameitem params = { boardscuffs left } } 
			{ pad_right toggle_gameitem params = { boardscuffs right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

// -----------------------------------------
// Respawn on new run
// -----------------------------------------

SCRIPT gamerunrespawns_get_string
	IF IsTrue m_gamerunrespawns
		FormatText TextName = gamerunrespawns_text "On"
	ELSE
		FormatText TextName = gamerunrespawns_text "Off"
	ENDIF
	RETURN text = <gamerunrespawns_text> 
ENDSCRIPT

SCRIPT gamerunrespawns_show_value
	gamerunrespawns_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = gamerunrespawns_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_gamerunrespawns
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <gamerunrespawns_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_gamerunrespawns 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_gamerunrespawns 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_gamerunrespawns 
		event_handlers = [ 
			{ pad_left toggle_gameitem params = { gamerunrespawns left } } 
			{ pad_right toggle_gameitem params = { gamerunrespawns right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

// -----------------------------------------
// Buttslap counter
// -----------------------------------------

SCRIPT bscounter_get_string
	IF IsTrue m_enable_bscounter
		FormatText TextName = bscounter_text "On"
	ELSE
		FormatText TextName = bscounter_text "Off"
	ENDIF
	RETURN text = <bscounter_text> 
ENDSCRIPT

SCRIPT bscounter_show_value
	bscounter_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = bscounter_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_bscounter
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <bscounter_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_bscounter 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_bscounter 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_bscounter 
		event_handlers = [ 
			{ pad_left toggle_gameitem params = { bscounter left } } 
			{ pad_right toggle_gameitem params = { bscounter right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

// -----------------------------------------
// Land pivots
// -----------------------------------------

SCRIPT bhra_get_string
	IF IsTrue m_enable_landpivots
		FormatText TextName = landpivots_text "On"
	ELSE
		FormatText TextName = landpivots_text "Off"
	ENDIF
	RETURN text = <landpivots_text> 
ENDSCRIPT

SCRIPT bhra_show_value
	bhra_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = landpivots_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_bhra
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <landpivots_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_bhra 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_bhra 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_bhra 
		event_handlers = [ 
			{ pad_left toggle_gameitem params = { bhra left } } 
			{ pad_right toggle_gameitem params = { bhra right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

// -----------------------------------------
// Select button
// -----------------------------------------

SCRIPT freecamselect_get_string
	IF IsTrue m_freecam_select
		FormatText TextName = freecamselect_text "Free cam"
	ELSE
		FormatText TextName = freecamselect_text "Default"
	ENDIF
	RETURN text = <freecamselect_text> 
ENDSCRIPT

SCRIPT freecamselect_show_value
	freecamselect_get_string
	FormatText ChecksumName = text_color "%i_unhighlighted_text_color" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] )
	FormatText textName = freecamselect_text "%v" v = <text>
	
	CreateScreenElement { 
		type = textElement 
		parent = menu_selectbutton
		font = small 
		just = [ center top ] 
		pos = PAIR(182.50000000000, -17.00000000000) 
		text = <freecamselect_text> 
		rgba = <text_color> 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_selectbutton 
		texture = left_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(115.00000000000, -17.00000000000) 
		just = [ right top ] 
		scale = 0.75000000000 
	} 
	CreateScreenElement { 
		type = SpriteElement 
		parent = menu_selectbutton 
		texture = right_arrow 
		rgba = [ 128 128 128 0 ] 
		pos = PAIR(250.00000000000, -17.00000000000) 
		just = [ left top ] 
		scale = 0.75000000000 
	} 
	SetScreenElementProps { 
		id = menu_selectbutton 
		event_handlers = [ 
			{ pad_left toggle_gameitem params = { freecamselect left } } 
			{ pad_right toggle_gameitem params = { freecamselect right } } 
		] 
		replace_handlers 
	} 
ENDSCRIPT

// -----------------------------------------
// Restarts
// -----------------------------------------

SCRIPT m_set_custom_restart
	IF ( ( GameModeEquals is_lobby ) | ( GameModeEquals is_singlesession ) )
		IF NOT M_IsSurveying
			IF NOT IsObserving
				change restart_exists = 1
				exit_pause_menu menu
				set_custom_restart
				
				IF Skater : walking 
					M_ResetPhysics Walk 
				ELSE 
					M_ResetPhysics 
				ENDIF
			ENDIF
		ENDIF
	ENDIF
ENDSCRIPT

SCRIPT m_skip_to_custom_restart
	IF ( ( GameModeEquals is_lobby ) | ( GameModeEquals is_singlesession ) )
		IF NOT M_IsSurveying
			IF NOT IsObserving
				exit_pause_menu menu
				skip_to_custom_restart
				IF Skater : walking 
					M_ResetPhysics Walk 
				ELSE 
					M_ResetPhysics 
				ENDIF
			ENDIF
		ENDIF
	ENDIF
ENDSCRIPT
