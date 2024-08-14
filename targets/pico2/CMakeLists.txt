cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)

pico_sdk_init()


# -- Set project name - -
project(quake)

# - - Define which board you have - -
set(BOARD_TYPE PIMORONI_PICO_PLUS2_RP2350)

# - - Set audio mode - -
# set(AUDIO_PDM 1)
# set(AUDIO_DAC 1)

# - - Set video mode - -
# ( DVI needs additional libraries from https://github.com/Wren6991/PicoDVI )
# set(VIDEO_DVI 1)
# set(VIDEO_LCD 1)


# - - Uncomment to run from sram - -
# set(RUNFROMSRAM 1)


if(DEFINED VIDEO_DVI)
	set(VIDEO_FILE sys/video/video_dvi.c)
elseif(DEFINED VIDEO_LCD)
	set(VIDEO_FILE sys/video/video_lcd.c)
else()
	set(VIDEO_FILE sys/video/video_na.c)
endif()


if(DEFINED AUDIO_PDM)
	set(AUDIO_FILE sys/audio/video_pdm.c)
elseif(DEFINED AUDIO_DAC)
	set(AUDIO_FILE sys/audio/audio_dac.c)
else()
	set(AUDIO_FILE sys/audio/audio_na.c)
endif()



if(DEFINED RUNFROMSRAM)
	add_compile_definitions(PICO_NO_FLASH=1)
endif()

add_compile_definitions(PICO_BOARD=${BOARD_TYPE})

add_executable(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME} PUBLIC
	main.c

	${VIDEO_FILE}
	${AUDIO_FILE}

    ../../quake/render/d_surf.c
    ../../quake/render/d_edge.c
    ../../quake/render/d_fill.c
    ../../quake/render/d_init.c
    ../../quake/render/d_modech.c
    ../../quake/render/d_part.c
    ../../quake/render/d_polyse.c
    ../../quake/render/d_scan.c
    ../../quake/render/d_sky.c
    ../../quake/render/d_sprite.c
    ../../quake/render/d_vars.c
    ../../quake/render/model.c
    ../../quake/render/r_aclip.c
    ../../quake/render/r_alias.c
    ../../quake/render/r_bsp.c
    ../../quake/render/r_draw.c
    ../../quake/render/r_edge.c
    ../../quake/render/r_efrag.c
    ../../quake/render/r_light.c
    ../../quake/render/r_main.c
    ../../quake/render/r_misc.c
    ../../quake/render/r_part.c
    ../../quake/render/r_sky.c
    ../../quake/render/r_sprite.c
    ../../quake/render/r_surf.c
    ../../quake/render/r_vars.c
    ../../quake/chase.c
    ../../quake/cl_demo.c
    ../../quake/cl_input.c
    ../../quake/cl_main.c
    ../../quake/cl_parse.c
    ../../quake/cl_tent.c
    ../../quake/cmd.c
    ../../quake/common.c
    ../../quake/console.c
    ../../quake/crc.c
    ../../quake/cvar.c
    ../../quake/draw.c
    ../../quake/host.c
    ../../quake/host_cmd.c
    ../../quake/keys.c
    ../../quake/mathlib.c
    ../../quake/menu.c
    ../../quake/net_loop.c
    ../../quake/net_main.c
    ../../quake/net_vcr.c
    ../../quake/pr_cmds.c
    ../../quake/pr_edict.c
    ../../quake/pr_exec.c
    ../../quake/sbar.c
    ../../quake/screen.c
    ../../quake/sv_main.c
    ../../quake/sv_move.c
    ../../quake/sv_phys.c
    ../../quake/sv_user.c
    ../../quake/view.c
    ../../quake/wad.c
    ../../quake/world.c
    ../../quake/zone.c
)

target_compile_definitions(
	${PROJECT_NAME} PRIVATE
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
	pico_stdlib
	pico_multicore
	hardware_dma
	hardware_pio
if(DEFINED VIDEO_DVI)
	libdvi
endif
)

if(NOT DEFINED RUNFROMSRAM)
	pico_add_extra_outputs(${PROJECT_NAME})
else()
	pico_set_binary_type(${PROJECT_NAME} no_flash)
endif()

# Print debug strings over usb
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Print them over uart
# pico_enable_stdio_uart(${PROJECT_NAME} 1)
