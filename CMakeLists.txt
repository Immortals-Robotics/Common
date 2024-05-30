cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_UNITY_BUILDS "Enable unity build to improve build times" OFF)
option(TRACE_BUILD_TIME "Use -ftime-trace to generate build time trace" OFF)
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)

# Features
option(FEATURE_UDP "UDP socket library based on asio" OFF)
option(FEATURE_NNG "NNG socket implementation" OFF)
option(FEATURE_STORAGE "Storage implementation based on lmdb" OFF)
option(FEATURE_RAYLIB "Base types interop with raylib types" OFF)
option(FEATURE_IMGUI "Base types interop with dear imgui types" OFF)
option(FEATURE_LOGGING "Logging implementation based on spdlog" OFF)
option(FEATURE_CONFIG_FILE "Config file implementation based on tomlplusplus" OFF)
option(FEATURE_DEBUG "Debug draw and log implementation using NNG and xxhash" OFF)

project(common CXX)
set(PACKAGE_NAME immortals-${PROJECT_NAME})

set(CMAKE_DEBUG_POSTFIX d)

set(libs)

if (${FEATURE_UDP})
    find_package(asio CONFIG REQUIRED)
    list(APPEND libs asio::asio)
endif ()
if (${FEATURE_NNG})
    find_package(nng CONFIG REQUIRED)
    list(APPEND libs nng::nng)
endif ()
if (${FEATURE_STORAGE})
    find_package(unofficial-lmdb CONFIG REQUIRED)
    list(APPEND libs unofficial::lmdb::lmdb)
endif ()
if (${FEATURE_RAYLIB})
    find_package(raylib CONFIG REQUIRED)
    list(APPEND libs raylib)
endif ()
if (${FEATURE_IMGUI})
    find_package(imgui CONFIG REQUIRED)
    list(APPEND libs imgui::imgui)
endif ()
if (${FEATURE_LOGGING})
    find_package(spdlog CONFIG REQUIRED)
    list(APPEND libs spdlog::spdlog)
endif ()
if (${FEATURE_CONFIG_FILE})
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
    list(APPEND libs PkgConfig::tomlplusplus)
endif ()
if (${FEATURE_DEBUG})
    if (NOT ${FEATURE_NNG})
        message(WARNING "Debug feature depends on NNG, disabling.")
        set(FEATURE_DEBUG OFF)
    else ()
        find_package(xxHash CONFIG REQUIRED)
        list(APPEND libs xxHash::xxhash)
    endif ()
endif ()

find_package(immortals-protos CONFIG REQUIRED)
list(APPEND libs immortals::protos)

# Enable ASAN
add_compile_options($<$<BOOL:${ENABLE_SANITIZERS}>:-fsanitize=address>)
add_link_options($<$<BOOL:${ENABLE_SANITIZERS}>:-fsanitize=address>)

# undefined behavior sanitizer is not available on MSVC
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options("$<$<BOOL:${ENABLE_SANITIZERS}>:-fno-omit-frame-pointer;-fsanitize=undefined>")
    add_link_options("$<$<BOOL:${ENABLE_SANITIZERS}>:-fno-omit-frame-pointer;-fsanitize=undefined>")
endif ()

# Set warning levels
if (MSVC)
    # level 4 as the base
    add_compile_options(/W4)
    # but without some annoying warnings
    add_compile_options(/wd4244 /wd4267)
else ()
    # set the base
    add_compile_options(-Wall -Wextra -Wpedantic)
    # and remove annoying warnings
    add_compile_options(-Wno-error=sign-compare)
endif ()

# And treat them as error
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

if (${TRACE_BUILD_TIME})
    add_compile_options(-ftime-trace)
endif ()

# enable utf-8 support in msvc
if (MSVC)
    add_compile_options(/utf-8)
endif ()

# enable multithreaded build in VS
if (MSVC)
    add_compile_options(/MP)
endif ()

