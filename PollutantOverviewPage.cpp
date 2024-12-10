#include "PollutantOverviewPage.hpp"
#include "dataset.hpp"
#include <QHeaderView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>

PollutantOverviewPage::PollutantOverviewPage(QWidget *parent)
    : QWidget(parent), networkManager(new QNetworkAccessManager(this)) {
    setupUI();
    loadDynamicData();

    // Connect the network manager for API calls
    connect(networkManager, &QNetworkAccessManager::finished, this, &PollutantOverviewPage::onNetworkReply);
}

void PollutantOverviewPage::setupUI() {
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search pollutants...");
    searchBar->setFixedHeight(30);
    searchBar->setContentsMargins(5, 5, 5, 5);
    connect(searchBar, &QLineEdit::textChanged, this, &PollutantOverviewPage::filterTable);

    dataTable = new QTableView(this);
    tableModel = new QStandardItemModel(this);

    chartView = new QChartView(this);
    chartView->setRubberBand(QChartView::RectangleRubberBand); // Enable zoom and pan

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(searchBar);
    layout->addWidget(dataTable);
    layout->addWidget(chartView);

    setLayout(layout);
}

void PollutantOverviewPage::loadDynamicData() {
    PollutantDataset dataset;
    try {
        dataset.loadData("/mnt/c/Users/lapya/Downloads/COMP2811UI24-25_-_CW3/cwk2_solution_final/solution/csv/Y-2024.csv");// put your filw path in  here 
        populateBarChart(dataset);

        // Trigger API fetch for additional details (using @id field)
        for (const auto &entry : dataset.getData()) {
            QUrl url(QString::fromStdString(entry.id)); // gets data from  `entry.id` 
            fetchRemoteData(url);
        }
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void PollutantOverviewPage::populateTable(const PollutantDataset &dataset) {
    tableModel->setHorizontalHeaderLabels({"Time", "Pollutant", "Concentration", "Compliance"});
    dataTable->setModel(tableModel);

    for (const auto &entry : dataset.getData()) {
        QList<QStandardItem *> row;
        row.append(new QStandardItem(QString::fromStdString(entry.time)));
        row.append(new QStandardItem(QString::fromStdString(entry.pollutantName)));
        row.append(new QStandardItem(QString::number(entry.concentration)));

        QString compliance = QString::fromStdString(entry.complianceStatus);
        QColor color = (compliance == "Compliant") ? Qt::green :
                       (compliance == "Caution") ? Qt::yellow : Qt::red;
        QStandardItem *complianceItem = new QStandardItem(compliance);
        complianceItem->setBackground(color);

        row.append(complianceItem);
        tableModel->appendRow(row);
    }

    dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    dataTable->resizeColumnsToContents();
    connect(dataTable, &QTableView::clicked, this, &PollutantOverviewPage::handleTableSelection);
}

void PollutantOverviewPage::populateBarChart(const PollutantDataset &dataset) {
    QChart *chart = new QChart();
    QBarSeries *barSeries = new QBarSeries();

    QMap<QString, QBarSet *> pollutantSets;

    for (const auto &entry : dataset.getData()) {
        QString pollutantName = QString::fromStdString(entry.pollutantName);

        if (!pollutantSets.contains(pollutantName)) {
            pollutantSets[pollutantName] = new QBarSet(pollutantName);
        }

        *(pollutantSets[pollutantName]) << entry.concentration;
    }

    // Add all bar sets to the series
    for (auto barSet : pollutantSets.values()) {
        barSeries->append(barSet);
    }

    chart->addSeries(barSeries);

    // Configure axes
    QStringList categories;
    for (const auto &entry : dataset.getData()) {
        categories << QString::fromStdString(entry.time);
    }

    QBarCategoryAxis *xAxis = new QBarCategoryAxis();
    xAxis->append(categories);
    xAxis->setTitleText("Time");

    QValueAxis *yAxis = new QValueAxis();
    yAxis->setTitleText("Concentration");
    yAxis->setRange(0, 200); // Adjust this based on the maximum concentration dynamically if needed

    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    barSeries->attachAxis(xAxis);
    barSeries->attachAxis(yAxis);

    chart->setTitle("Pollutant Concentration Trends Over Time");
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void PollutantOverviewPage::filterTable(const QString &text) {
    for (int i = 0; i < tableModel->rowCount(); ++i) {
        bool match = tableModel->item(i, 1)->text().contains(text, Qt::CaseInsensitive);
        dataTable->setRowHidden(i, !match);
    }
}

void PollutantOverviewPage::handleTableSelection(const QModelIndex &index) {
    QString pollutant = tableModel->item(index.row(), 1)->text();
    QString compliance = tableModel->item(index.row(), 3)->text();
    double concentration = tableModel->item(index.row(), 2)->text().toDouble();

    QMessageBox::information(this, "Pollutant Details",
                              QString("Pollutant: %1\nConcentration: %2\nCompliance: %3")
                                  .arg(pollutant)
                                  .arg(concentration)
                                  .arg(compliance));
}

void PollutantOverviewPage::fetchRemoteData(const QUrl &url) {
    QNetworkRequest request(url);
    networkManager->get(request);
}

void PollutantOverviewPage::onNetworkReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Error", "Failed to fetch data: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

    if (!jsonDoc.isObject()) {
        QMessageBox::critical(this, "Error", "Invalid JSON response.");
        reply->deleteLater();
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    processRemoteData(jsonObject);

    reply->deleteLater();
}

void PollutantOverviewPage::processRemoteData(const QJsonObject &jsonObject) {
    QString pollutantName = jsonObject["determinand.label"].toString();
    double concentration = jsonObject["result"].toDouble();
    QString compliance = jsonObject["codedResultInterpretation.interpretation"].toString();

    QList<QStandardItem *> row;
    row.append(new QStandardItem(jsonObject["sample.sampleDateTime"].toString()));
    row.append(new QStandardItem(pollutantName));
    row.append(new QStandardItem(QString::number(concentration)));
    row.append(new QStandardItem(compliance));

    tableModel->appendRow(row);
}
void PollutantOverviewPage::handlePointClick(const QPointF &point) {
    QMessageBox::information(this, "Pollutant Point",
                              QString("Concentration: %1").arg(point.y()));
}

