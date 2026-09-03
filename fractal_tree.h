#ifndef FRACTALTREE_H
#define FRACTALTREE_H

#include <QWidget>

// FractalTree is a Qt5 fullscreen widget that visualizes recursive drawing:
// a "trunk" splits into two shorter branches, each of which splits again,
// and so on, forming a tree-shaped fractal.
//
// The tree cycles through four animation states:
//   Growing     -> currentDepth increases, revealing one more recursion
//                  level each tick, until it reaches maxDepth.
//   PausedFull  -> holds the fully-grown tree on screen for a moment.
//   Degrowing   -> currentDepth decreases, which (because drawBranch only
//                  draws branches at or below currentDepth) makes the
//                  outermost twigs disappear first, working back toward
//                  the trunk -- the "degrow" effect.
//   PausedEmpty -> holds the bare trunk for a moment before growing again.
class FractalTree : public QWidget {
    Q_OBJECT

public:
    FractalTree(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // Advances the animation by one step. What that step does depends on
    // the current AnimState -- see the state machine in the .cpp file.
    void advanceAnimation();

private:
    enum AnimState { Growing, PausedFull, Degrowing, PausedEmpty };
    AnimState state;

    // How many recursion levels are currently being drawn (0..maxDepth).
    // Increases during Growing, decreases during Degrowing.
    int currentDepth;

    // The deepest level the tree grows to before pausing and degrowing.
    static const int maxDepth = 9;

    // How many animation ticks to hold at full growth / bare trunk before
    // moving to the next state.
    static const int pauseTicksTotal = 5;
    int pauseTicksRemaining;

    // The core recursive drawing function. Draws one branch starting at
    // (x, y), pointing in direction "angle" (degrees, 0 = pointing right),
    // with the given "length". If the current recursion "level" is still
    // at or below currentDepth, it then recursively draws two smaller
    // child branches from the tip of this one.
    void drawBranch(QPainter &painter, double x, double y,
                     double angle, double length, int level);
};

#endif // FRACTALTREE_H
