#pragma once

#include "strategy.h"
#include "sync_data.h"

namespace Tyr::Strategy
{
class Gui : public QWidget
{
    Q_OBJECT

public:
    Gui(QWidget *parent = 0);
    ~Gui() = default;

    Strategy  *strategy[100];
    int        strategy_counter = 0;
    int        current_strategy = 0;

    Robot::Type current_robot    = Robot::GK;
    int         current_waypoint = 0;
    int         clicked_robot    = 0;
    int         clicked_waypoint = 0;
    bool        clicked          = false;
    int         key_type         = 0;
    int         key_sync         = 0;

    SyncData sync_data[10];
    int      current_sync = 0;
    int      sync_counter = 0;

protected:
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    QPainter painter;

public slots:
};
} // namespace Tyr::Strategy
