#pragma once

namespace Immortals::Common
{
struct FieldState
{
    float width  = 6000.0f;
    float height = 4500.0f;

    float goal_width  = 1800.0f;
    float goal_depth  = 180.0f;
    float goal_height = 200.0f;

    float boundary_width = 300.0f;

    float penalty_area_depth = 1800.0f;
    float penalty_area_width = 3600.0f;

    float center_circle_radius = 500.0f;

    float line_thickness = 10.0f;

    float goal_center_to_penalty_mark = 8000.0f;

    float ball_radius  = 21.5f;
    float robot_radius = 90.0f;

    FieldState() = default;

    explicit FieldState(const Protos::Ssl::Vision::GeometryFieldSize &t_field)
    {
        width  = t_field.field_length() / 2.0f;
        height = t_field.field_width() / 2.0f;

        goal_width = t_field.goal_width();
        goal_depth = t_field.goal_depth();
        if (t_field.has_goal_height())
            goal_height = t_field.goal_height();

        boundary_width = t_field.boundary_width();

        if (t_field.has_penalty_area_depth())
            penalty_area_depth = t_field.penalty_area_depth();
        if (t_field.has_penalty_area_width())
            penalty_area_width = t_field.penalty_area_width();

        if (t_field.has_center_circle_radius())
            center_circle_radius = t_field.center_circle_radius();

        if (t_field.has_line_thickness())
            line_thickness = t_field.line_thickness();

        if (t_field.has_goal_center_to_penalty_mark())
            goal_center_to_penalty_mark = t_field.goal_center_to_penalty_mark();

        if (t_field.has_ball_radius())
            ball_radius = t_field.ball_radius();

        if (t_field.has_max_robot_radius())
            robot_radius = t_field.max_robot_radius();
    }
};
} // namespace Immortals::Common
