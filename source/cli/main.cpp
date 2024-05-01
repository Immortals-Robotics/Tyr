#include "app.h"

int main()
{
    Tyr::Cli::Application app{};

    if (!app.initialize())
    {
        return 1;
    }

    app.start();

    app.shutdown();

    return 0;
}
