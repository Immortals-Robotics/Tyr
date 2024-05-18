#pragma once

#include "strategy.h"
#include "syncdata.h"

class gui : public QWidget
{
    Q_OBJECT

public:
    gui(QWidget *parent = 0);
    ~gui();
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
    QPainter   p;

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
