#include "frame.h"

namespace Tyr::Strategy
{
Frame::Frame(QWidget *parent) : QMainWindow(parent)
{
    m_frame = std::make_unique<Ui::Frame>();
    m_frame->setupUi(this);

    m_gui = std::make_unique<Gui>(this);
    m_gui->show();
    m_gui->setParent(m_frame->widget);
    m_gui->current_robot    = Robot::GK;
    m_gui->strategy_counter = 0;
    setWindowTitle("Strategy Center");
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer.start(300);
    m_udp = std::make_unique<Common::UdpServer>();
}

void Frame::update()
{
    if (m_gui->strategy_counter > 0)
        on_pushButton_3_clicked();

    if (m_gui->clicked)
    {
        QVariant t = m_gui->strategy_counter;
        this->setWindowTitle(t.toString());

        QVariant b = m_gui->clicked_waypoint + 1;
        m_frame->comboBox_2->setCurrentIndex(m_gui->clicked_robot);
        m_frame->label_6->setText(b.toString());
        m_gui->clicked = 0;
        b              = m_gui->strategy[m_gui->current_strategy]
                ->robot[m_gui->clicked_robot]
                .waypoint[m_gui->clicked_waypoint]
                ->waypoint_type;
        m_frame->comboBox_4->setCurrentIndex(b.toInt());
        m_frame->comboBox_5->setCurrentIndex(m_gui->strategy[m_gui->current_strategy]
                                                 ->robot[m_gui->current_robot]
                                                 .waypoint[m_gui->current_waypoint]
                                                 ->need_rrt);
        m_frame->comboBox_7->setCurrentIndex(m_gui->strategy[m_gui->current_strategy]
                                                 ->robot[m_gui->current_robot]
                                                 .waypoint[m_gui->current_waypoint]
                                                 ->velocity_profile);
        b = m_gui->strategy[m_gui->current_strategy]
                ->robot[m_gui->current_robot]
                .waypoint[m_gui->current_waypoint]
                ->tolerance;
        m_frame->lineEdit_4->setText(b.toString());
        b = m_gui->strategy[m_gui->current_strategy]
                ->robot[m_gui->current_robot]
                .waypoint[m_gui->current_waypoint]
                ->time;
        m_frame->lineEdit_5->setText(b.toString());
        b = m_gui->strategy[m_gui->current_strategy]
                ->robot[m_gui->current_robot]
                .waypoint[m_gui->current_waypoint]
                ->speed;
        m_frame->lineEdit_6->setText(b.toString());

        b = m_gui->strategy[m_gui->current_strategy]->weight;
        m_frame->lineEdit_7->setText(b.toString());

        b = m_gui->strategy[m_gui->current_strategy]->getName();
        m_frame->lineEdit->setText(b.toString());

        b = m_gui->strategy[m_gui->current_strategy]->min_y;
        m_frame->lineEdit_10->setText(b.toString());

        b = m_gui->strategy[m_gui->current_strategy]->max_y;
        m_frame->lineEdit_11->setText(b.toString());

        m_frame->comboBox_6->setCurrentIndex(
            m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].final_role);

        // if(m_gui->current_robot == Robot::Attack)
        {
            b = m_gui->strategy[m_gui->current_strategy]
                    ->robot[m_gui->current_robot]
                    .waypoint[m_gui->current_waypoint]
                    ->tolerance;
            m_frame->comboBox_8->setCurrentIndex(b.toInt());
            b = m_gui->strategy[m_gui->current_strategy]
                    ->robot[m_gui->current_robot]
                    .waypoint[m_gui->current_waypoint]
                    ->waypoint_type;
            m_frame->comboBox_9->setCurrentIndex(b.toInt());
        }

        // m_frame->textEdit->clear();
        for (int i = 0; i < m_gui->strategy[m_gui->current_strategy]
                                ->robot[m_gui->clicked_robot]
                                .waypoint[m_gui->clicked_waypoint]
                                ->sync_waypoints.size();
             i++)
        {
            b = m_gui->strategy[m_gui->current_strategy]
                    ->robot[m_gui->clicked_robot]
                    .waypoint[m_gui->clicked_waypoint]
                    ->sync_waypoints.at(i) +
                1;
            //  m_frame->textEdit->append(b.toString());
            // m_frame->textEdit->moveCursor(QTextCursor::Down);
        }

        // if(m_gui->sync_data[m_gui->current_sync].point.size() != m_frame->listWidget->size())
        {
            // m_frame->listWidget->clear();
            for (int i = 0; i < m_gui->sync_data[m_gui->current_sync].point.size(); i++)
            {
                b = m_gui->sync_data[m_gui->current_sync].point.at(i);
                // m_frame->listWidget->addItem(b.toString());
            }
        }
    }
}

