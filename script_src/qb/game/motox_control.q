// This is just a random empty file which is loaded automatically without messing with qdir.txt
// Put all the helper and utility functions here
RejoinNextGame = 0
m_observe_hud_visible = 1
M_ObserveOn = 0
observe_overview_cam_state = 0
observe_overview_cam_mode = 0
JoinedAsObserver = 0
JoinedGameInProgress = 0
M_EnteringNetLevel = 0
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

SCRIPT M_RejoinNextGame
	IF GotParam FromPauseMenu 
		//M_ObserveDestroyPauseMenu 
		handle_pause_continue
	ENDIF 
	Change RejoinNextGame = 1 
ENDSCRIPT

SCRIPT M_ObserveMode
	IF GotParam on 
		IF LocalSkaterExists 
			IF Skater : OnLip 
				RETURN 
			ENDIF 
		ENDIF 
	ENDIF
	IF GotParam FromPauseMenu 
		//M_ObserveDestroyPauseMenu 
		handle_pause_continue
	ENDIF 
	IF GotParam off 
		IF IsTrue AlreadyEndedRun 
			Change M_ObserveOn = 1 
			RETURN 
		ENDIF 
		M_ClearAllObserveValues 
		IF ScreenElementExists id = observe_input_handler 
			DestroyScreenElement id = observe_input_handler 
		ENDIF 
		IF GameModeEquals is_lobby 
			ExitSurveyorMode 
			MakeSkaterGoto M_EnablePlayerInput 
			IF LocalSkaterExists 
				IF NOT Skater : Walking 
					M_ResetPhysics 
				ELSE 
					M_ResetPhysics Walk 
				ENDIF 
			ENDIF 
		ELSE 
			IF IsTrue RejoinNextGame 
				ExitSurveyorMode 
				MakeSkaterGoto M_EnablePlayerInput 
			ENDIF 
		ENDIF 
		Change M_ObserveOn = 0 
	ELSE 
		IF GotParam on 
			M_ObserveOverview
			IF LocalSkaterExists 
				IF Skater : Obj_FlagSet FLAG_SKATER_DROPPING_IN 
					Wait 1 second 
				ENDIF 
				IF Skater : IsInBail 
					Skater : ReturnBoardToSkater 
				ENDIF
				IF M_UberFriggedThisFrame 
					ResetSkaters 
				ENDIF 
				MakeSkaterGoto M_DisablePlayerInput
				SlowSkaterToStop_NoBrake
				IF NOT GameModeEquals is_lobby 
					Change AlreadyEndedRun = 1 
					FireEvent type = EndOfRunDone 
					EndOfRunDone 
				ENDIF 
			ENDIF 
			Change M_ObserveOn = 1 
		ENDIF 
	ENDIF 
ENDSCRIPT

SCRIPT M_ObserveDestroyPauseMenu 
	exit_pause_menu menu_id = pause_menu 
	IF ScreenElementExists id = current_menu_anchor 
		DestroyScreenElement id = current_menu_anchor 
	ENDIF 
	destroy_menu_background_title 
ENDSCRIPT

SCRIPT M_EnablePlayerInput
	EnablePlayerInput
ENDSCRIPT

SCRIPT M_DisablePlayerInput
	DisablePlayerInput
ENDSCRIPT

SCRIPT M_ClearAllObserveValues
	m_observe_hud_set_visible 
	Change M_ObserveOn = 0 
	Change JoinedAsObserver = 0
	M_ResetViewer 
	IF NOT GotParam quit 
		M_ObserveFunctionCall func = ObserveSelf 
		KillSkaterCamAnim all 
		restore_skater_camera 
	ENDIF 
	//unhide_special_meter 
	//update_all_player_names 
	Change GameStillRunning = 0 //??
ENDSCRIPT

SCRIPT SlowSkaterToStop_NoBrake
	MakeSkaterGoto EndOfRun Params = { Instant } 
	WaitForEvent Type = EndofRunDone
ENDSCRIPT

SCRIPT M_ResetViewer 
	IF IsTrue view_mode 
		IF GotParam unfreeze 
			IF NOT InNetGame 
				ToggleViewMode 
				RETURN 
			ENDIF 
		ENDIF 
		Change view_mode = 0 
		SetViewMode 0 
	ENDIF 
	IF GotParam force_reset 
		Change view_mode = 0 
		SetViewMode 0 
	ENDIF 
