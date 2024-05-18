#include "frame.h"

#include "ui_frame.h"

namespace Tyr::Strategy
{
Frame::Frame(QWidget *parent) : QMainWindow(parent), ui(new Ui::Frame)
{
    ui->setupUi(this);
    gui = new Gui(this);
    gui->show();
    gui->setParent(ui->widget);
    gui->currentRobot   = 0;
    gui->strategCounter = 0;
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

    if (gui->strategCounter > 0)
        on_pushButton_3_clicked();

    if (gui->clicked)
    {
        QVariant t = gui->strategCounter;
        this->setWindowTitle(t.toString());

        QVariant b = gui->clickedWayPoint + 1;
        ui->comboBox_2->setCurrentIndex(gui->clickedRobot);
        ui->label_6->setText(b.toString());
        gui->clicked = 0;
        b = gui->strategy[gui->currentStrategy]->robot[gui->clickedRobot].waypoint[gui->clickedWayPoint]->wayPointType;
        ui->comboBox_4->setCurrentIndex(b.toInt());
        ui->comboBox_5->setCurrentIndex(
            gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->needRRT);
        ui->comboBox_7->setCurrentIndex(
            gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->velProfile);
        b = gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->tolerance;
        ui->lineEdit_4->setText(b.toString());
        b = gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->time;
        ui->lineEdit_5->setText(b.toString());
        b = gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->speed;
        ui->lineEdit_6->setText(b.toString());

        b = gui->strategy[gui->currentStrategy]->weight;
        ui->lineEdit_7->setText(b.toString());

        b = gui->strategy[gui->currentStrategy]->getName();
        ui->lineEdit->setText(b.toString());

        b = gui->strategy[gui->currentStrategy]->minY;
        ui->lineEdit_10->setText(b.toString());

        b = gui->strategy[gui->currentStrategy]->maxY;
        ui->lineEdit_11->setText(b.toString());

        ui->comboBox_6->setCurrentIndex(gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].finalRole);

        // if(gui->currentRobot == Robot::Attack)
        {
            QVariant b =
                gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->tolerance;
            ui->comboBox_8->setCurrentIndex(b.toInt());
            b = gui->strategy[gui->currentStrategy]
                    ->robot[gui->currentRobot]
                    .waypoint[gui->currentWaypoint]
                    ->wayPointType;
            ui->comboBox_9->setCurrentIndex(b.toInt());
        }

        // ui->textEdit->clear();
        for (int i = 0; i < gui->strategy[gui->currentStrategy]
                                ->robot[gui->clickedRobot]
                                .waypoint[gui->clickedWayPoint]
                                ->syncWayPoints.size();
             i++)
        {
            b = gui->strategy[gui->currentStrategy]
                    ->robot[gui->clickedRobot]
                    .waypoint[gui->clickedWayPoint]
                    ->syncWayPoints.at(i) +
                1;
            //  ui->textEdit->append(b.toString());
            // ui->textEdit->moveCursor(QTextCursor::Down);
        }

        // if(gui->syncDatas[gui->currentSync].point.size() != ui->listWidget->size())
        {
            // ui->listWidget->clear();
            for (int i = 0; i < gui->syncDatas[gui->currentSync].point.size(); i++)
            {
                b = gui->syncDatas[gui->currentSync].point.at(i);
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
    for (int i = 0; i < gui->strategCounter; i++)
    {
        if (gui->strategy[i]->getName().compare(ui->lineEdit->text()) == 0)
        {
            return;
        }
    }
    ui->lineEdit_10->setText("-3000");
    ui->lineEdit_11->setText("3000");
    gui->strategy[gui->strategCounter] = new Strategys();
    gui->strategy[gui->strategCounter]->SetName(ui->lineEdit->text());
    QVariant bb                                 = ui->lineEdit_8->text();
    gui->strategy[gui->strategCounter]->minX    = bb.toInt();
    bb                                          = ui->lineEdit_9->text();
    gui->strategy[gui->strategCounter]->maxX    = bb.toInt();
    QVariant ee                                 = 0;
    bb                                          = ui->lineEdit_10->text();
    gui->strategy[gui->strategCounter]->minY    = bb.toInt();
    bb                                          = ui->lineEdit_11->text();
    gui->strategy[gui->strategCounter]->maxY    = bb.toInt();
    gui->strategy[gui->currentStrategy]->weight = 0;
    ui->comboBox_3->addItem(gui->strategy[gui->strategCounter]->getName());
    gui->strategCounter++;
    ui->comboBox_3->setCurrentIndex(gui->strategCounter - 1);
    gui->currentStrategy = ui->comboBox_3->currentIndex();
    gui->currentRobot    = ui->comboBox_2->currentIndex();
    gui->update();
    QVariant b = gui->strategy[gui->currentStrategy]->getName();
    ui->lineEdit->setText(b.toString());
}
void Frame::on_pushButton_2_clicked()
{
    for (int i = 0; i < gui->strategCounter; i++)
    {
        if (gui->strategy[i]->getName().compare(ui->comboBox_3->currentText()) == 0)
        {
            for (int y = 0; y < gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypointCounter; y++)
            {
                gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[y]->close();
            }
            for (int j = i; j < gui->strategCounter - 1; j++)
            {
                gui->strategy[j] = gui->strategy[j + 1];
            }
            ui->comboBox_3->removeItem(i);
            gui->strategCounter--;
        }
    }
    gui->currentStrategy = ui->comboBox_3->currentIndex();
    gui->currentRobot    = ui->comboBox_2->currentIndex();
    gui->update();
    QVariant b = gui->strategy[gui->currentStrategy]->getName();
    ui->lineEdit->setText(b.toString());
}

void Frame::on_comboBox_3_currentIndexChanged(int index)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < gui->strategy[gui->currentStrategy]->robot[i].waypointCounter; j++)
        {
            gui->strategy[gui->currentStrategy]->robot[i].waypoint[j]->close();
        }
    }
    gui->currentStrategy = index;
    gui->currentRobot    = ui->comboBox_2->currentIndex();
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < gui->strategy[gui->currentStrategy]->robot[i].waypointCounter; j++)
        {
            gui->strategy[gui->currentStrategy]->robot[i].waypoint[j]->show();
        }
    }
    gui->update();
    QVariant b = gui->strategy[gui->currentStrategy]->getName();
    ui->lineEdit->setText(b.toString());

    b = gui->strategy[gui->currentStrategy]->weight;
    ui->lineEdit_7->setText(b.toString());

    b = gui->strategy[gui->currentStrategy]->minX;
    ui->lineEdit_8->setText(b.toString());

    b = gui->strategy[gui->currentStrategy]->maxX;
    ui->lineEdit_9->setText(b.toString());

    b = gui->strategy[gui->currentStrategy]->minY;
    ui->lineEdit_10->setText(b.toString());

    b = gui->strategy[gui->currentStrategy]->maxY;
    ui->lineEdit_11->setText(b.toString());

    if (gui->strategCounter > 0)
    {
        if (gui->strategy[gui->currentStrategy]->robot[Robot::Attack].waypoint[gui->currentWaypoint]->wayPointType == 0)
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
    gui->currentRobot = ui->comboBox_2->currentIndex();
    ui->comboBox_6->setCurrentIndex(gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].finalRole);
    if (gui->currentRobot == Robot::Attack)
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
    gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->wayPointType = index;
}

