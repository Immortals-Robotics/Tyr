#pragma once

#include "gui.h"

namespace Ui
{
class Frame;
}

namespace Tyr::Strategy
{
class Frame : public QMainWindow
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = 0);
    ~Frame();
    gui                        *g;
    Protos::Immortals::PlayBook playBook;
    Common::UdpServer          *udp;

    QPainter paint;

    std::string file_path;

protected:
    void   paintEvent(QPaintEvent *event);
    void   dragEnterEvent(QDragEnterEvent *event);
    void   dragMoveEvent(QDragMoveEvent *event);
    void   dropEvent(QDropEvent *event);
    void   mousePressEvent(QMouseEvent *event);
    QTimer timer;

private:
    Ui::Frame *ui;

private slots:
    void on_lineEdit_4_editingFinished();
    void on_lineEdit_9_editingFinished();
    void on_lineEdit_8_editingFinished();
    void on_lineEdit_11_editingFinished();
    void on_lineEdit_10_editingFinished();
    void on_pushButton_7_clicked();
    void on_lineEdit_9_returnPressed();
    void on_lineEdit_8_returnPressed();
    void on_pushButton_6_clicked();
    void on_pushButton_5_clicked();
    void on_lineEdit_7_selectionChanged();
    void on_lineEdit_6_selectionChanged();
    void on_lineEdit_5_selectionChanged();
    void on_lineEdit_4_selectionChanged();
    void on_comboBox_9_currentIndexChanged(int index);
    void on_comboBox_8_currentIndexChanged(int index);
    void on_lineEdit_7_returnPressed();
    void on_comboBox_5_currentIndexChanged(int index);
    void on_lineEdit_4_returnPressed();
    void on_lineEdit_5_returnPressed();
    void on_lineEdit_6_returnPressed();
    void on_comboBox_7_currentIndexChanged(int index);
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_toolButton_4_clicked();
    void on_toolButton_3_clicked();
    void on_comboBox_6_currentIndexChanged(int index);
    void on_toolButton_clicked();
    void on_comboBox_4_currentIndexChanged(int index);
    void on_comboBox_2_currentIndexChanged(int index);
    void on_comboBox_3_currentIndexChanged(int index);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void update();
};
} // namespace Tyr::Strategy