void Frame::paintEvent([[maybe_unused]] QPaintEvent *event)
{}

void Frame::dragEnterEvent([[maybe_unused]] QDragEnterEvent *event)
{}

void Frame::dragMoveEvent([[maybe_unused]] QDragMoveEvent *event)
{}

void Frame::dropEvent([[maybe_unused]] QDropEvent *event)
{}

void Frame::mousePressEvent([[maybe_unused]] QMouseEvent *event)
{}

void Frame::on_pushButton_clicked() // Add Strategy
{
    for (int i = 0; i < m_gui->strategy_counter; i++)
    {
        if (m_gui->strategy[i]->getName().compare(m_frame->lineEdit->text()) == 0)
        {
            return;
        }
    }
    m_frame->lineEdit_10->setText("-3000");
    m_frame->lineEdit_11->setText("3000");
    m_gui->strategy[m_gui->strategy_counter] = new Strategy();
    m_gui->strategy[m_gui->strategy_counter]->setName(m_frame->lineEdit->text());
    QVariant bb                                      = m_frame->lineEdit_8->text();
    m_gui->strategy[m_gui->strategy_counter]->min_x  = bb.toInt();
    bb                                               = m_frame->lineEdit_9->text();
    m_gui->strategy[m_gui->strategy_counter]->max_x  = bb.toInt();
    QVariant ee                                      = 0;
    bb                                               = m_frame->lineEdit_10->text();
    m_gui->strategy[m_gui->strategy_counter]->min_y  = bb.toInt();
    bb                                               = m_frame->lineEdit_11->text();
    m_gui->strategy[m_gui->strategy_counter]->max_y  = bb.toInt();
    m_gui->strategy[m_gui->current_strategy]->weight = 0;
    m_frame->comboBox_3->addItem(m_gui->strategy[m_gui->strategy_counter]->getName());
    m_gui->strategy_counter++;
    m_frame->comboBox_3->setCurrentIndex(m_gui->strategy_counter - 1);
    m_gui->current_strategy = m_frame->comboBox_3->currentIndex();
    m_gui->current_robot    = static_cast<Robot::Type>(m_frame->comboBox_2->currentIndex());
    m_gui->update();
    QVariant b = m_gui->strategy[m_gui->current_strategy]->getName();
    m_frame->lineEdit->setText(b.toString());
}
void Frame::on_pushButton_2_clicked()
{
    for (int i = 0; i < m_gui->strategy_counter; i++)
    {
        if (m_gui->strategy[i]->getName().compare(m_frame->comboBox_3->currentText()) == 0)
        {
            for (int y = 0; y < m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint_counter;
                 y++)
            {
                m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[y]->close();
            }
            for (int j = i; j < m_gui->strategy_counter - 1; j++)
            {
                m_gui->strategy[j] = m_gui->strategy[j + 1];
            }
            m_frame->comboBox_3->removeItem(i);
            m_gui->strategy_counter--;
        }
    }
    m_gui->current_strategy = m_frame->comboBox_3->currentIndex();
    m_gui->current_robot    = static_cast<Robot::Type>(m_frame->comboBox_2->currentIndex());
    m_gui->update();
    QVariant b = m_gui->strategy[m_gui->current_strategy]->getName();
    m_frame->lineEdit->setText(b.toString());
}

