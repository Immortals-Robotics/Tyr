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
    ~Gui();
    Strategys *strategy[100];
    int        strategCounter;
    int        currentStrategy;
    int        currentRobot;
    int        currentWaypoint;
    int        clickedRobot;
    int        clickedWayPoint;
    bool       clicked;
    int        keyType;
    int        keySync;
    SyncDatas  syncDatas[10];
    int        currentSync;
    int        syncCounter;

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
    QImage   fieldGeometry;
    QTimer  *timer;
    bool     mouseMoveAccepted;

public slots:
};
} // namespace Tyr::Strategy
