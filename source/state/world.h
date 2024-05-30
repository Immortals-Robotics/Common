#pragma once

#include "../config/config.h"
#include "../time/time_point.h"

namespace Immortals::Common
{
enum class SeenState
{
    CompletelyOut  = 0,
    TemporarilyOut = 1,
    Seen           = 2,
};

struct RawFrame
{
    unsigned camera_id    = 0;
    unsigned frame_number = 0;

    TimePoint t_capture;
    TimePoint t_sent;
    TimePoint t_capture_camera;

    RawFrame() = default;

    explicit RawFrame(const Protos::Ssl::Vision::DetectionFrame &t_frame)
    {
        camera_id    = t_frame.camera_id();
        frame_number = t_frame.frame_number();

        t_capture        = TimePoint::fromSeconds(t_frame.t_capture());
        t_sent           = TimePoint::fromSeconds(t_frame.t_sent());
        t_capture_camera = TimePoint::fromSeconds(t_frame.t_capture_camera());
    }

    explicit RawFrame(const Protos::Immortals::RawFrame &t_frame)
    {
        camera_id    = t_frame.camera_id();
        frame_number = t_frame.frame_number();

        t_capture        = TimePoint::fromMicroseconds(t_frame.t_capture());
        t_sent           = TimePoint::fromMicroseconds(t_frame.t_sent());
        t_capture_camera = TimePoint::fromMicroseconds(t_frame.t_capture_camera());
    }

    void fillProto(Protos::Immortals::RawFrame *const t_frame) const
    {
        t_frame->set_camera_id(camera_id);
        t_frame->set_frame_number(frame_number);

        t_frame->set_t_capture(t_capture.microseconds());
        t_frame->set_t_sent(t_sent.microseconds());
        t_frame->set_t_capture_camera(t_capture_camera.microseconds());
    }
};

struct RawRobotState
{
    float confidence = 0.0f;

    int       id;
    TeamColor color;

    float height = 0.0f;

    Vec2 position;
    Vec2 pixel_position;

    Angle angle;

    RawRobotState() = default;

    RawRobotState(const Protos::Ssl::Vision::DetectionRobot &t_robot, const TeamColor t_color,
                  const std::vector<RawFrame> *const t_frames, const unsigned t_frame_idx)
    {
        m_frames    = t_frames;
        m_frame_idx = t_frame_idx;

        confidence = t_robot.confidence();

        id    = t_robot.robot_id();
        color = t_color;

        height = t_robot.height();

        position       = Vec2(t_robot.x(), t_robot.y());
        pixel_position = Vec2(t_robot.pixel_x(), t_robot.pixel_y());

        angle = Angle::fromRad(t_robot.orientation());
    }

    explicit RawRobotState(const Protos::Immortals::RawRobotState &t_robot, const std::vector<RawFrame> *const t_frames)
    {
        m_frames    = t_frames;
        m_frame_idx = t_robot.frame_idx();

        confidence = t_robot.confidence();

        id    = t_robot.id();
        color = static_cast<TeamColor>(t_robot.color());

        height = t_robot.height();

        position       = Vec2{t_robot.position()};
        pixel_position = Vec2{t_robot.pixel_position()};

        angle = Angle{t_robot.angle()};
    }

    void fillProto(Protos::Immortals::RawRobotState *const t_state) const
    {
        t_state->set_frame_idx(m_frame_idx);

        t_state->set_confidence(confidence);

        t_state->set_id(id);
        t_state->set_color(static_cast<Protos::Immortals::TeamColor>(color));

        t_state->set_height(height);

        position.fillProto(t_state->mutable_position());
        pixel_position.fillProto(t_state->mutable_pixel_position());

        angle.fillProto(t_state->mutable_angle());
    }

    const RawFrame &frame() const
    {
        return (*m_frames)[m_frame_idx];
    }

private:
    const std::vector<RawFrame> *m_frames    = nullptr;
    unsigned                     m_frame_idx = 0;
};

struct RawBallState
{
    float confidence = 0.0f;

