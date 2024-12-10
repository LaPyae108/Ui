#include <QApplication>
#include <QFile>
#include <QDebug>
#include "MainWindow.hpp"
#include "dataset.hpp"

int main(int argc, char *argv[]) {
    qputenv("QT_QPA_PLATFORM", QByteArray("xcb"));

    QApplication app(argc, argv);

    // Update the file path to the correct absolute or relative path
    std::string filePath = "/mnt/c/Users/lapya/Downloads/COMP2811UI24-25_-_CW3/cwk2_solution_final/solution/csv/Y-2024.csv";

    PollutantDataset dataset;
    try {
        dataset.loadData(filePath);  // Pass the correct file path
    } catch (const std::exception &e) {
        qCritical() << "Error loading dataset:" << e.what();
        return -1;
    }

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
