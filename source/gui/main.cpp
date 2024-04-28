#include "app.h"

int main(int argc, char *argv[])
{
    Tyr::Gui::Application app{};
    app.init();

    while (!app.shouldClose())
        app.update();

    return app.shutdown();
}