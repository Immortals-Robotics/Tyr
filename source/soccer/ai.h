#pragma once

#define NEW_FIELD_2018

#define mark_in_stop 0

namespace Tyr::Soccer
{
class Ai
{
private:
    Common::WorldState *worldState;
	Common::RefereeState *refereeState;
    Sender::Sender     *senderBase;
    Common::GameState  *REF_playState;

    Common::Random random;

    float field_width;
    float field_height;
    float goal_width;

    float penalty_area_r;
    float penalty_area_width;

    std::map<std::string, void (Ai::*)()> AIPlayBook;
    std::string                           currentPlay;
    uint32_t                              currentPlayParam;

    void InitAIPlayBook(void);

    int FUNC_state = 0;
    int FUNC_CNT   = 0;

    bool gkIntercepting;

    int chip_head;

    float penaltyAngle;

    int oppGK;

    float randomParam;
    int   target_str;

    int rmf;
    int gk;
    int def;
    int dmf;
    int lmf;
    int cmf;
    int rw;
    int lw;

    int attack;
    int mid1;
    int mid2;

    int *stm2AInum[Common::Setting::kMaxOnFieldTeamRobots];

    Common::vec2 allafPos[Common::Setting::kMaxOnFieldTeamRobots];

    std::map<int *, int> markMap;

    int lastReferee;

    Common::Timer timer;
    Common::vec2 *targetBallPlacement;

    bool isDefending;
    bool oppRestarted;

    float beta;
    float gamma;
    float shootK;

    bool reached;

    const int                     maxBallHist;
    std::deque<Common::BallState> ballHist;
    Common::Linear                ballLine;
    Common::BallState             ball;
    Common::RobotState            OppRobot[Common::Setting::kMaxRobots];
    int                           OwnRobotNum, OppRobotNum;
    Planner                       planner[Common::Setting::kMaxOnFieldTeamRobots];
    Dss                          *dss;

    OneTouchDetector oneTouchDetector[Common::Setting::kMaxOnFieldTeamRobots];
    enum OneTouchType
    {
        oneTouch = 0,
        shirje,
        gool,
        allaf
    };
    OneTouchType oneTouchType[Common::Setting::kMaxOnFieldTeamRobots];
    bool         oneTouchTypeUsed[Common::Setting::kMaxOnFieldTeamRobots];

    bool navigated[Common::Setting::kMaxOnFieldTeamRobots];
    int  side;

    VelocityProfile BALL_PLACE_KHEYLI_SOOSKI;
    VelocityProfile VELOCITY_PROFILE_AROOM;
    VelocityProfile VELOCITY_PROFILE_KHARAKI;
    VelocityProfile VELOCITY_PROFILE_MAMOOLI;

    // Helpers
    Common::vec2 PointOnConnectingLine(Common::vec2 FirstPoint, Common::vec2 SecondPoint, float distance);
    Common::vec2 GK_Ghuz(float predictBallT, float rMul, int def_count = 2);
    Common::vec2 GK_Ghuz_2018(float predictBallT, float rMul, int def_count = 2);
    Common::vec2 DefGhuz(Common::vec2 *defendTarget = NULL);
    Common::vec2 CalculatePassPos(int robot_num, const Common::vec2 &target, const Common::vec2 &statPos,
                                  float bar = 89.0f);
    void         CalculateBallTrajectory(void);
    float        calculateRobotReachTime(int robot_num, Common::vec2 dest, VelocityProfile *vel_profile);
    float        calculateBallRobotReachTime(int robot_num, VelocityProfile *vel_profile);

    // boz ha
    void         ManageAttRoles(void);
    void         CalcIsDefending(void);
    void         MarkManager(bool restart = true);
    PlayBook    *playBook;
    int          strategy_weight(void);
    float        lastBallDirection, lastBallMagnitude;
    Common::vec2 PredictedBall;
    bool         circleReachedBehindBall;
    float        calculateOneTouchAngle(int robot_num, Common::vec2 oneTouchPosition);

    void SetEvaulateOBJs(int robot_num1, int robot_num2);
    void PassEvaluation(void);

    Common::vec2 calculateOpenAngleToGoal(Common::vec2 init, int robot_num);

    bool         ballIsGoaling(void);
    int          findNearestAsshole(Common::vec2 pos, int mask, bool acceptNearBall = true);
    int          findCruncherOpp(int mask1, int mask2 = -1, bool acceptNearBall = false);
    int          findKickerOpp(int mask, float max_dis = 500.0f);
    int          findGusheRobot(int mask);
    int          findJeloOpps(float minX, int *ans, int mask1, int mask2, bool acceptNearBall, bool acceptGooshe);
    bool         isGooshe(int id, bool sameSideAsBall);
    bool         goal_blocked(Common::vec2 init_pos, float max_shoot_blocker_dis, float shoot_blocker_r);
    bool         attackFuckingAngle(void);
    Common::vec2 predictBallForwardAI(float timeAhead);
    float        oneTouchScore(int robot_num);
    float        calculateOppThreat(int opp, bool restart = false);
    float        calculateMarkCost(int robot_num, int opp);
    float        calculateSwicthToAttackerScore(int robot_num);
    float        outOfField(Common::vec2 point);

    // These functions make sure the required robots are present (in case if any of the robots got out):
    void want_this_robot(int robot_num); // First we tell which robot we need
    bool position_robots(bool avoid_GK  = true,
                         bool avoid_DEF = true); // now we swap the apsent robots (TRUE if it succeeds)
    bool requiredRobots[Common::Setting::kMaxOnFieldTeamRobots];

