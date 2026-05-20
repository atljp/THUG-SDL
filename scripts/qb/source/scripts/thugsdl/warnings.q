// -----------------------------------------------
//
//  W A R N I N G   L O G S
//      Similar to GHWT:DE's warning log system.
//		Ported from reTHAWed
//		Credits: Zedek the Plague Doctor
//
//  Since we bloat the console with so much junk,
//  we want a more streamlined warning system. This
//  can / should be enabled for normal users as well,
//  since it could show important warning messages.
//
// -----------------------------------------------

warning_message_can_show = 1
warning_message_text_scale = 1.00000000000
warning_message_height = 20.00000000000
warning_message_padding = 1.00000000000
warning_message_hold_time = 3.00000000000
warning_message_fade_time = 1.00000000000
warning_message_limit = 10

// -----------------------------------------------
// Scale a sprite element to a desired width and height. GH hack.
// id
// texture
// width
// height
// -----------------------------------------------

SCRIPT FitSpriteElement 
    desired_width = <width>
    desired_height = <height>
    DoScreenElementMorph id = <id> Scale = PAIR(1.00000000000, 1.00000000000)
    
    IF GotParam texture
        SetScreenElementProps id = <id> texture = <texture>
    ENDIF
    
    GetScreenElementDims id = <id>
    
    x_scale = ((<desired_width> / <width>) * PAIR(1.00000000000, 0.00000000000))
    y_scale = ((<desired_height> / <height>) * PAIR(0.00000000000, 1.00000000000))
    
    final_scale = (<x_scale> + <y_scale>)
    
    IF GotParam scale_mult
        final_scale = (<final_scale> * <scale_mult>)
    ENDIF
    
    DoScreenElementMorph id = <id> Scale = <final_scale>
ENDSCRIPT

// -----------------------------------------------
// Get pos_anchor position from an element.
//
//  id
//  pos
//  pos_anchor
// -----------------------------------------------

SCRIPT resolve_pos_anchor pos=PAIR(0.00000000000, 0.00000000000) pos_anchor=[left top]
    IF NOT ScreenElementExists id=<id>
        M_GetParentScript

        IF GotParam parent_script
            printf "resolve_pos_anchor fail, ID %g, parent script %h" g=<id> h=<parent_script>
        ELSE
            printf "resolve_pos_anchor fail, ID %g" g=<id>
        ENDIF

        RETURN pos=PAIR(0.00000000000, 0.00000000000)
    ENDIF

    GetScreenElementDims id=<id>
    parent_width = <width>
    parent_height = <height>
    
    anchor_x = (<pos_anchor> [0])
    anchor_y = (<pos_anchor> [1])
    
    x_pos = 0.00000000000
    y_pos = 0.00000000000
    
    SWITCH <anchor_x>
        CASE left
            x_pos = 0.00000000000
        CASE center
            x_pos = (<parent_width> * 0.50000000000)
        CASE right
            x_pos = <parent_width>
	ENDSWITCH
    
    SWITCH <anchor_y>
        CASE top
            y_pos = 0.00000000000
        CASE left
            y_pos = 0.00000000000
        CASE center
            y_pos = (<parent_height> * 0.50000000000)
        CASE bottom
            y_pos = <parent_height>
        CASE right
            y_pos = <parent_height>
	ENDSWITCH
            
    x_pos = (<x_pos> * PAIR(1.00000000000, 0.00000000000))
    y_pos = (<y_pos> * PAIR(0.00000000000, 1.00000000000))
    
	IF (<id> = root_window)
		 RETURN pos=PAIR(320.00000000000,16.00000000000)
	ELSE
		RETURN pos=((<x_pos> + <y_pos>) + <pos>)
	ENDIF
ENDSCRIPT

// -----------------------------------------------
// Wrapper function for CreateScreenElement.
// Does what stock function does, but adds extra
// functionality.
//
//  dims
//      For SpriteElements, this will make the
//      image stretch to the desired dimensions.
//
//  pos_anchor
//      Array, like just. Will create the element
//      anchored to a certain point on the parent.
//      This will NOT anchor it to that point
//      over time if the parent changes dims!
// -----------------------------------------------

SCRIPT CreateScreenElementEx 
    IF GotParam pos_anchor
        IF NOT GotParam pos
            pos=PAIR(0.00000000000, 0.00000000000)
        ENDIF
        
        IF GotParam parent
            resolve_pos_anchor id=<parent> pos=<pos> pos_anchor=<pos_anchor>
        ENDIF
        
        RemoveParameter pos_anchor
    ENDIF
    
    IF (<type> = SpriteElement)
        IF GotParam dims
            sprite_dims = <dims>
            RemoveParameter dims
        ENDIF
    ENDIF
    
    CreateScreenElement <...>
    
    IF GotParam sprite_dims
        x_scale = 1.00000000000
        y_scale = 1.00000000000
        
        IF GotParam flip_h
            y_scale = -1.00000000000
        ENDIF
        
        IF GotParam flip_v
            x_scale = -1.00000000000
        ENDIF
        
        IF GotParam id
            IF ScreenElementExists id=<id>
                FitSpriteElement id=<id> width=(<sprite_dims>.PAIR(1.00000000000, 0.00000000000)) height=(<sprite_dims>.PAIR(0.00000000000, 1.00000000000)) scale_mult=( (<x_scale>*PAIR(1.00000000000,0.00000000000)) + (<y_scale>*PAIR(0.00000000000,1.00000000000)) )
            ELSE
                printf "CreateScreenElementEx fail: Element %g not created. Check log." g=<id>
                PrintStruct { <...> }
            ENDIF
        ELSE
            printf "CreateScreenElementEx fail: Element had no ID. Check log."            
            PrintStruct { <...> }
        ENDIF
    ENDIF
    
    RETURN id=<id>
