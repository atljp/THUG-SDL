
RejoinNextGame = 0
m_observe_hud_visible = 1
M_ObserveOn = 0
observe_overview_cam_state = 0
observe_overview_cam_mode = 0
JoinedAsObserver = 0
JoinedGameInProgress = 0
M_EnteringNetLevel = 0
block_pause_menu = 0
warning_message_can_show = 1

// -----------------------------------------
// Observe mode
// -----------------------------------------

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
				change block_pause_menu = 1
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
	change block_pause_menu = 0
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
			IF M_IsSurveying
				create_observe_menu 
				RETURN 
			ENDIF 
		ENDIF 
	ENDIF 
ENDSCRIPT