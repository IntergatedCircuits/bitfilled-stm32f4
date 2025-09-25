cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME bitfilled-stm32f4)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(${CMAKE_PROJECT_NAME}
    LANGUAGES C CXX ASM
)
message("Build type: " ${CMAKE_BUILD_TYPE})

include(cmake/get_cpm.cmake)
CPMAddPackage("gh:IntergatedCircuits/bitfilled#c5f625b3bc83d3da2aaadd5be343d69b253a2c38")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CPM_PACKAGE_bitfilled_SOURCE_DIR}/.clang-format ${CMAKE_SOURCE_DIR}/.clang-format
)
# STM32 sources are fetched and symlinked to their expected locations
CPMAddPackage("gh:STMicroelectronics/stm32f4xx-hal-driver@1.8.5")
CPMAddPackage("gh:STMicroelectronics/cmsis-device-f4@2.6.11")
CPMAddPackage("gh:STMicroelectronics/cmsis-core@5.9.0")

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CPM_PACKAGE_cmsis-core_SOURCE_DIR}/Include ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CPM_PACKAGE_cmsis-device-f4_SOURCE_DIR}/Include ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CPM_PACKAGE_stm32f4xx-hal-driver_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver
)

add_executable(${CMAKE_PROJECT_NAME}
    Src/main.cpp
)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

set(BITFILLED_SOLUTION 1)
set(USE_BIT_BAND 0)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    RESET_TO_MAIN # to disable this, the linker script also needs to be restored: ENTRY(Reset_Handler), and uncomment sections
    BITFILLED_SOLUTION=${BITFILLED_SOLUTION}
    USE_BIT_BAND=${USE_BIT_BAND}
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    bitfilled
)
