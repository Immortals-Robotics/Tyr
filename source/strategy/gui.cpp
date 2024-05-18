#include "gui.h"
#include "waypoint.h"

namespace Tyr::Strategy
{
gui::gui(QWidget *parent)
{
    setWindowTitle("Strategy Center");
    setGeometry(0, 0, 1600, 1000);
    setFixedSize(1600, 1000);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true);

    strategCounter  = 0;
    currentStrategy = 0;
    currentRobot    = 0;
    clickedRobot    = 0;
    clickedWayPoint = 0;
    clicked         = 0;
    keyType         = 0;
    syncCounter     = 0;
    currentSync     = 0;
}

gui::~gui()
{
    // p.end();
}

QColor col[7] = {QColor(15, 127, 127), Qt::black, Qt::yellow, Qt::red, Qt::blue, QColor(150, 0, 150)};

void gui::paintEvent(QPaintEvent *event)
{
    painter.begin(this);
    painter.drawImage(0, 0, QImage(QString(DATA_DIR) + "/strategy-images/field.png"));
    // painter.drawImage(0,0,fieldGeometry);
    if (strategCounter > 0)
    {
        for (int j = 0; j < 8; j++)
        {
            for (int i = 0; i < strategy[currentStrategy]->robot[j].waypointCounter - 1; i++)
            {
                painter.setPen(col[j]);
                painter.drawLine(strategy[currentStrategy]->robot[j].waypoint[i]->x() + 12,
                                 strategy[currentStrategy]->robot[j].waypoint[i]->y() + 12,
                                 strategy[currentStrategy]->robot[j].waypoint[i + 1]->x() + 12,
                                 strategy[currentStrategy]->robot[j].waypoint[i + 1]->y() + 12);
            }
        }
        int x1 = (((strategy[currentStrategy]->minX / 10.0) + 450) * 3.0 / 2);
        int x2 = (((strategy[currentStrategy]->maxX / 10.0) + 450) * 3.0 / 2);
        int y1 = ((-(strategy[currentStrategy]->minY / 10.0) + 300) * 3.0 / 2);
        int y2 = ((-(strategy[currentStrategy]->maxY / 10.0) + 300) * 3.0 / 2);

        x1 += 106;
        x2 += 106;
        y1 += 45;
        y2 += 45;
        painter.fillRect(x1, y1 - 10, (x2 - x1), 20, QColor(255, 0, 0, strategy[currentStrategy]->weight * 20));
    }

    painter.end();
}

void gui::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
            this->update();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void gui::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
            clicked = 1;
            this->update();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void gui::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        QByteArray  itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint  offset;
        dataStream >> pixmap >> offset;
        strategy[currentStrategy]->robot[clickedRobot].waypoint[clickedWayPoint]->setPixmap(pixmap);
        strategy[currentStrategy]->robot[clickedRobot].waypoint[clickedWayPoint]->move(event->pos() - offset);
        strategy[currentStrategy]->robot[clickedRobot].waypoint[clickedWayPoint]->show();
        clicked = 1;
        this->update();
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void gui::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift)
    {
        keyType = 1;
    }
    else if (event->key() == Qt::Key_Alt)
    {
        keySync = 1;
    }
    else if (event->key() == Qt::Key_Delete)
    {
        for (int i = currentWaypoint; i < strategy[currentStrategy]->robot[currentRobot].waypointCounter - 1; i++)
        {
            int x, y;
            x = strategy[currentStrategy]->robot[currentRobot].waypoint[i + 1]->x();
            y = strategy[currentStrategy]->robot[currentRobot].waypoint[i + 1]->y();
            strategy[currentStrategy]->robot[currentRobot].waypoint[i]->move(x, y);
            strategy[currentStrategy]->robot[currentRobot].waypoint[i]->show();
        }
        strategy[currentStrategy]->robot[currentRobot].waypointCounter--;
        strategy[currentStrategy]
            ->robot[currentRobot]
            .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
            ->close();
    }
}

void gui::keyReleaseEvent(QKeyEvent *event)
{
    keyType = 0;
    keySync = 0;
}

