#include "frame.h"

#include "ui_frame.h"

namespace Tyr::Strategy
{
Frame::Frame(QWidget *parent) : QMainWindow(parent), ui(new Ui::Frame)
{
    ui->setupUi(this);
    g = new gui(this);
    g->show();
    g->setParent(ui->widget);
    g->currentRobot   = 0;
    g->strategCounter = 0;
    setWindowTitle("Strategy Center");
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    timer.start(300);
    udp = new Common::UdpServer();
}

Frame::~Frame()
{
    delete ui;
}

void Frame::update()
{

    if (g->strategCounter > 0)
        on_pushButton_3_clicked();

    if (g->clicked)
    {
        QVariant t = g->strategCounter;
        this->setWindowTitle(t.toString());

        QVariant b = g->clickedWayPoint + 1;
        ui->comboBox_2->setCurrentIndex(g->clickedRobot);
        ui->label_6->setText(b.toString());
        g->clicked = 0;
        b          = g->strategy[g->currentStrategy]->robot[g->clickedRobot].waypoint[g->clickedWayPoint]->wayPointType;
        ui->comboBox_4->setCurrentIndex(b.toInt());
        ui->comboBox_5->setCurrentIndex(
            g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->needRRT);
        ui->comboBox_7->setCurrentIndex(
            g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->velProfile);
        b = g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->tolerance;
        ui->lineEdit_4->setText(b.toString());
        b = g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->time;
        ui->lineEdit_5->setText(b.toString());
        b = g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->speed;
        ui->lineEdit_6->setText(b.toString());

        b = g->strategy[g->currentStrategy]->weight;
        ui->lineEdit_7->setText(b.toString());

        b = g->strategy[g->currentStrategy]->getName();
        ui->lineEdit->setText(b.toString());

        b = g->strategy[g->currentStrategy]->minY;
        ui->lineEdit_10->setText(b.toString());

        b = g->strategy[g->currentStrategy]->maxY;
        ui->lineEdit_11->setText(b.toString());

        ui->comboBox_6->setCurrentIndex(g->strategy[g->currentStrategy]->robot[g->currentRobot].finalRole);

        // if(g->currentRobot == Robot::Attack)
        {
            QVariant b =
                g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->tolerance;
            ui->comboBox_8->setCurrentIndex(b.toInt());
            b = g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->wayPointType;
            ui->comboBox_9->setCurrentIndex(b.toInt());
        }

        // ui->textEdit->clear();
        for (int i = 0;
             i <
             g->strategy[g->currentStrategy]->robot[g->clickedRobot].waypoint[g->clickedWayPoint]->syncWayPoints.size();
             i++)
        {
            b = g->strategy[g->currentStrategy]->robot[g->clickedRobot].waypoint[g->clickedWayPoint]->syncWayPoints.at(
                    i) +
                1;
            //  ui->textEdit->append(b.toString());
            // ui->textEdit->moveCursor(QTextCursor::Down);
        }

        // if(g->syncDatas[g->currentSync].point.size() != ui->listWidget->size())
        {
            // ui->listWidget->clear();
            for (int i = 0; i < g->syncDatas[g->currentSync].point.size(); i++)
            {
                b = g->syncDatas[g->currentSync].point.at(i);
                // ui->listWidget->addItem(b.toString());
            }
        }
    }
}

void Frame::paintEvent(QPaintEvent *event)
{}

void Frame::dragEnterEvent(QDragEnterEvent *event)
{}

void Frame::dragMoveEvent(QDragMoveEvent *event)
{}

void Frame::dropEvent(QDropEvent *event)
{}

void Frame::mousePressEvent(QMouseEvent *event)
{}

void Frame::on_pushButton_clicked() // Add Strategy
{
    for (int i = 0; i < g->strategCounter; i++)
    {
        if (g->strategy[i]->getName().compare(ui->lineEdit->text()) == 0)
        {
            return;
        }
    }
    ui->lineEdit_10->setText("-3000");
    ui->lineEdit_11->setText("3000");
    g->strategy[g->strategCounter] = new Strategys();
    g->strategy[g->strategCounter]->SetName(ui->lineEdit->text());
    QVariant bb                             = ui->lineEdit_8->text();
    g->strategy[g->strategCounter]->minX    = bb.toInt();
    bb                                      = ui->lineEdit_9->text();
    g->strategy[g->strategCounter]->maxX    = bb.toInt();
    QVariant ee                             = 0;
    bb                                      = ui->lineEdit_10->text();
    g->strategy[g->strategCounter]->minY    = bb.toInt();
    bb                                      = ui->lineEdit_11->text();
    g->strategy[g->strategCounter]->maxY    = bb.toInt();
    g->strategy[g->currentStrategy]->weight = 0;
    ui->comboBox_3->addItem(g->strategy[g->strategCounter]->getName());
    g->strategCounter++;
    ui->comboBox_3->setCurrentIndex(g->strategCounter - 1);
    g->currentStrategy = ui->comboBox_3->currentIndex();
    g->currentRobot    = ui->comboBox_2->currentIndex();
    g->update();
    QVariant b = g->strategy[g->currentStrategy]->getName();
    ui->lineEdit->setText(b.toString());
}
void Frame::on_pushButton_2_clicked()
{
    for (int i = 0; i < g->strategCounter; i++)
    {
        if (g->strategy[i]->getName().compare(ui->comboBox_3->currentText()) == 0)
        {
            for (int y = 0; y < g->strategy[g->currentStrategy]->robot[g->currentRobot].waypointCounter; y++)
            {
                g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[y]->close();
            }
            for (int j = i; j < g->strategCounter - 1; j++)
            {
                g->strategy[j] = g->strategy[j + 1];
            }
            ui->comboBox_3->removeItem(i);
            g->strategCounter--;
        }
    }
    g->currentStrategy = ui->comboBox_3->currentIndex();
    g->currentRobot    = ui->comboBox_2->currentIndex();
    g->update();
    QVariant b = g->strategy[g->currentStrategy]->getName();
    ui->lineEdit->setText(b.toString());
}

void Frame::on_comboBox_3_currentIndexChanged(int index)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < g->strategy[g->currentStrategy]->robot[i].waypointCounter; j++)
        {
            g->strategy[g->currentStrategy]->robot[i].waypoint[j]->close();
        }
    }
    g->currentStrategy = index;
    g->currentRobot    = ui->comboBox_2->currentIndex();
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < g->strategy[g->currentStrategy]->robot[i].waypointCounter; j++)
        {
            g->strategy[g->currentStrategy]->robot[i].waypoint[j]->show();
        }
    }
    g->update();
    QVariant b = g->strategy[g->currentStrategy]->getName();
    ui->lineEdit->setText(b.toString());

    b = g->strategy[g->currentStrategy]->weight;
    ui->lineEdit_7->setText(b.toString());

    b = g->strategy[g->currentStrategy]->minX;
    ui->lineEdit_8->setText(b.toString());

    b = g->strategy[g->currentStrategy]->maxX;
    ui->lineEdit_9->setText(b.toString());

    b = g->strategy[g->currentStrategy]->minY;
    ui->lineEdit_10->setText(b.toString());

    b = g->strategy[g->currentStrategy]->maxY;
    ui->lineEdit_11->setText(b.toString());

    if (g->strategCounter > 0)
    {
        if (g->strategy[g->currentStrategy]->robot[Robot::Attack].waypoint[g->currentWaypoint]->wayPointType == 0)
            ui->label_8->setText("Kick");
        else
            ui->label_8->setText("Chip");
    }
    else
    {
        ui->label_8->setText("");
    }
}

