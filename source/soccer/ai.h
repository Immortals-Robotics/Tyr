#pragma once

#include "dss/dss.h"
#include "errt/errt.h"
#include "helpers/one_touch_detector.h"
#include "plays/play_book.h"

namespace Tyr::Soccer
{
class Ai
{
private:
    Common::WorldState   m_world_state;
    Common::RefereeState m_ref_state;

    std::unique_ptr<Common::NngClient> m_world_client;
    std::unique_ptr<Common::NngClient> m_ref_client;

    std::unique_ptr<Common::UdpClient> m_strategy_client;

    std::unique_ptr<Common::NngServer> m_cmd_server;

    Common::Random m_random;

    using Play = void (Ai::*)();
    Play m_current_play;

    int m_func_state = 0;
    int m_func_count = 0;

    bool m_gk_intercepting;

    Common::Angle m_chip_head;

    float m_random_param;
    int   m_target_str;

    int m_gk     = 0;
    int m_def    = 1;
    int m_dmf    = 2;
    int m_mid2   = 3;
    int m_mid1   = 4;
    int m_attack = 5;
    int m_rw     = 6;
    int m_lw     = 7;

    int *m_stm_to_ai_num[Common::Setting::kMaxRobots] = {};

    Common::Vec2 m_allaf_pos[Common::Setting::kMaxRobots];

    std::map<int *, int> m_mark_map;

    int m_last_referee;

    Common::Timer m_timer;

    bool m_is_defending;
    bool m_opp_restarted;

    static constexpr int          kMaxBallHist = 240;
    std::deque<Common::BallState> m_ball_hist;
    Common::Linear                m_ball_line;

    Planner m_planner[Common::Setting::kMaxRobots];
    Dss    *m_dss;

    OneTouchDetector m_one_touch_detector[Common::Setting::kMaxRobots];
    enum class OneTouchType
    {
        OneTouch = 0,
        Shirje,
        Gool,
        Allaf,
    };
    OneTouchType m_one_touch_type[Common::Setting::kMaxRobots];
    bool         m_one_touch_type_used[Common::Setting::kMaxRobots];

    int m_side;

    // Helpers
    Common::Vec2 calculatePassPos(int t_robot_num, const Common::Vec2 &t_target, const Common::Vec2 &t_stat_pos,
                                  float t_bar = 89.0f);
    void         calculateBallTrajectory();
    float        calculateRobotReachTime(int t_robot_num, Common::Vec2 t_dest, VelocityProfile t_profile);
    float        calculateBallRobotReachTime(int t_robot_num, VelocityProfile t_profile);

    // boz ha
    void manageAttRoles();
    void calcIsDefending();
    void markManager(bool t_restart = true);

    PlayBook m_playbook;
    int      strategyWeight();

    Common::Vec2  m_predicted_ball;
    bool          m_circle_reached_behind_ball;
    Common::Angle calculateOneTouchAngle(int t_robot_num, Common::Vec2 t_one_touch_position);

    struct OpenAngle
    {
        // Center of the angle
        Common::Angle center;
        // how big it is
        Common::Angle magnitude;
    };

    OpenAngle calculateOpenAngleToGoal(Common::Vec2 t_pos, int t_robot_num);

    bool         ballIsGoaling();
    int          findNearestOpp(Common::Vec2 t_pos, int t_mask, bool t_accept_near_ball = true);
    int          findKickerOpp(int t_mask, float t_max_dis = 500.0f);
    bool         goalBlocked(Common::Vec2 t_init_pos, float t_max_shoot_blocker_dis, float t_shoot_blocker_r);
    bool         attackFuckingAngle();
    Common::Vec2 predictBallForwardAI(float t_time_ahead);
    float        calculateOppThreat(int t_opp, bool t_restart = false);
    float        calculateMarkCost(int t_robot_num, int t_opp);
    float        calculateSwitchToAttackerScore(int t_robot_num);