void Frame::on_comboBox_3_currentIndexChanged(int index)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint_counter; j++)
        {
            m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[j]->close();
        }
    }
    m_gui->current_strategy = index;
    m_gui->current_robot    = static_cast<Robot::Type>(m_frame->comboBox_2->currentIndex());
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint_counter; j++)
        {
            m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[j]->show();
        }
    }
    m_gui->update();
    QVariant b = m_gui->strategy[m_gui->current_strategy]->getName();
    m_frame->lineEdit->setText(b.toString());

    b = m_gui->strategy[m_gui->current_strategy]->weight;
    m_frame->lineEdit_7->setText(b.toString());

    b = m_gui->strategy[m_gui->current_strategy]->min_x;
    m_frame->lineEdit_8->setText(b.toString());

    b = m_gui->strategy[m_gui->current_strategy]->max_x;
    m_frame->lineEdit_9->setText(b.toString());

    b = m_gui->strategy[m_gui->current_strategy]->min_y;
    m_frame->lineEdit_10->setText(b.toString());

    b = m_gui->strategy[m_gui->current_strategy]->max_y;
    m_frame->lineEdit_11->setText(b.toString());

    if (m_gui->strategy_counter > 0)
    {
        if (m_gui->strategy[m_gui->current_strategy]
                ->robot[Robot::Attack]
                .waypoint[m_gui->current_waypoint]
                ->waypoint_type == 0)
            m_frame->label_8->setText("Kick");
        else
            m_frame->label_8->setText("Chip");
    }
    else
    {
        m_frame->label_8->setText("");
    }
}

void Frame::on_comboBox_2_currentIndexChanged([[maybe_unused]] int index)
{
    m_gui->current_robot = static_cast<Robot::Type>(m_frame->comboBox_2->currentIndex());
    m_frame->comboBox_6->setCurrentIndex(
        m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].final_role);
    if (m_gui->current_robot == Robot::Attack)
    {
        m_frame->comboBox_8->setEnabled(true);
        m_frame->comboBox_9->setEnabled(true);
    }
    else
    {
        m_frame->comboBox_8->setEnabled(false);
        m_frame->comboBox_9->setEnabled(false);
    }
}

void Frame::on_comboBox_4_currentIndexChanged(const int index)
{
    m_gui->strategy[m_gui->current_strategy]
        ->robot[m_gui->current_robot]
        .waypoint[m_gui->current_waypoint]
        ->waypoint_type = index;
}

// void Frame::on_lineEdit_2_returnPressed()
//{
//     QVariant b = m_frame->lineEdit_2->text();
//     int y =
//     m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint].y(); b =
//     ((b.toInt()+3000)*4.0/25) + 20;
//     m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint].move(b.toInt()-18,y);
//     m_gui->update();
//     m_gui->clicked=1;
//     this->update();
// }

// void Frame::on_lineEdit_3_returnPressed()
//{
//     QVariant b = m_frame->lineEdit_3->text();
//     int x =
//     m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint].x(); b =
//     ((b.toInt()+2000)*4.0/25) + 4;
//     m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint].move(x,b.toInt()-18);
//     m_gui->update();
//     m_gui->clicked=1;
//     this->update();
// }

void Frame::on_toolButton_clicked()
{
    QString  str = "Sync Num ";
    QVariant b   = (int) m_gui->sync_counter + 1;
    str.append(b.toString());
    // m_frame->comboBox_6->addItem(str);
    m_gui->sync_counter++;
}

// void Frame::on_toolButton_2_clicked()
//{
//     m_frame->comboBox_6->removeItem(m_gui->current_sync);
//     m_gui->current_sync--;
// }

void Frame::on_comboBox_6_currentIndexChanged(const int index)
{
    m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].final_role = index;
}

void Frame::on_toolButton_3_clicked()
{
    m_gui->sync_data[m_gui->current_sync].point.remove(m_gui->current_waypoint);
}