// void Frame::on_lineEdit_2_returnPressed()
//{
//     QVariant b = ui->lineEdit_2->text();
//     int y = gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint].y();
//     b = ((b.toInt()+3000)*4.0/25) + 20;
//     gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint].move(b.toInt()-18,y);
//     gui->update();
//     gui->clicked=1;
//     this->update();
// }

// void Frame::on_lineEdit_3_returnPressed()
//{
//     QVariant b = ui->lineEdit_3->text();
//     int x = gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint].x();
//     b = ((b.toInt()+2000)*4.0/25) + 4;
//     gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint].move(x,b.toInt()-18);
//     gui->update();
//     gui->clicked=1;
//     this->update();
// }

void Frame::on_toolButton_clicked()
{
    QString  str = "Sync Num ";
    QVariant b   = (int) gui->syncCounter + 1;
    str.append(b.toString());
    // ui->comboBox_6->addItem(str);
    gui->syncCounter++;
}

// void Frame::on_toolButton_2_clicked()
//{
//     ui->comboBox_6->removeItem(gui->currentSync);
//     gui->currentSync--;
// }

void Frame::on_comboBox_6_currentIndexChanged(int index)
{
    gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].finalRole = index;
}

void Frame::on_toolButton_3_clicked()
{
    gui->syncDatas[gui->currentSync].point.remove(gui->currentWaypoint);
}

void Frame::on_toolButton_4_clicked()
{
    for (int i = 0; i < gui->syncDatas[gui->currentSync].point.size(); i++)
    {
        if (((gui->currentRobot + 1) * 100 + gui->currentWaypoint + 1) == gui->syncDatas[gui->currentSync].point.at(i))
        {
            return;
        }
    }
    gui->syncDatas[gui->currentSync].point.append((gui->currentRobot + 1) * 100 + gui->currentWaypoint + 1);
    gui->clicked = 1;
}

