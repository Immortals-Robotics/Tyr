#include "app.h"

Tyr::Cli::Application app{};

void onExit()
{
    app.shutdown();
    std::exit(EXIT_SUCCESS);
}

int main()
{
    if (!app.initialize())
    {
        return EXIT_FAILURE;
    }

    app.start();

    std::set_terminate(onExit);
    std::atexit(onExit);

    std::this_thread::sleep_for(std::chrono::seconds::max());

    return EXIT_SUCCESS;
}