void Frame::on_comboBox_2_currentIndexChanged(int index)
{
    g->currentRobot = ui->comboBox_2->currentIndex();
    ui->comboBox_6->setCurrentIndex(g->strategy[g->currentStrategy]->robot[g->currentRobot].finalRole);
    if (g->currentRobot == Robot::Attack)
    {
        ui->comboBox_8->setEnabled(true);
        ui->comboBox_9->setEnabled(true);
    }
    else
    {
        ui->comboBox_8->setEnabled(false);
        ui->comboBox_9->setEnabled(false);
    }
}

void Frame::on_comboBox_4_currentIndexChanged(int index)
{
    g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->wayPointType = index;
}

// void Frame::on_lineEdit_2_returnPressed()
//{
//     QVariant b = ui->lineEdit_2->text();
//     int y = g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->y();
//     b = ((b.toInt()+3000)*4.0/25) + 20;
//     g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->move(b.toInt()-18,y);
//     g->update();
//     g->clicked=1;
//     this->update();
// }

// void Frame::on_lineEdit_3_returnPressed()
//{
//     QVariant b = ui->lineEdit_3->text();
//     int x = g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->x();
//     b = ((b.toInt()+2000)*4.0/25) + 4;
//     g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->move(x,b.toInt()-18);
//     g->update();
//     g->clicked=1;
//     this->update();
// }

