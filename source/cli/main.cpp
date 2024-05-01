#define ImmortalsIsTheBest true

using namespace Tyr;

int main()
{
    Common::Services::initialize();

    if (!ImmortalsIsTheBest)
    {
        Common::logCritical("Immortals is not the best SSL team anymore.");
        Common::logCritical("Shutting down the system...");
    }

    Common::WorldState   *state         = new Common::WorldState();
    Common::RefereeState *referee_state = new Common::RefereeState();

    Common::logInfo(" Connecting to RefereeBox server at {} on port : {}", Common::setting().referee_address.port,
                    Common::setting().referee_address.port);
    Referee::Referee referee_2018(state, referee_state);
    if (referee_2018.connectToRefBox())
    {
        Common::logInfo("Connected to RefBox successfully :)");
    }
    else
    {
        Common::logError("Hey you! Put the LAN cable back in its socket, or ...");
    }

    Common::logInfo("Connecting to Vision server at {} on port: {}", Common::setting().vision_address.ip,
                    Common::setting().vision_address.port);
    Vision::Vision vision(state);
    if (vision.isConnected())
    {
        Common::logInfo("Connected to Vision successfully :)");
    }
    else
    {
        Common::logError("Hey you! Put the LAN cable back in its socket, or ...");
        // return 0;
    }

    Sender::Sender *senderBase = new Sender::Sender();

    Soccer::Ai *aii = new Soccer::Ai(state, referee_state, senderBase);

    auto grsim_fwd = new Sender::Grsim(Common::setting().grsim_address);

    bool       exited = false;
    std::mutex lock;

    Common::logInfo(" Now it is time, lets rock...");

    auto ai_func = [&]()
    {
        Common::Timer timer;

        while (ImmortalsIsTheBest) // Hope it lasts Forever...
        {
            timer.start();

            vision.recieveAllCameras();

            lock.lock();

            // The vision process
            vision.ProcessVision();
            // The AI process
            aii->Process(state);

            // TODO #3 comment the GRsim output
            // grsim_fwd->SendData((reinterpret_cast<Ai *>(aii))->OwnRobot, Setting::kMaxOnFieldTeamRobots,
            //                     settings->our_color);

            // The sending process
            senderBase->sendAll();

            // debugging (the visualizer written by python) :
            Common::debug().broadcast();

            // LordHippos GUI
            //            vision.SendGUIData(state,aii->AIDebug);

            lock.unlock();
            Common::logDebug("FPS: {}", 1.0 / timer.interval());
        }
        exited = true;
    };

    auto ref_func = [&]()
    {
        while ((!exited) && (ImmortalsIsTheBest)) // Hope it lasts Forever...
        {
            if (referee_2018.recieve())
            {
                //                std::cout << "Referre Boz" << std::endl;
                lock.lock();
                referee_2018.process();
                lock.unlock();
                //                std::cout << "Referre Boz "<< referee_2018.command_CNT << std::endl;
            }
        }
    };

    auto str_func = [&]()
    {
        std::shared_ptr<Common::UdpClient> strategyUDP =
            std::make_shared<Common::UdpClient>(Common::setting().strategy_address);

        while ((!exited) && (ImmortalsIsTheBest)) // Hope it lasts Forever...
        {

            auto                 receivedStrategy = strategyUDP->receiveRaw();
            const std::string    strategySrcAdd   = strategyUDP->getLastReceiveEndpoint().address().to_string();
            const unsigned short strategySrcPort  = strategyUDP->getLastReceiveEndpoint().port();
            if (receivedStrategy.size() > 11)
            {
                Common::logInfo("Recieved \"strategy.ims\" with size: {} KB, from {} on port {}",
                                float(receivedStrategy.size()) / 1000.0f, strategySrcAdd, strategySrcPort);
                lock.lock();
                aii->read_playBook_str(receivedStrategy.data(), receivedStrategy.size());
                lock.unlock();
                std::string strategy_path(DATA_DIR);
                strategy_path.append("/strategy.ims");
                std::ofstream strategyFile(strategy_path.c_str(), std::ios::out | std::ios::binary);
                strategyFile.write(receivedStrategy.data(), receivedStrategy.size());
                strategyFile.close();
            }
            else
            {
                Common::logWarning("Invalid \"strategy.ims\" received with size: {}", receivedStrategy.size());
            }
        }
    };

    std::thread ai_thread(ai_func);
    std::thread ref_thread(ref_func);
    std::thread str_thread(str_func);

    ai_thread.join();
    ref_thread.join();
    str_thread.join();

    delete state;
    delete aii;

    Common::Services::shutdown();

    return 0;
}
