cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
project(imu-datalogger C CXX ASM)
pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(lib/FatFs_SPI) 

add_executable(${PROJECT_NAME}  
        main.c
        lib/hw_config.c
        lib/mpu6050/mpu6050.c
        lib/SD/SD.c
        lib/ssd1306/ssd1306.c
        lib/push_button/button.c
        )

# pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/lib/ws2812/ws2812.pio)

target_link_libraries(${PROJECT_NAME} 
        pico_stdlib
        FatFs_SPI
        hardware_clocks       
        hardware_i2c
        )

target_include_directories(${PROJECT_NAME}  PRIVATE   ${CMAKE_CURRENT_LIST_DIR} 
        lib/mpu6050
        lib/SD
        lib/ssd1306
        lib/push_button
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


pico_add_extra_outputs(${PROJECT_NAME})