ENDSCRIPT

SCRIPT M_ObserveSelf 
	IF NOT IsTrue view_mode 
		M_ObserveFunctionCall func = ObserveSelf 
	ENDIF 
ENDSCRIPT

SCRIPT M_ObserveNext 
	IF NOT IsTrue view_mode 
		M_ObserveFunctionCall func = ObserveNextPlayer 
	ENDIF 
ENDSCRIPT

SCRIPT M_ObservePrev 
	IF NOT IsTrue view_mode 
			M_ObserveFunctionCall func = ObservePrevPlayer 
	ENDIF 
ENDSCRIPT

SCRIPT M_ObserveLeader 
	IF NOT IsTrue view_mode 
		M_ObserveFunctionCall func = ObserveLeader 
	ENDIF 
ENDSCRIPT

SCRIPT create_observe_menu 
    printf "Creating Observe Menu"
	IF NOT IsTrue JoinedAsObserver 
		IF NOT IsTrue JoinedGameInProgress 
			IF NOT M_IsSurveying 
				IF NOT IsTrue M_ObserveOn 
					M_ClearAllObserveValues 
					RETURN 
				ENDIF 
			ENDIF 
		ENDIF 
	ENDIF 
	reset_exit_pause_menu_delay
	IF ScreenElementExists id = host_options_menu 
		RETURN 
	ENDIF 
	IF ScreenElementExists id = rankings_anchor 
		RETURN 
	ENDIF 
	IF ObjectExists id = rankings_anchor 
		RETURN 
	ENDIF
	IF ScreenElementExists id = observe_input_handler 
		RETURN 
	ENDIF 
	IF ScreenElementExists id = current_menu_anchor 
		IF ScreenElementExists id = keyboard_anchor 
			IF ScreenElementExists id = keyboard_bg_anchor 
				DestroyScreenElement id = current_menu_anchor 
			ENDIF 
		ELSE 
			DestroyScreenElement id = current_menu_anchor 
		ENDIF 
	ENDIF 
	IF ScreenElementExists id = current_menu 
		DestroyScreenElement id = current_menu 
	ENDIF 
	destroy_menu_background_title 
	CreateScreenElement { 
		id = observe_input_handler 
		parent = root_window 
		type = ContainerElement 
		pos = PAIR(0.00000000000, 0.00000000000) 
		event_handlers = [ 
			{ pad_left M_ObservePrev params = { } } 
			{ pad_right M_ObserveNext params = { } } 
			{ pad_circle M_ObservePrev params = { } } 
			{ pad_choose M_ObserveNext params = { } }
			{ pad_option m_observe_toggle_hud params = { } }  // square
			{ pad_expand M_WarpToPlayer params = { } } 
		] 
	} 
	CreateScreenElement { 
		type = TextElement 
		parent = observe_input_handler 
		id = menu_obs_player_name 
		text = "" 
		font = small
		scale = 1.00000000000 
		pos = PAIR(320.00000000000, 380.00000000000) 
		alpha = 1.00000000000 
		just = [ center center ] 
	} 
	RunScriptOnScreenElement id = menu_obs_player_name do_obs_player_name_update 
	pause_menu_gradient off 
	IF ObjectExists id = helper_text_anchor 
		DestroyScreenElement id = helper_text_anchor 
	ENDIF 
	M_ObserveMenuRefreshHelperText 
	Wait 4 frames 
	FireEvent type = focus target = observe_input_handler 
	restore_start_key_binding 
ENDSCRIPT

SCRIPT reset_exit_pause_menu_delay 
	KillSpawnedScript name = exit_pause_menu_delay 
	Change exit_pause_menu_ready = 1 
ENDSCRIPT

SCRIPT exit_pause_menu_delay time = 0.30000001192 
	Change exit_pause_menu_ready = 0 
	wait <time> seconds 
	IF ScreenElementExists id = current_menu_anchor 
		Change exit_pause_menu_ready = 0 
		RETURN 
	ENDIF 
	Change exit_pause_menu_ready = 1 
ENDSCRIPT

SCRIPT M_ObserveOverview
	restore_skater_camera 
	M_ObserveFunctionCall func = RefreshCamera 
	//update_all_player_names 
	M_ObserveMenuRefreshHelperText 
ENDSCRIPT

