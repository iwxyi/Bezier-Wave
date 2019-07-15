贝塞尔曲线波浪
===

## 安装与运行环境

- 语言：C++
- 框架：Qt 11.3
- 平台：Windows

## 截图

![截图](pictures/picture.gif)

## 算法

将屏幕水平平均分为10块，在一定范围内随机高度的12个点（左右出头），通过贝塞尔曲线连接连续的点，即可绘制成一段段连续的波浪。

但是每两端波浪都会有折线凸起，根据贝塞尔曲线的原理，两个控制点中点作为新的控制点，原来的两点连线即为该控制点在新曲线上的切线，这样既可绘制成只由三个控制点影响的`B样条`。

为了逼真，通过时钟以及大量状态机确定随机范围，例如在某一段时间整体偏下方，某一段时间整体偏上方，模仿大自然水面潮涨潮落。

## 用法

创建：

```C++
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    bw1 = new BezierWaveBean(this);
    bw1->set_offsety(geometry().height()/20); // 垂直偏移位置【可选】
    bw1->set_speedx(4); // 设置速度
    bw1->start();
}
```

绘制：

```C++
void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿

    painter.fillPath(bw1->getPainterPath(painter), QColor(255, 0, 0, 50));

    return QMainWindow::paintEvent(e);
}
```

尺寸：

```C++
void MainWindow::resizeEvent(QResizeEvent *e)
{
    bw1->set_rect(geometry());
    return QMainWindow::resizeEvent(e);
}
```

开关：

```C++
bw1->start();  // 开始
bw1->pause();  // 暂停
bw1->resume(); // 继续
```

