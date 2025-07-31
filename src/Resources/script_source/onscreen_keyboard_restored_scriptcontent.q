// scripts\\engine\\menu\\keyboard.qb
SCRIPT create_onscreen_keyboard { keyboard_title = "KEYBOARD" 
		keyboard_cancel_script = keyboard_cancel 
		pos = PAIR(320.00000000000, 240.00000000000) 
		max_length = 20 
		display_text = "_" 
		text = "" 
		display_text_scale = 0.85000002384 
		display_text_offset = PAIR(0.00000000000, 0.00000000000) 
	} 
	Change keyboard_toggling_char_set = 0 
	FormatText ChecksumName = highlight_color "%i_HIGHLIGHTED_TEXT_COLOR" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] ) 
	FormatText ChecksumName = unhighlight_color "%i_UNHIGHLIGHTED_TEXT_COLOR" i = ( THEME_COLOR_PREFIXES [ current_theme_prefix ] ) 
	destroy_onscreen_keyboard 
	SetScreenElementLock id = root_window Off 
	IF GotParam no_buttons 
		CreateScreenElement { 
			Type = ContainerElement 
			parent = root_window 
			id = keyboard_anchor 
			pos = <pos> 
			dims = PAIR(640.00000000000, 480.00000000000) 
			z_priority = 15 
		} 
	ELSE 
		CreateScreenElement { 
			Type = ContainerElement 
			parent = root_window 
			id = keyboard_bg_anchor 
			pos = <pos> 
			dims = PAIR(640.00000000000, 480.00000000000) 
		} 
		CreateScreenElement { 
			Type = ContainerElement 
			parent = keyboard_bg_anchor 
			id = keyboard_anchor 
			focusable_child = keyboard_vmenu 
			pos = <pos> 
			dims = PAIR(640.00000000000, 480.00000000000) 
		} 
	ENDIF 
	AssignAlias id = keyboard_anchor alias = current_menu_anchor 
	IF GotParam password 
		keyboard_anchor : SetTags password 
	ENDIF 
	IF NOT GotParam no_buttons 
		IF GotParam allow_cancel 
			create_helper_text { helper_text_elements = [ { text = "\\b7/\\b4/\\b6/\\b5=Select" } 
					{ text = "\\bn=Cancel" } 
					{ text = "\\bm=Accept" } 
					{ text = "\\bs/\\bt=Character Set" } 
				] 
				helper_pos = <helper_pos> 
				parent = keyboard_bg_anchor 
			} 
		ELSE 
			create_helper_text { helper_text_elements = [ { text = "\\b7/\\b4/\\b6/\\b5=Select" } 
					{ text = "\\bm=Accept" } 
					{ text = "\\mf/\\mg=Character Set" } 
				] 
				helper_pos = <helper_pos> 
				parent = keyboard_bg_anchor 
			} 
		ENDIF 
	ENDIF 
	<org_text> = <text> 
	FormatText textname = text "%s_" s = <text> 
	IF GotParam no_buttons 
		keyboard_anchor : SetTags no_buttons 
		SetScreenElementProps { 
			id = keyboard_anchor 
			event_handlers = [ { pad_choose keyboard_done params = <...> } 
				{ pad_back generic_menu_pad_back params = { callback = <keyboard_cancel_script> <keyboard_cancel_params> } } 
			] 
		} 
		CreateScreenElement { 
			Type = TextElement 
			parent = keyboard_anchor 
			font = testtitle 
			id = kb_no_button_hdr 
			text = <display_text> 
			pos = PAIR(320.00000000000, 65.00000000000) 
			just = [ center top ] 
			z_priority = 15 
		} 
		IF GotParam text_block 
			CreateScreenElement { 
				Type = TextBlockElement 
				parent = keyboard_anchor 
				id = keyboard_display_string 
				allow_expansion 
				font = dialog 
				just = [ center top ] 
				text = "_" 
				internal_just = [ center center ] 
				not_focusable 
				pos = PAIR(320.00000000000, 85.00000000000) 
				dims = ( PAIR(1.00000000000, 0.00000000000) * keyboard_text_block_width + PAIR(0.00000000000, 10.00000000000) ) 
			} 
		ELSE 
			CreateScreenElement { 
				Type = TextElement 
				parent = keyboard_anchor 
				id = keyboard_display_string 
				font = dialog 
				just = [ center top ] 
				text = "_" 
				not_focusable 
				pos = PAIR(320.00000000000, 85.00000000000) 
			} 
		ENDIF 
		printf "*** 9" 
		IF GotParam password 
			GetTextElementLength id = keyboard_display_string 
			SetScreenElementProps id = keyboard_display_string text = "" 
			IF ( <length> > 1 ) 
				BEGIN 
					TextElementConcatenate id = keyboard_display_string "*" 
				REPEAT ( <length> -1 ) 
				TextElementConcatenate id = keyboard_display_string "_" 
			ENDIF 
		ENDIF 
		CreateScreenElement { 
			Type = TextBlockElement 
			parent = keyboard_anchor 
			id = keyboard_current_string 
			font = dialog 
			just = [ center top ] 
			text = <org_text> 
			not_focusable 
			pos = PAIR(320.00000000000, 85.00000000000) 
			dims = ( PAIR(1.00000000000, 0.00000000000) * keyboard_text_block_width + PAIR(0.00000000000, 10.00000000000) ) 
			allow_expansion 
			scale = 0 
		} 
		IF GotParam allowed_characters 
			keyboard_current_string : SetTags allowed_characters = <allowed_characters> 
		ENDIF 
		IF GotParam max_length 
			IF ( <max_length> < 1 ) 
				script_assert "create_onscreen_keyboard called with bad max_length" 
			ENDIF 
			BEGIN 
				GetTextElementLength id = keyboard_current_string 
				IF ( <length> > <max_length> ) 
					TextElementBackspace id = keyboard_current_string 
					TextElementBackspace id = keyboard_display_string 
				ELSE 
					BREAK 
				ENDIF 
			REPEAT 
		ENDIF 
		FireEvent Type = focus target = keyboard_anchor 
	ELSE 
		IF GotParam text_block 
			theme_dialog_background { parent = keyboard_anchor 
				width = 3.50000000000 
				pos = PAIR(320.00000000000, 85.00000000000) 
				num_parts = 7.50000000000 
				z_priority = 1 
				top_height = 2.54999995232 
				no_icon = no_icon 
			} 
		ELSE 
			theme_dialog_background { parent = keyboard_anchor 
				width = 3.50000000000 
				pos = PAIR(320.00000000000, 85.00000000000) 
				num_parts = 6 
				z_priority = 1 
				top_height = 1 
				no_icon = no_icon 
			} 
		ENDIF 
		IF NOT GotParam no_buttons 
			IF levelis load_skateshop 
				build_top_and_bottom_blocks parent = keyboard_bg_anchor bot_z = 10 
				IF GotParam in_ss 
					make_mainmenu_3d_plane { 
						parent = keyboard_bg_anchor 
						Model = "mainmenu_bg\\mainmenu_bg.mdl" 
						scale = PAIR(1.25000000000, 1.25000000000) 
						pos = PAIR(360.00000000000, 217.00000000000) 
					} 
					CreateScreenElement { 
						Type = SpriteElement 
						parent = keyboard_bg_anchor 
						id = mm_building 
						texture = ss_sidewall 
						just = [ center center ] 
						scale = PAIR(2.00000000000, 1.79999995232) 
						pos = PAIR(-40.00000000000, 195.00000000000) 
						z_priority = -3 
						alpha = 1 
					} 
					make_mainmenu_clouds parent = keyboard_bg_anchor 
				ELSE 
					make_mainmenu_3d_plane parent = keyboard_bg_anchor 
					IF GotParam in_net_lobby 
						IF NOT ScreenElementExists id = globe 
							CreateScreenElement { 
								Type = SpriteElement 
								parent = keyboard_bg_anchor 
								id = globe 
								texture = globe 
								scale = 1 
								pos = PAIR(550.00000000000, 240.00000000000) 
								just = [ center center ] 
								alpha = 0.20000000298 
								z_priority = -1 
							} 
							RunScriptOnScreenElement id = globe rotate_internet_options_globe 
						ENDIF 
					ENDIF 
					IF GotParam in_profile_options 
						CreateScreenElement { 
							Type = SpriteElement 
							parent = keyboard_bg_anchor 
							id = globe 
							texture = globe 
							scale = 1.29999995232 
							pos = PAIR(320.00000000000, 560.00000000000) 
							just = [ center center ] 
							alpha = 0.30000001192 
							z_priority = -1 
						} 
						RunScriptOnScreenElement id = globe rotate_internet_options_globe 
					ENDIF 
				ENDIF 
			ELSE 
				pause_menu_gradient on 
			ENDIF 
		ENDIF 
		IF ( <keyboard_title> = "ENTER CHEAT" ) 
			FormatText ChecksumName = title_icon "%i_cheats" i = ( THEME_PREFIXES [ current_theme_prefix ] ) parent = keyboard_bg_anchor 
		ELSE 
			IF ( <keyboard_title> = "ENTER MESSAGE" ) 
				FormatText ChecksumName = title_icon "%i_name_cat" i = ( THEME_PREFIXES [ current_theme_prefix ] ) parent = keyboard_bg_anchor 
			ELSE 
				FormatText ChecksumName = title_icon "%i_options" i = ( THEME_PREFIXES [ current_theme_prefix ] ) parent = keyboard_bg_anchor 
			ENDIF 
		ENDIF 
		build_theme_sub_title title = <keyboard_title> title_icon = <title_icon> parent = keyboard_bg_anchor 
		IF NOT levelis load_skateshop 
			FormatText ChecksumName = paused_icon "%i_paused_icon" i = ( THEME_PREFIXES [ current_theme_prefix ] ) 
			build_theme_box_icons icon_texture = <paused_icon> parent = keyboard_bg_anchor 
			build_grunge_piece parent = keyboard_bg_anchor 
			build_top_bar pos = PAIR(0.00000000000, 62.00000000000) parent = keyboard_bg_anchor 
		ENDIF 
		IF GotParam text_block 
			CreateScreenElement { 
				Type = VMenu 
				parent = keyboard_anchor 
				id = keyboard_vmenu 
				pos = PAIR(320.00000000000, 247.00000000000) 
				internal_just = [ center top ] 
				regular_space_amount = 30 
				event_handlers = [ { pad_l2 keyboard_handle_L2 params = { max_length = <max_length> } } 
					{ pad_r2 keyboard_handle_R2 params = { max_length = <max_length> } } 
					{ pad_l1 keyboard_handle_L2 params = { max_length = <max_length> } } 
					{ pad_r1 keyboard_handle_R2 params = { max_length = <max_length> } } 
					{ pad_up keyboard_change_key_sound } 
					{ pad_down keyboard_change_key_sound } 
				] 
			} 
			IF NOT GotParam no_back 
				SetScreenElementProps { 
					id = keyboard_vmenu 
					event_handlers = [ 
						{ pad_back generic_menu_pad_back params = { callback = <keyboard_cancel_script> <keyboard_cancel_params> } } 
					] 
				} 
			ELSE 
				kill_start_key_binding 
			ENDIF 
		ELSE 
			CreateScreenElement { 
				Type = VMenu 
				parent = keyboard_anchor 
				id = keyboard_vmenu 
				pos = PAIR(320.00000000000, 197.00000000000) 
				internal_just = [ center top ] 
				regular_space_amount = 30 
				event_handlers = [ { pad_l2 keyboard_handle_L2 params = { max_length = <max_length> } } 
					{ pad_r2 keyboard_handle_R2 params = { max_length = <max_length> } } 
					{ pad_l1 keyboard_handle_L2 params = { max_length = <max_length> } } 
					{ pad_r1 keyboard_handle_R2 params = { max_length = <max_length> } } 
					{ pad_up keyboard_change_key_sound } 
					{ pad_down keyboard_change_key_sound } 
				] 
			} 
			IF NOT GotParam no_back 
				SetScreenElementProps { 
					id = keyboard_vmenu 
					event_handlers = [ 
						{ pad_back generic_menu_pad_back params = { callback = <keyboard_cancel_script> <keyboard_cancel_params> } } 
					] 
				} 
			ELSE 
				kill_start_key_binding 
			ENDIF 
		ENDIF 
		SetScreenElementProps { id = keyboard_vmenu 
			event_handlers = [ { pad_backspace keyboard_handle_backspace } 
				{ pad_space keyboard_handle_space params = { max_length = <max_length> } } 
			] 
		} 
		IF NOT GotParam no_back 
			SetScreenElementProps { 
				id = keyboard_vmenu 
				event_handlers = [ 
					{ pad_back generic_menu_pad_back params = { callback = <keyboard_cancel_script> <keyboard_cancel_params> } } 
				] 
			} 
		ELSE 
			kill_start_key_binding 
		ENDIF 
		IF GotParam text_block 
			keyboard_anchor : SetTags text_block 
			CreateScreenElement { 
				Type = ContainerElement 
				parent = keyboard_vmenu 
				internal_just = [ center center ] 
				dims = PAIR(10.00000000000, 20.00000000000) 
				not_focusable 
			} 
			CreateScreenElement { 
				Type = TextBlockElement 
				parent = keyboard_anchor 
				id = keyboard_display_string 
				font = dialog 
				just = [ center top ] 
				internal_just = [ Left center ] 
				text = <text> 
				not_focusable 
				pos = PAIR(320.00000000000, 90.00000000000) 
				dims = ( PAIR(1.00000000000, 0.00000000000) * keyboard_text_block_width + PAIR(20.00000000000, 400.00000000000) ) 
				allow_expansion 
				line_spacing = 0.86000001431 
				scale = 0.85000002384 
				rgba = <unhighlight_color> 
			} 
		ELSE 
			CreateScreenElement { 
				Type = ContainerElement 
				parent = keyboard_vmenu 
				id = keyboard_display_string_container 
				dims = PAIR(10.00000000000, 20.00000000000) 
				not_focusable 
			} 
			CreateScreenElement { 
				Type = TextElement 
				parent = <id> 
				id = keyboard_display_string 
				pos = ( PAIR(-139.00000000000, 7.00000000000) + <display_text_offset> ) 
				font = dialog 
				just = [ Left top ] 
				text = <text> 
				scale = <display_text_scale> 
				not_focusable 
				rgba = <unhighlight_color> 
			} 
		ENDIF 
		IF GotParam password 
			GetTextElementLength id = keyboard_display_string 
			SetScreenElementProps id = keyboard_display_string text = "" 
			IF ( <length> > 1 ) 
				BEGIN 
					TextElementConcatenate id = keyboard_display_string "*" 
				REPEAT ( <length> -1 ) 
				TextElementConcatenate id = keyboard_display_string "_" 
			ENDIF 
		ENDIF 
		IF GotParam text_block 
			CreateScreenElement { 
				Type = TextBlockElement 
				parent = keyboard_anchor 
				id = keyboard_current_string 
				font = dialog 
				just = [ center top ] 
				text = <org_text> 
				not_focusable 
				pos = PAIR(320.00000000000, 85.00000000000) 
				dims = ( PAIR(1.00000000000, 0.00000000000) * keyboard_text_block_width + PAIR(0.00000000000, 20.00000000000) ) 
				allow_expansion 
				scale = 0 
			} 
		ELSE 
			CreateScreenElement { 
				Type = TextElement 
				parent = keyboard_anchor 
				id = keyboard_current_string 
				font = dialog 
				just = [ center top ] 
				text = <org_text> 
				not_focusable 
				pos = PAIR(320.00000000000, 85.00000000000) 
				scale = 0 
			} 
		ENDIF 
		IF GotParam allowed_characters 
			keyboard_current_string : SetTags allowed_characters = <allowed_characters> 
		ENDIF 
		IF GotParam max_length 
			IF ( <max_length> < 1 ) 
				script_assert "create_onscreen_keyboard called with bad max_length" 
			ENDIF 
			BEGIN 
				GetTextElementLength id = keyboard_current_string 
				IF ( <length> > <max_length> ) 
					TextElementBackspace id = keyboard_current_string 
					TextElementBackspace id = keyboard_display_string 
				ELSE 
					BREAK 
				ENDIF 
			REPEAT 
		ENDIF 
		CreateScreenElement { 
			Type = SpriteElement 
			parent = keyboard_anchor 
			id = keyboard_options_bg 
			texture = options_bg 
			scale = PAIR(1.00000000000, 1.00000000000) 
			rgba = [ 0 0 0 0 ] 
			just = [ center center ] 
			pos = PAIR(267.00000000000, 80.00000000000) 
		} 
		keyboard_character_set_guide 
		keyboard_create_key_sprites <...> 
		keyboard_change_charset charset = alphanumeric_charset_lower max_length = <max_length> 
		Change keyboard_current_charset = alphanumeric_lower 
		FireEvent Type = focus target = keyboard_vmenu 
		DoScreenElementMorph id = keyboard_anchor pos = PAIR(320.00000000000, 600.00000000000) 
		DoScreenElementMorph id = keyboard_anchor pos = PAIR(320.00000000000, 240.00000000000) time = 0.30000001192 
	ENDIF 
	IF NOT GotParam no_buttons 
		AssignAlias id = keyboard_bg_anchor alias = current_menu_anchor 
	ENDIF 
	StartKeyboardHandler max_length = <max_length> 
ENDSCRIPT

