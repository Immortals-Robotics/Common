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
        pch.h
        services.h

        config/base.h
        config/common.h
        config/file.h
        config/network.h
        config/config.h
        config/soccer.h
        config/vision.h

        debugging/color.h
        debugging/draw.h
        debugging/hub.h
        debugging/log.h
        debugging/source_location.h
        debugging/wrapper.h

        logging/debug_sink.h
        logging/logging.h
        logging/macros.h

        math/angle.h
        math/helpers.h
        math/linear.h
        math/median_filter.h
        math/random.h
        math/vec2.h
        math/vec3.h
        math/geom/circle.h
        math/geom/line_segment.h
        math/geom/line.h
        math/geom/rect.h
        math/geom/triangle.h

        network/address.h
        network/nng_client.h
        network/nng_server.h
        network/udp_client.h
        network/nng_message.h
        network/udp_server.h

        state/referee.h
        state/world.h
        storage/dumper.h
        storage/storage.h

        time/duration.h
        time/time_point.h
        time/timer.h)

set(SOURCE_FILES
        services.cpp

        config/file.cpp

        logging/logging.cpp
        math/linear.cpp
        math/geom/circle.cpp
        math/geom/line.cpp
        network/nng_client.cpp
        network/nng_server.cpp
        network/udp_client.cpp
        network/udp_server.cpp
        storage/dumper.cpp
        storage/storage.cpp)

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

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_target
        LIBRARY DESTINATION lib)

install(DIRECTORY ${PROTO_GEN_BASE_DIR}/
        DESTINATION include/immortals/common
        FILES_MATCHING
        PATTERN "*.h"
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${PACKAGE_NAME}-config.cmake.in ${PACKAGE_NAME}-config.cmake INSTALL_DESTINATION share/${PACKAGE_NAME})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake
        DESTINATION share/${PACKAGE_NAME})

install(EXPORT ${PROJECT_NAME}_target
        NAMESPACE immortals::
        FILE ${PACKAGE_NAME}-targets.cmake
        DESTINATION share/${PACKAGE_NAME})
