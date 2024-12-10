#include "dataset.hpp"
#include <QFile>
#include <QTextStream>
#include <stdexcept>

void PollutantDataset::loadData(const std::string &filePath) {
    QFile file("/mnt/c/Users/lapya/Downloads/COMP2811UI24-25_-_CW3/cwk2_solution_final/solution/csv/Y-2024.csv");


    if (!file.exists()) {
        throw std::runtime_error("File not found: " + filePath);
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    QTextStream in(&file);
    data.clear();

    bool isHeader = true; // Skip header row
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (isHeader) {
            isHeader = false;
            continue;
        }

        if (fields.size() < 11) continue; // Ensure enough columns are present

        std::string id = fields[0].toStdString(); // @id column
        std::string time = fields[4].toStdString(); // sample.sampleDateTime column
        std::string pollutantName = fields[5].toStdString(); // determinand.label column
        double concentration = fields[9].toDouble(); // result column
        std::string location = fields[3].toStdString(); // sample.samplingPoint.label column
        std::string compliance = (concentration <= 50.0) ? "Compliant" :
                                  (concentration <= 100.0) ? "Near Limit" : "Exceeds Limit";

        data.push_back(Pollutant(id, time, pollutantName, concentration, compliance, location));
    }
}

std::vector<Pollutant> PollutantDataset::getData() const {
    checkDataExists();
    return data;
}

void PollutantDataset::checkDataExists() const {
    if (data.empty()) {
        throw std::runtime_error("Dataset is empty!");
    }
}
