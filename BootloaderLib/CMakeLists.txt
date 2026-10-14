add_library(bootLib OBJECT BootloaderLib.c)
target_include_directories(bootLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(bootLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Common)
target_link_libraries(bootLib PUBLIC stm32cubemx)