void Frame::on_toolButton_clicked()
{
    QString  str = "Sync Num ";
    QVariant b   = (int) g->syncCounter + 1;
    str.append(b.toString());
    // ui->comboBox_6->addItem(str);
    g->syncCounter++;
}

// void Frame::on_toolButton_2_clicked()
//{
//     ui->comboBox_6->removeItem(g->currentSync);
//     g->currentSync--;
// }

void Frame::on_comboBox_6_currentIndexChanged(int index)
{
    g->strategy[g->currentStrategy]->robot[g->currentRobot].finalRole = index;
}

void Frame::on_toolButton_3_clicked()
{
    g->syncDatas[g->currentSync].point.remove(g->currentWaypoint);
}

void Frame::on_toolButton_4_clicked()
{
    for (int i = 0; i < g->syncDatas[g->currentSync].point.size(); i++)
    {
        if (((g->currentRobot + 1) * 100 + g->currentWaypoint + 1) == g->syncDatas[g->currentSync].point.at(i))
        {
            return;
        }
    }
    g->syncDatas[g->currentSync].point.append((g->currentRobot + 1) * 100 + g->currentWaypoint + 1);
    g->clicked = 1;
}

void Frame::on_pushButton_3_clicked()
{
    playBook.Clear();
    for (int i = 0; i < g->strategCounter; i++)
    {
        Common::logDebug("BOZ");
        Protos::Immortals::Strategy *strag = playBook.add_strategy();
        strag->set_name(g->strategy[i]->getName().toStdString());
        strag->set_min_x(g->strategy[i]->minX);
        strag->set_max_x(g->strategy[i]->maxX);
        strag->set_min_y(g->strategy[i]->minY);
        strag->set_max_y(g->strategy[i]->maxY);
        playBook.add_weight(g->strategy[i]->weight);
        for (int j = 0; j < 8; j++)
        {
            Protos::Immortals::Role *r = strag->add_role();
            r->set_id(j);
            r->set_name(g->strategy[i]->robot[j].Name.toStdString());
            r->set_afterlife(g->strategy[i]->robot[j].finalRole);
            for (int k = 0; k < g->strategy[i]->robot[j].waypointCounter; k++)
            {
                Protos::Immortals::Waypoint *way = r->add_path();
                int                          xx  = g->strategy[i]->robot[j].waypoint[k]->x();
                int                          yy  = g->strategy[i]->robot[j].waypoint[k]->y();
                xx                               = xx - 106; // UP LEFT CORNER OFFSET
                yy                               = yy - 45;  // UP LEFT CORNER OFFSET
                              //               qDebug()<<"OLD x : "<<xx*10<<" Y : "<<yy*10;
                xx = ((xx * 100) / 7) - 4971;   //(((xx+13))*2/3)-450;
                yy = ((yy * -450) / 31) + 4384; //(-((yy+13)*2/3))+300;
                                                //               qDebug()<<"NEW x : "<<xx<<" Y : "<<yy;

                way->set_x(xx);
                way->set_y(yy);
                way->set_type((Protos::Immortals::Waypoint_Type) g->strategy[i]->robot[j].waypoint[k]->wayPointType);
                way->set_need_rrt(g->strategy[i]->robot[j].waypoint[k]->needRRT);
                way->set_speed(g->strategy[i]->robot[j].waypoint[k]->speed);
                way->set_velocity_profile(
                    (Protos::Immortals::Waypoint_VelocityProfile) g->strategy[i]->robot[j].waypoint[k]->velProfile);
                way->set_tolerance(g->strategy[i]->robot[j].waypoint[k]->tolerance);
                way->set_time(g->strategy[i]->robot[j].waypoint[k]->time);
            }
        }
    }

    std::ofstream file(file_path.c_str(), std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        QMessageBox msg;
        msg.setText("Can't Write to File !!! ");
        msg.exec();
    }
    std::string str;
    playBook.SerializeToString(&str);
    file.clear();
    file.write(str.data(), str.length());
    file.close();
    udp->send(playBook, Common::setting().strategy_address);
}

