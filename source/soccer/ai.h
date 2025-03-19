#pragma once

#include "robot/robot.h"
#include "plays/play_book.h"

#include "skills/skill.h"

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

    // Helpers
    Common::Vec2 calculatePassPos(const Common::Vec2 &t_target, const Common::Vec2 &t_stat_pos, float t_bar = 89.0f);
    float        calculateRobotReachTime(const Robot& t_robot, Common::Vec2 t_dest, VelocityProfile t_profile) const;
    float        calculateBallRobotReachTime(const Robot& t_robot, Common::Angle angle,  VelocityProfile t_profile, float t_wait) const;

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

    Common::Angle calculateOneTouchAngle(const Robot& t_robot, Common::Vec2 t_one_touch_position);

    struct OpenAngle
    {
        // Center of the angle
        Common::Angle center;
        // how big it is
        Common::Angle magnitude;
    };

    OpenAngle calculateOpenAngleToGoal(Common::Vec2 t_pos, const Robot& t_robot);

    bool         ballIsGoaling();
    int          findNearestOpp(Common::Vec2 t_pos, int t_mask, bool t_accept_near_ball = true);
    int          findKickerOpp(int t_mask, float t_max_dis = 500.0f);
    bool         goalBlocked(Common::Vec2 t_init_pos, float t_max_shoot_blocker_dis, float t_shoot_blocker_r);
    bool         attackFuckingAngle();
    float        calculateOppThreat(int t_opp, bool t_restart = false);
    float        calculateMarkCost(int t_robot_num, int t_opp);

    Common::BallState predictBall(float t_time_ahead) const;

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

    // ball placement states
    enum class OurBallPlacementState
    {
        Idle,
        KissInit,
        KissTouch,
        Kissing,
        Stuck,
        LongDistance,
        KissingDone,
        Done,
    };

    // Skills
    void halt(Robot& t_robot);

    void mark(Robot& t_robot, int t_opp, float t_dist = 220.0f);
    void mark2Goal(Robot& t_robot, int t_opp, float t_dist = 220.0f, bool t_def_area_mark = false);
    void mark2Ball(Robot& t_robot, int t_opp, float t_dist = 220.0f);

    void gkShirje(Robot& t_robot);
    void gkHi(Robot& t_robot);
    void runningDef(Robot& t_robot, Common::Vec2 t_target, Common::Vec2 *t_defend_target);
    void defenceWall(Robot& t_robot, bool t_kick_off = false);
    void attacker(Robot& t_robot, Common::Angle t_angle, float t_kick, int t_chip, bool t_kiss, bool t_dribbler, bool precise = false);
    void waitForPass(Robot& t_robot, bool t_chip = false, const Common::Vec2 *t_target = nullptr,
                     Common::Vec2 *t_stat_pos = nullptr);
    void waitForOmghi(Robot& t_robot);
    void waitForGool(Robot& t_robot);
    void receivePass(Robot& t_robot, Common::Vec2 t_static_pos, bool t_chip = false);
    void circleBall(Robot& t_robot, Common::Angle t_tagret_angle, float t_shoot_pow, int t_chip_pow,
                    float t_near_dis_override = -1.0f);

    bool m_gk_intercepting = false;

    // Tactics
    void defHi(Robot& t_robot_1, Robot& t_robot_2, Common::Vec2 *t_defend_target);
    void defShirje(Robot& t_robot_1, Robot& t_robot_2);

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
    void placeBall();
    void theirPlaceBall();

    void calcMinBallWallDist(double &t_min_dist, Common::Vec2 &t_closest_point);
    void placeBallLongDistance();
    void switchBallPlacementStateDelayed(int t_wait_frames, OurBallPlacementState t_new_state);
    void resetBallPlacementStateFrameCounter();
    bool placeBallLost();
    void generateKissPoints(double t_distance, Common::Vec2 &t_pos1, Common::Vec2 &t_pos2);
    // Strategy
    int strategyWeight();

    PlayBook                                m_playbook;
    std::unordered_map<int *, Common::Vec2> m_allaf_pos;

    // ball placement
    OurBallPlacementState m_our_ball_placement_state = OurBallPlacementState::Idle;
    OurBallPlacementState m_our_last_ball_placement_state = OurBallPlacementState::Idle;
    int m_our_ball_placement_state_wait_frames = 0;
    bool m_our_ball_placement_force_stuck = false;
    int m_our_ball_placement_stuck_count = 0;
    Common::Vec2 m_our_ball_placement_attack_final_pos;
    Common::Vec2 m_our_ball_placement_mid5_final_pos;


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
