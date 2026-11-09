foreach(required_variable SOURCE_DIR OUTPUT_FILE TEMPLATE_FILE)
    if(NOT DEFINED ${required_variable})
        message(FATAL_ERROR "${required_variable} is required")
    endif()
endforeach()

if(NOT DEFINED FW_VERSION_PREFIX_MAJOR)
    set(FW_VERSION_PREFIX_MAJOR "M")
endif()

if(NOT DEFINED FW_VERSION_DEFAULT)
    set(FW_VERSION_DEFAULT "A1.2.3-a243dt3c")
endif()

if(NOT FW_VERSION_PREFIX_MAJOR MATCHES "^[A-Za-z0-9]$")
    message(FATAL_ERROR "FW_VERSION_PREFIX_MAJOR must be one alphanumeric character")
endif()

if(NOT FW_VERSION_DEFAULT MATCHES "^[A-Za-z0-9][A-Za-z0-9.+-]*$")
    message(FATAL_ERROR "FW_VERSION_DEFAULT contains characters unsafe for a C string")
endif()

set(FW_VERSION "${FW_VERSION_DEFAULT}")
set(FW_VERSION_GIT_TAG "")
set(FW_VERSION_GIT_HASH "")
set(FW_VERSION_MAJOR 0)
set(FW_VERSION_MINOR 0)
set(FW_VERSION_PATCH 0)
set(FW_VERSION_IS_TAGGED 0)
set(FW_VERSION_IS_RELEASE 0)
set(FW_VERSION_IS_DIRTY 0)

if(FW_VERSION_DEFAULT MATCHES "^[A-Za-z]([0-9]+)\\.([0-9]+)\\.([0-9]+)")
    set(FW_VERSION_MAJOR "${CMAKE_MATCH_1}")
    set(FW_VERSION_MINOR "${CMAKE_MATCH_2}")
    set(FW_VERSION_PATCH "${CMAKE_MATCH_3}")
endif()

execute_process(
    COMMAND git -C "${SOURCE_DIR}" rev-parse --is-inside-work-tree
    RESULT_VARIABLE git_repo_result
    OUTPUT_VARIABLE git_repo
    ERROR_VARIABLE git_repo_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(git_repo_result EQUAL 0 AND git_repo STREQUAL "true")
    execute_process(
        COMMAND git -C "${SOURCE_DIR}" rev-parse --short=8 HEAD
        RESULT_VARIABLE git_hash_result
        OUTPUT_VARIABLE git_hash
        ERROR_VARIABLE git_hash_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT git_hash_result EQUAL 0 OR NOT git_hash MATCHES "^[0-9a-fA-F]+$")
        message(FATAL_ERROR "Unable to determine firmware Git hash: ${git_hash_error}")
    endif()
    set(FW_VERSION_GIT_HASH "${git_hash}")

    execute_process(
        COMMAND git -C "${SOURCE_DIR}" describe --tags --exact-match
                --match "${FW_VERSION_PREFIX_MAJOR}[0-9]*" HEAD
        RESULT_VARIABLE exact_tag_result
        OUTPUT_VARIABLE exact_tag
        ERROR_VARIABLE exact_tag_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(exact_tag_result EQUAL 0)
        set(git_tag "${exact_tag}")
        set(git_tag_result 0)
        set(FW_VERSION_IS_RELEASE 1)
    else()
        execute_process(
            COMMAND git -C "${SOURCE_DIR}" describe --tags --abbrev=0
                    --match "${FW_VERSION_PREFIX_MAJOR}[0-9]*" HEAD
            RESULT_VARIABLE git_tag_result
            OUTPUT_VARIABLE git_tag
            ERROR_VARIABLE git_tag_error
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()

    if(git_tag_result EQUAL 0)
        if(NOT git_tag MATCHES "^${FW_VERSION_PREFIX_MAJOR}([0-9]+)\\.([0-9]+)\\.([0-9]+)([-+][0-9A-Za-z.-]+)?$")
            message(FATAL_ERROR
                "Unsupported firmware tag '${git_tag}'. Use "
                "${FW_VERSION_PREFIX_MAJOR}MAJOR.MINOR.PATCH with optional SemVer metadata.")
        endif()
        set(FW_VERSION_MAJOR "${CMAKE_MATCH_1}")
        set(FW_VERSION_MINOR "${CMAKE_MATCH_2}")
        set(FW_VERSION_PATCH "${CMAKE_MATCH_3}")
        set(FW_VERSION_GIT_TAG "${git_tag}")
        set(FW_VERSION
            "${FW_VERSION_PREFIX_MAJOR}${FW_VERSION_MAJOR}.${FW_VERSION_MINOR}.${FW_VERSION_PATCH}${CMAKE_MATCH_4}")
        if(NOT FW_VERSION_IS_RELEASE)
            string(APPEND FW_VERSION "-${git_hash}")
        endif()
        set(FW_VERSION_IS_TAGGED 1)
    elseif(NOT git_tag_result MATCHES "^[0-9]+$")
        message(FATAL_ERROR "Unable to determine firmware Git tag: ${git_tag_error}")
    endif()

    execute_process(
        COMMAND git -C "${SOURCE_DIR}" status --porcelain
        RESULT_VARIABLE git_status_result
        OUTPUT_VARIABLE git_status
        ERROR_VARIABLE git_status_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT git_status_result EQUAL 0)
        message(FATAL_ERROR "Unable to determine firmware worktree status: ${git_status_error}")
    endif()
    if(NOT git_status STREQUAL "")
        set(FW_VERSION_IS_DIRTY 1)
        message(WARNING
            "Building firmware from a dirty Git worktree. FW_VERSION identifies HEAD; "
            "FW_VERSION_IS_DIRTY is set in FwVersion.h.")
    endif()
else()
    message(STATUS "Git metadata unavailable; using default firmware version ${FW_VERSION}")
endif()

string(LENGTH "${FW_VERSION}" firmware_version_length)
if(firmware_version_length GREATER 16)
    message(FATAL_ERROR
        "Firmware version '${FW_VERSION}' is ${firmware_version_length} bytes; "
        "the BLE firmware-version characteristic supports at most 16 bytes.")
endif()

get_filename_component(output_directory "${OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${output_directory}")
configure_file("${TEMPLATE_FILE}" "${OUTPUT_FILE}" @ONLY)

if(NOT FW_VERSION_IS_TAGGED)
    message(STATUS "No reachable ${FW_VERSION_PREFIX_MAJOR}MAJOR.MINOR.PATCH tag found; "
        "using default firmware version ${FW_VERSION}")
endif()
