add_library(logger OBJECT Logger.c)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(logger PRIVATE stm32cubemx)