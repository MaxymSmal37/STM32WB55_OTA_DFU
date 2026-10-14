add_library(common OBJECT Common.c
    BootloaderConfig.h)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common PUBLIC stm32cubemx)