    // Skills
    void Navigate2Point(int robot_num, Common::vec2 dest, bool accurate = false, int speed = 80,
                        VelocityProfile *velocityProfile = NULL, bool use_dss = false);
    void Navigate2Point_2018(int robot_num, Common::vec2 dest, int speed = 80, VelocityProfile *velocityProfile = NULL);
    void ERRTNavigate2Point(int robot_num, Common::vec2 dest, bool accurate = false, int speed = 80,
                            VelocityProfile *velocityProfile = NULL);
    void ERRTSetObstacles(int robot_num, bool bll = false, bool field = true);
    void ERRTSetGkClearObstacles(int robot_num);
    void Mark(int robot_num, int opp, float dist = 220.0f);
    void Mark2Goal(int robot_num, int opp, float dist = 220.0f);
    void Mark2Ball(int robot_num, int opp, float dist = 220.0f);
    void Halt(int robot_num);
    void GK(int robot_num = 0, int defence_num = 2, bool stop = false);
    void GK_shirje(int robot_num = 0);
    void GK_shirje_2018(int robot_num, VelocityProfile *VELOCITY_PROFILE);
    void GKHi(int robot_num = 0, bool stop = false);
    void GKHi_Simple(int robot_num = 0, bool stop = false);
    void OneDef(int robot_num = 1, Common::vec2 *defendTarget = NULL, bool stop = false);
    void TwoDef(int robot_num1 = 1, int robot_num2 = 2, Common::vec2 *defendTarget = NULL);
    void DefHi(int robot_num, Common::vec2 *defendTarget = NULL, bool stop = false);
    void PenaltyUs(int robot_num, float angle, int kick = 0, int chip = 0);
    void DefenceWall(int robot_num, bool kickOff = false);
    void tech_circle(int robot_num, float angle, int kick = 0, int chip = 0, bool needRRT = true,
                     bool gameRestart = false, bool kiss = false, bool dribbler = false, bool needOppRRT = false);
    void intercept_ball(int robot_num, float angle, int shoot_pow, int chip_pow);
    void WaitForPass(int robot_num, bool chip = false, Common::vec2 *target = NULL, Common::vec2 *statPos = NULL);
    void WaitForOmghi(int robot_num, bool chip = false);
    void WaitForGool(int robot_num, bool chip = false);
    void recievePass(int robot_num, Common::vec2 staticPos, bool chip = false);
    void backPass(int robot_num, float target, float t);
    void dribble(int robot_num, Common::vec2 target);
    void circle_ball(int robot_num, float tagret_angle, int shoot_pow, int chip_pow, float precision,
                     float near_dis_override = -1.0f);
    void circle_ball_free(int robot_num, float tagret_angle, int shoot_pow, int chip_pow, float precision,
                          float near_dis_override = -1.0f);
    void circle_ball_free_V2(int robot_num, float tagret_angle, int shoot_pow, int chip_pow, float precision,
                             VelocityProfile temp_vel, float near_dis_override = -1.0f);

    void DefMid(int &robot_num, int &rightdef_num, int &leftdef_num, Common::vec2 *defendTarget = NULL,
                bool stop = false, bool replace = true);
    void DefBy3(int robot_num, int rightdef_num, int leftdef_num, Common::vec2 *defendTarget = NULL, bool stop = false);
    void DefBy2(int rightdef_num, int leftdef_num, Common::vec2 *defendTarget = NULL, bool stop = false);
    void DefBy1(int thelastdef_num, Common::vec2 *defendTarget = NULL, bool stop = false);
    void runningDef(int robot_num, Common::vec2 target, Common::vec2 *defendTarget, bool stop);

    // Plays
    void Stop();
    void Stop_def();
    void NewNormalPlay(void);
    void NormalPlayDef(void);
    void NormalPlayAtt(void);
    void HaltAll(void);
    void penalty_us_shootout(void);
    void kickoff_us_chip(void);
    void throwin_chip_shoot(void);
    void kickoff_their_one_wall(void);
    void penalty_their_simple(void);
    void corner_their_global(void);
    void strategy_maker(void);
    void tech_challenge(void);
    void tech_mexico(void);
    void tech_cmu(void);
    void tech_khers_pass(void);
    void tech_khers_def(void);
    void tech_motion_ann(void);
    void our_place_ball_shoot(void);
    void our_place_ball_shoot_V2(void);
    void our_place_ball_shoot_taki(void);
    void their_place_ball(void);
    void far_penalty_shoot(void);

    void my_test();

    void internalProcessData(Common::WorldState *worldState);
    void internalFinalize(Common::WorldState *worldState);

public:
    Robot OwnRobot[Common::Setting::kMaxOnFieldTeamRobots];
    Ai(Common::WorldState *worldState, Common::RefereeState *refereeState, Sender::Sender *sender);
    void Process(Common::WorldState *worldState);
    bool read_playBook(const char *fileName);
    bool read_playBook_str(char *buffer, int length);

protected:
    bool debugDraw;
    void AddDebugPoint(const Common::vec2 &p, const Common::CommonColor _color = Common::White);
    void AddDebugLine(const Common::vec2 &p1, const Common::vec2 &p2, const Common::CommonColor _color = Common::White);
    void AddDebugRect(const Common::vec2 &p, const float w, const float h,
                      const Common::CommonColor _color = Common::White);
    void AddDebugCircle(const Common::vec2 &p, const float r, const Common::CommonColor _color = Common::White);
};
} // namespace Tyr::Soccer
