#pragma once

#include "strategy.h"
#include "sync_data.h"

namespace Tyr::Strategy
{
class Gui final : public QWidget
{
    Q_OBJECT

public:
    explicit Gui(QWidget *parent = nullptr);

    ~Gui() override = default;

    Strategy *strategy[100];
    int       strategy_counter = 0;
    int       current_strategy = 0;

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
    void paintEvent(QPaintEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    QPainter m_painter;

public slots:
};
} // namespace Tyr::Strategy
