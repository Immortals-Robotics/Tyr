#pragma once

#include "gui.h"

namespace Tyr::Strategy
{
class Frame final : public QMainWindow
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = nullptr);

    ~Frame() override = default;

protected:
    void paintEvent(QPaintEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    std::unique_ptr<Ui::Frame> m_frame;
    std::unique_ptr<Gui>       m_gui;
    QTimer                     m_timer;

    std::unique_ptr<Common::UdpServer> m_udp;

    Protos::Immortals::PlayBook m_playBook;

    std::filesystem::path m_file_path;

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
