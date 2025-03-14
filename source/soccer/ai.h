#pragma once

#include "navigation/dss/dss.h"
#include "navigation/errt/errt.h"
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
    std::unique_ptr<Common::NngServer> m_state_server;

    std::unique_ptr<Vision::Ekf3D> m_ball_predictor;

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
    int m_def1   = 1;
    int m_def2   = 2;
    int m_mid1   = 3;
    int m_mid2   = 4;
    int m_mid3   = 5;
    int m_mid4   = 6;
    int m_mid5   = 7;
    int m_mid6   = 8;
    int m_mid7   = 9;
    int m_attack = 10;

    std::vector<int *> m_ids;
    std::vector<int *> m_strategy_ids;

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

    // static pos for mids
    struct Zone
    {
        static constexpr float kZoneWidth  = 2000.0f;
        static constexpr float kZoneHeight = 3000.0f;

        Common::Rect rect;

        Common::Vec2 best_pos;
        float        score;
    };

    std::vector<Zone>         m_zones;
    std::vector<const Zone *> m_sorted_zones;

    std::unordered_map<int *, Common::Vec2> m_mids_static_pos;

    void staticZoneScore(Zone &t_zone) const;

    float staticPosScoreDefence(Common::Vec2 t_pos) const;
    float staticPosScoreAttack(Common::Vec2 t_pos) const;

    // boz ha

    void calcIsDefending();

    void markManager();

    std::map<int *, int> m_mark_map;
    std::vector<int *>   m_prioritized_mids;

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
    Common::Vec2 predictBallForwardAINew(const float t_time_ahead);
    float        calculateOppThreat(int t_opp, bool t_restart = false);
    float        calculateMarkCost(int t_robot_num, int t_opp);

    /**
     * Calculates a score indicating the suitability of a robot for switching to the attacker role.
     *
     * This function evaluates the potential of a robot to become the attacker based on its current state,
     * position relative to the ball, and specific capabilities. The score is a combination of distance to the
     * ball and the robot's capabilities, adjusted by various conditions such as whether the robot is waiting
     * for a pass or marking an opponent.
     *
     * @param t_robot_num The ID of the robot being evaluated.
     * @return A float score representing the suitability of the robot for the attacker role.
     * - -1: if the switch results in fatal error, ie. robot is the goalkeeper or is out of the game
     * - [0:1]: switch is possible, a higher score indicates a more suitable candidate.
     */
    float calculateSwitchToAttackerScore(int t_robot_num);

    // Field helpers
    inline Common::Vec2        ownGoal() const;
    inline Common::Vec2        ownGoalPostTop() const;
    inline Common::Vec2        ownGoalPostBottom() const;
    inline Common::LineSegment ownGoalLine() const;
    inline Common::Vec2        oppGoal() const;
    inline Common::Vec2        oppGoalPostTop() const;
    inline Common::Vec2        oppGoalPostBottom() const;
    inline Common::LineSegment oppGoalLine() const;
    inline Common::Rect        ownPenaltyArea() const;
    inline Common::Rect        oppPenaltyArea() const;
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

    ObstacleMap          m_obsMap[Common::Config::Common::kMaxRobots];
    Planner              m_planner[Common::Config::Common::kMaxRobots];
    std::unique_ptr<Dss> m_dss;

    // Skills
    void mark(int t_robot_num, int t_opp, float t_dist = 220.0f);
    void mark2Goal(int t_robot_num, int t_opp, float t_dist = 220.0f, bool t_def_area_mark = false);
    void mark2Ball(int t_robot_num, int t_opp, float t_dist = 220.0f);
    void halt(int t_robot_num);

    void gkShirje(int t_robot_num);
    void gkHi(int t_robot_num);
    void runningDef(int t_robot_num, Common::Vec2 t_target, Common::Vec2 *t_defend_target);
    void defenceWall(int t_robot_num, bool t_kick_off = false);
    void attacker(int t_robot_num, Common::Angle t_angle, float t_kick, int t_chip, bool t_kiss, bool t_dribbler, bool precise = false);
    void waitForPass(int t_robot_num, bool t_chip = false, const Common::Vec2 *t_target = nullptr,
                     Common::Vec2 *t_stat_pos = nullptr);
    void waitForOmghi(int t_robot_num);
    void waitForGool(int t_robot_num);
    void receivePass(int t_robot_num, Common::Vec2 t_static_pos, bool t_chip = false);
    void circleBall(int t_robot_num, Common::Angle t_tagret_angle, float t_shoot_pow, int t_chip_pow,
                    float t_near_dis_override = -1.0f);

    bool m_gk_intercepting = false;

    // Tactics
    void defHi(int t_def1_num, int t_def2_num, Common::Vec2 *t_defend_target);
    void defShirje(const int t_def_1, const int t_def_2);

    // Plays
    void stop();
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

    PlayBook                                m_playbook;
    std::unordered_map<int *, Common::Vec2> m_allaf_pos;

    // FSM
    int m_func_state = 0;
    int m_func_count = 0;

    void internalProcessData();

    struct Assignment
    {
        enum class Priority
        {
            None = 0,
            Low,
            Medium,
            High,
            Max,
            Rules,
        };

        Priority priority = Priority::None;

        // pointer to m_gk, m_defn, m_midn, m_attack
        int *role = nullptr;

        int currentAssignee() const
        {
            return role == nullptr ? -1 : *role;
        }

        // The following are used to compute the cost of the assignment

        // either the target that robot should go to
        // or a point that the target will be closest to
        // ie. the position of the opp that robot should mark
        // or ball position for the attacker
        Common::Vec2 target_point;

        // the index of the target robot that the robot should mark
        int target_idx;

        bool needs_shoot = false;
        bool needs_chip  = false;

        // cost function
        using CostFunction         = std::function<int(int, const Assignment &)>;
        CostFunction cost_function = nullptr;

        // this is the result of the assignment
        int new_assignee    = -1;
        int assignment_cost = 0;
    };

    void assignRoles();
    void assignRolesInternal(Assignment::Priority t_priority);

    // cost functions
    int gkRoleCost(int t_robot_idx, const Assignment &t_assignment) const;
    int staticRoleCost(int t_robot_idx, const Assignment &t_assignment) const;
    int markRoleCost(int t_robot_idx, const Assignment &t_assignment) const;
    int attackRoleCost(int t_robot_idx, const Assignment &t_assignment);

    // assignment creators
    void createGkAssignment();
    void createDefAssignments();
    void createAttackAssignment();
    void createMidAssignments();

    // this uses the last target position as the target point
    void createStaticAssignment(int *t_role, Assignment::Priority t_priority, bool t_shoot = false,
                                bool t_chip = false);

    std::vector<Assignment> m_assignments;

public:
    Robot m_own_robot[Common::Config::Common::kMaxRobots];

    Ai();

    bool receiveWorld();
    bool receiveReferee();

    bool publishCommands() const;
    bool publishState() const;

    void process();

    bool receivePlayBook();
    bool loadPlayBook(const std::filesystem::path &t_path);
    bool setPlayBook(const Protos::Immortals::PlayBook &t_playbook);
};
} // namespace Tyr::Soccer

#include "helpers/field.inl"