    // Skills
    enum NavigationFlags
    {
        NavigationFlagsNone                    = 0,
        NavigationFlagsForceNoObstacles        = (1 << 1), // only used in ball placement
        NavigationFlagsForceBallObstacle       = (1 << 2), // 500.0f
        NavigationFlagsForceBallMediumObstacle = (1 << 3), // 230.0f
        NavigationFlagsForceBallSmallObstacle  = (1 << 4), // 60.0f
    };

    void navigate(int t_robot_num, Common::Vec2 t_dest, VelocityProfile t_profile = VelocityProfile::mamooli(),
                  NavigationFlags t_flags = NavigationFlagsNone);
    void setObstacles(int t_robot_num, NavigationFlags t_flags = NavigationFlagsNone);

    void mark(int t_robot_num, int t_opp, float t_dist = 220.0f);
    void mark2Goal(int t_robot_num, int t_opp, float t_dist = 220.0f);
    void mark2Ball(int t_robot_num, int t_opp, float t_dist = 220.0f);
    void halt(int t_robot_num);

    void gkShirje(int t_robot_num);
    void gkHi(int t_robot_num);
    void defHi(int t_robot_num, int t_right_def_num, int t_left_def_num, Common::Vec2 *t_defend_target);
    void runningDef(int t_robot_num, Common::Vec2 t_target, Common::Vec2 *t_defend_target);
    void defenceWall(int t_robot_num, bool t_kick_off = false);
    void attacker(int t_robot_num, Common::Angle t_angle, int t_kick = 0, int t_chip = 0, bool t_restart = false,
                  bool t_kiss = false, bool t_dribbler = false);
    void waitForPass(int t_robot_num, bool t_chip = false, const Common::Vec2 *t_target = nullptr,
                     Common::Vec2 *t_stat_pos = nullptr);
    void waitForOmghi(int t_robot_num, bool t_chip = false);
    void waitForGool(int t_robot_num, bool t_chip = false);
    void receivePass(int t_robot_num, Common::Vec2 t_static_pos, bool t_chip = false);
    void circleBall(int t_robot_num, Common::Angle t_tagret_angle, int t_shoot_pow, int t_chip_pow, float t_precision,
                    float t_near_dis_override = -1.0f);

    // Plays
    void stop();
    void stopDef();
    void newNormalPlay();
    void normalPlayDef();
    void normalPlayAtt();
    void haltAll();
    void penaltyUsShootout();
    void kickoffUsChip();
    void throwinChipShoot();
    void kickoffTheirOneWall();
    void penaltyTheirSimple();
    void cornerTheirGlobal();
    void strategy();
    void ourPlaceBall();
    void theirPlaceBall();

    void internalProcessData();

    inline Common::Vec2 ownGoal() const
    {
        return Common::Vec2(m_side * Common::field().width, 0);
    }

    inline Common::Vec2 ownGoalPostTop() const
    {
        return ownGoal() + Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    inline Common::Vec2 ownGoalPostBottom() const
    {
        return ownGoal() - Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    inline Common::LineSegment ownGoalLine() const
    {
        return {ownGoalPostBottom(), ownGoalPostTop()};
    }

    inline Common::Vec2 oppGoal() const
    {
        return Common::Vec2(-m_side * Common::field().width, 0);
    }

    inline Common::Vec2 oppGoalPostTop() const
    {
        return oppGoal() + Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    inline Common::Vec2 oppGoalPostBottom() const
    {
        return oppGoal() - Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    inline Common::LineSegment oppGoalLine() const
    {
        return {oppGoalPostBottom(), oppGoalPostTop()};
    }

    inline bool isOut(Common::Vec2 t_point, const float t_margin = 0.0f) const
    {
        return std::fabs(t_point.x) > Common::field().width + t_margin ||
               std::fabs(t_point.y) > Common::field().height + t_margin;
    }

public:
    Robot m_own_robot[Common::Setting::kMaxRobots];

    Ai();

    bool receiveWorld();
    bool receiveReferee();

    bool publishCommands() const;

    void process();

    bool receivePlayBook();
    bool loadPlayBook(const std::filesystem::path &t_path);
    bool setPlayBook(const Protos::Immortals::PlayBook &t_playbook);
};
} // namespace Tyr::Soccer
