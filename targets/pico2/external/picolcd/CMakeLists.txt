add_library(picolcd INTERFACE)

pico_generate_pio_header(picolcd ${CMAKE_CURRENT_LIST_DIR}/picolcd.pio)

target_sources(picolcd INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/picolcd.c
)
target_include_directories(picolcd INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(picolcd INTERFACE
    pico_stdlib
    hardware_dma
    hardware_pio
)
