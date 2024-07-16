#pragma once

namespace Immortals::Common::Soccer
{
enum class Role
{
    Unknown = 0,
    Gk      = 1,
    Def     = 2,
    Dmf     = 3,
    Mid2    = 4,
    Mid1    = 5,
    Attack  = 6,
    Rw      = 7,
    Lw      = 8,
};

enum class OneTouchType
{
    OneTouch = 0,
    Shirje   = 1,
    Gool     = 2,
    Allaf    = 3,
};

class Robot
{
public:
    Robot() = default;

    explicit Robot(const Protos::Immortals::Soccer::Robot &t_robot)
    {
        id = t_robot.id();

        role = static_cast<Role>(t_robot.role());

        shoot     = t_robot.shoot();
        chip      = t_robot.chip();
        dribbler  = t_robot.dribbler();
        navigated = t_robot.navigated();
        halted    = t_robot.halted();

        one_touch_type      = static_cast<OneTouchType>(t_robot.one_touch_type());
        one_touch_arriving  = t_robot.one_touch_arriving();
        one_touch_type_used = t_robot.one_touch_type_used();

        mark_target = t_robot.mark_target();
    }

    void fillProto(Protos::Immortals::Soccer::Robot *const t_robot) const
    {
        t_robot->set_id(id);

        t_robot->set_role(static_cast<Protos::Immortals::Soccer::Role>(role));

        t_robot->set_shoot(shoot);
        t_robot->set_chip(chip);
        t_robot->set_dribbler(dribbler);
        t_robot->set_navigated(navigated);
        t_robot->set_halted(halted);

        t_robot->set_one_touch_type(static_cast<Protos::Immortals::Soccer::OneTouchType>(one_touch_type));
        t_robot->set_one_touch_arriving(one_touch_arriving);
        t_robot->set_one_touch_type_used(one_touch_type_used);

        t_robot->set_mark_target(mark_target);
    }

    int id = -1;

    Role role = Role::Unknown;

    float shoot     = 0.0f;
    float chip      = 0.0f;
    float dribbler  = 0.0f;
    bool  navigated = false;
    bool  halted    = false;

    OneTouchType one_touch_type      = OneTouchType::OneTouch;
    bool         one_touch_arriving  = false;
    bool         one_touch_type_used = false;

    int mark_target = -1;
};
} // namespace Immortals::Common::Soccer
