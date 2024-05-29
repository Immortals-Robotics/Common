#pragma once

namespace Immortals::Common
{
struct Robot
{
    Robot(const Vec2 t_center, const float t_radius, const Angle t_angle)
        : center(t_center), radius(t_radius), angle(t_angle)
    {}

    // distance between front face of the robot to the center
    float frontDis() const
    {
        return radius * kHalfArcAngle.cos();
    }

    bool inside(const Vec2 t_point) const
    {
        const Vec2  rel = t_point - center;
        const float dis = rel.length();

        const Angle start_angle = angle - kHalfArcAngle;
        const Angle end_angle   = angle + kHalfArcAngle;

        return dis < frontDis() || (dis <= radius && rel.toAngle().isBetween(start_angle, end_angle));
    }

    static const inline Angle kHalfArcAngle = Angle::fromDeg(25.0f);

    Vec2  center;
    float radius;
    Angle angle;
};
} // namespace Immortals::Common
