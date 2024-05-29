#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <numbers>
#include <optional>
#include <ostream>
#include <random>
#include <source_location>
#include <span>
#include <string.h>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#if FEATURE_UDP
#if defined(_WIN32)
#define NOGDI  // All GDI defines and routines
#define NOUSER // All USER defines and routines
#endif

#include <asio.hpp>

#if defined(_WIN32) // raylib uses these names as function parameters
#undef near
#undef far
#endif

#include <google/protobuf/message_lite.h>
#endif

#if FEATURE_NNG
#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>

#include <google/protobuf/message_lite.h>
#endif

#if FEATURE_STORAGE
#include <lmdb.h>
#endif

#if FEATURE_RAYLIB
#include <raylib.h>
#endif

#if FEATURE_IMGUI
#include <imgui.h>
#endif

#if FEATURE_LOGGING
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#endif

#if FEATURE_CONFIG_FILE
#include <toml++/toml.hpp>
#endif

#if FEATURE_DEBUG
#include <xxhash.h>
#endif

#include <protos/immortals/debug.pb.h>
#include <protos/immortals/state.pb.h>
#include <protos/ssl-vision/messages_robocup_ssl_wrapper.pb.h>

#include "services.h"

#include "math/angle.h"
#include "math/geom/circle.h"
#include "math/geom/line.h"
#include "math/geom/line_segment.h"
#include "math/geom/rect.h"
#include "math/geom/robot.h"
#include "math/geom/triangle.h"
#include "math/helpers.h"
#include "math/linear.h"
#include "math/median_filter.h"
#include "math/random.h"
#include "math/vec2.h"
#include "math/vec3.h"

#include "config/base.h"
#include "config/common.h"
#include "config/config.h"
#include "config/network.h"
#include "config/soccer.h"
#include "config/vision.h"

#if FEATURE_CONFIG_FILE
#include "config/file.h"
#endif

#if FEATURE_DEBUG
#include "debugging/color.h"
#include "debugging/draw.h"
#include "debugging/execution_time.h"
#include "debugging/hub.h"

#if FEATURE_LOGGING
#include "debugging/log.h"
#endif

#include "debugging/source_location.h"
#include "debugging/wrapper.h"
#endif

#if FEATURE_DEBUG && FEATURE_LOGGING
#include "logging/debug_sink.h"
#endif

#if FEATURE_LOGGING
#include "logging/logging.h"
#endif

#include "network/address.h"
#if FEATURE_UDP
#include "network/udp_client.h"
#include "network/udp_server.h"
#endif

#if FEATURE_NNG
#include "network/nng_client.h"
#include "network/nng_message.h"
#include "network/nng_server.h"
#endif

#include "state/referee.h"
#include "state/world.h"

#if FEATURE_STORAGE
#if FEATURE_NNG
#include "storage/dumper.h"
#endif
#include "storage/storage.h"
#endif

#include "time/duration.h"
#include "time/time_point.h"
#include "time/timer.h"
