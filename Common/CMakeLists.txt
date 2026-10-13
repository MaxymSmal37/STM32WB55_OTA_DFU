add_library(common STATIC Common.c)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})