    Vec3 position;
    Vec2 pixel_position;

    unsigned area = 0;

    RawBallState() = default;

    explicit RawBallState(const Protos::Ssl::Vision::DetectionBall &t_ball, const std::vector<RawFrame> *const t_frames,
                          const unsigned t_frame_idx)
    {
        m_frames    = t_frames;
        m_frame_idx = t_frame_idx;

        confidence = t_ball.confidence();

        position       = Vec3(t_ball.x(), t_ball.y(), t_ball.z());
        pixel_position = Vec2(t_ball.pixel_x(), t_ball.pixel_y());

        area = t_ball.area();
    }

    explicit RawBallState(const Protos::Immortals::RawBallState &t_ball, const std::vector<RawFrame> *const t_frames)
    {
        m_frames    = t_frames;
        m_frame_idx = t_ball.frame_idx();

        confidence = t_ball.confidence();

        position       = Vec3{t_ball.position()};
        pixel_position = Vec2{t_ball.pixel_position()};

        area = t_ball.area();
    }

    void fillProto(Protos::Immortals::RawBallState *const t_ball) const
    {
        t_ball->set_frame_idx(m_frame_idx);

        t_ball->set_confidence(confidence);

        position.fillProto(t_ball->mutable_position());
        pixel_position.fillProto(t_ball->mutable_pixel_position());

        t_ball->set_area(area);
    }

    const RawFrame &frame() const
    {
        return (*m_frames)[m_frame_idx];
    }

private:
    const std::vector<RawFrame> *m_frames    = nullptr;
    unsigned                     m_frame_idx = 0;
};

struct RawWorldState
{
    TimePoint time;

    std::vector<RawFrame> frames;

    std::vector<RawBallState> balls;

    std::vector<RawRobotState> yellow_robots;
    std::vector<RawRobotState> blue_robots;

    RawWorldState() = default;

    explicit RawWorldState(const Protos::Immortals::RawWorldState &t_state)
    {
        time = TimePoint::fromMicroseconds(t_state.time());

        for (const auto &frame : t_state.frames())
        {
            frames.emplace_back(frame);
        }

        for (const auto &ball : t_state.balls())
        {
            balls.emplace_back(ball, &frames);
        }

        for (const auto &robot : t_state.yellow_robots())
        {
            yellow_robots.emplace_back(robot, &frames);
        }

        for (const auto &robot : t_state.blue_robots())
        {
            blue_robots.emplace_back(robot, &frames);
        }
    }

    void fillProto(Protos::Immortals::RawWorldState *const t_state) const
    {
        t_state->set_time(time.microseconds());

        for (const auto &frame : frames)
        {
            frame.fillProto(t_state->add_frames());
        }

        for (const auto &ball : balls)
        {
            ball.fillProto(t_state->add_balls());
        }

        for (const auto &robot : yellow_robots)
        {
            robot.fillProto(t_state->add_yellow_robots());
        }

        for (const auto &robot : blue_robots)
        {
            robot.fillProto(t_state->add_blue_robots());
        }
    }

    void addFrame(const Protos::Ssl::Vision::DetectionFrame &t_frame)
    {
        const unsigned frame_idx = frames.size();
        frames.emplace_back(t_frame);

        for (const auto &ball : t_frame.balls())
        {
            balls.emplace_back(ball, &frames, frame_idx);
        }

        for (const auto &robot : t_frame.robots_yellow())
        {
            yellow_robots.emplace_back(robot, TeamColor::Yellow, &frames, frame_idx);
        }

        for (const auto &robot : t_frame.robots_blue())
        {
            blue_robots.emplace_back(robot, TeamColor::Blue, &frames, frame_idx);
        }
    }
};

struct RobotState
{
    int       vision_id;
    TeamColor color;

    Vec2 position;
    Vec2 velocity;