SCRIPT M_ObserveMenuRefreshHelperText 
	//hide_special_meter 
	helper_text_freeskate = 1 
	IF NOT GameModeEquals is_lobby
		helper_text_freeskate = 0 
	ENDIF 
	IF IsTrue JoinedAsObserver
		helper_text_freeskate = 0
	ENDIF
	IF ScreenElementExists id = observe_input_handler 
		create_helper_text { 
			parent = observe_input_handler 
			anchor_id = observe_helper_text_anchor 
			helper_pos = PAIR(320.00000000000, 464.00000000000) 
			helper_text_elements = [ 
				{ text = "\\b3/\\b2 = Cycle Cameras" } 
				{ enabled = <helper_text_freeskate> text = "\\b0 = Warp" } 
				{ enabled = 1 text = "\\b1 = Toggle HUD" } 
			] 
		} 
	ENDIF 
ENDSCRIPT

SCRIPT destroy_menu_background_title 
	IF ScreenElementExists id = bg_container 
		DestroyScreenElement id = bg_container 
	ENDIF 
	m_menu_destroy_title 
	kill_pause_deck 
ENDSCRIPT

SCRIPT m_menu_destroy_title 
	IF ScreenElementExists id = pause_gradient 
		DestroyScreenElement id = pause_gradient 
	ENDIF 
	pause_menu_gradient off 
	IF ScreenElementExists id = menu_title_container 
		DestroyScreenElement id = menu_title_container 
	ENDIF 
	IF ScreenElementExists id = bg_container 
		DestroyScreenElement id = bg_container 
	ENDIF 
ENDSCRIPT

SCRIPT m_observe_toggle_hud 
	IF ( m_observe_hud_visible ) 
		m_observe_hud_set_invisible 
	ELSE 
		m_observe_hud_set_visible 
	ENDIF 
ENDSCRIPT

SCRIPT m_observe_hud_set_visible 
	IF NOT ScreenElementExists id = rankings_anchor 
		show_all_hud_items 
	ENDIF 
	Change m_observe_hud_visible = 1 
	IF ObjectExists id = root_window 
		DoScreenElementMorph { 
			id = root_window 
			scale = 1 
		} 
	ENDIF 
ENDSCRIPT

SCRIPT m_observe_hud_set_invisible 
	hide_all_hud_items 
	Change m_observe_hud_visible = 0 
	
	IF ObjectExists id = root_window 
		DoScreenElementMorph { 
			id = root_window 
			scale = 0 
		} 
	ENDIF 
ENDSCRIPT

SCRIPT hide_all_hud_items 
	dialog_box_exit no_pad_start <...> 
	IF ScreenElementExists id = console_message_vmenu 
		DoScreenElementMorph id = console_message_vmenu time = 0 scale = 0 
	ENDIF 
	IF ScreenElementExists id = first_time_goal_info 
		DestroyScreenElement id = first_time_goal_info 
	ENDIF 
	IF ScreenElementExists id = cutscene_camera_hud_anchor 
		change camera_hud_is_hidden = 1 
		DoScreenElementMorph id = cutscene_camera_hud_anchor alpha = 0 
	ENDIF 
	IF ScreenElementExists id = nightvision_hud_anchor 
		change nightvision_hud_is_hidden = 1 
		DoScreenElementMorph id = nightvision_hud_anchor alpha = 0 
	ENDIF 
	IF ScreenElementExists id = the_time 
		SetScreenElementProps id = the_time hide 
	ENDIF 
	IF ScreenElementExists id = digital_timer_anchor 
		SetScreenElementProps id = digital_timer_anchor hide 
	ENDIF 
	IF ScreenElementExists id = new_ammo_message 
		DestroyScreenElement id = new_ammo_message 
	ENDIF 
	IF ScreenElementExists id = new_ammo_message2 
		DestroyScreenElement id = new_ammo_message2 
	ENDIF 
	pause_balance_meter 
	pause_run_timer 
	hide_goal_panel_messages 
	GoalManager_HideGoalPoints 
	GoalManager_HidePoints 
	hide_3d_goal_arrow 
	hide_landing_msg 
	hide_console_window 
	goal_skate_hide_letters 
	hide_combo_letters 
	hide_net_scores 
	hide_stat_message 
	hide_tips 
	pause_trick_text 
	hide_death_msg 
	kill_blur 
