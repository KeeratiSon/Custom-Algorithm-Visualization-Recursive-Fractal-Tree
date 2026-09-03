# Custom Algorithm Visualization — Recursive Fractal Tree

A Qt5 fullscreen screensaver that visualizes recursive branching by
growing a fractal tree, one recursion level at a time, then shrinking it
back down and repeating. Built as the Extra Assignment for Task 2
("Implement your own screensaver application with Qt5 that visualizes an
algorithm of your choice") in *Software Development Practice 1*.

## The Algorithm

The tree is built with a simple recursive rule:

> Draw a branch. From its tip, draw two smaller branches, each rotated
> away from the parent's direction. Repeat for each of those, until a
> maximum depth is reached.

```cpp
void FractalTree::drawBranch(QPainter &painter, double x, double y,
                              double angle, double length, int level) {
    if (level > currentDepth || length < 2.0) {
        return; // base case
    }

    // ...compute the branch's endpoint and draw it...

    // recursive case: two smaller child branches from the tip
    drawBranch(painter, endX, endY, angle - SPREAD_DEGREES + jitter,
               length * SHRINK_FACTOR, level + 1);
    drawBranch(painter, endX, endY, angle + SPREAD_DEGREES + jitter,
               length * SHRINK_FACTOR, level + 1);
}
```

Each child branch is 72% the length of its parent and diverges by ±25°
(with a small random jitter for a less mechanical, more organic look).

### Grow / degrow animation

Rather than drawing the finished tree instantly, the screensaver cycles
through four states on a timer:

```
Growing → PausedFull → Degrowing → PausedEmpty → Growing → ...
```

`currentDepth` increases during **Growing**, revealing one more level of
recursion per tick, and decreases during **Degrowing**. Because
`drawBranch()` already skips any branch deeper than `currentDepth`, simply
counting `currentDepth` back down makes the outermost twigs disappear
first, shrinking the tree back toward the trunk — no separate "shrink"
drawing logic is needed; the same recursive function drives both
directions.

### Visual encoding (cosmetic, not part of the algorithm itself)

- **Color**: shallow levels (trunk) are brown, deep levels (twigs) are green
- **Thickness**: shallow levels are thick, deep levels are thin

This makes recursion depth visually readable at a glance, but has no
effect on the branching logic itself.

## Project Structure

```
.
├── cpp_qt_fractal_tree.pro   # Qt project file (qmake build)
├── main.cpp                  # Application entry point
├── fractal_tree.h            # FractalTree widget declaration
└── fractal_tree.cpp          # Recursive drawing + animation logic
```

## Building

### Qt5 (qmake)

```bash
sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-examples \
    qtcreator qttools5-dev-tools qttools5-dev

qmake && make
./FractalTree
```

### Qt6 (CMake) — optional alternative build

```bash
sudo apt install -y qt6-base-dev qt6-base-dev-tools cmake

mkdir build && cd build
cmake ..
make
./cpp_qt6_fractal_tree
```

## Controls

| Key | Action |
|---|---|
| `Esc` | Quit the screensaver |

## Tuning

A few constants near the top of `fractal_tree.cpp` / `fractal_tree.h`
control the tree's look:

| Constant | Effect |
|---|---|
| `trunkLength` (in `paintEvent()`) | Overall size of the tree |
| `SHRINK_FACTOR` | How quickly branches taper (closer to 1.0 = wider tree) |
| `SPREAD_DEGREES` | How "bushy" the tree looks |
| `maxDepth` | Recursion depth / level of detail |
| `pauseTicksTotal` | How long the tree holds at full growth / bare trunk |

## Course Info

Built for **Software Development Practice 1** — *GUI App Development on
Ubuntu / Raspberry Pi OS*, Extra Assignment for Task 2.
