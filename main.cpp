#include <QApplication>
#include "fractal_tree.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    FractalTree window;
    window.show();

    return app.exec();
}
