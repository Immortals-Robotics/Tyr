#pragma once

namespace Tyr::Vision
{
class KickDetector
{
public:
    KickDetector()  = default;
    ~KickDetector() = default;

    struct Kick
    {
        Kick(const Common::Vec2 &t_position, const Common::Vec2 &t_velocity, const Common::Angle &t_direction,
             const Common::TimePoint &t_time, const bool &t_is_kicked)
            : position(t_position), velocity(t_velocity), direction(t_direction), time(t_time), is_kicked(t_is_kicked)
        {}
        Kick()
        {
            is_kicked = false;
        }
        Common::Vec2      position;
        Common::Vec2      velocity;
        Common::Angle     direction;
        Common::TimePoint time;
        bool              is_kicked;
    };

    struct BallTime
    {
        BallTime(const Common::TimePoint &t_time, const Common::Vec2 &t_ball) : time(t_time), ball(t_ball)
        {}
        Common::TimePoint time;
        Common::Vec2      ball;
    };

    inline void reset()
    {
        m_can_kick           = false;
        m_candidate_kick_pos = Common::Vec2(-1000000, -1000000);
        m_candidate_robot    = Common::Robot(Common::Vec2(0, 0), 0, Common::Angle::fromDeg(0));
    }

    inline bool distanceIncreasingValidator(const Common::Vec2 &t_robot_kicker_pos)
    {
        return t_robot_kicker_pos.distanceTo(m_ball_records.back().ball) >
               t_robot_kicker_pos.distanceTo(m_ball_records.front().ball) + 50.f; // Need to be tuned
    }

    inline bool velocityValidator(const double &t_ball_velocity)
    {
        return t_ball_velocity > 600.f; // Need to be tuned
    }

    inline bool kickerValidator(const Common::Robot &t_robot)
    {
        return t_robot.canKick(m_ball_records.front().ball, Common::config().vision.kicker_depth);
    }

    inline Kick detect(const Common::Vec2 &t_ball_position, const Common::TimePoint &t_time,
                       const Common::RobotState t_own_robots[], const Common::RobotState t_opp_robots[],
                       const double &t_threshold = 0.8, const bool t_fast = false)
    {
        m_ball_records.push_back(BallTime(t_time, t_ball_position));
        Kick kick;

        if (m_ball_records.size() > (t_fast ? kFastRecordNum : kSlowRecordNum))
        {
            m_ball_records.pop_front();
        }

        std::vector<Common::Robot> robots;

        for (auto i = 0; i < Common::Config::Common::kMaxRobots; i++)
        {
            if (t_own_robots[i].seen_state != Common::SeenState::CompletelyOut)
            {
                robots.push_back(
                    Common::Robot(t_own_robots[i].position, Common::field().robot_radius, t_own_robots[i].angle));
            }
            if (t_opp_robots[i].seen_state != Common::SeenState::CompletelyOut)
            {
                robots.push_back(
                    Common::Robot(t_opp_robots[i].position, Common::field().robot_radius, t_opp_robots[i].angle));
            }
        }

        for (auto const &robot : robots)
        {
            if (robot.canKick(t_ball_position, Common::config().vision.kicker_depth))
            {
                m_candidate_kick_pos = t_ball_position;
                m_candidate_robot    = robot;
                m_can_kick           = true;
                continue;
            }
        }

        if (m_ball_records.size() < (t_fast ? kFastRecordNum : kSlowRecordNum) || !m_can_kick || !distanceIncreasingValidator(m_candidate_robot.center) || !kickerValidator(m_candidate_robot))
        {
            return kick;
        }

        std::vector<Common::Vec2> velocities;
        for (auto i = 0; i < m_ball_records.size() - 1; i++)
        {
            velocities.push_back((m_ball_records[i + 1].ball - m_ball_records[i].ball) * 1000000. /
                                 (m_ball_records[i + 1].time.microseconds() - m_ball_records[i].time.microseconds()));
        }

        std::vector<double> velocity_magnitudes;
        for (const auto &velocity : velocities)
        {
            velocity_magnitudes.push_back(velocity.length());
        }

        std::vector<Common::Vec2> velocity_changes;
        for (auto i = 0; i < velocities.size() - 1; i++)
        {
            velocity_changes.push_back(velocities[i + 1] - velocities[i]);
        }

        std::vector<double> velocity_changes_magnitudes;
        for (const auto &change : velocity_changes)
        {
            velocity_changes_magnitudes.push_back(change.length());
        }

        const double mean_velocity =
            std::accumulate(velocity_magnitudes.begin(), velocity_magnitudes.end(), 0) / velocity_magnitudes.size();
        const double sq_sum      = std::inner_product(velocity_magnitudes.begin(), velocity_magnitudes.end(),
                                                      velocity_magnitudes.begin(), 0.0);
        const double ev_velocity = std::sqrt(sq_sum / velocity_magnitudes.size() - mean_velocity * mean_velocity);
        const double threshold   = mean_velocity + t_threshold * ev_velocity;
        const bool   kick_detected =
            std::any_of(velocity_changes_magnitudes.begin(), velocity_changes_magnitudes.end(),
                        [threshold](double t_change_magnitude) { return t_change_magnitude > threshold; });

        Common::logError("kick detected {}", kick_detected);
        if (kick_detected && velocityValidator(velocities.back().length()))
        {
            kick = Kick(m_candidate_kick_pos, velocities.back(), m_candidate_robot.angle, m_ball_records.front().time,
                        true);
        }
        else
        {
            reset();
        }

        return kick;
    }

private:
    std::deque<BallTime> m_ball_records;
    Common::Vec2         m_candidate_kick_pos = Common::Vec2(-1000000, -1000000);
    Common::Robot        m_candidate_robot    = Common::Robot(Common::Vec2(0, 0), 0, Common::Angle::fromDeg(0));
    bool                 m_can_kick           = false;