void Frame::on_toolButton_4_clicked()
{
    for (int i = 0; i < m_gui->sync_data[m_gui->current_sync].point.size(); i++)
    {
        if (((m_gui->current_robot + 1) * 100 + m_gui->current_waypoint + 1) ==
            m_gui->sync_data[m_gui->current_sync].point.at(i))
        {
            return;
        }
    }
    m_gui->sync_data[m_gui->current_sync].point.append((m_gui->current_robot + 1) * 100 + m_gui->current_waypoint + 1);
    m_gui->clicked = 1;
}

void Frame::on_pushButton_3_clicked()
{
    m_playBook.Clear();
    for (int i = 0; i < m_gui->strategy_counter; i++)
    {
        Common::logDebug("BOZ");
        Protos::Immortals::Strategy *strag = m_playBook.add_strategy();
        strag->set_name(m_gui->strategy[i]->getName().toStdString());
        strag->set_min_x(m_gui->strategy[i]->min_x);
        strag->set_max_x(m_gui->strategy[i]->max_x);
        strag->set_min_y(m_gui->strategy[i]->min_y);
        strag->set_max_y(m_gui->strategy[i]->max_y);
        m_playBook.add_weight(m_gui->strategy[i]->weight);
        for (int j = 0; j < 8; j++)
        {
            Protos::Immortals::Role *r = strag->add_role();
            r->set_id(j);
            r->set_name(m_gui->strategy[i]->robot[j].name.toStdString());
            r->set_afterlife(m_gui->strategy[i]->robot[j].final_role);
            for (int k = 0; k < m_gui->strategy[i]->robot[j].waypoint_counter; k++)
            {
                Protos::Immortals::Waypoint *way = r->add_path();
                int                          xx  = m_gui->strategy[i]->robot[j].waypoint[k]->x();
                int                          yy  = m_gui->strategy[i]->robot[j].waypoint[k]->y();
                xx                               = xx - 106; // UP LEFT CORNER OFFSET
                yy                               = yy - 45;  // UP LEFT CORNER OFFSET
                              //               qDebug()<<"OLD x : "<<xx*10<<" Y : "<<yy*10;
                xx = ((xx * 100) / 7) - 4971;   //(((xx+13))*2/3)-450;
                yy = ((yy * -450) / 31) + 4384; //(-((yy+13)*2/3))+300;
                                                //               qDebug()<<"NEW x : "<<xx<<" Y : "<<yy;

                way->set_x(xx);
                way->set_y(yy);
                way->set_type(
                    static_cast<Protos::Immortals::Waypoint_Type>(
                    m_gui->strategy[i]->robot[j].waypoint[k]->waypoint_type));
                way->set_need_rrt(m_gui->strategy[i]->robot[j].waypoint[k]->need_rrt);
                way->set_speed(m_gui->strategy[i]->robot[j].waypoint[k]->speed);
                way->set_velocity_profile(static_cast<Protos::Immortals::Waypoint_VelocityProfile>(
                    m_gui->strategy[i]->robot[j].waypoint[k]->velocity_profile));
                way->set_tolerance(m_gui->strategy[i]->robot[j].waypoint[k]->tolerance);
                way->set_time(m_gui->strategy[i]->robot[j].waypoint[k]->time);
            }
        }
    }

    std::ofstream file(m_file_path, std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        QMessageBox msg;
        msg.setText("Can't Write to File !!! ");
        msg.exec();
    }
    std::string str;
    m_playBook.SerializeToString(&str);
    file.clear();
    file.write(str.data(), str.length());
    file.close();
    m_udp->send(m_playBook, Common::setting().strategy_address);
}

