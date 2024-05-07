#include "app.h"

int main(int argc, char *argv[])
{
    Tyr::Gui::Application app{};
    app.initialize();

    app.start();

    while (!app.shouldClose())
        app.update();

    return app.shutdown();
}