    constexpr static int kFastRecordNum = 3;
    constexpr static int kSlowRecordNum = 5;
};

class ChipEstimator
{
public:
    struct ChipSolveResult
    {
        Eigen::VectorXd x;
        double          l1_error;
        double          t_offset;
        ChipSolveResult(Eigen::VectorXd t_x, double t_l1_error, double t_offset)
            : x(t_x), l1_error(t_l1_error), t_offset(t_offset) {};
    };

    struct Ball3D
    {
        Ball3D(const Common::Vec3 &t_position, const Common::Vec3 &t_velocity)
            : position(t_position), velocity(t_velocity)
        {}
        Ball3D(const Common::Vec2 &t_2d_position)
            : position(Common::Vec3(t_2d_position.x, t_2d_position.y, 0)), velocity(Common::Vec3(0, 0, 0))
        {}
        Common::Vec3 position;
        Common::Vec3 velocity;
    };

    static inline Common::Vec3 getCameraPos(const int t_id)
    {
        return Common::field().camera_calibrations[t_id].derived_camera_world_t;
    }

    inline ChipSolveResult estimateSpeedWithOffset(const double &t_offset, const Common::Vec3 &t_camera_pos)
    {
        if (m_ball_records.size() > Common::config().vision.chip_min_records)
        {
            Eigen::Vector3d f;
            f << t_camera_pos.x, t_camera_pos.y, t_camera_pos.z;
            Eigen::MatrixXd mat_a       = Eigen::MatrixXd(m_ball_records.size() * 2, 3);
            int             row         = 0;
            float           time_offset = 0.0;
            Eigen::VectorXd b           = Eigen::VectorXd(m_ball_records.size() * 2);

            double t_zero = m_ball_records.front().time.microseconds();

            for (const auto &record : m_ball_records)
            {
                time_offset = (record.time.microseconds() - t_zero) / 1000000. + t_offset;
                Eigen::Vector2d g(record.ball.x, record.ball.y);
                mat_a.row(row * 2) << f.z() * time_offset, 0, (g.x() - f.x()) * time_offset;
                mat_a.row((row * 2) + 1) << 0, f.z() * time_offset, (g.y() - f.y()) * time_offset;
                b(row * 2) = ((0.5 * kG * time_offset * time_offset * (g.x() - f.x())) + (g.x() * f.z())) -
                             (m_kick_position.x() * f.z());
                b(row * 2 + 1) = ((0.5 * kG * time_offset * time_offset * (g.y() - f.y())) + (g.y() * f.z())) -
                                 (m_kick_position.y() * f.z());
                row++;
            }

            Eigen::HouseholderQR<Eigen::MatrixXd> qr(mat_a);
            Eigen::VectorXd                       x;
            x = qr.solve(b);
            return ChipSolveResult(x, (mat_a * x - b).lpNorm<1>(), t_offset);
        }
        else
        {
            auto dummy = Eigen::VectorXd(3);
            dummy << kInvalidData, kInvalidData, kInvalidData;
            return ChipSolveResult(dummy, kInvalidData, kInvalidData);
        }
    }