void Frame::on_pushButton_4_clicked()
{
    std::ifstream::pos_type size;
    char                   *memblock;
    m_file_path = std::filesystem::path{DATA_DIR} / "strategy.ims";
    std::ifstream file(m_file_path, std::ios::in | std::ios::binary);
    if (file.is_open())
    {
        file.seekg(0, std::ios::end);
        size     = file.tellg();
        memblock = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(memblock, size);
        m_playBook.ParseFromArray(memblock, size);
        for (int i = 0; i < m_gui->strategy_counter; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int k = 0; k < m_gui->strategy[i]->robot[j].waypoint_counter; k++)
                {
                    m_gui->strategy[i]->robot[j].waypoint[k]->close();
                }
            }
        }
        m_gui->strategy_counter = m_playBook.strategy_size();
        for (int i = 0; i < m_playBook.strategy_size(); i++)
        {
            m_gui->strategy[i] = new Strategy();
            QString r          = m_playBook.strategy(i).name().c_str();
            m_gui->strategy[i]->setName(r);
            m_gui->strategy[i]->min_x  = m_playBook.strategy(i).min_x();
            m_gui->strategy[i]->max_x  = m_playBook.strategy(i).max_x();
            m_gui->strategy[i]->min_y  = m_playBook.strategy(i).min_y();
            m_gui->strategy[i]->max_y  = m_playBook.strategy(i).max_y();
            m_gui->strategy[i]->weight = m_playBook.weight(i);
            for (int j = 0; j < m_playBook.strategy(i).role_size(); j++)
            {
                m_gui->strategy[i]->robot[j].waypoint_counter = m_playBook.strategy(i).role(j).path_size();
                m_gui->strategy[i]->robot[j].final_role       = m_playBook.strategy(i).role(j).afterlife();
                for (int k = 0; k < m_playBook.strategy(i).role(j).path_size(); k++)
                {
                    int x =
                        (((m_playBook.strategy(i).role(j).path(k).x() / 1.0) + 4971) * 7) / 100; //((xx * 100)/7) - 4971
                    int y =
                        (((m_playBook.strategy(i).role(j).path(k).y() / 1.0) - 4384) * 31) / -450; //((-+300)*3.0/2)-13;

                    Common::logDebug("KHAR");
                    m_gui->strategy[i]->robot[j].waypoint[k]->move(x + 106, y + 45);
                    m_gui->strategy[i]->robot[j].waypoint[k]->setParent(m_gui.get());
                    m_gui->strategy[i]->robot[j].waypoint[k]->waypoint_type =
                        m_playBook.strategy(i).role(j).path(k).type();
                    m_gui->strategy[i]->robot[j].waypoint[k]->need_rrt =
                        m_playBook.strategy(i).role(j).path(k).need_rrt();
                    m_gui->strategy[i]->robot[j].waypoint[k]->speed = m_playBook.strategy(i).role(j).path(k).speed();
                    m_gui->strategy[i]->robot[j].waypoint[k]->velocity_profile =
                        m_playBook.strategy(i).role(j).path(k).velocity_profile();
                    m_gui->strategy[i]->robot[j].waypoint[k]->tolerance =
                        m_playBook.strategy(i).role(j).path(k).tolerance();
                    m_gui->strategy[i]->robot[j].waypoint[k]->time = m_playBook.strategy(i).role(j).path(k).time();
                    QString  str                                   = QString(DATA_DIR) + "/strategy-images/";
                    QVariant b                                     = j + 1;
                    str.append(b.toString());
                    str.append("/");
                    b = k + 1;
                    str.append(b.toString());
                    str.append(".png");
                    m_gui->strategy[i]->robot[j].waypoint[k]->setPixmap(str);
                }
            }
        }

        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < m_gui->strategy[m_gui->current_strategy]->robot[j].waypoint_counter; k++)
            {
                m_gui->strategy[m_gui->current_strategy]->robot[j].waypoint[k]->show();
            }
        }
        m_gui->clicked          = 1;
        m_gui->current_robot    = Robot::GK;
        m_gui->current_strategy = 0;
        m_gui->current_waypoint = 0;
        m_frame->lineEdit->setText(m_gui->strategy[m_gui->current_strategy]->getName());
        m_frame->comboBox_3->setCurrentIndex(m_gui->current_strategy);
        for (int i = 0; i < m_gui->strategy_counter; i++)
        {
            m_frame->comboBox_3->addItem(m_gui->strategy[i]->getName());
        }
        m_gui->update();
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

void Frame::on_comboBox_7_currentIndexChanged(const int index)
{
    m_gui->strategy[m_gui->current_strategy]
        ->robot[m_gui->current_robot]
        .waypoint[m_gui->current_waypoint]
        ->velocity_profile = index;
}