ENDSCRIPT

SCRIPT show_all_hud_items 
	GoalManager_ShowGoalPoints 
	GoalManager_ShowPoints 
	unhide_3d_goal_arrow 
	unhide_landing_msg 
	unhide_stat_message 
	unhide_tips 
	goal_skate_unhide_letters 
	unhide_combo_letters 
	unhide_death_msg 
	unpause_trick_text 
	IF NOT InNetGame 
		unpause_trick_text 
	ENDIF 
	Unpause_Balance_Meter 
	unpause_run_timer 
	IF ScreenElementExists id = cutscene_camera_hud_anchor 
		IF ( camera_hud_is_hidden = 1 ) 
			change camera_hud_is_hidden = 0 
			DoScreenElementMorph id = cutscene_camera_hud_anchor alpha = 1 
		ENDIF 
	ENDIF 
	IF ScreenElementExists id = nightvision_hud_anchor 
		IF ( nightvision_hud_is_hidden = 1 ) 
			change nightvision_hud_is_hidden = 0 
			DoScreenElementMorph id = nightvision_hud_anchor alpha = 1 
		ENDIF 
	ENDIF 
	IF ScreenElementExists id = the_time 
		SetScreenElementProps id = the_time unhide 
	ENDIF 
	IF ScreenElementExists id = digital_timer_anchor 
		SetScreenElementProps id = digital_timer_anchor unhide 
	ENDIF 
	show_goal_panel_messages 
	IF NOT InMultiPlayerGame 
		GoalManager_ShowPoints 
		IF NOT GoalManager_HasActiveGoals 
			GoalManager_ShowGoalPoints 
		ELSE 
			GoalManager_HideGoalPoints 
		ENDIF 
	ENDIF 
	IF ObjectExists id = console_message_vmenu 
		DoScreenElementMorph id = console_message_vmenu time = 0 scale = 1 
	ENDIF 
	IF ( HIDEHUD = 1 ) 
		printf "hiding" 
		hide_root_window 
	ENDIF 
	IF GetGlobalFlag flag = NO_DISPLAY_HUD 
		GoalManager_HideGoalPoints 
		GoalManager_HidePoints 
	ENDIF 
	IF NOT GetGlobalFlag flag = NO_DISPLAY_CHATWINDOW 
		unhide_console_window 
	ELSE 
		hide_console_window 
	ENDIF 
	IF NOT GetGlobalFlag flag = NO_DISPLAY_NET_SCORES 
		unhide_net_scores 
	ELSE 
		hide_net_scores 
	ENDIF 
ENDSCRIPT

SCRIPT update_all_player_names 
	IF ObjectExists id = skater_name_0 
		DestroyScreenElement id = skater_name_0 
		create_object_label id = skater_name_0
	ENDIF 
	IF ObjectExists id = skater_name_1 
		DestroyScreenElement id = skater_name_1 
		create_object_label id = skater_name_1
	ENDIF 
	IF ObjectExists id = skater_name_2 
		DestroyScreenElement id = skater_name_2 
		create_object_label id = skater_name_2
	ENDIF 
	IF ObjectExists id = skater_name_3 
		DestroyScreenElement id = skater_name_3 
		create_object_label id = skater_name_3
	ENDIF 
	IF ObjectExists id = skater_name_4 
		DestroyScreenElement id = skater_name_4 
		create_object_label id = skater_name_4
	ENDIF 
	IF ObjectExists id = skater_name_5 
		DestroyScreenElement id = skater_name_5 
		create_object_label id = skater_name_5
	ENDIF 
	IF ObjectExists id = skater_name_6 
		DestroyScreenElement id = skater_name_6 
		create_object_label id = skater_name_6
	ENDIF 
	IF ObjectExists id = skater_name_7 
		DestroyScreenElement id = skater_name_7 
		create_object_label id = skater_name_7
	ENDIF 
ENDSCRIPT

SCRIPT create_object_label_update 
	SetScreenElementLock id = root_window OFF 
	<scale> = (m_playername_scale_real)
	CreateScreenElement { 
		id = <id> 
		type = textElement 
		parent = root_window 
		font = impact 
		text = "" 
		scale = <scale> 
		pos3D = VECTOR(0.00000000000, 0.00000000000, 0.00000000000) 
		rgba = [ 128 128 0 128 ] 
		alpha = 0.80000001192 
	} 