ENDSCRIPT

// -----------------------------------------
// Creates elements for the warning menu
// if they don't exist already.
// -----------------------------------------

SCRIPT Warn_CreateElements
    IF ScreenElementExists id=warnings_vmenu
        RETURN
    ENDIF

    // Figure out how much we'd like to scale this element by.
    // We ideally want this to be 1:1 based on whatever our resolution is.
    
    M_GetScreenValues
    
    // Create a VMenu that will hold the warnings.
    // Each warning will be an item in that list.
	
    CreateScreenElementEx {
        type = VMenu
        parent = root_window
        id = warnings_vmenu
        pos = PAIR(0.00000000000, 16.00000000000)
        dims = PAIR(640.00000000000, 320.00000000000)
        pos_anchor = [ center top ]
        just = [ center top ]
        internal_just = [ center top ]
        z_priority = 0
        scale = <screen_scaling>
    }
ENDSCRIPT

// -----------------------------------------
// Animate a warning element.
//
// Run this ON THE MAIN CONTAINER.
// -----------------------------------------

SCRIPT Warn_AnimateSelf
    wait warning_message_hold_time seconds
    DoMorph alpha=0.00000000000 time=warning_message_fade_time
    Die
ENDSCRIPT

// -----------------------------------------
// Show an actual warning. This should be
// called internally from a CFunc, since
// we can't really access arguments in the
// traditional way via qb.
//
//  text
//      The text to show.
//
//  type
//      Checksum. Type of message.
//      - warning
//      - info
// -----------------------------------------

SCRIPT Warn_AddMessage type=warning
    // Not allowed / not initialized.
    IF NOT IsTrue warning_message_can_show
        RETURN
    ENDIF
    
    IF NOT ScreenElementExists id=warnings_vmenu
        Warn_CreateElements
    ENDIF
    
    // This seems to cause position issues if we don't do this.
    // I still would like to know what this does exactly.
    
    SetScreenElementLock id = warnings_vmenu off
    
    IF ScreenElementExists id = {warnings_vmenu child = (warning_message_limit - 1)}
        DestroyScreenElement id = { warnings_vmenu child = 0 }
    ENDIF
    
    container_height = ( (warning_message_height * PAIR(0.00000000000, 1.00000000000)) + (warning_message_padding * PAIR(0.00000000000, 2.00000000000)) )
      
    CreateScreenElement {
        type = ContainerElement
        parent = warnings_vmenu
        dims = (PAIR(200.00000000000, 0.00000000000) + <container_height>)
        just = [center top]
    }
    
    container_id = <id>
    
    CreateScreenElementEx {
        id = <mangled_id>
        type = TextElement
        parent = <container_id>
        pos_anchor = [center center]
        text = <text>
        just = [center center]
        pos = PAIR(0.00000000000, 0.00000000000)
        scale = (warning_message_text_scale * PAIR(0.90000000000, 1.00000000000))
        font = small
        texture = white
        rgba = [80 80 80 128]
        z_priority = 50001
    }
    
    text_id = <id>
    GetScreenElementDims id=<text_id>
    
    bg_size_add = ( PAIR(80.00000000000, 0.00000000000) + (warning_message_height * PAIR(0.00000000000, 1.00000000000)) )
    bg_size_final = ( (<width> * PAIR(1.00000000000, 0.00000000000)) + <bg_size_add> )
    
    CreateScreenElementEx {
        id = <mangled_id>
        type = ContainerElement
        parent = <container_id>
        pos_anchor = [center center]
        just = [center center]
        dims = <bg_size_final>
        pos = PAIR(0.00000000000, 0.00000000000)
    }
     
    holster_id = <id>
    
    IF (<type> = warning)
        background_color = [128 100 0 128]
    ELSE
        background_color = [80 80 80 128]
    ENDIF
    
    CreateScreenElementEx {
        id = <mangled_id>
        type = SpriteElement
        parent = <holster_id>
        pos_anchor = [center center]
        just = [center center]
        dims = <bg_size_final>
        pos = PAIR(0.00000000000, 0.00000000000)
        texture = white
        rgba = <background_color>
        z_priority = 50000
    }
    
    CreateScreenElementEx {
        type = SpriteElement
        parent = <holster_id>
        pos_anchor = [left center]
        just = [left center]
        scale = 1.00000000000
        pos = PAIR(4.00000000000, 0.00000000000)
        rgba = [128 128 128 128]
        z_priority = 50001
    }
    
    CreateScreenElementEx {
        type = SpriteElement
        parent = <holster_id>
        pos_anchor = [right center]
        just = [right center]
        scale = 1.00000000000
        pos = PAIR(-4.00000000000, 0.00000000000)
        rgba = [128 128 128 128]
        z_priority = 50001
    }
    
    RunScriptOnScreenElement id=<container_id> Warn_AnimateSelf
ENDSCRIPT

// -----------------------------------------
// Wrapper function.
// -----------------------------------------

SCRIPT InfoMsg
    warn <...> type=info
ENDSCRIPT