void Frame::on_lineEdit_6_returnPressed()
{
    QVariant b = m_frame->lineEdit_6->text();
    m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint]->speed =
        b.toFloat();
}

void Frame::on_lineEdit_5_returnPressed()
{
    QVariant b = m_frame->lineEdit_5->text();
    m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint]->time =
        b.toFloat();
}

void Frame::on_lineEdit_4_returnPressed()
{
    QVariant b = m_frame->lineEdit_4->text();
    m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint]->tolerance =
        b.toFloat();
}

void Frame::on_comboBox_5_currentIndexChanged(const int index)
{
    if (index == 1)
        m_gui->strategy[m_gui->current_strategy]
            ->robot[m_gui->current_robot]
            .waypoint[m_gui->current_waypoint]
            ->need_rrt = true;
    else if (index == 0)
        m_gui->strategy[m_gui->current_strategy]
            ->robot[m_gui->current_robot]
            .waypoint[m_gui->current_waypoint]
            ->need_rrt = false;
}

void Frame::on_lineEdit_7_returnPressed()
{
    QVariant b = m_frame->lineEdit_7->text();
    if (m_gui->strategy_counter > 0)
        m_gui->strategy[m_gui->current_strategy]->weight = b.toInt();
}

void Frame::on_comboBox_8_currentIndexChanged(const int index)
{
    if (m_gui->current_robot == Robot::Attack && m_gui->strategy_counter > 0)
    {
        m_gui->strategy[m_gui->current_strategy]
            ->robot[m_gui->current_robot]
            .waypoint[m_gui->current_waypoint]
            ->tolerance = index;
        // QVariant t =index;
        // m_frame->lineEdit_4->setText(t.toString());
    }
}

void Frame::on_comboBox_9_currentIndexChanged(const int index)
{
    if (m_gui->current_robot == Robot::Attack && m_gui->strategy_counter > 0)
    {
        m_gui->strategy[m_gui->current_strategy]
            ->robot[m_gui->current_robot]
            .waypoint[m_gui->current_waypoint]
            ->waypoint_type = index;
    }
}

void Frame::on_lineEdit_4_selectionChanged()
{
    QVariant b = m_frame->lineEdit_4->text();
    if (m_gui->strategy_counter > 0)
        m_gui->strategy[m_gui->current_strategy]
            ->robot[m_gui->current_robot]
            .waypoint[m_gui->current_waypoint]
            ->tolerance = b.toFloat();
}

void Frame::on_lineEdit_5_selectionChanged()
{
    QVariant t = m_frame->lineEdit_5->text();
    if (m_gui->strategy_counter > 0)
        m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint]->time =
            t.toFloat();
}

void Frame::on_lineEdit_6_selectionChanged()
{
    QVariant b = m_frame->lineEdit_6->text();
    if (m_gui->strategy_counter > 0)
        m_gui->strategy[m_gui->current_strategy]->robot[m_gui->current_robot].waypoint[m_gui->current_waypoint]->speed =
            b.toFloat();
}

void Frame::on_lineEdit_7_selectionChanged()
{
    QVariant b = m_frame->lineEdit_7->text();
    if (m_gui->strategy_counter > 0)
        m_gui->strategy[m_gui->current_strategy]->weight = b.toInt();
}

