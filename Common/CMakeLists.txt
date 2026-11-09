add_library(common OBJECT Common.c
    BootloaderConfig.h)
add_dependencies(common generate_fw_version)
target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FW_VERSION_GENERATED_DIR})
target_link_libraries(common PUBLIC stm32cubemx)