#include "Controller.hpp"
#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    Controller c;
    return QApplication::exec();
}
