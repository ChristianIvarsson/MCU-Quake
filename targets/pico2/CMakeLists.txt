cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)

pico_sdk_init()


# -- Set project name - -
project(quake)


# - - Set audio mode - -
# set(AUDIO_PDM 1)
# set(AUDIO_DAC 1)

# - - Set video mode - -
# ( DVI needs additional libraries from https://github.com/Wren6991/PicoDVI )
# set(VIDEO_DVI 1)
set(VIDEO_LCD 1)


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

# External libraries
include(
    ${CMAKE_CURRENT_SOURCE_DIR}/external/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/CMakeLists.txt
)
include(
    ${CMAKE_CURRENT_SOURCE_DIR}/external/picolcd/CMakeLists.txt
)



# This one is likely changing the size for the both of them
# PICO_STACK_SIZE=0x2000
# Was easier to just diy stuff in the linker script. There is enough stack

add_compile_definitions(
    MAX_NET_DRIVERS=1
)

# Incompatible with SDK
# -fno-short-enums
# -Werror

add_compile_options(
    -trigraphs
    -Wall
    -Wpedantic

    # Fixing these will take some effort so just shut them up for now
    -Wno-dangling-pointer
    -Wno-strict-aliasing

    # And th
    -Wno-stringop-truncation
    -Wno-trigraphs
    -Wno-format
)

add_executable(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME} PUBLIC
	main.c
    qfile.c
    external/sfe_psram.c
    external/sd_config.c

    sys/video/video_common.c
    ${VIDEO_FILE}

    sys/audio/audio_common.c
	${AUDIO_FILE}

    sys/sys_rp2.c
    sys/input/input_common.c
    sys/net/net_common.c
    sys/cd_null.c

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
    ../../quake/qfile.c

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
    pico_flash
    picolcd
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

target_include_directories(
    ${PROJECT_NAME} PRIVATE 
    .
    external/.
    external/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source/
    external/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include/
    external/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/
)

pico_set_linker_script(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/pico2.ld)

#if(DEFINED VIDEO_DVI)
#	libdvi
# endif

pico_add_extra_outputs(${PROJECT_NAME})


# Print debug strings over usb
pico_enable_stdio_usb(${PROJECT_NAME} 0)

# Print them over uart
pico_enable_stdio_uart(${PROJECT_NAME} 1)