void Frame::on_pushButton_4_clicked()
{
    std::ifstream::pos_type size;
    char                   *memblock;
    file_path = DATA_DIR;
    file_path += "/strategy.ims";
    std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
    if (file.is_open())
    {
        file.seekg(0, std::ios::end);
        size     = file.tellg();
        memblock = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(memblock, size);
        playBook.ParseFromArray(memblock, size);
        for (int i = 0; i < g->strategCounter; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int k = 0; k < g->strategy[i]->robot[j].waypointCounter; k++)
                {
                    g->strategy[i]->robot[j].waypoint[k]->close();
                }
            }
        }
        g->strategCounter = playBook.strategy_size();
        for (int i = 0; i < playBook.strategy_size(); i++)
        {
            g->strategy[i] = new Strategys();
            QString r      = playBook.strategy(i).name().c_str();
            g->strategy[i]->SetName(r);
            g->strategy[i]->minX   = playBook.strategy(i).min_x();
            g->strategy[i]->maxX   = playBook.strategy(i).max_x();
            g->strategy[i]->minY   = playBook.strategy(i).min_y();
            g->strategy[i]->maxY   = playBook.strategy(i).max_y();
            g->strategy[i]->weight = playBook.weight(i);
            for (int j = 0; j < playBook.strategy(i).role_size(); j++)
            {
                g->strategy[i]->robot[j].waypointCounter = playBook.strategy(i).role(j).path_size();
                g->strategy[i]->robot[j].finalRole       = playBook.strategy(i).role(j).afterlife();
                for (int k = 0; k < playBook.strategy(i).role(j).path_size(); k++)
                {
                    int x =
                        (((playBook.strategy(i).role(j).path(k).x() / 1.0) + 4971) * 7) / 100; //((xx * 100)/7) - 4971
                    int y =
                        (((playBook.strategy(i).role(j).path(k).y() / 1.0) - 4384) * 31) / -450; //((-+300)*3.0/2)-13;

                    Common::logDebug("KHAR");
                    g->strategy[i]->robot[j].waypoint[k]->move(x + 106, y + 45);
                    g->strategy[i]->robot[j].waypoint[k]->setParent(g);
                    g->strategy[i]->robot[j].waypoint[k]->wayPointType = playBook.strategy(i).role(j).path(k).type();
                    g->strategy[i]->robot[j].waypoint[k]->needRRT = playBook.strategy(i).role(j).path(k).need_rrt();
                    g->strategy[i]->robot[j].waypoint[k]->speed   = playBook.strategy(i).role(j).path(k).speed();
                    g->strategy[i]->robot[j].waypoint[k]->velProfile =
                        playBook.strategy(i).role(j).path(k).velocity_profile();
                    g->strategy[i]->robot[j].waypoint[k]->tolerance = playBook.strategy(i).role(j).path(k).tolerance();
                    g->strategy[i]->robot[j].waypoint[k]->time      = playBook.strategy(i).role(j).path(k).time();
                    QString  str                                    = QString(DATA_DIR) + "/strategy-images/";
                    QVariant b                                      = j + 1;
                    str.append(b.toString());
                    str.append("/");
                    b = k + 1;
                    str.append(b.toString());
                    str.append(".png");
                    g->strategy[i]->robot[j].waypoint[k]->setPixmap(str);
                }
            }
        }

        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < g->strategy[g->currentStrategy]->robot[j].waypointCounter; k++)
            {
                g->strategy[g->currentStrategy]->robot[j].waypoint[k]->show();
            }
        }
        g->clicked         = 1;
        g->currentRobot    = 0;
        g->currentStrategy = 0;
        g->currentWaypoint = 0;
        ui->lineEdit->setText(g->strategy[g->currentStrategy]->getName());
        ui->comboBox_3->setCurrentIndex(g->currentStrategy);
        for (int i = 0; i < g->strategCounter; i++)
        {
            ui->comboBox_3->addItem(g->strategy[i]->getName());
        }
        g->update();
        file.close();
        delete[] memblock;
    }
    else
    {
        QMessageBox msg;
        msg.setText("Can't Open File !!! ");
        msg.exec();
    }
}