void gui::mousePressEvent(QMouseEvent *event)
{
    int     x, y;
    QLabel *child = static_cast<QLabel *>(childAt(event->pos()));
    if (!child)
    {
        QString  str = QString(DATA_DIR) + "/strategy-images/";
        QVariant b;
        if (strategCounter == 0)
        {
            QMessageBox msg;
            msg.setText("Olagh esm bede be Strategy");
            msg.setWindowTitle("Khar");
            msg.exec();
        }
        else
        {
            strategy[currentStrategy]
                ->robot[currentRobot]
                .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                ->move(event->x() - 13, event->y() - 13);
            b = currentRobot + 1;
            str.append(b.toString());
            str.append("/");
            b = strategy[currentStrategy]->robot[currentRobot].waypointCounter + 1;
            if (b.toInt() > 8)
                b = 8;
            str.append(b.toString());
            str.append(".png");
            std::cout << "THE STR IS: " << str.toStdString() << std::endl;
            strategy[currentStrategy]
                ->robot[currentRobot]
                .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                ->setPixmap(str);
            strategy[currentStrategy]
                ->robot[currentRobot]
                .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                ->wayPointType = keyType;
            strategy[currentStrategy]
                ->robot[currentRobot]
                .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                ->setParent(this);
            if (strategy[currentStrategy]->robot[currentRobot].waypointCounter > 0)
            {
                strategy[currentStrategy]
                    ->robot[currentRobot]
                    .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                    ->wayPointType =
                    strategy[currentStrategy]->robot[currentRobot].waypoint[currentWaypoint]->wayPointType;
                strategy[currentStrategy]
                    ->robot[currentRobot]
                    .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                    ->needRRT = strategy[currentStrategy]->robot[currentRobot].waypoint[currentWaypoint]->needRRT;
                strategy[currentStrategy]
                    ->robot[currentRobot]
                    .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                    ->tolerance = strategy[currentStrategy]->robot[currentRobot].waypoint[currentWaypoint]->tolerance;
                strategy[currentStrategy]
                    ->robot[currentRobot]
                    .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                    ->time = strategy[currentStrategy]->robot[currentRobot].waypoint[currentWaypoint]->time;
                strategy[currentStrategy]
                    ->robot[currentRobot]
                    .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                    ->speed = strategy[currentStrategy]->robot[currentRobot].waypoint[currentWaypoint]->speed;
                strategy[currentStrategy]
                    ->robot[currentRobot]
                    .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                    ->velProfile = strategy[currentStrategy]->robot[currentRobot].waypoint[currentWaypoint]->velProfile;
            }
            strategy[currentStrategy]
                ->robot[currentRobot]
                .waypoint[strategy[currentStrategy]->robot[currentRobot].waypointCounter]
                ->show();
            strategy[currentStrategy]->robot[currentRobot].waypointCounter++;
            currentWaypoint = strategy[currentStrategy]->robot[currentRobot].waypointCounter - 1;
            clickedWayPoint = currentWaypoint;
            this->update();
        }
        return;
    }
    x = child->x();
    y = child->y();

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < strategy[currentStrategy]->robot[i].waypointCounter; j++)
        {
            if (x == strategy[currentStrategy]->robot[i].waypoint[j]->x() &&
                y == strategy[currentStrategy]->robot[i].waypoint[j]->y())
            {
                if (keySync == 1)
                {
                    int a = (i + 1) * 100 + (j + 1);
                    syncDatas[currentSync].point.append(a);
                    clicked = 1;
                    return;
                }
                std::cout << i << "   " << j << "\n";
                clickedRobot    = i;
                clickedWayPoint = j;
                currentWaypoint = j;
                clicked         = 1;
                // return;
            }
        }
    }
    QPixmap     pixmap = *child->pixmap();
    QByteArray  itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos());
    //! [1]

    //! [2]
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    //! [2]

    //! [3]
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());
    //! [3]

    QPixmap  tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
    painter.end();

    child->setPixmap(tempPixmap);

    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
        int y = 0; // child->close();
    else
    {
        child->show();
        child->setPixmap(pixmap);
    }
}
} // namespace Tyr::Strategy
