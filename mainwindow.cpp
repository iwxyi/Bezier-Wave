#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    bw1(nullptr), bw2(nullptr), bw3(nullptr), bw4(nullptr)
{
    ui->setupUi(this);

    bw1 = new BezierWaveBean(this);
    bw1->start();

    bw2 = new BezierWaveBean(this);
    bw2->start();

    bw3 = new BezierWaveBean(this);
    bw3->start();

    bw4 = new BezierWaveBean(this);
    bw4->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    // 开始画图，设置painter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::black);

    // 画进行判断的点
    /*for (int i = 0; i < pots.length(); i++)
    {
        QPoint p = pots.at(i);
        painter.drawEllipse(QRect(p.x()-2, p.y()-2, 4, 4));
    }*/

    QPainterPath bezier;

    if (bw1 != nullptr)
        bezier = bw1->getPainterPath();
    painter.fillPath(bezier, QBrush(QColor(255, 0, 0, 50)));

    if (bw2 != nullptr)
        bezier = bw2->getPainterPath();
    painter.fillPath(bezier, QBrush(QColor(255, 0, 0, 50)));

    if (bw3 != nullptr)
        bezier = bw3->getPainterPath();
    painter.fillPath(bezier, QBrush(QColor(255, 0, 0, 50)));

    if (bw4 != nullptr)
        bezier = bw4->getPainterPath();
    painter.fillPath(bezier, QBrush(QColor(255, 0, 0, 50)));

    return QMainWindow::paintEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    /*inter = geometry().width() / (count - 4);
    // 调整所有点的横坐标
    for (int i = 0; i < keys.length(); i++)
    {
        int x = inter*(i-2);
        keys[i].setX(x);
        aim_keys[i].setX(x);
    }*/

    return QMainWindow::resizeEvent(e);
}
