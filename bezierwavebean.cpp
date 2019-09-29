#include "bezierwavebean.h"

BezierWaveBean::BezierWaveBean(QWidget* parent) : QObject(parent), target(parent), mt(rd())
{
    srand(static_cast<unsigned int>(time(nullptr)));

    count = 8; // 点的总数
    inter = target->geometry().width()/(count-4);
    appear_speedy = 3; // 出现时的最大速度

//    speedy = appear_speedy; // y移动速度
    offsety = 0; // y累计偏移
    offsety_direct = -1; // y短期偏移方向，-1上 和 1下
    speedx = 3; // x移动速度
    offsetx = 0; // x累计偏移

    speedy_step = 3;
    _offsety = 0;
    _max_offsety = target->geometry().height()/10; // 上下最大偏移的位置（注意：效果是正负翻倍的）
    _rect = target->geometry();
    running = false;

    // 更新目标点的时钟：1.5秒钟左右
    update_timer = new QTimer(target);
    update_timer->setInterval(1000);
    connect(update_timer, SIGNAL(timeout()), this, SLOT(slotUpdateAims())); // 每隔一段时间更新一下位置

    // 移动波浪的时钟：40毫秒
    move_timer = new QTimer(this);
    move_timer->setInterval(getRandom(35, 45));
    connect(move_timer, SIGNAL(timeout()), this, SLOT(slotMovePoints()));

    // y偏移波浪的时钟：4秒左右
    offset_timer = new QTimer(this);
    offset_timer->setInterval(4000);
    connect(offset_timer, SIGNAL(timeout()), this, SLOT(slotSetOffset()));

    // 慢慢暂停的时钟
    pause_timer = new QTimer(this);
    pause_timer->setInterval(3000);
    connect(pause_timer, &QTimer::timeout, [=]{
        if (!running)
        {
            update_timer->stop();
            move_timer->stop();
            offset_timer->stop();

            for(int i = 0; i < keys.length(); i++)
                keys[i].setY(target->geometry().height());
            offsety = -_max_offsety/2; // 清空偏移（消失时默认都在最下面的，现在恢复）
        }
    });
}

void BezierWaveBean::start()
{
    // 初始化一个值
    aim_keys.clear();
    keys.clear();
    speedys.clear();
    running = true;
    for (int i = 0; i < count; i++)
        aim_keys.append(QPoint(inter*(i-2), getRandomHeight()));
    for (int i = 0; i < aim_keys.length(); i++)
        keys.append(QPoint(aim_keys.at(i).x(), target->geometry().height()));
    for (int i = 0; i < aim_keys.length(); i++)
        speedys.append(- getRandom(appear_speedy * speedy_step / 2, appear_speedy * speedy_step));
    qint64 timestamp = getTimestamp();
    for (int  i = 0; i < aim_keys.length(); i++)
        aim_updates_timestamp.append(timestamp);

    update_timer->start();
    move_timer->start();
    offset_timer->start();

}

void BezierWaveBean::resume()
{
    if (keys.length() == 0)
        return start();
    if (running) return ; // 正在动画中，没必要
    update_timer->start();
    move_timer->start();
    offset_timer->start();
    pause_timer->stop();
    running = true;
    slotUpdateAims(); // 立即更新目标
    for (int i = 0; i < speedys.length(); i++)
        speedys[i] = appear_speedy * speedy_step;
}

void BezierWaveBean::pause()
{
    running = false;
    for (int i = 0; i < aim_keys.length(); i++)
        aim_keys[i].setY(getRandomHeight());
    pause_timer->start();
}

void BezierWaveBean::set_count(int x)
{
    if (x > 5)
        count = x;
}

void BezierWaveBean::set_offsety(int x)
{
    _offsety = x;
}

void BezierWaveBean::set_speedx(int x)
{
    speedx = x;
}

void BezierWaveBean::set_rect(QRect rect)
{
    int delta = rect.height() - _rect.height();
    inter = rect.width() / (count - 4);
    for (int i = 0; i < keys.length(); i++)
    {
        // 修改所有点的间隔
        //keys[i].setX(keys.at(i).x() * width / _rect.width()); // 按比例缩放……
        keys[i].setX(inter*(i-2)+offsetx);
        aim_keys[i].setX(keys.at(i).x());
        // 修改整体的高度
        keys[i].setY(keys.at(i).y()+delta);
        aim_keys[i].setY(aim_keys.at(i).y()+delta);
    }
    double time = pow(rect.width(), 1.0 / 5); // 代表一个点显示几秒钟
    speedx = rect.width() / (1000 / move_timer->interval()) / time;
    _rect = rect;
}

QPainterPath BezierWaveBean::getPainterPath(QPainter& painter)
{
    Q_UNUSED(painter)

    // 从关键点生成绘图点
    QList<QPoint>pots;
    for (int i = 0; i < keys.length(); i++)
    {
        if (i & 1) // 奇数
        {
            int x = (keys.at(i-1).x()+keys.at(i).x())/2;
            int y = (keys.at(i-1).y()+keys.at(i).y())/2;
            pots.append(QPoint(x, y));
            pots.append(keys.at(i));
        }
        else // 偶数
            pots.append(keys.at(i));
    }

    // y方向的偏移
    for (int i = 0; i < pots.length(); i++)
    {
        pots[i].setY(pots[i].y()+offsety);
    }

    // 画进行判断的点
    /*for (int i = 0; i < pots.length(); i++)
    {
        QPoint p = pots.at(i);
        painter.drawEllipse(QRect(p.x()-2, p.y()-2, 4, 4));
    }*/

    // 开始画图
    QPainterPath bezier;
    bezier.moveTo(0, target->geometry().height());
    bezier.lineTo(pots.at(1));
    //painter.drawLine(pots.at(0), pots.at(1));
    for (int i = 2; i+2 < pots.count(); i+=3)
    {
        // 画切线线条
        //painter.drawLine(pots.at(i-1), pots.at(i));
        //painter.drawLine(pots.at(i+1), pots.at(i+2));

        // 画三阶贝塞尔曲线
        bezier.cubicTo(pots.at(i), pots.at(i+1), pots.at(i+2));
    }
    bezier.lineTo(target->geometry().bottomRight());

    return bezier;
}