void Frame::on_pushButton_3_clicked()
{
    playBook.Clear();
    for (int i = 0; i < gui->strategCounter; i++)
    {
        Common::logDebug("BOZ");
        Protos::Immortals::Strategy *strag = playBook.add_strategy();
        strag->set_name(gui->strategy[i]->getName().toStdString());
        strag->set_min_x(gui->strategy[i]->minX);
        strag->set_max_x(gui->strategy[i]->maxX);
        strag->set_min_y(gui->strategy[i]->minY);
        strag->set_max_y(gui->strategy[i]->maxY);
        playBook.add_weight(gui->strategy[i]->weight);
        for (int j = 0; j < 8; j++)
        {
            Protos::Immortals::Role *r = strag->add_role();
            r->set_id(j);
            r->set_name(gui->strategy[i]->robot[j].Name.toStdString());
            r->set_afterlife(gui->strategy[i]->robot[j].finalRole);
            for (int k = 0; k < gui->strategy[i]->robot[j].waypointCounter; k++)
            {
                Protos::Immortals::Waypoint *way = r->add_path();
                int                          xx  = gui->strategy[i]->robot[j].waypoint[k]->x();
                int                          yy  = gui->strategy[i]->robot[j].waypoint[k]->y();
                xx                               = xx - 106; // UP LEFT CORNER OFFSET
                yy                               = yy - 45;  // UP LEFT CORNER OFFSET
                              //               qDebug()<<"OLD x : "<<xx*10<<" Y : "<<yy*10;
                xx = ((xx * 100) / 7) - 4971;   //(((xx+13))*2/3)-450;
                yy = ((yy * -450) / 31) + 4384; //(-((yy+13)*2/3))+300;
                                                //               qDebug()<<"NEW x : "<<xx<<" Y : "<<yy;

                way->set_x(xx);
                way->set_y(yy);
                way->set_type((Protos::Immortals::Waypoint_Type) gui->strategy[i]->robot[j].waypoint[k]->wayPointType);
                way->set_need_rrt(gui->strategy[i]->robot[j].waypoint[k]->needRRT);
                way->set_speed(gui->strategy[i]->robot[j].waypoint[k]->speed);
                way->set_velocity_profile(
                    (Protos::Immortals::Waypoint_VelocityProfile) gui->strategy[i]->robot[j].waypoint[k]->velProfile);
                way->set_tolerance(gui->strategy[i]->robot[j].waypoint[k]->tolerance);
                way->set_time(gui->strategy[i]->robot[j].waypoint[k]->time);
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
        for (int i = 0; i < gui->strategCounter; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int k = 0; k < gui->strategy[i]->robot[j].waypointCounter; k++)
                {
                    gui->strategy[i]->robot[j].waypoint[k]->close();
                }
            }
        }
        gui->strategCounter = playBook.strategy_size();
        for (int i = 0; i < playBook.strategy_size(); i++)
        {
            gui->strategy[i] = new Strategys();
            QString r        = playBook.strategy(i).name().c_str();
            gui->strategy[i]->SetName(r);
            gui->strategy[i]->minX   = playBook.strategy(i).min_x();
            gui->strategy[i]->maxX   = playBook.strategy(i).max_x();
            gui->strategy[i]->minY   = playBook.strategy(i).min_y();
            gui->strategy[i]->maxY   = playBook.strategy(i).max_y();
            gui->strategy[i]->weight = playBook.weight(i);
            for (int j = 0; j < playBook.strategy(i).role_size(); j++)
            {
                gui->strategy[i]->robot[j].waypointCounter = playBook.strategy(i).role(j).path_size();
                gui->strategy[i]->robot[j].finalRole       = playBook.strategy(i).role(j).afterlife();
                for (int k = 0; k < playBook.strategy(i).role(j).path_size(); k++)
                {
                    int x =
                        (((playBook.strategy(i).role(j).path(k).x() / 1.0) + 4971) * 7) / 100; //((xx * 100)/7) - 4971
                    int y =
                        (((playBook.strategy(i).role(j).path(k).y() / 1.0) - 4384) * 31) / -450; //((-+300)*3.0/2)-13;

                    Common::logDebug("KHAR");
                    gui->strategy[i]->robot[j].waypoint[k]->move(x + 106, y + 45);
                    gui->strategy[i]->robot[j].waypoint[k]->setParent(gui);
                    gui->strategy[i]->robot[j].waypoint[k]->wayPointType = playBook.strategy(i).role(j).path(k).type();
                    gui->strategy[i]->robot[j].waypoint[k]->needRRT = playBook.strategy(i).role(j).path(k).need_rrt();
                    gui->strategy[i]->robot[j].waypoint[k]->speed   = playBook.strategy(i).role(j).path(k).speed();
                    gui->strategy[i]->robot[j].waypoint[k]->velProfile =
                        playBook.strategy(i).role(j).path(k).velocity_profile();
                    gui->strategy[i]->robot[j].waypoint[k]->tolerance =
                        playBook.strategy(i).role(j).path(k).tolerance();
                    gui->strategy[i]->robot[j].waypoint[k]->time = playBook.strategy(i).role(j).path(k).time();
                    QString  str                                 = QString(DATA_DIR) + "/strategy-images/";
                    QVariant b                                   = j + 1;
                    str.append(b.toString());
                    str.append("/");
                    b = k + 1;
                    str.append(b.toString());
                    str.append(".png");
                    gui->strategy[i]->robot[j].waypoint[k]->setPixmap(str);
                }
            }
        }

        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < gui->strategy[gui->currentStrategy]->robot[j].waypointCounter; k++)
            {
                gui->strategy[gui->currentStrategy]->robot[j].waypoint[k]->show();
            }
        }
        gui->clicked         = 1;
        gui->currentRobot    = 0;
        gui->currentStrategy = 0;
        gui->currentWaypoint = 0;
        ui->lineEdit->setText(gui->strategy[gui->currentStrategy]->getName());
        ui->comboBox_3->setCurrentIndex(gui->currentStrategy);
        for (int i = 0; i < gui->strategCounter; i++)
        {
            ui->comboBox_3->addItem(gui->strategy[i]->getName());
        }
        gui->update();
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
    gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->velProfile = index;
}