ENDSCRIPT

SCRIPT do_obs_player_name_update
	BEGIN 
		M_ObserveFunctionCall func = GetCurrentPlayerName 
		IF GotParam player_name 
			m_observe_menu_set_display_text text = <player_name> 
		ENDIF 
	Wait 30 gameframe 
	REPEAT 
ENDSCRIPT

SCRIPT m_observe_menu_set_display_text
	IF NOT GotParam text 
		text = "" 
	ENDIF 
	IF ObjectExists id = menu_obs_player_name 
		SetScreenElementProps { 
			id = menu_obs_player_name 
			text = <text> 
		} 
	ENDIF 
ENDSCRIPT

SCRIPT M_WarpToPlayer
    IF GameModeEquals is_lobby
		IF NOT IsTrue JoinedAsObserver 
			GetSkaterId 
			IF NOT <objId> : Obj_FlagSet FLAG_SKATER_DROPPING_IN 
				IF IsTrue M_ObserveOn
					m_observe_hud_set_visible
					<objId> : PlaceBeforeCamera
					IF IsTrue view_mode 
							M_ResetViewer force_reset 
					ENDIF 
					M_ObserveMode off 
				ENDIF
				wait 12 frames
				IF M_UberFriggedThisFrame 
					ResetSkaters 
				ENDIF
			ENDIF
		ENDIF
	ENDIF
ENDSCRIPT

AlreadyEndedRun = 0
SCRIPT M_NetEndRun
	IF InNetGame 
		IF GameIsOver
			Change AlreadyEndedRun = 1 
			FireEvent type = EndOfRunDone 
			EndOfRunDone 
		ENDIF 
	ENDIF 
ENDSCRIPT

SCRIPT M_ResetPhysics_or_ResetSkaters
	IF ( m_gamerunrespawns = 1 )
		ResetSkaters 
	ELSE 
		IF Skater : walking 
			M_ResetPhysics Walk 
		ELSE 
			M_ResetPhysics 
		ENDIF 
	ENDIF 
ENDSCRIPT

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
    
	//Aspect Ratio, manual bps, wp input, bhra
ENDSCRIPT

SCRIPT exit_pause_menu_maybe_create_observe_menu
	IF IsTrue M_ObserveOn 
		create_observe_menu 
		RETURN 
	ENDIF 
	IF IsTrue IsObserving 
		create_observe_menu 
		RETURN 
	ENDIF 
	IF IsTrue JoinedAsObserver 
		create_observe_menu 
		RETURN 
	ENDIF 
	IF IsTrue JoinedGameInProgress 
		create_observe_menu 
		RETURN 
	ENDIF 
	IF NOT GameModeEquals is_lobby 
		IF NOT GameModeEquals is_singlesession 
			IF M_IsSurveying // Added M_
				create_observe_menu 
				RETURN 
			ENDIF 
		ENDIF 
	ENDIF 
ENDSCRIPT

// ----------------------------------------------------------------------------------------------------