void Frame::on_pushButton_5_clicked()
{
    for (int i = 0; i < m_gui->strategy_counter; i++)
    {
        if (m_gui->strategy[i]->getName().compare(m_frame->lineEdit->text() + "_Dup") == 0)
        {
            return;
        }
    }
    m_gui->strategy[m_gui->strategy_counter] = new Strategy();
    m_gui->strategy[m_gui->strategy_counter]->setName(m_frame->lineEdit->text() + "_Dup");
    m_gui->strategy[m_gui->strategy_counter]->min_x  = m_gui->strategy[m_gui->current_strategy]->min_x;
    m_gui->strategy[m_gui->strategy_counter]->max_x  = m_gui->strategy[m_gui->current_strategy]->max_x;
    m_gui->strategy[m_gui->strategy_counter]->min_y  = m_gui->strategy[m_gui->current_strategy]->min_y;
    m_gui->strategy[m_gui->strategy_counter]->max_y  = m_gui->strategy[m_gui->current_strategy]->max_y;
    QVariant ee                                      = m_frame->lineEdit_7->text();
    m_gui->strategy[m_gui->strategy_counter]->weight = ee.toInt();
    m_frame->comboBox_3->addItem(m_gui->strategy[m_gui->strategy_counter]->getName());

    for (int i = 0; i < 8; i++)
    {
        m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint_counter =
            m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint_counter;
        m_gui->strategy[m_gui->strategy_counter]->robot[i].final_role =
            m_gui->strategy[m_gui->current_strategy]->robot[i].final_role;
        for (int k = 0; k < m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint_counter; k++)
        {
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->move(
                m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[k]->pos().x(),
                m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[k]->pos().y());
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->waypoint_type =
                (m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[k]->waypoint_type);
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->speed =
                (m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[k]->speed);
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->tolerance =
                (m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[k]->tolerance);
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->need_rrt =
                (m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[k]->need_rrt);
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->time =
                (m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[k]->time);
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->velocity_profile =
                (m_gui->strategy[m_gui->current_strategy]->robot[i].waypoint[k]->velocity_profile);
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
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->setPixmap(str);
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->setParent(m_gui.get());
            m_gui->strategy[m_gui->strategy_counter]->robot[i].waypoint[k]->show();
        }
    }
    m_frame->comboBox_3->setCurrentIndex(m_gui->strategy_counter);
    m_gui->current_strategy = m_frame->comboBox_3->currentIndex();
    m_gui->current_robot    = static_cast<Robot::Type>(m_frame->comboBox_2->currentIndex());
    m_gui->update();
    QVariant b = m_gui->strategy[m_gui->current_strategy]->getName();
    m_frame->lineEdit->setText(b.toString());
    m_gui->strategy_counter++;
}

void Frame::on_pushButton_6_clicked()
{
    // UDPSocket *
}

void Frame::on_lineEdit_8_returnPressed()
{
    QVariant b                                      = m_frame->lineEdit_8->text();
    m_gui->strategy[m_gui->current_strategy]->min_x = b.toInt();
}

void Frame::on_lineEdit_9_returnPressed()
{
    QVariant b                                      = m_frame->lineEdit_9->text();
    m_gui->strategy[m_gui->current_strategy]->max_x = b.toInt();
}

void Frame::on_pushButton_7_clicked()
{
    m_gui->strategy[m_gui->current_strategy]->setName(m_frame->lineEdit->text());
    m_frame->comboBox_3->setItemText(m_frame->comboBox_3->currentIndex(), m_frame->lineEdit->text());
}

void Frame::on_lineEdit_10_editingFinished()
{
    QVariant b                                      = m_frame->lineEdit_10->text();
    m_gui->strategy[m_gui->current_strategy]->min_y = b.toInt();
}

void Frame::on_lineEdit_11_editingFinished()
{
    QVariant b                                      = m_frame->lineEdit_11->text();
    m_gui->strategy[m_gui->current_strategy]->max_y = b.toInt();
}

void Frame::on_lineEdit_8_editingFinished()
{
    QVariant b                                      = m_frame->lineEdit_8->text();
    m_gui->strategy[m_gui->current_strategy]->min_x = b.toInt();
}

void Frame::on_lineEdit_9_editingFinished()
{
    QVariant b                                      = m_frame->lineEdit_9->text();
    m_gui->strategy[m_gui->current_strategy]->max_x = b.toInt();
}

void Frame::on_lineEdit_4_editingFinished()
{
    QVariant b = m_frame->lineEdit_4->text();
    if (m_gui->strategy_counter > 0)
        m_gui->strategy[m_gui->current_strategy]
            ->robot[m_gui->current_robot]
            .waypoint[m_gui->current_waypoint]
            ->tolerance = b.toInt();
}
} // namespace Tyr::Strategy