void Frame::on_lineEdit_6_returnPressed()
{
    QVariant b = ui->lineEdit_6->text();
    gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->speed = b.toFloat();
}

void Frame::on_lineEdit_5_returnPressed()
{
    QVariant b = ui->lineEdit_5->text();
    gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->time = b.toFloat();
}

void Frame::on_lineEdit_4_returnPressed()
{
    QVariant b = ui->lineEdit_4->text();
    gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->tolerance =
        b.toFloat();
}

void Frame::on_comboBox_5_currentIndexChanged(int index)
{
    if (index == 1)
        gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->needRRT = true;
    else if (index == 0)
        gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->needRRT = false;
}

void Frame::on_lineEdit_7_returnPressed()
{
    QVariant b = ui->lineEdit_7->text();
    if (gui->strategCounter > 0)
        gui->strategy[gui->currentStrategy]->weight = b.toInt();
}

void Frame::on_comboBox_8_currentIndexChanged(int index)
{
    if (gui->currentRobot == Robot::Attack && gui->strategCounter > 0)
    {
        gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->tolerance = index;
        // QVariant t =index;
        // ui->lineEdit_4->setText(t.toString());
    }
}

void Frame::on_comboBox_9_currentIndexChanged(int index)
{
    if (gui->currentRobot == Robot::Attack && gui->strategCounter > 0)
    {
        gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->wayPointType =
            index;
    }
}

void Frame::on_lineEdit_4_selectionChanged()
{
    QVariant b = ui->lineEdit_4->text();
    if (gui->strategCounter > 0)
        gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->tolerance =
            b.toFloat();
}

void Frame::on_lineEdit_5_selectionChanged()
{
    QVariant t = ui->lineEdit_5->text();
    if (gui->strategCounter > 0)
        gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->time =
            t.toFloat();
}

void Frame::on_lineEdit_6_selectionChanged()
{
    QVariant b = ui->lineEdit_6->text();
    if (gui->strategCounter > 0)
        gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->speed =
            b.toFloat();
}

void Frame::on_lineEdit_7_selectionChanged()
{
    QVariant b = ui->lineEdit_7->text();
    if (gui->strategCounter > 0)
        gui->strategy[gui->currentStrategy]->weight = b.toInt();
}

