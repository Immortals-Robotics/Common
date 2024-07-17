#pragma once

#include "base.h"

namespace Immortals::Common::Config
{
struct RobotPhysicalStatus
{
    bool has_dribbler    = true;
    bool has_direct_kick = true;
    bool has_chip_kick   = true;
    bool is_3D_printed   = false;
};

struct Soccer final : IConfig
{
#if FEATURE_CONFIG_FILE
    void load(const toml::node_view<const toml::node> t_node) override
    {
        one_touch_beta    = t_node["one_touch_beta"].value_or(one_touch_beta);
        one_touch_gamma   = t_node["one_touch_gamma"].value_or(one_touch_gamma);
        one_touch_shoot_k = t_node["one_touch_shoot_k"].value_or(one_touch_shoot_k);

        gk_tight_start_angle  = t_node["gk_tight_start_angle"].value_or(gk_tight_start_angle);
        def_tight_start_angle = t_node["def_tight_start_angle"].value_or(def_tight_start_angle);
        def_prediction_time   = t_node["def_prediction_time"].value_or(def_prediction_time);
        def_max_extension_area   = t_node["def_max_extension_area"].value_or(def_max_extension_area);
        def_ball_distance_coef   = t_node["def_ball_distance_coef"].value_or(def_ball_distance_coef);

        mark_in_stop = t_node["mark_in_stop"].value_or(mark_in_stop);

        if (auto *robot_physical_status_array = t_node["robot_physical_status"].as_array())
        {
            for (size_t i = 0; i < robot_physical_status_array->size(); i++)
            {
                int id = t_node["robot_physical_status"][i]["id"].value_or(-1);
                if (id == -1)
                {
                    logError("Robot ID for #{} is not defined in the .toml config file", i);
                }
                if (id != i)
                {
                    // This check is to make sure the config file is written in order so
                    // less mistake will happen during the match setup
                    logError("Robot ID is out of order in iteration #{} the id should is {} but should be {}", i, id,
                             i);
                }
                robot_physical_status[id].has_dribbler =
                    t_node["robot_physical_status"][i]["has_dribbler"].value_or(false);
                robot_physical_status[id].has_direct_kick =
                    t_node["robot_physical_status"][i]["has_direct_kick"].value_or(false);
                robot_physical_status[id].has_chip_kick =
                    t_node["robot_physical_status"][i]["has_chip_kick"].value_or(false);
                robot_physical_status[id].is_3D_printed =
                    t_node["robot_physical_status"][i]["is_3D_printed"].value_or(false);
            }
        }
    }
#endif

    RobotPhysicalStatus robot_physical_status[Common::kMaxRobots];

    float one_touch_beta        = 0.4f;  // Damping factor
    float one_touch_gamma       = 0.14f; // Reflect factor
    float one_touch_shoot_k     = 4000.0f;
    float gk_tight_start_angle  = 20.0f;
    float def_tight_start_angle = 40.0f;
    float def_prediction_time   = 0.5f;
    float def_max_extension_area = 1100.0f;
    float def_ball_distance_coef = 0.7f;
    bool mark_in_stop = false;
};
} // namespace Immortals::Common::Config
