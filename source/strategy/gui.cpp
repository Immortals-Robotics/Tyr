#include "gui.h"
#include "waypoint.h"

namespace Tyr::Strategy
{
Gui::Gui(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Strategy Center");
    setGeometry(0, 0, 1600, 1000);
    setFixedSize(1600, 1000);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true);
}

QColor col[7] = {QColor(15, 127, 127), Qt::black, Qt::yellow, Qt::red, Qt::blue, QColor(150, 0, 150)};

void Gui::paintEvent([[maybe_unused]] QPaintEvent *event)
{
    m_painter.begin(this);
    m_painter.drawImage(0, 0, QImage(QString(DATA_DIR) + "/strategy-images/field.png"));
    // painter.drawImage(0,0,fieldGeometry);
    if (strategy_counter > 0)
    {
        for (int j = 0; j < 8; j++)
        {
            for (int i = 0; i < strategy[current_strategy]->robot[j].waypoint_counter - 1; i++)
            {
                m_painter.setPen(col[j]);
                m_painter.drawLine(strategy[current_strategy]->robot[j].waypoint[i]->x() + 12,
                                   strategy[current_strategy]->robot[j].waypoint[i]->y() + 12,
                                   strategy[current_strategy]->robot[j].waypoint[i + 1]->x() + 12,
                                   strategy[current_strategy]->robot[j].waypoint[i + 1]->y() + 12);
            }
        }
        int x1 = (((strategy[current_strategy]->min_x / 10.0) + 450) * 3.0 / 2);
        int x2 = (((strategy[current_strategy]->max_x / 10.0) + 450) * 3.0 / 2);
        int y1 = ((-(strategy[current_strategy]->min_y / 10.0) + 300) * 3.0 / 2);
        int y2 = ((-(strategy[current_strategy]->max_y / 10.0) + 300) * 3.0 / 2);

        x1 += 106;
        x2 += 106;
        y1 += 45;
        y2 += 45;
        m_painter.fillRect(x1, y1 - 10, (x2 - x1), 20, QColor(255, 0, 0, strategy[current_strategy]->weight * 20));
    }

    m_painter.end();
}

void Gui::dragEnterEvent(QDragEnterEvent *event)
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

void Gui::dragMoveEvent(QDragMoveEvent *event)
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

void Gui::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        QByteArray  itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint  offset;
        dataStream >> pixmap >> offset;
        strategy[current_strategy]->robot[clicked_robot].waypoint[clicked_waypoint]->setPixmap(pixmap);
        strategy[current_strategy]->robot[clicked_robot].waypoint[clicked_waypoint]->move(event->pos() - offset);
        strategy[current_strategy]->robot[clicked_robot].waypoint[clicked_waypoint]->show();
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

void Gui::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift)
    {
        key_type = 1;
    }
    else if (event->key() == Qt::Key_Alt)
    {
        key_sync = 1;
    }
    else if (event->key() == Qt::Key_Delete)
    {
        for (int i = current_waypoint; i < strategy[current_strategy]->robot[current_robot].waypoint_counter - 1; i++)
        {
            int x, y;
            x = strategy[current_strategy]->robot[current_robot].waypoint[i + 1]->x();
            y = strategy[current_strategy]->robot[current_robot].waypoint[i + 1]->y();
            strategy[current_strategy]->robot[current_robot].waypoint[i]->move(x, y);
            strategy[current_strategy]->robot[current_robot].waypoint[i]->show();
        }
        strategy[current_strategy]->robot[current_robot].waypoint_counter--;
        strategy[current_strategy]
            ->robot[current_robot]
            .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
            ->close();
    }
}

void Gui::keyReleaseEvent([[maybe_unused]] QKeyEvent *event)
{
    key_type = 0;
    key_sync = 0;
}

void Gui::mousePressEvent(QMouseEvent *event)
{
    int     x, y;
    QLabel *child = static_cast<QLabel *>(childAt(event->pos()));
    if (!child)
    {
        QString  str = QString(DATA_DIR) + "/strategy-images/";
        QVariant b;
        if (strategy_counter == 0)
        {
            QMessageBox msg;
            msg.setText("Olagh esm bede be Strategy");
            msg.setWindowTitle("Khar");
            msg.exec();
        }
        else
        {
            strategy[current_strategy]
                ->robot[current_robot]
                .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                ->move(event->x() - 13, event->y() - 13);
            b = current_robot + 1;
            str.append(b.toString());
            str.append("/");
            b = strategy[current_strategy]->robot[current_robot].waypoint_counter + 1;
            if (b.toInt() > 8)
                b = 8;
            str.append(b.toString());
            str.append(".png");
            Common::logDebug("THE STR IS: {}", str.toStdString());
            strategy[current_strategy]
                ->robot[current_robot]
                .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                ->setPixmap(str);
            strategy[current_strategy]
                ->robot[current_robot]
                .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                ->waypoint_type = key_type;
            strategy[current_strategy]
                ->robot[current_robot]
                .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                ->setParent(this);
            if (strategy[current_strategy]->robot[current_robot].waypoint_counter > 0)
            {
                strategy[current_strategy]
                    ->robot[current_robot]
                    .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                    ->waypoint_type =
                    strategy[current_strategy]->robot[current_robot].waypoint[current_waypoint]->waypoint_type;
                strategy[current_strategy]
                    ->robot[current_robot]
                    .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                    ->need_rrt = strategy[current_strategy]->robot[current_robot].waypoint[current_waypoint]->need_rrt;
                strategy[current_strategy]
                    ->robot[current_robot]
                    .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                    ->tolerance =
                    strategy[current_strategy]->robot[current_robot].waypoint[current_waypoint]->tolerance;
                strategy[current_strategy]
                    ->robot[current_robot]
                    .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                    ->time = strategy[current_strategy]->robot[current_robot].waypoint[current_waypoint]->time;
                strategy[current_strategy]
                    ->robot[current_robot]
                    .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                    ->speed = strategy[current_strategy]->robot[current_robot].waypoint[current_waypoint]->speed;
                strategy[current_strategy]
                    ->robot[current_robot]
                    .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                    ->velocity_profile =
                    strategy[current_strategy]->robot[current_robot].waypoint[current_waypoint]->velocity_profile;
            }
            strategy[current_strategy]
                ->robot[current_robot]
                .waypoint[strategy[current_strategy]->robot[current_robot].waypoint_counter]
                ->show();
            strategy[current_strategy]->robot[current_robot].waypoint_counter++;
            current_waypoint = strategy[current_strategy]->robot[current_robot].waypoint_counter - 1;
            clicked_waypoint = current_waypoint;
            this->update();
        }
        return;
    }
    x = child->x();
    y = child->y();

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < strategy[current_strategy]->robot[i].waypoint_counter; j++)
        {
            if (x == strategy[current_strategy]->robot[i].waypoint[j]->x() &&
                y == strategy[current_strategy]->robot[i].waypoint[j]->y())
            {
                if (key_sync == 1)
                {
                    int a = (i + 1) * 100 + (j + 1);
                    sync_data[current_sync].point.append(a);
                    clicked = 1;
                    return;
                }
                Common::logTrace("{}   {}", i, j);
                clicked_robot    = i;
                clicked_waypoint = j;
                current_waypoint = j;
                clicked          = 1;
                // return;
            }
        }
    }
    QPixmap    pixmap = child->pixmap(Qt::ReturnByValue);
    QByteArray itemData;

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
    {
        // child->close();
    }
    else
    {
        child->show();
        child->setPixmap(pixmap);
    }
}
} // namespace Tyr::Strategy