    inline ChipSolveResult estimateSpeedStartPositionWithOffset(const double &t_offset, const Common::Vec3 &t_camera_pos)
    {
        if (m_ball_records.size() > Common::config().vision.chip_min_records)
        {
            Eigen::Vector3d f;
            f << t_camera_pos.x, t_camera_pos.y, t_camera_pos.z;
            Eigen::MatrixXd mat_a = Eigen::MatrixXd(m_ball_records.size() * 2, 5);
            int row = 0;
            float time_offset = 0.0;
            Eigen::VectorXd b = Eigen::VectorXd(m_ball_records.size() * 2);

            double t_zero = m_ball_records.front().time.microseconds();

            for (const auto &record : m_ball_records)
            {
                time_offset = (record.time.microseconds() - t_zero) / 1000000. + t_offset;
                Eigen::Vector2d g(record.ball.x, record.ball.y);
                
                // Matrix rows for x coordinate
                mat_a.row(row * 2) << f.z(), 0, f.z() * time_offset, 0, (g.x() - f.x()) * time_offset;
                // Matrix rows for y coordinate
                mat_a.row((row * 2) + 1) << 0, f.z(), 0, f.z() * time_offset, (g.y() - f.y()) * time_offset;
                
                // Right hand side of the equations
                b(row * 2) = (0.5 * kG * time_offset * time_offset * (g.x() - f.x())) + (g.x() * f.z());
                b(row * 2 + 1) = (0.5 * kG * time_offset * time_offset * (g.y() - f.y())) + (g.y() * f.z());
                
                row++;
            }

            // Solve the system using QR decomposition
            Eigen::HouseholderQR<Eigen::MatrixXd> qr(mat_a);
            Eigen::VectorXd x;
            try 
            {
                x = qr.solve(b);
                return ChipSolveResult(x, (mat_a * x - b).lpNorm<1>(), t_offset);
            }
            catch (...)
            {
                // Handle singular matrix exception
                auto dummy = Eigen::VectorXd(5);
                dummy << kInvalidData, kInvalidData, kInvalidData, kInvalidData, kInvalidData;
                return ChipSolveResult(dummy, kInvalidData, kInvalidData);
            }
        }
        else
        {
            auto dummy = Eigen::VectorXd(5);
            dummy << kInvalidData, kInvalidData, kInvalidData, kInvalidData, kInvalidData;
            return ChipSolveResult(dummy, kInvalidData, kInvalidData);
        }
    }

    inline void reset()
    {
        m_ball_records.clear();
        m_kick_detected = false;
        m_ball_height   = 0;
        m_v_0           = 0;
        m_result_found  = false;
    }

    static inline Common::Vec2 projectToGround(const Common::Vec3 t_ball_pos, Common::Vec3 t_camera_pos)
    {
        double scale = t_ball_pos.z / t_camera_pos.z;
        return Common::Vec2((t_camera_pos.x - t_ball_pos.x) * scale + t_ball_pos.x,
                            (t_camera_pos.y - t_ball_pos.y) * scale + t_ball_pos.y);
    }

    // Find the optimal time offset using iterative method


    inline ChipSolveResult findOptimalTimeOffsetSpeed(const Common::Vec3 &t_camera_pos)
    {
        if (m_ball_records.size() < Common::config().vision.chip_min_records)
        {
            auto dummy = Eigen::VectorXd(3);
            dummy << kInvalidData, kInvalidData, kInvalidData;
            return ChipSolveResult(dummy, kInvalidData, kInvalidData);
        }

        // Start with initial offset value
        double t_off = 0.05;
        double inc   = t_off / 2;

        // Iteratively refine the offset
        while (inc > 1e-3)
        {
            auto result_neg = estimateSpeedWithOffset(t_off - 1e-5, t_camera_pos);
            auto result_pos = estimateSpeedWithOffset(t_off + 1e-5, t_camera_pos);

            if (result_neg.l1_error == kInvalidData || result_pos.l1_error == kInvalidData)
            {
                auto dummy = Eigen::VectorXd(3);
                dummy << kInvalidData, kInvalidData, kInvalidData;
                return ChipSolveResult(dummy, kInvalidData, kInvalidData);
            }

            if (result_neg.l1_error > result_pos.l1_error)
            {
                t_off += inc;
            }
            else
            {
                t_off -= inc;
            }

            inc /= 2;
        }
       
        // Return the best result with the optimized offset
        return estimateSpeedWithOffset(t_off, t_camera_pos);
    }