add_library(${PROJECT_NAME} "")
add_library(immortals::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set(HEADER_FILES
        source/pch.h
        source/services.h

        source/logging/macros.h

        source/math/angle.h
        source/math/helpers.h
        source/math/linear.h
        source/math/median_filter.h
        source/math/random.h
        source/math/vec2.h
        source/math/vec3.h
        source/math/geom/circle.h
        source/math/geom/line_segment.h
        source/math/geom/line.h
        source/math/geom/rect.h
        source/math/geom/robot.h
        source/math/geom/triangle.h

        source/network/address.h

        source/state/ball.h

        source/state/referee.h
        source/state/robot.h
        source/state/seen_state.h
        source/state/world.h
        source/state/field/ball_model.h
        source/state/field/camera_calibration.h
        source/state/field/field.h
        source/state/raw/ball.h
        source/state/raw/frame.h
        source/state/raw/robot.h
        source/state/raw/world.h

        source/time/duration.h
        source/time/time_point.h
        source/time/timer.h

        source/config/base.h
        source/config/common.h
        source/config/network.h
        source/config/config.h
        source/config/soccer.h
        source/config/vision.h)

set(SOURCE_FILES
        source/services.cpp

        source/math/linear.cpp
        source/math/geom/circle.cpp
        source/math/geom/line.cpp)

if (${FEATURE_UDP})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FEATURE_UDP=1)
    if (WIN32)
        target_compile_definitions(${PROJECT_NAME} PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN10)
    endif ()
    list(APPEND HEADER_FILES
            source/network/udp_client.h
            source/network/udp_server.h)
    list(APPEND SOURCE_FILES
            source/network/udp_client.cpp
            source/network/udp_server.cpp)
endif ()
if (${FEATURE_NNG})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FEATURE_NNG=1)
    list(APPEND HEADER_FILES
            source/network/nng_client.h
            source/network/nng_server.h
            source/network/nng_message.h)
    list(APPEND SOURCE_FILES
            source/network/nng_client.cpp
            source/network/nng_server.cpp)
endif ()
if (${FEATURE_STORAGE})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FEATURE_STORAGE=1)
    list(APPEND HEADER_FILES
            source/storage/storage.h)
    list(APPEND SOURCE_FILES
            source/storage/storage.cpp)

    if (${FEATURE_NNG})
        list(APPEND HEADER_FILES
                source/storage/dumper.h)
        list(APPEND SOURCE_FILES
                source/storage/dumper.cpp)
    endif ()
endif ()
if (${FEATURE_RAYLIB})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FEATURE_RAYLIB=1)
endif ()
if (${FEATURE_IMGUI})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FEATURE_IMGUI=1)
endif ()
if (${FEATURE_LOGGING})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FEATURE_LOGGING=1)
    list(APPEND HEADER_FILES
            source/logging/logging.h)
    list(APPEND SOURCE_FILES
            source/logging/logging.cpp)

    if (${FEATURE_DEBUG})
        list(APPEND HEADER_FILES
                source/logging/debug_sink.h)
    endif ()
endif ()
if (${FEATURE_CONFIG_FILE})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FEATURE_CONFIG_FILE=1)
    list(APPEND HEADER_FILES
            source/config/file.h)
    list(APPEND SOURCE_FILES
            source/config/file.cpp)
endif ()
if (${FEATURE_DEBUG})
    target_compile_definitions(${PROJECT_NAME} PUBLIC FEATURE_DEBUG=1)
    list(APPEND HEADER_FILES
            source/debugging/color.h
            source/debugging/draw.h
            source/debugging/execution_time.h
            source/debugging/hub.h
            source/debugging/log.h
            source/debugging/source_location.h
            source/debugging/wrapper.h)
endif ()

target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC ${libs})

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME} PRIVATE source/pch.h)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_target
        LIBRARY DESTINATION lib)

install(DIRECTORY source/
        DESTINATION include/common
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "pch.h" EXCLUDE)

install(FILES source/pch.h
        DESTINATION include/common
        RENAME common.h)

include(CMakePackageConfigHelpers)
configure_package_config_file(${PACKAGE_NAME}-config.cmake.in ${PACKAGE_NAME}-config.cmake INSTALL_DESTINATION share/${PACKAGE_NAME})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake
        DESTINATION share/${PACKAGE_NAME})

install(EXPORT ${PROJECT_NAME}_target
        NAMESPACE immortals::
        FILE ${PACKAGE_NAME}-targets.cmake
        DESTINATION share/${PACKAGE_NAME})
