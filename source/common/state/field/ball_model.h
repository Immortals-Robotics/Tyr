#pragma once

namespace Immortals::Common
{
// Two-Phase model for straight-kicked balls.
// There are two phases with different accelerations during the ball kicks:
// 1. Sliding
// 2. Rolling
// The full model is described in the TDP of ER-Force from 2016, which can be found here:
// https://ssl.robocup.org/wp-content/uploads/2019/01/2016_ETDP_ER-Force.pdf
struct BallModelStraightTwoPhase
{
    // Ball sliding acceleration [m/s^2] (should be negative)
    double acc_slide = 0.0f;
    // Ball rolling acceleration [m/s^2] (should be negative)
    double acc_roll = 0.0f;
    // Fraction of the initial velocity where the ball starts to roll
    double k_switch = 0.0f;

    BallModelStraightTwoPhase() = default;

    explicit BallModelStraightTwoPhase(const Protos::Ssl::Vision::BallModelStraightTwoPhase &t_model)
    {
        acc_slide = t_model.acc_slide();
        acc_roll  = t_model.acc_roll();
        k_switch  = t_model.k_switch();
    }
};

// Fixed-Loss model for chipped balls.
// Uses fixed damping factors for xy and z direction per hop.
struct BallModelChipFixedLoss
{
    // Chip kick velocity damping factor in XY direction for the first hop
    double damping_xy_first_hop = 0.0f;
    // Chip kick velocity damping factor in XY direction for all following hops
    double damping_xy_other_hops = 0.0f;
    // Chip kick velocity damping factor in Z direction for all hops
    double damping_z = 0.0f;

    BallModelChipFixedLoss() = default;

    explicit BallModelChipFixedLoss(const Protos::Ssl::Vision::BallModelChipFixedLoss &t_model)
    {
        damping_xy_first_hop  = t_model.damping_xy_first_hop();
        damping_xy_other_hops = t_model.damping_xy_other_hops();
        damping_z             = t_model.damping_z();
    }
};
} // namespace Immortals::Common
