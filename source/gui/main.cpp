#include "app.h"

int main()
{
    Tyr::Gui::Application app{};
    app.initialize();

    app.start();

    while (!app.shouldClose())
        app.update();

    return app.shutdown();
}
