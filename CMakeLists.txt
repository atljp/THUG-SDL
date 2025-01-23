cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME_VAR "THUG_SDL")
project(${PROJECT_NAME_VAR})

set(SRC "${CMAKE_SOURCE_DIR}/src")

set(SOURCE_FILES
	"${SRC}/main.cpp"
	"${SRC}/exports.def"
	"${SRC}/Resources/Resources.rc"
	"${SRC}/patch.cpp"
	"${SRC}/config.cpp"
	"${SRC}/input.cpp"
	"${SRC}/script.cpp"
	"${SRC}/displayconfig.cpp"
	"${SRC}/Logger/Logger.cpp"
	"${SRC}/Logger/ErrorManager.cpp"
	"${SRC}/QB/LazyStruct.cpp"
	"${SRC}/QB/LazyArray.cpp"
	"${SRC}/QB/QB.cpp"
	"${SRC}/QB/CFuncs.cpp"
	"${SRC}/Mod/mod_template.cpp"
	"${SRC}/modloader.cpp"
	"${SRC}/event.cpp"
	"${SRC}/Util/sb.cpp"
)

set(HEADER_FILES
	"${SRC}/Resources/Resources.h"
	"${SRC}/d3d9.h"
	"${SRC}/patch.h"
	"${SRC}/config.h"
	"${SRC}/input.h"
	"${SRC}/displayconfig.h"
	"${SRC}/script.h"
	"${SRC}/keycodes.h"
	"${SRC}/Logger/Logger.h"
	"${SRC}/Logger/ErrorManager.h"
	"${SRC}/QB/LazyStruct.h"
	"${SRC}/QB/LazyArray.h"
	"${SRC}/QB/malloc.h"
	"${SRC}/QB/QB.h"
	"${SRC}/QB/CFuncs.h"
	"${SRC}/QB/scriptcontent.h"
	"${SRC}/Mod/mod_template.h"
	"${SRC}/modloader.h"
	"${SRC}/event.h"
	"${SRC}/Util/sb.h"
)

find_package(SDL2 REQUIRED)
list(APPEND LIBS
	SDL2::SDL2
	SDL2::SDL2main
)
list(APPEND INCLUDES ${SDL2_INCLUDE_DIRS})

add_library(${PROJECT_NAME_VAR} SHARED)

target_sources(${PROJECT_NAME_VAR} PRIVATE
	${SOURCE_FILES}
	${HEADER_FILES}
)

target_link_libraries(${PROJECT_NAME_VAR} PUBLIC ${LIBS} dbghelp)

target_include_directories(${PROJECT_NAME_VAR} PUBLIC
	"${PROJECT_SOURCE_DIR}/src"
	${INCLUDES}
)

set_source_files_properties(exports.def PROPERTIES HEADER_FILE_ONLY TRUE)

set_target_properties(
    ${PROJECT_NAME_VAR} 
    PROPERTIES 
        OUTPUT_NAME "d3d9"
)

add_executable(tsdl-launcher)

set(LSRC "${CMAKE_SOURCE_DIR}/launcher")

set(LSOURCE_FILES
	"${LSRC}/main.c"
	"${LSRC}/pgui.c"
	"${LSRC}/pagegeneral.c"
	"${LSRC}/pagekeyboard.c"
	"${LSRC}/pagegamepad.c"
	"${LSRC}/pagecontrol.c"
	"${LSRC}/Resources.rc"
)

set (LHEADER_FILES
	"${LSRC}/pgui.h"
	"${LSRC}/pagegeneral.h"
	"${LSRC}/pagekeyboard.h"
	"${LSRC}/pagegamepad.h"
	"${LSRC}/pagecontrol.h"
	"${LSRC}/global.h"
	"${LSRC}/Resources.h"
)

target_sources(tsdl-launcher PUBLIC 
	${LSOURCE_FILES}
	${LHEADER_FILES}
)
target_link_libraries(tsdl-launcher PRIVATE 
	${LIBS}
	comctl32.lib
	UxTheme.lib
)
target_include_directories(tsdl-launcher PUBLIC
	"${PROJECT_SOURCE_DIR}/src"
	${INCLUDES}
)

add_executable(thugsdlpatcher)

target_sources(thugsdlpatcher PUBLIC
	"${CMAKE_SOURCE_DIR}/patcher/main.c"
)

target_include_directories(thugsdlpatcher PUBLIC
	"${CMAKE_SOURCE_DIR}/src"
	${THIRDPARTY}
)