int BezierWaveBean::getRandomHeight()
{
    if (running)
    {
        int y = getRandom(target->geometry().height() / 2, target->geometry().height() * 7 / 8);
        return y;
    }
    else
        return int(target->geometry().height()*1.5);
}

int BezierWaveBean::getRandom(int min, int max)
{
#if defined (Q_OS_WIN) || defined(Q_OS_LINUX)
    return mt() % (max-min+1) + min;
#elif defined(Q_OS_MAC)

#else
    return rand() % (max-min+1) + min;
#endif
}

qint64 BezierWaveBean::getTimestamp()
{
    return QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void BezierWaveBean::slotUpdateAims()
{
    if (!running) return ;
    /*for (int i = 0; i < speedys.length(); i++)
        if (speedys.at(i) == appear_speedy*speedy_step)
            speedys[i] = 1;*/
    // 生成随机的目标关键点
    int delta = target->geometry().height()/4;
    qint64 timestamp = getTimestamp();
    for (int i = 0; i < aim_keys.length(); i++)
    {
        if (aim_updates_timestamp.at(i) + 3000 > timestamp) // 3秒更新一次
            continue;
        int y = aim_keys.at(i).y() + getRandom(-delta, delta);
        if (y < target->geometry().height() / 2)
            y = target->geometry().height() / 2;
        else if (y > target->geometry().height()*7/8)
            y = target->geometry().height()*7/8;
        aim_keys[i].setY(y + _offsety);
        aim_updates_timestamp[i] = timestamp;
    }
}

void BezierWaveBean::slotMovePoints()
{
    // Y方向慢慢移动当前关键点到目标关键点
    for (int i = 0; i < keys.length(); i++)
    {
        QPoint& cur = keys[i];
        QPoint aim = aim_keys[i];
        int del = aim.y()-cur.y();
        if (del > 0) // 应当往下移动
        {
            speedys[i]++; // 向下的加速度
//            speedys[i] += sqrt(del)/speedx/speedy_step+1;
            /*if (speedys.at(i)/speedy_step + abs(del)/100 < del)
                cur.setY(cur.y()+speedys.at(i)/speedy_step+del/100);
            else
                cur.setY(cur.y()+del);*/
        }
        else if (del < 0) // 应当往上移动
        {
            speedys[i]--; // 向上的加速度/
//            speedys[i] -= sqrt(-del)/speedx/speedy_step+1;
            /*if (speedys.at(i)/speedy_step+abs(del)/50 < -del)
                cur.setY(cur.y()-speedys.at(i)/speedy_step+del/50);
            else
                cur.setY(cur.y()+del);*/
        }
        cur.setY(cur.y()+speedys.at(i)/speedy_step);
//        qDebug() << sqrt(abs(del))/speedx/speedy_step+1;
    }
//    qDebug() << aim_keys[3].y() << keys[3].y() << speedys[3];

    // 每次绘图，更新一次Y方向的整体偏移量
    if (offsety+offsety_direct > -_max_offsety && offsety+offsety_direct < _max_offsety)
        offsety += offsety_direct;
    else if (offsety <= -_max_offsety)
        offsety = -_max_offsety;
    else if (offsety >= _max_offsety)
        offsety = _max_offsety;

    // x轴方向的偏移
    for (int i = 0; i < keys.length(); i++)
    {
        keys[i].setX(keys[i].x()+speedx);
        aim_keys[i].setX(keys[i].x());
    }
    offsetx += speedx;

    // 判断需不需要把右边移动到左边去
    if (offsetx >= inter)
    {
        qint64 timestamp = getTimestamp();
        QPoint p1(keys[0].x()-inter*2, getRandomHeight());
        QPoint p2(keys[0].x()-inter, getRandomHeight());
        keys.insert(0, p2);
        keys.insert(0, p1);
        aim_keys.insert(0, QPoint(p2));
        aim_keys.insert(0, QPoint(p1));
        speedys.insert(0, speedys.at(speedys.length()-1));
        speedys.insert(0, speedys.at(speedys.length()-2));
        aim_updates_timestamp.insert(0, timestamp);
        aim_updates_timestamp.insert(0, timestamp);

        offsetx -= inter*2;

        if (keys.length() > count+2)
        {
            keys.removeLast();
            keys.removeLast();
            aim_keys.removeLast();
            aim_keys.removeLast();
            speedys.removeLast();
            speedys.removeLast();
            aim_updates_timestamp.removeLast();
            aim_updates_timestamp.removeLast();
        }
    }

    // 更新当前界面
    target->update();

    /*QString ans = "";
    for (int i = 5; i < 8; i++)
        ans += " (" + QString::number(keys.at(i).y()) + ","+QString::number(keys.at(i).y())+")"+QString::number(speedys.at(i));
    qDebug() << ans;*/
}

void BezierWaveBean::slotSetOffset()
{
    if (!running)
    {
        offsety_direct = 1;
        return ;
    }
    // 每隔一段时间，稍微修改波浪的上下偏移位置
    if (getRandom(0,1))
        offsety_direct = 1;
    else
        offsety_direct = -1;
}
