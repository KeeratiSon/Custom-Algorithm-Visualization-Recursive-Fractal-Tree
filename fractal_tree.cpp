#include "fractal_tree.h"

#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QApplication>
#include <QRandomGenerator>
#include <cmath>

// Each recursive split shrinks the branch length by this factor, and
// spreads the two children this many degrees apart from the parent's
// direction.
static const double SHRINK_FACTOR = 0.85;
static const double SPREAD_DEGREES = 25.0;

FractalTree::FractalTree(QWidget *parent)
    : QWidget(parent), state(Growing), currentDepth(0),
      pauseTicksRemaining(pauseTicksTotal) {

    setWindowState(Qt::WindowFullScreen);
    setStyleSheet("background-color: black;");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FractalTree::advanceAnimation);
    timer->start(600); // milliseconds between animation steps
}

void FractalTree::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        QApplication::quit();
    }
}

void FractalTree::advanceAnimation() {
    // A small state machine driving the grow -> pause -> degrow -> pause
    // -> grow cycle. Each state decides what currentDepth does this tick
    // and when to transition to the next state.
    switch (state) {
        case Growing:
            currentDepth++;
            if (currentDepth >= maxDepth) {
                currentDepth = maxDepth;
                state = PausedFull;
                pauseTicksRemaining = pauseTicksTotal;
            }
            break;

        case PausedFull:
            pauseTicksRemaining--;
            if (pauseTicksRemaining <= 0) {
                state = Degrowing;
            }
            break;

        case Degrowing:
            // Decreasing currentDepth means drawBranch's "level >
            // currentDepth -> skip" check now excludes the deepest
            // (outermost) branches first, so the tree visibly shrinks
            // back from the tips toward the trunk.
            currentDepth--;
            if (currentDepth <= 0) {
                currentDepth = 0;
                state = PausedEmpty;
                pauseTicksRemaining = pauseTicksTotal;
            }
            break;

        case PausedEmpty:
            pauseTicksRemaining--;
            if (pauseTicksRemaining <= 0) {
                state = Growing;
            }
            break;
    }

    update(); // schedules a call to paintEvent()
}

void FractalTree::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    double startX = width() / 2.0;
    double startY = height();
    double startAngle = -90.0; // straight up
    double trunkLength = height() / 5;

    drawBranch(painter, startX, startY, startAngle, trunkLength, /*level=*/0);
}

void FractalTree::drawBranch(QPainter &painter, double x, double y,
                              double angle, double length, int level) {
    if (level > currentDepth || length < 2.0) {
        return;
    }

    double radians = angle * M_PI / 180.0;
    double endX = x + length * std::cos(radians);
    double endY = y + length * std::sin(radians);

    double depthRatio = static_cast<double>(level) / maxDepth;
    int brownness = static_cast<int>(150 * (1.0 - depthRatio));
    int greenness = static_cast<int>(80 + 150 * depthRatio);
    QColor branchColor(brownness, greenness, 40);

    double penWidth = std::max(1.0, 24.0 * (1.0 - depthRatio));
    painter.setPen(QPen(branchColor, penWidth));
    painter.drawLine(QPointF(x, y), QPointF(endX, endY));

    double jitter = QRandomGenerator::global()->bounded(-6, 6); // degrees

    drawBranch(painter, endX, endY,
               angle - SPREAD_DEGREES + jitter,
               length * SHRINK_FACTOR,
               level + 1);

    drawBranch(painter, endX, endY,
               angle + SPREAD_DEGREES + jitter,
               length * SHRINK_FACTOR,
               level + 1);
}
