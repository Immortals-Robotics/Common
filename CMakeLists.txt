cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_UNITY_BUILDS "Enable unity build to improve build times" OFF)
option(TRACE_BUILD_TIME "Use -ftime-trace to generate build time trace" OFF)
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)

project(common)
set(PACKAGE_NAME immortals-${PROJECT_NAME})

set(CMAKE_DEBUG_POSTFIX d)

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

find_package(immortals-protos CONFIG REQUIRED)

find_package(asio CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(unofficial-lmdb CONFIG REQUIRED)
find_package(nng CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)

# needed to define type conversions
find_package(raylib CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

set(HEADER_FILES
        source/pch.h
        source/services.h

        source/config/base.h
        source/config/common.h
        source/config/file.h
        source/config/network.h
        source/config/config.h
        source/config/soccer.h
        source/config/vision.h

        source/debugging/color.h
        source/debugging/draw.h
        source/debugging/hub.h
        source/debugging/log.h
        source/debugging/source_location.h
        source/debugging/wrapper.h

        source/logging/debug_sink.h
        source/logging/logging.h
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
        source/math/geom/triangle.h

        source/network/address.h
        source/network/nng_client.h
        source/network/nng_server.h
        source/network/udp_client.h
        source/network/nng_message.h
        source/network/udp_server.h

        source/state/referee.h
        source/state/world.h
        source/storage/dumper.h
        source/storage/storage.h

        source/time/duration.h
        source/time/time_point.h
        source/time/timer.h)

set(SOURCE_FILES
        source/services.cpp

        source/config/file.cpp

        source/logging/logging.cpp
        source/math/linear.cpp
        source/math/geom/circle.cpp
        source/math/geom/line.cpp
        source/network/nng_client.cpp
        source/network/nng_server.cpp
        source/network/udp_client.cpp
        source/network/udp_server.cpp
        source/storage/dumper.cpp
        source/storage/storage.cpp)

add_library(${PROJECT_NAME}
        ${SOURCE_FILES} ${HEADER_FILES})
add_library(immortals::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
        immortals::protos
        asio::asio
        PkgConfig::tomlplusplus
        spdlog::spdlog
        unofficial::lmdb::lmdb
        nng::nng
        xxHash::xxhash
        raylib
        imgui::imgui)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN10)
endif ()

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
