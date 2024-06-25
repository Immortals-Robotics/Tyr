#pragma once

#include "dss/dss.h"
#include "errt/errt.h"
#include "helpers/ball_trajectory.h"
#include "helpers/one_touch_detector.h"
#include "plays/play_book.h"

namespace Tyr::Soccer
{
class Ai
{
private:
    Common::WorldState     m_world_state;
    Common::Referee::State m_ref_state;

    std::unique_ptr<Common::NngClient> m_world_client;
    std::unique_ptr<Common::NngClient> m_ref_client;

    std::unique_ptr<Common::UdpClient> m_strategy_client;

    std::unique_ptr<Common::NngServer> m_cmd_server;

    Common::Random m_random;
    Common::Timer  m_timer;

    using Play = void (Ai::*)();
    Play              m_current_play;

    Common::Angle m_chip_head = Common::Angle::fromDeg(200);

    float m_random_param = 0.0f;
    int   m_target_str   = -1;

    Common::Referee::GameState m_last_referee = Common::Referee::GameState::None;

    bool m_is_defending  = false;
    bool m_opp_restarted = false;

    int m_side;

    // Roles
    int m_gk     = 0;
    int m_def    = 1;
    int m_dmf    = 2;
    int m_mid2   = 3;
    int m_mid1   = 4;
    int m_attack = 5;
    int m_rw     = 6;
    int m_lw     = 7;

    void manageAttRoles();

    OneTouchDetector m_one_touch_detector[Common::Config::Common::kMaxRobots];
    enum class OneTouchType
    {
        OneTouch = 0,
        Shirje,
        Gool,
        Allaf,
    };
    OneTouchType m_one_touch_type[Common::Config::Common::kMaxRobots];
    bool         m_one_touch_type_used[Common::Config::Common::kMaxRobots];

    // Helpers
    Common::Vec2 calculatePassPos(const Common::Vec2 &t_target, const Common::Vec2 &t_stat_pos, float t_bar = 89.0f);
    float        calculateRobotReachTime(int t_robot_num, Common::Vec2 t_dest, VelocityProfile t_profile);
    float        calculateBallRobotReachTime(int t_robot_num, VelocityProfile t_profile);

    BallTrajectory m_ball_trajectory;

    // boz ha

    void calcIsDefending();

    void                 markManager();
    std::map<int *, int> m_mark_map;

    Common::Vec2  m_predicted_ball;
    bool          m_circle_reached_behind_ball = false;
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

    // Field helpers
    inline Common::Vec2        ownGoal() const;
    inline Common::Vec2        ownGoalPostTop() const;
    inline Common::Vec2        ownGoalPostBottom() const;
    inline Common::LineSegment ownGoalLine() const;
    inline Common::Vec2        oppGoal() const;
    inline Common::Vec2        oppGoalPostTop() const;
    inline Common::Vec2        oppGoalPostBottom() const;
    inline Common::LineSegment oppGoalLine() const;
    inline bool                isOut(Common::Vec2 t_point, float t_margin = 0.0f) const;

    // Navigation
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

    Planner              m_planner[Common::Config::Common::kMaxRobots];
    std::unique_ptr<Dss> m_dss;

    // Skills
    void mark(int t_robot_num, int t_opp, float t_dist = 220.0f);
    void mark2Goal(int t_robot_num, int t_opp, float t_dist = 220.0f);
    void mark2Ball(int t_robot_num, int t_opp, float t_dist = 220.0f);
    void halt(int t_robot_num);

    void gkShirje(int t_robot_num);
    void gkHi(int t_robot_num);
    void runningDef(int t_robot_num, Common::Vec2 t_target, Common::Vec2 *t_defend_target);
    void defenceWall(int t_robot_num, bool t_kick_off = false);
    void attacker(int t_robot_num, Common::Angle t_angle, int t_kick, int t_chip, bool t_kiss, bool t_dribbler);
    void waitForPass(int t_robot_num, bool t_chip = false, const Common::Vec2 *t_target = nullptr,
                     Common::Vec2 *t_stat_pos = nullptr);
    void waitForOmghi(int t_robot_num);
    void waitForGool(int t_robot_num);
    void receivePass(int t_robot_num, Common::Vec2 t_static_pos, bool t_chip = false);
    void circleBall(int t_robot_num, Common::Angle t_tagret_angle, int t_shoot_pow, int t_chip_pow,
                    float t_near_dis_override = -1.0f);

    bool m_gk_intercepting = false;

    // Tactics
    void defHi(int t_robot_num, int t_right_def_num, int t_left_def_num, Common::Vec2 *t_defend_target);

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

    // Strategy
    int strategyWeight();

    PlayBook     m_playbook;
    int         *m_stm_to_ai_num[Common::Config::Common::kMaxRobots] = {};
    Common::Vec2 m_allaf_pos[Common::Config::Common::kMaxRobots];

    // FSM
    int m_func_state = 0;
    int m_func_count = 0;

    void internalProcessData();

public:
    Robot m_own_robot[Common::Config::Common::kMaxRobots];

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

#include "helpers/field.inl"