    Angle angle;
    Angle angular_velocity;

    SeenState seen_state         = SeenState::CompletelyOut;
    bool      out_for_substitute = true;

    RobotState() = default;

    explicit RobotState(const Protos::Immortals::RobotState &t_robot)
    {
        vision_id = t_robot.id();
        color     = static_cast<TeamColor>(t_robot.color());

        position = Vec2{t_robot.position()};
        velocity = Vec2{t_robot.velocity()};

        angle            = Angle{t_robot.angle()};
        angular_velocity = Angle{t_robot.angular_velocity()};

        seen_state         = static_cast<SeenState>(t_robot.seen_state());
        out_for_substitute = t_robot.out_for_substitute();
    }

    void fillProto(Protos::Immortals::RobotState *const t_robot) const
    {
        t_robot->set_id(vision_id);
        t_robot->set_color(static_cast<Protos::Immortals::TeamColor>(color));

        position.fillProto(t_robot->mutable_position());
        velocity.fillProto(t_robot->mutable_velocity());

        angle.fillProto(t_robot->mutable_angle());
        angular_velocity.fillProto(t_robot->mutable_angular_velocity());

        t_robot->set_seen_state(static_cast<Protos::Immortals::SeenState>(seen_state));
        t_robot->set_out_for_substitute(out_for_substitute);
    }
};

struct BallState
{
    Vec2 position;
    Vec2 velocity;

    SeenState seen_state = SeenState::CompletelyOut;

    BallState() = default;

    explicit BallState(const Protos::Immortals::BallState &t_ball)
    {
        position = Vec2{t_ball.position()};
        velocity = Vec2{t_ball.velocity()};

        seen_state = static_cast<SeenState>(t_ball.seen_state());
    }

    explicit BallState(const Protos::Ssl::Vision::DetectionBall &t_ball)
    {
        position   = Vec2(t_ball.x(), t_ball.y());
        seen_state = SeenState::Seen;
    }

    void fillProto(Protos::Immortals::BallState *const t_ball) const
    {
        position.fillProto(t_ball->mutable_position());
        velocity.fillProto(t_ball->mutable_velocity());

        t_ball->set_seen_state(static_cast<Protos::Immortals::SeenState>(seen_state));
    }

    Line line() const
    {
        return Line::fromPointAndAngle(position, velocity.toAngle());
    }
};

struct FieldState
{
    float width  = 6000.0f;
    float height = 4500.0f;

    float goal_width = 1800.0f;
    float goal_depth = 180.0f;

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

struct WorldState
{
    TimePoint time;

    BallState ball;

    RobotState own_robot[Config::Common::kMaxRobots];
    RobotState opp_robot[Config::Common::kMaxRobots];

    WorldState()
    {
        for (int i = 0; i < Config::Common::kMaxRobots; i++)
        {
            own_robot[i].vision_id = i;
        }
        for (int i = 0; i < Config::Common::kMaxRobots; i++)
        {
            opp_robot[i].vision_id = i;
        }
    }

    explicit WorldState(const Protos::Immortals::WorldState &t_state)
    {
        time = TimePoint::fromMicroseconds(t_state.time());

        ball = static_cast<BallState>(t_state.ball());

        for (const auto &robot : t_state.own_robot())
        {
            own_robot[robot.id()] = static_cast<RobotState>(robot);
        }

        for (const auto &robot : t_state.opp_robot())
        {
            opp_robot[robot.id()] = static_cast<RobotState>(robot);
        }
    }

    void fillProto(Protos::Immortals::WorldState *const t_state) const
    {
        t_state->set_time(time.microseconds());

        ball.fillProto(t_state->mutable_ball());

        for (const auto &robot : own_robot)
        {
            robot.fillProto(t_state->add_own_robot());
        }

        for (const auto &robot : opp_robot)
        {
            robot.fillProto(t_state->add_opp_robot());
        }
    }
};
} // namespace Immortals::Common