    inline ChipSolveResult findOptimalTimeOffsetSpeedStartPosition(const Common::Vec3 &t_camera_pos)
    {
        if (m_ball_records.size() < Common::config().vision.chip_min_records)
        {
            auto dummy = Eigen::VectorXd(3);
            dummy << kInvalidData, kInvalidData, kInvalidData;
            return ChipSolveResult(dummy, kInvalidData, kInvalidData);
        }

         // Start with initial offset value
        double t_off = 0.05;
        double inc   = t_off / 2;

        // Iteratively refine the offset
        while (inc > 1e-3)
        {
            auto result_neg = estimateSpeedStartPositionWithOffset(t_off - 1e-5, t_camera_pos);
            auto result_pos = estimateSpeedStartPositionWithOffset(t_off + 1e-5, t_camera_pos);

            if (result_neg.l1_error == kInvalidData || result_pos.l1_error == kInvalidData)
            {
                auto dummy = Eigen::VectorXd(5);
                dummy << kInvalidData, kInvalidData, kInvalidData, kInvalidData, kInvalidData;
                return ChipSolveResult(dummy, kInvalidData, kInvalidData);
            }

            if (result_neg.l1_error > result_pos.l1_error)
            {
                t_off += inc;
            }
            else
            {
                t_off -= inc;
            }

            inc /= 2;
        }

        return estimateSpeedStartPositionWithOffset(t_off, t_camera_pos);
    }


    inline ChipSolveResult findOptimalMethod(const Common::Vec3 &t_camera_pos) {
        auto result_speed = findOptimalTimeOffsetSpeed(t_camera_pos);
        auto result_speed_start_position = findOptimalTimeOffsetSpeedStartPosition(t_camera_pos);

        if (result_speed.l1_error < result_speed_start_position.l1_error) {
            return result_speed;
        }
        return result_speed_start_position;
    }

    inline Ball3D getBall3D(const Common::Vec2 &t_ball_position, const int &t_camera_id,
                            const Common::TimePoint &t_time, const Common::RobotState t_own_robots[],
                            const Common::RobotState t_opp_robots[])
    {
        Ball3D result(t_ball_position);

        m_ball_records.push_back(KickDetector::BallTime(t_time, t_ball_position));
        if (m_ball_records.size() > Common::config().vision.chip_max_records)
        {
            m_ball_records.pop_front();
        }

        KickDetector::Kick kick = m_kick_detector.detect(t_ball_position, t_time, t_own_robots, t_opp_robots,
                                                         Common::config().vision.kick_threshold);

        if (kick.is_kicked && !m_kick_detected)
        {
            m_kick_detected = true;
            m_kick          = kick;
            m_kick_position << kick.position.x, kick.position.y;
        }

        if (m_ball_records.size() > 1 &&
            (m_ball_records.back().ball - m_ball_records[m_ball_records.size() - 2].ball).length() < 20.)
        {
            reset();
        }

        if (!m_kick_detected)
        {
            m_ball_records.clear();
            m_result_found = false;
            return result;
        }

        if (m_ball_records.size() < Common::config().vision.chip_min_records)
        {
            m_result_found = false;
            return result;
        }

        // Use the optimized time offset approach instead of fixed offset
        auto solved_result = findOptimalMethod(ChipEstimator::getCameraPos(t_camera_id));
        Common::logError("solved_result size {}", solved_result.x.size());
        auto vel           = solved_result.x.size() == 5 ? solved_result.x.segment<3>(2) : solved_result.x;

        Common::logError("vel z {} error {} offset {}", vel.z(), solved_result.l1_error, solved_result.t_offset);

        if ((solved_result.l1_error == kInvalidData || vel.z() < 100. ||
             vel.z() > Common::config().vision.chip_max_vel_z) &&
            !m_result_found)
        {
            return result;
        }

        if (solved_result.l1_error < Common::config().vision.chip_max_error && solved_result.l1_error > 0 &&
            vel.z() <= Common::config().vision.chip_max_vel_z)
        {
            m_v_0 = vel.z() +
                    kG * static_cast<double>(m_ball_records.front().time.microseconds() - m_kick.time.microseconds()) /
                        1000000;
            m_result_found = true;
        }

        double dt = static_cast<double>(t_time.microseconds() - m_kick.time.microseconds()) / 1000000;

        m_ball_height = m_v_0 * dt - 0.5 * kG * dt * dt;
        Common::logCritical("height {} vel {} error {} ", m_ball_height, vel.z(), solved_result.l1_error);
        if (m_ball_height <= 0)
        {
            m_ball_height   = 0;
            m_v_0           = 0;
            m_kick_detected = false;
        }

        result.position.z = m_ball_height;
        result.velocity   = Common::Vec3(vel.x(), vel.y(), m_v_0 - kG * dt);
        return result;
    }

private:
    std::deque<KickDetector::BallTime> m_ball_records;
    Eigen::Vector2d                    m_kick_position;
    bool                               m_kick_detected = false;
    KickDetector                       m_kick_detector;
    double                             m_ball_height = 0;
    double                             m_v_0         = 0;
    KickDetector::Kick                 m_kick;
    bool                               m_result_found = false;
    constexpr static float             kG             = 9810;
    constexpr static float             kInvalidData    = -1000000;
};
} // namespace Tyr::Vision