SCRIPT launch_mod_menu
	FormatText ChecksumName = title_icon "%i_special" i = ( THEME_PREFIXES [ current_theme_prefix ] ) 
	make_new_themed_sub_menu title = "THUG SDL OPTIONS" title_icon = <title_icon> 
	create_helper_text { helper_text_elements = [ { text = "\\b7/\\b4 = Select" } 
			{ text = "\\bn = Back" } 
			{ text = "\\bm = Accept" } 
		] 
	} 
	SetScreenElementProps { id = sub_menu 
		event_handlers = [ 
			{ pad_back generic_menu_pad_back params = { callback = create_pause_menu } } 
		] 
	} 
	
	M_GetINIValue section = "Graphics" key = "FOV" default = 72
	menu_camera_fov_get_string
	theme_menu_add_item text = "Field Of View" extra_text = <text> id = menu_fov
	SetScreenElementProps { 
		id = menu_fov
		event_handlers = [
			{ pad_left change_fov params = { dec } } 
			{ pad_right change_fov params = { inc } } 
		] 
		replace_handlers 
	} 
	
	M_GetINIValue section = "Miscellaneous" key = "SingleTapBP" default = 0
	IF IsTrue <value> 
		hud_text = "Single Tap" 
	ELSE 
		hud_text = "Double Tap"
	ENDIF
	theme_menu_add_item { text = "THPS4 Boostplant Input" 
		extra_text = <hud_text> 
		id = menu_singletapbp
		pad_choose_script = toggle_gameitem pad_choose_params  = { singletapbp }
	} 
		
	IF GotParam NetGame
		// RESPAWN ON NEW RUN
		M_GetINIValue section = "Multiplayer" key = "GameRunRespawns" default = 1
		IF IsTrue <value> 
			hud_text = "On" 
		ELSE 
			hud_text = "Off"
		ENDIF
		theme_menu_add_item { text = "Respawn on new run" 
			extra_text = <hud_text> 
			id = menu_gamerunrespawns 
			pad_choose_script = toggle_gameitem pad_choose_params  = { gamerunrespawns }
		} 
		
		// CHAT MESSAGE SIZE
		M_GetINIValue section = "Chat" key = "ChatSize" default = 3
		chat_size_get_string
		theme_menu_add_item { text = "Chat size" 
			extra_text = <text> 
			id = menu_chatsize
			pad_choose_script = change_chatsize
		}
		
		// PLAYER NAME SIZE
		M_GetINIValue section = "Multiplayer" key = "PlayerNameSize" default = 3
		playername_size_get_string
		theme_menu_add_item { text = "Player Name size" 
			extra_text = <text> 
			id = menu_playernamesize
			pad_choose_script = change_playernamesize
		}
	ELSE 
		// SINGLEPLAYER OPTIONS
		printf "Mod menu: Single player options: Empty"
	ENDIF 
    // BUTTSLAP COUNTER
    M_GetINIValue section = "Miscellaneous" key = "BSCounter" default = 0
    IF IsTrue <value> 
        hud_text = "On" 
    ELSE 
        hud_text = "Off"
    ENDIF
    theme_menu_add_item { text = "Buttslap Counter" 
        extra_text = <hud_text> 
        id = menu_bscounter
        pad_choose_script = toggle_gameitem pad_choose_params  = { bscounter }
    }
    // BHRA
    M_GetINIValue section = "Miscellaneous" key = "BHRA" default = 0
    IF IsTrue <value> 
        hud_text = "On" 
    ELSE 
        hud_text = "Off"
    ENDIF
    theme_menu_add_item { text = "Land Pivots" 
        extra_text = <hud_text> 
        id = menu_bhra
        pad_choose_script = toggle_gameitem pad_choose_params  = { bhra }
    }
	// FREE CAM SELECT
	M_GetINIValue section = "Miscellaneous" key = "FreeCamOnSelect" default = 0
	IF IsTrue <value> 
		hud_text = "Free Cam" 
	ELSE 
		hud_text = "Default"
	ENDIF
	theme_menu_add_item { text = "Select Button" 
		extra_text = <hud_text> 
		id = menu_selectbutton
		pad_choose_script = toggle_gameitem pad_choose_params  = { freecamselect }
	}
	// SET AND GOTO RESTART
	IF ( restart_exists = 0 ) 
		theme_menu_add_item text = "Set Restart" id = set_restart_custom pad_choose_script = m_set_custom_restart last_menu_item = 1
	ELSE 
		theme_menu_add_item text = "Set Restart" id = set_restart_custom pad_choose_script = m_set_custom_restart
		theme_menu_add_item text = "Go to Restart" id = goto_restart_custom pad_choose_script = m_skip_to_custom_restart last_menu_item = 1
	ENDIF
	finish_themed_sub_menu 
ENDSCRIPT

