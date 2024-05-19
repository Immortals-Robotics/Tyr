#include "application.h"
#include "frame.h"

int main(int argc, char *argv[])
{
    Tyr::Strategy::Application app(argc, argv);

    Tyr::Strategy::Frame frame;
    frame.show();

    return app.exec();
}
