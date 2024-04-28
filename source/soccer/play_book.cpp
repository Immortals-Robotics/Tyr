#include "ai.h"

namespace Tyr::Soccer
{
void Ai::InitAIPlayBook(void)
{
    AIPlayBook.clear();

    AIPlayBook["my_test"]                = &Ai::my_test;
    AIPlayBook["NewNormalPlay"]          = &Ai::NewNormalPlay;
    AIPlayBook["NormalPlayDef"]          = &Ai::NormalPlayDef;
    AIPlayBook["NormalPlayAtt"]          = &Ai::NormalPlayAtt;
    AIPlayBook["Stop"]                   = &Ai::Stop;
    AIPlayBook["kickoff_us_chip"]        = &Ai::kickoff_us_chip;
    AIPlayBook["throwin_chip_shoot"]     = &Ai::throwin_chip_shoot;
    AIPlayBook["kickoff_their_one_wall"] = &Ai::kickoff_their_one_wall;
    AIPlayBook["Stop_def"]               = &Ai::Stop_def;
    AIPlayBook["strategy_maker"]         = &Ai::strategy_maker;
    AIPlayBook["penalty_us_shootout"]    = &Ai::penalty_us_shootout;
    AIPlayBook["penalty_their_simple"]   = &Ai::penalty_their_simple;

    AIPlayBook["corner_their_global"]       = &Ai::corner_their_global;
    AIPlayBook["tech_khers_pass"]           = &Ai::tech_khers_pass;
    AIPlayBook["our_place_ball_shoot"]      = &Ai::our_place_ball_shoot;
    AIPlayBook["our_place_ball_shoot_V2"]   = &Ai::our_place_ball_shoot_V2;
    AIPlayBook["our_place_ball_shoot_taki"] = &Ai::our_place_ball_shoot_taki;
    AIPlayBook["their_place_ball"]          = &Ai::their_place_ball;
}
} // namespace Tyr::Soccer
