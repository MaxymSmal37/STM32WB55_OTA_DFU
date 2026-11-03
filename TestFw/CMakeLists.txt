#############################################################################################################################
# file:  CMakeLists.txt
# brief: Template "CMakeLists.txt" for building of executables and static libraries.
#
# usage: Edit "VARIABLES"-section to suit project requirements.
#        For debug build:
#          cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Debug -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
#          make -C Debug VERBOSE=1 -j
#        For release build:
#          cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Release -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
#          make -C Release VERBOSE=1 -j
#############################################################################################################################
cmake_minimum_required(VERSION 3.20)

###################### CONSTANTS ######################################
set (PROJECT_TYPE_EXECUTABLE          "exe")
set (PROJECT_TYPE_STATIC_LIBRARY      "static-lib")
set (MCPU_CORTEX_M0				      "-mcpu=cortex-m0")
set (MCPU_CORTEX_M0PLUS				  "-mcpu=cortex-m0plus")
set (MCPU_CORTEX_M3				      "-mcpu=cortex-m3")
set (MCPU_CORTEX_M4				      "-mcpu=cortex-m4")
set (MCPU_CORTEX_M7				      "-mcpu=cortex-m7")
set (MCPU_CORTEX_M33				  "-mcpu=cortex-m33")
set (MCPU_CORTEX_M55				  "-mcpu=cortex-m55")
set (MCPU_CORTEX_M85				  "-mcpu=cortex-m85")
set (MFPU_FPV4_SP_D16                 "-mfpu=fpv4-sp-d16")
set (MFPU_FPV5_D16                    "-mfpu=fpv5-d16")
set (RUNTIME_LIBRARY_REDUCED_C        "--specs=nano.specs")
set (RUNTIME_LIBRARY_STD_C            "")
set (RUNTIME_LIBRARY_SYSCALLS_MINIMAL "--specs=nosys.specs")
set (RUNTIME_LIBRARY_SYSCALLS_NONE    "")
set (MFLOAT_ABI_SOFTWARE              "-mfloat-abi=soft")
set (MFLOAT_ABI_HARDWARE              "-mfloat-abi=hard")
set (MFLOAT_ABI_MIX                   "-mfloat-abi=softfp")
#######################################################################

###################### VARIABLES ######################################
set (PROJECT_NAME             "TestFw_for_OtaUpdate")
set (PROJECT_TYPE             "exe")
set (LINKER_SCRIPT            "${CMAKE_CURRENT_LIST_DIR}/STM32WB55CGUX_FLASH.ld")
set (MCPU                     "-mcpu=cortex-m4")
set (STM32CUBE_FW_PATH "$ENV{STM32CUBE_FW_PATH}" CACHE PATH "Path to the STM32CubeWB firmware package")
if (NOT STM32CUBE_FW_PATH)
  file (GLOB STM32CUBE_FW_DRIVERS "$ENV{HOME}/STM32Cube/Repository/STM32Cube_FW_WB_V*/Drivers")
  list (SORT STM32CUBE_FW_DRIVERS COMPARE NATURAL ORDER DESCENDING)
  if (STM32CUBE_FW_DRIVERS)
    list (GET STM32CUBE_FW_DRIVERS 0 STM32CUBE_FW_DRIVERS_DIR)
    get_filename_component (STM32CUBE_FW_PATH "${STM32CUBE_FW_DRIVERS_DIR}" DIRECTORY)
    set (STM32CUBE_FW_PATH "${STM32CUBE_FW_PATH}" CACHE PATH "Path to the STM32CubeWB firmware package" FORCE)
  endif()
endif()
if (NOT EXISTS "${STM32CUBE_FW_PATH}/Drivers/CMSIS/Device/ST/STM32WBxx/Include/stm32wb55xx.h")
  message (FATAL_ERROR "STM32CubeWB was not found. Set STM32CUBE_FW_PATH to the firmware package directory or STM32CUBE_FW_PATH environment variable.")
endif()
set (STM32WB_CMSIS_INCLUDE
  "${STM32CUBE_FW_PATH}/Drivers/CMSIS/Core/Include"
  "${STM32CUBE_FW_PATH}/Drivers/CMSIS/Device/ST/STM32WBxx/Include"
)
set (STM32WB_SYSTEM_SOURCE
  "${STM32CUBE_FW_PATH}/Drivers/CMSIS/Device/ST/STM32WBxx/Source/Templates/system_stm32wbxx.c"
)
set (MFPU                 "-mfpu=fpv4-sp-d16")
set (MFLOAT_ABI               "")
set (RUNTIME_LIBRARY          "--specs=nano.specs")
set (RUNTIME_LIBRARY_SYSCALLS "--specs=nosys.specs")


set (PROJECT_SOURCES
	# LIST SOURCE FILES HERE
	Startup/startup_stm32wb55cgux.s
	Sources/main.c
	Sources/syscalls.c
	Sources/sysmem.c
	${STM32WB_SYSTEM_SOURCE}


	)

set (PROJECT_DEFINES
	# LIST COMPILER DEFINITIONS HERE
	STM32WB55xx
    )

set (PROJECT_INCLUDES
	# LIST INCLUDE DIRECTORIES HERE
	${STM32WB_CMSIS_INCLUDE}
    )

############ MODIFY ACCORDING TO REQUIREMENTS) ########################

#######################################################################

################## PROJECT SETUP ######################################
if (NOT CMAKE_TOOLCHAIN_FILE)
  set (CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cubeide-gcc.cmake" CACHE FILEPATH "STM32 cross-compilation toolchain")
endif()

project(${PROJECT_NAME})
enable_language(ASM)

if (${PROJECT_TYPE} MATCHES ${PROJECT_TYPE_EXECUTABLE})
  add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
  add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMAND ${CMAKE_OBJCOPY} -O binary
      $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
      "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${CMAKE_PROJECT_NAME}>.bin"
    VERBATIM
  )
elseif (${PROJECT_TYPE} MATCHES ${PROJECT_TYPE_STATIC_LIBRARY})
  add_library(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

add_compile_definitions (${PROJECT_DEFINES})
include_directories (${PROJECT_INCLUDES})

set (CMAKE_EXECUTABLE_SUFFIX ".elf")
set (CMAKE_STATIC_LIBRARY_SUFFIX ".a")
set (CMAKE_C_FLAGS "${MCPU} -std=gnu11 ${MFPU} ${MFLOAT_ABI} ${RUNTIME_LIBRARY} -mthumb -Wall -Werror")
set (CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${RUNTIME_LIBRARY_SYSCALLS} -Wl,-Map=test.map -Wl,--gc-sections -static -Wl,--start-group -lc -lm -Wl,--end-group")
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")
