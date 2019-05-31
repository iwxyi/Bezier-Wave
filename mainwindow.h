#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QList>
#include <QPainter>
#include <random>
#include <QPropertyAnimation>
#include <QTimer>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    int getRandomHeight();

public slots:
    void slotUpdatePositions();
    void slotMovePositions();
    void slotSetOffset();

private:
    Ui::MainWindow *ui;
    QTimer* update_timer;
    QTimer* move_timer;
    QTimer* offset_timer;

    int count;
    int inter;
    int def_speed1;

    int speedy, offsety;
    int offsety_direct;
    int speedx, offsetx;

    QList<QPoint>keys;
    QList<QPoint>aim_keys;
};

#endif // MAINWINDOW_H