void Frame::on_comboBox_7_currentIndexChanged(int index)
{
    g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->velProfile = index;
}

void Frame::on_lineEdit_6_returnPressed()
{
    QVariant b = ui->lineEdit_6->text();
    g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->speed = b.toFloat();
}

void Frame::on_lineEdit_5_returnPressed()
{
    QVariant b                                                                                 = ui->lineEdit_5->text();
    g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->time = b.toFloat();
}

void Frame::on_lineEdit_4_returnPressed()
{
    QVariant b = ui->lineEdit_4->text();
    g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->tolerance = b.toFloat();
}

void Frame::on_comboBox_5_currentIndexChanged(int index)
{
    if (index == 1)
        g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->needRRT = true;
    else if (index == 0)
        g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->needRRT = false;
}

void Frame::on_lineEdit_7_returnPressed()
{
    QVariant b = ui->lineEdit_7->text();
    if (g->strategCounter > 0)
        g->strategy[g->currentStrategy]->weight = b.toInt();
}

void Frame::on_comboBox_8_currentIndexChanged(int index)
{
    if (g->currentRobot == Robot::Attack && g->strategCounter > 0)
    {
        g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->tolerance = index;
        // QVariant t =index;
        // ui->lineEdit_4->setText(t.toString());
    }
}

void Frame::on_comboBox_9_currentIndexChanged(int index)
{
    if (g->currentRobot == Robot::Attack && g->strategCounter > 0)
    {
        g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->wayPointType = index;
    }
}

void Frame::on_lineEdit_4_selectionChanged()
{
    QVariant b = ui->lineEdit_4->text();
    if (g->strategCounter > 0)
        g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->tolerance = b.toFloat();
}

void Frame::on_lineEdit_5_selectionChanged()
{
    QVariant t = ui->lineEdit_5->text();
    if (g->strategCounter > 0)
        g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->time = t.toFloat();
}

void Frame::on_lineEdit_6_selectionChanged()
{
    QVariant b = ui->lineEdit_6->text();
    if (g->strategCounter > 0)
        g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->speed = b.toFloat();
}

void Frame::on_lineEdit_7_selectionChanged()
{
    QVariant b = ui->lineEdit_7->text();
    if (g->strategCounter > 0)
        g->strategy[g->currentStrategy]->weight = b.toInt();
}

