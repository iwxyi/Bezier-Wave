#ifndef BEZIERWAVEBEAN_H
#define BEZIERWAVEBEAN_H

#include <QObject>
#include <QWidget>
#include <QPaintEvent>
#include <QList>
#include <QPainter>
#include <random>
#include <QTimer>
#include <QDebug>

class BezierWaveBean : public QObject
{
    Q_OBJECT
public:
    BezierWaveBean(QWidget *parent);

    void start();

    QPainterPath getPainterPath();

protected:
    int getRandomHeight();

signals:

public slots:
    void slotUpdatePositions();
    void slotMovePositions();
    void slotSetOffset();

protected:
    QWidget* target;

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

#endif // BEZIERWAVEBEAN_H
