set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
set(TOOLCHAIN_PREFIX                arm-none-eabi-)

file(GLOB ARM_GNU_TOOLCHAIN_BIN_DIRS
    "/Applications/ArmGNUToolchain/*/arm-none-eabi/bin"
)

find_program(ARM_NONE_EABI_GCC
    NAMES ${TOOLCHAIN_PREFIX}gcc
    HINTS ${ARM_GNU_TOOLCHAIN_BIN_DIRS}
    REQUIRED
)
find_program(ARM_NONE_EABI_GXX
    NAMES ${TOOLCHAIN_PREFIX}g++
    HINTS ${ARM_GNU_TOOLCHAIN_BIN_DIRS}
    REQUIRED
)
find_program(ARM_NONE_EABI_OBJCOPY
    NAMES ${TOOLCHAIN_PREFIX}objcopy
    HINTS ${ARM_GNU_TOOLCHAIN_BIN_DIRS}
    REQUIRED
)
find_program(ARM_NONE_EABI_SIZE
    NAMES ${TOOLCHAIN_PREFIX}size
    HINTS ${ARM_GNU_TOOLCHAIN_BIN_DIRS}
    REQUIRED
)

set(CMAKE_C_COMPILER                ${ARM_NONE_EABI_GCC})
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${ARM_NONE_EABI_GXX})
set(CMAKE_LINKER                    ${ARM_NONE_EABI_GXX})
set(CMAKE_OBJCOPY                   ${ARM_NONE_EABI_OBJCOPY})
set(CMAKE_SIZE                      ${ARM_NONE_EABI_SIZE})

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU specific flags
set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections -fstack-usage")

# The cyclomatic-complexity parameter must be defined for the Cyclomatic complexity feature in STM32CubeIDE to work.
# However, most GCC toolchains do not support this option, which causes a compilation error; for this reason, the feature is disabled by default.
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcyclomatic-complexity")

set(CMAKE_C_FLAGS_DEBUG "-Og -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${CMAKE_SOURCE_DIR}/stm32wb55xx_flash_cm4.ld\"")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
set(TOOLCHAIN_LINK_LIBRARIES "m")