void Frame::on_pushButton_5_clicked()
{
    for (int i = 0; i < g->strategCounter; i++)
    {
        if (g->strategy[i]->getName().compare(ui->lineEdit->text() + "_Dup") == 0)
        {
            return;
        }
    }
    g->strategy[g->strategCounter] = new Strategys();
    g->strategy[g->strategCounter]->SetName(ui->lineEdit->text() + "_Dup");
    g->strategy[g->strategCounter]->minX   = g->strategy[g->currentStrategy]->minX;
    g->strategy[g->strategCounter]->maxX   = g->strategy[g->currentStrategy]->maxX;
    g->strategy[g->strategCounter]->minY   = g->strategy[g->currentStrategy]->minY;
    g->strategy[g->strategCounter]->maxY   = g->strategy[g->currentStrategy]->maxY;
    QVariant ee                            = ui->lineEdit_7->text();
    g->strategy[g->strategCounter]->weight = ee.toInt();
    ui->comboBox_3->addItem(g->strategy[g->strategCounter]->getName());

    for (int i = 0; i < 8; i++)
    {
        g->strategy[g->strategCounter]->robot[i].waypointCounter =
            g->strategy[g->currentStrategy]->robot[i].waypointCounter;
        g->strategy[g->strategCounter]->robot[i].finalRole = g->strategy[g->currentStrategy]->robot[i].finalRole;
        for (int k = 0; k < g->strategy[g->currentStrategy]->robot[i].waypointCounter; k++)
        {
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->move(
                g->strategy[g->currentStrategy]->robot[i].waypoint[k]->pos().x(),
                g->strategy[g->currentStrategy]->robot[i].waypoint[k]->pos().y());
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->wayPointType =
                (g->strategy[g->currentStrategy]->robot[i].waypoint[k]->wayPointType);
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->speed =
                (g->strategy[g->currentStrategy]->robot[i].waypoint[k]->speed);
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->tolerance =
                (g->strategy[g->currentStrategy]->robot[i].waypoint[k]->tolerance);
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->needRRT =
                (g->strategy[g->currentStrategy]->robot[i].waypoint[k]->needRRT);
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->time =
                (g->strategy[g->currentStrategy]->robot[i].waypoint[k]->time);
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->velProfile =
                (g->strategy[g->currentStrategy]->robot[i].waypoint[k]->velProfile);
            QString  str = QString(DATA_DIR) + "/strategy-images/";
            QVariant bb;
            bb = i + 1;
            str.append(bb.toString());
            str.append("/");
            bb = k + 1;
            if (bb.toInt() > 8)
                bb = 8;
            str.append(bb.toString());
            str.append(".png");
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->setPixmap(str);
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->setParent(g);
            g->strategy[g->strategCounter]->robot[i].waypoint[k]->show();
        }
    }
    ui->comboBox_3->setCurrentIndex(g->strategCounter);
    g->currentStrategy = ui->comboBox_3->currentIndex();
    g->currentRobot    = ui->comboBox_2->currentIndex();
    g->update();
    QVariant b = g->strategy[g->currentStrategy]->getName();
    ui->lineEdit->setText(b.toString());
    g->strategCounter++;
}

void Frame::on_pushButton_6_clicked()
{
    // UDPSocket *
}

void Frame::on_lineEdit_8_returnPressed()
{
    QVariant b                            = ui->lineEdit_8->text();
    g->strategy[g->currentStrategy]->minX = b.toInt();
}

void Frame::on_lineEdit_9_returnPressed()
{
    QVariant b                            = ui->lineEdit_9->text();
    g->strategy[g->currentStrategy]->maxX = b.toInt();
}

void Frame::on_pushButton_7_clicked()
{
    g->strategy[g->currentStrategy]->SetName(ui->lineEdit->text());
    ui->comboBox_3->setItemText(ui->comboBox_3->currentIndex(), ui->lineEdit->text());
}

void Frame::on_lineEdit_10_editingFinished()
{
    QVariant b                            = ui->lineEdit_10->text();
    g->strategy[g->currentStrategy]->minY = b.toInt();
}

void Frame::on_lineEdit_11_editingFinished()
{
    QVariant b                            = ui->lineEdit_11->text();
    g->strategy[g->currentStrategy]->maxY = b.toInt();
}

void Frame::on_lineEdit_8_editingFinished()
{
    QVariant b                            = ui->lineEdit_8->text();
    g->strategy[g->currentStrategy]->minX = b.toInt();
}

void Frame::on_lineEdit_9_editingFinished()
{
    QVariant b                            = ui->lineEdit_9->text();
    g->strategy[g->currentStrategy]->maxX = b.toInt();
}

void Frame::on_lineEdit_4_editingFinished()
{
    QVariant b = ui->lineEdit_4->text();
    if (g->strategCounter > 0)
        g->strategy[g->currentStrategy]->robot[g->currentRobot].waypoint[g->currentWaypoint]->tolerance = b.toInt();
}
} // namespace Tyr::Strategy