SCRIPT toggle_gameitem
	IF GotParam gamerunrespawns
		M_GetINIValue section = "Multiplayer" key = "GameRunRespawns" default = 1
		IF IsTrue <value>
			SetScreenElementProps id = { menu_gamerunrespawns child = 3 } text = "Off" 
			M_SetINIValue section = "Multiplayer" key = "GameRunRespawns" value = 0
			Change m_gamerunrespawns = 0
		ELSE 
			SetScreenElementProps id = { menu_gamerunrespawns child = 3 } text = "On" 
			M_SetINIValue section = "Multiplayer" key = "GameRunRespawns" value = 1				
			Change m_gamerunrespawns = 1
		ENDIF 
	ENDIF
	IF GotParam singletapbp
		M_GetINIValue section = "Miscellaneous" key = "SingleTapBP" default = 0
		IF IsTrue <value> 
			SetScreenElementProps id = { menu_singletapbp child = 3 } text = "Double Tap" 
			M_SetINIValue section = "Miscellaneous" key = "SingleTapBP" value = 0
			Change m_singletapbp = 0
		ELSE 	
			SetScreenElementProps id = { menu_singletapbp child = 3 } text = "Single Tap" 
			M_SetINIValue section = "Miscellaneous" key = "SingleTapBP" value = 1
			Change m_singletapbp = 1			
		ENDIF 	
	ENDIF
	IF GotParam freecamselect
		M_GetINIValue section = "Miscellaneous" key = "FreeCamOnSelect" default = 0
		IF IsTrue <value>
			SetScreenElementProps id = { menu_selectbutton child = 3 } text = "Default" 
			M_SetINIValue section = "Miscellaneous" key = "FreeCamOnSelect" value = 0
			Change m_freecam_select = 0
		ELSE 
			SetScreenElementProps id = { menu_selectbutton child = 3 } text = "Free Cam" 
			M_SetINIValue section = "Miscellaneous" key = "FreeCamOnSelect" value = 1				
			Change m_freecam_select = 1
		ENDIF
	ENDIF
    IF GotParam bscounter
        M_GetINIValue section = "Miscellaneous" key = "BSCounter" default = 0
        IF IsTrue <value>
            SetScreenElementProps id = { menu_bscounter child = 3 } text = "Off" 
            M_SetINIValue section = "Miscellaneous" key = "BSCounter" value = 0
            Change m_enable_bscounter = 0
        ELSE 
            SetScreenElementProps id = { menu_bscounter child = 3 } text = "On" 
            M_SetINIValue section = "Miscellaneous" key = "BSCounter" value = 1				
            Change m_enable_bscounter = 1
        ENDIF
    ENDIF
    IF GotParam bhra
        M_GetINIValue section = "Miscellaneous" key = "BHRA" default = 0
        IF IsTrue <value>
            SetScreenElementProps id = { menu_bhra child = 3 } text = "Off" 
            M_SetINIValue section = "Miscellaneous" key = "BHRA" value = 0
            Change m_enable_landpivots = 0
        ELSE 
            SetScreenElementProps id = { menu_bhra child = 3 } text = "On" 
            M_SetINIValue section = "Miscellaneous" key = "BHRA" value = 1				
            Change m_enable_landpivots = 1
        ENDIF
    ENDIF
ENDSCRIPT

SCRIPT menu_camera_fov_get_string 
	FormatText TextName = fov_text "%a" a = ( camera_fov_value ) 
	RETURN text = <fov_text> 
ENDSCRIPT

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

SCRIPT change_fov
	IF GotParam dec
		IF ( ( ( camera_fov_value ) - 1 ) > ( camera_fov_min ) )
			Change camera_fov_value = ( ( camera_fov_value ) - 1 ) 
			PlaySound MenuUp
		ENDIF 	
		menu_camera_fov_get_string 
		SetScreenElementProps { 
			id = { menu_fov child = 3 } 
			text = <text> 
		}
		M_SetINIValue section = "Graphics" key = "FOV" value = (camera_fov_value)
		M_SetFOV fov_value = camera_fov_value
		
		
	ELSE
		IF ( ( ( camera_fov_value ) + 1 ) < ( camera_fov_max ) ) 
			Change camera_fov_value = ( ( camera_fov_value ) + 1 )
			PlaySound MenuUp
		ENDIF 
		menu_camera_fov_get_string
		SetScreenElementProps { 
			id = { menu_fov child = 3 } 
			text = <text> 
		}
		M_SetINIValue section = "Graphics" key = "FOV" value = (camera_fov_value)
		M_SetFOV fov_value = camera_fov_value
	ENDIF
ENDSCRIPT

SCRIPT change_chatsize
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
	chat_size_get_string
	SetScreenElementProps { 
		id = { menu_chatsize child = 3 } 
		text = <text> 
	}
	M_SetINIValue section = "Chat" key = "ChatSize" value = (m_chat_scale)
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

SCRIPT change_playernamesize
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
	playername_size_get_string
	SetScreenElementProps { 
		id = { menu_playernamesize child = 3 } 
		text = <text> 
	}
	M_SetINIValue section = "Multiplayer" key = "PlayerNameSize" value = (m_playername_scale)
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

