#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::read_playBook(const std::filesystem::path &t_path)
{
    std::ifstream file(t_path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        return false;

    // get length of file:
    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    Common::logInfo("reading strategy file \"{}\" with size {}", t_path, length);
    file.seekg(0, std::ios::beg);

    // allocate memory:
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(length);

    // read data as a block:
    file.read(buffer.get(), length);
    file.close();

    return read_playBook_str({buffer.get(), length});
}

bool Ai::read_playBook_str(std::span<char> buffer)
{
    if (!playBook)
        playBook = new Protos::Immortals::PlayBook();

    if (!playBook->ParseFromArray(buffer.data(), buffer.size()))
    {
        delete playBook;
        playBook = nullptr;
        return false;
    }

    if (playBook->strategy_size() != playBook->weight_size())
    {
        delete playBook;
        playBook = nullptr;
        return false;
    }

    for (int strategy_idx = 0; strategy_idx < playBook->strategy_size(); ++strategy_idx)
    {
        Common::logInfo("STRATEGY: {}, weight: {}", playBook->strategy(strategy_idx).name(),
                        playBook->weight(strategy_idx));
    }

    return true;
}

int        step[Common::Setting::kMaxOnFieldTeamRobots]    = {0, 0, 0, 0, 0, 0, 0, 0};
float      lastAdv[Common::Setting::kMaxOnFieldTeamRobots] = {0, 0, 0, 0, 0, 0, 0, 0};
static int curr_str_id                                     = -1;

bool receivers_reached = false;

void Ai::strategy_maker()
{
    if (timer.time() < 0.5)
    {
        curr_str_id       = target_str;
        receivers_reached = false;
    }

    if (curr_str_id == -1)
    {
        HaltAll();
        return;
    }

    Protos::Immortals::Strategy strategy = playBook->strategy(curr_str_id);
    Common::logInfo("STRATEGY: {}", strategy.name());

    int xSgn = side;
    int ySgn = Common::sign(-m_state.ball.position.y);

    Common::logDebug("timer: {}", timer.time());
    if (timer.time() < 0.5)
    {
        for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
        {
            // FOR NOW: advance to the last step
            step[i]    = std::max(0, strategy.role(i).path_size() - 2);
            lastAdv[i] = timer.time();
            Common::logDebug("zeroed: {}", i);
        }
        Common::Angle passAngle = Common::Angle::fromDeg(90.0f - side * 90.0f);
        circle_ball(attack, passAngle, 0, 0, 1.0f);
        return;
    }
    else
    {
        for (int i = 0; i < strategy.role_size(); i++)
        {
            if (strategy.role(i).path_size() == 0)
                continue;

            if (step[i] >= strategy.role(i).path_size() - 1)
            {
                step[i]    = strategy.role(i).path_size() - 1;
                lastAdv[i] = timer.time();
                Common::logDebug("zeroed: {}", i);
                continue;
            }

            if ((strategy.role(i).path(step[i]).type() == 1) || (*stm2AInum[i] == attack))
            {
                if (timer.time() - lastAdv[i] > strategy.role(i).path(step[i]).time() * 0.1f)
                {
                    step[i]    = std::min(strategy.role(i).path_size() - 1, step[i] + 1);
                    lastAdv[i] = timer.time();
                    Common::logDebug("stepped: {}    {}", i, step[i]);
                }
            }
            else
            {
                if (Common::Vec2(strategy.role(i).path(step[i]).x() * xSgn, strategy.role(i).path(step[i]).y() * ySgn)
                        .distanceTo(OwnRobot[*stm2AInum[i]].state().position) <
                    strategy.role(i).path(step[i]).tolerance())
                {
                    step[i]    = std::min(strategy.role(i).path_size() - 1, step[i] + 1);
                    lastAdv[i] = timer.time();
                    Common::logDebug("stepped: {}    {}", i, step[i]);
                }
            }
        }
    }

    bool new_receivers_reached = true;
    DefMid(def, rw, lw, nullptr, false, false);
    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        // if ((*stm2AInum[i]==gk)||(*stm2AInum[i]==def)) {
        //	continue;
        // }

        if (strategy.role(i).path_size() == 0)
        {
            if (*stm2AInum[i] == gk)
                GKHi(gk, true);
            else if (*stm2AInum[i] == def && *stm2AInum[i] == lw && *stm2AInum[i] == rw) // No need to halt these guys
                continue;
            //            else if (*stm2AInum[i]==def)
            //            {
            //				DefMid(def,rw,lw,nullptr,false,false);
            //            }
            //			else if (*stm2AInum[i]==lw)
            //			{
            //				DefMid(def,rw,lw,nullptr,false,false);
            //			}
            //			else if (*stm2AInum[i]==rw)
            //			{
            //				DefMid(def,rw,lw,nullptr,false,false);
            //			}
            else
                Halt(*stm2AInum[i]);
            continue;
        }

        else if (*stm2AInum[i] == attack)
        {
            int shoot = 0;
            int chip  = 0;

            if (strategy.role(i).path(step[i]).type() == 0)
            {
                shoot = strategy.role(i).path(step[i]).tolerance();
                Common::logDebug("ATTACK: shoot: {}", shoot);
            }
            else
            {
                chip = strategy.role(i).path(step[i]).tolerance();
                Common::logDebug("ATTACK: chip:{}", chip);
            }

            if (step[i] == strategy.role(i).path_size() - 1 && receivers_reached && timer.time() > 3)
            {
                Common::Angle passAngle =
                    Common::Vec2(strategy.role(i).path(step[i]).x() * xSgn, strategy.role(i).path(step[i]).y() * ySgn)
                        .angleWith(m_state.ball.position);
                float tmp_mult = 1; // TODO #11 remove this multiplier and fix that strategy maker
                circle_ball(*stm2AInum[i], passAngle, shoot * tmp_mult, chip, 1.0f);
            }
            else if (step[i] == strategy.role(i).path_size() - 2)
            {
                Common::Angle passAngle =
                    Common::Vec2(strategy.role(i).path(step[i]).x() * xSgn, strategy.role(i).path(step[i]).y() * ySgn)
                        .angleWith(m_state.ball.position);
                circle_ball(*stm2AInum[i], passAngle, 0, 0, 1.0f, 140.0f);
            }
            else
            {
                Common::Angle passAngle =
                    Common::Vec2(strategy.role(i).path(step[i]).x() * xSgn, strategy.role(i).path(step[i]).y() * ySgn)
                        .angleWith(m_state.ball.position);
                circle_ball(*stm2AInum[i], passAngle, 0, 0, 1.0f);
            }
        }

        else
        {
            if (1)
            { // strategy.role(i).path(step[i]).needrrt()==0) {
                ERRTSetObstacles(*stm2AInum[i], 1, 1);
                // ERRTSetObstacles(*stm2AInum[i], 0, 0);
            }
            else
            {
                ERRTSetObstacles(*stm2AInum[i], 1, 1);
            }

            VelocityProfile::Type profile = VelocityProfile::Type::Mamooli;
            switch (strategy.role(i).path(step[i]).velocity_profile())
            {
            case Protos::Immortals::Waypoint_VelocityProfile_Aroom:
                profile = VelocityProfile::Type::Aroom;
                break;
            case Protos::Immortals::Waypoint_VelocityProfile_Mamooli:
                profile = VelocityProfile::Type::Mamooli;
                break;
            case Protos::Immortals::Waypoint_VelocityProfile_Kharaki:
                profile = VelocityProfile::Type::Kharaki;
                break;
            }
            if (step[i] != strategy.role(i).path_size() - 1)
            {
                // float dis_to_reach = Common::Vec2::distance(OwnRobot[*stm2AInum[i]].state().position,
                // Common::Vec2(strategy.role(i).path(step[i]).x(),strategy.role(i).path(step[i]).y())); if
                // ((step[i]>=strategy.role(i).path_size()-2) || (dis_to_reach < 500))
                OwnRobot[*stm2AInum[i]].face(Common::Vec2(-side * Common::field().width, 0));
                // else
                //     OwnRobot[*stm2AInum[i]].face(Common::Vec2(strategy.role(i).path(step[i]).x(),strategy.role(i).path(step[i]).y()));
                ERRTNavigate2Point(
                    *stm2AInum[i],
                    Common::Vec2(strategy.role(i).path(step[i]).x() * xSgn, strategy.role(i).path(step[i]).y() * ySgn),
                    strategy.role(i).path(step[i]).speed(), profile);
            }
            else
            {
                receivePass(*stm2AInum[i], Common::Vec2(strategy.role(i).path(step[i]).x() * xSgn,
                                                        strategy.role(i).path(step[i]).y() * ySgn));
            }
        }

        const float remainingDis =
            Common::Vec2(strategy.role(i).path(step[i]).x() * xSgn, strategy.role(i).path(step[i]).y() * ySgn)
                .distanceTo(OwnRobot[*stm2AInum[i]].state().position);

        switch (strategy.role(i).afterlife())
        {
        case 0:
            oneTouchType[*stm2AInum[i]] = gool;
            break;
        case 1:
            oneTouchType[*stm2AInum[i]] = oneTouch;
            if (strategy.role(i).path_size() == 0)
                allafPos[*stm2AInum[i]] = Common::Vec2(0, 0);
            else
                allafPos[*stm2AInum[i]] =
                    Common::Vec2(strategy.role(i).path(strategy.role(i).path_size() - 1).x() * xSgn,
                                 strategy.role(i).path(strategy.role(i).path_size() - 1).y() * ySgn);

            if (step[i] != strategy.role(i).path_size() - 1)
                // if (i == dmf && remainingDis > 150)
                new_receivers_reached = false;
            break;
        case 2:
            oneTouchType[*stm2AInum[i]] = shirje;
            break;
        case 3:
            oneTouchType[*stm2AInum[i]] = allaf;
            if (*stm2AInum[i] == attack)
            {
                allafPos[*stm2AInum[i]] = m_state.ball.position;
            }
            else
            {
                if (strategy.role(i).path_size() == 0)
                    allafPos[*stm2AInum[i]] = Common::Vec2(0, 0);
                else
                    allafPos[*stm2AInum[i]] =
                        Common::Vec2(strategy.role(i).path(strategy.role(i).path_size() - 1).x() * xSgn,
                                     strategy.role(i).path(strategy.role(i).path_size() - 1).y() * ySgn);
            }
            break;
        default:
            oneTouchType[*stm2AInum[i]] = oneTouch;
            break;
        }
    }

    receivers_reached = new_receivers_reached || timer.time() > 4.5;
}
} // namespace Tyr::Soccer
