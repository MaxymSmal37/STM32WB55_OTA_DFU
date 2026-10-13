add_library(app STATIC App.c)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})