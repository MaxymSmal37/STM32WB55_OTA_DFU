add_library(app OBJECT App.c AppComm.c)
target_include_directories(app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../BootloaderLib)
target_link_libraries(app PUBLIC stm32cubemx common logger)