void Frame::on_pushButton_5_clicked()
{
    for (int i = 0; i < gui->strategCounter; i++)
    {
        if (gui->strategy[i]->getName().compare(ui->lineEdit->text() + "_Dup") == 0)
        {
            return;
        }
    }
    gui->strategy[gui->strategCounter] = new Strategys();
    gui->strategy[gui->strategCounter]->SetName(ui->lineEdit->text() + "_Dup");
    gui->strategy[gui->strategCounter]->minX   = gui->strategy[gui->currentStrategy]->minX;
    gui->strategy[gui->strategCounter]->maxX   = gui->strategy[gui->currentStrategy]->maxX;
    gui->strategy[gui->strategCounter]->minY   = gui->strategy[gui->currentStrategy]->minY;
    gui->strategy[gui->strategCounter]->maxY   = gui->strategy[gui->currentStrategy]->maxY;
    QVariant ee                                = ui->lineEdit_7->text();
    gui->strategy[gui->strategCounter]->weight = ee.toInt();
    ui->comboBox_3->addItem(gui->strategy[gui->strategCounter]->getName());

    for (int i = 0; i < 8; i++)
    {
        gui->strategy[gui->strategCounter]->robot[i].waypointCounter =
            gui->strategy[gui->currentStrategy]->robot[i].waypointCounter;
        gui->strategy[gui->strategCounter]->robot[i].finalRole =
            gui->strategy[gui->currentStrategy]->robot[i].finalRole;
        for (int k = 0; k < gui->strategy[gui->currentStrategy]->robot[i].waypointCounter; k++)
        {
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->move(
                gui->strategy[gui->currentStrategy]->robot[i].waypoint[k]->pos().x(),
                gui->strategy[gui->currentStrategy]->robot[i].waypoint[k]->pos().y());
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->wayPointType =
                (gui->strategy[gui->currentStrategy]->robot[i].waypoint[k]->wayPointType);
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->speed =
                (gui->strategy[gui->currentStrategy]->robot[i].waypoint[k]->speed);
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->tolerance =
                (gui->strategy[gui->currentStrategy]->robot[i].waypoint[k]->tolerance);
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->needRRT =
                (gui->strategy[gui->currentStrategy]->robot[i].waypoint[k]->needRRT);
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->time =
                (gui->strategy[gui->currentStrategy]->robot[i].waypoint[k]->time);
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->velProfile =
                (gui->strategy[gui->currentStrategy]->robot[i].waypoint[k]->velProfile);
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
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->setPixmap(str);
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->setParent(gui);
            gui->strategy[gui->strategCounter]->robot[i].waypoint[k]->show();
        }
    }
    ui->comboBox_3->setCurrentIndex(gui->strategCounter);
    gui->currentStrategy = ui->comboBox_3->currentIndex();
    gui->currentRobot    = ui->comboBox_2->currentIndex();
    gui->update();
    QVariant b = gui->strategy[gui->currentStrategy]->getName();
    ui->lineEdit->setText(b.toString());
    gui->strategCounter++;
}

void Frame::on_pushButton_6_clicked()
{
    // UDPSocket *
}

void Frame::on_lineEdit_8_returnPressed()
{
    QVariant b                                = ui->lineEdit_8->text();
    gui->strategy[gui->currentStrategy]->minX = b.toInt();
}

void Frame::on_lineEdit_9_returnPressed()
{
    QVariant b                                = ui->lineEdit_9->text();
    gui->strategy[gui->currentStrategy]->maxX = b.toInt();
}

void Frame::on_pushButton_7_clicked()
{
    gui->strategy[gui->currentStrategy]->SetName(ui->lineEdit->text());
    ui->comboBox_3->setItemText(ui->comboBox_3->currentIndex(), ui->lineEdit->text());
}

void Frame::on_lineEdit_10_editingFinished()
{
    QVariant b                                = ui->lineEdit_10->text();
    gui->strategy[gui->currentStrategy]->minY = b.toInt();
}

void Frame::on_lineEdit_11_editingFinished()
{
    QVariant b                                = ui->lineEdit_11->text();
    gui->strategy[gui->currentStrategy]->maxY = b.toInt();
}

void Frame::on_lineEdit_8_editingFinished()
{
    QVariant b                                = ui->lineEdit_8->text();
    gui->strategy[gui->currentStrategy]->minX = b.toInt();
}

void Frame::on_lineEdit_9_editingFinished()
{
    QVariant b                                = ui->lineEdit_9->text();
    gui->strategy[gui->currentStrategy]->maxX = b.toInt();
}

void Frame::on_lineEdit_4_editingFinished()
{
    QVariant b = ui->lineEdit_4->text();
    if (gui->strategCounter > 0)
        gui->strategy[gui->currentStrategy]->robot[gui->currentRobot].waypoint[gui->currentWaypoint]->tolerance =
            b.toInt();
}
} // namespace Tyr::Strategy
