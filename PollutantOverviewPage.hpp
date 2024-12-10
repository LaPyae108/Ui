#ifndef POLLUTANTOVERVIEWPAGE_HPP
#define POLLUTANTOVERVIEWPAGE_HPP

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QStandardItemModel>
#include <QTableView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include "dataset.hpp" // Include the dataset header

class PollutantOverviewPage : public QWidget {
    Q_OBJECT

public:
    explicit PollutantOverviewPage(QWidget *parent = nullptr);

private:
    QLineEdit *searchBar;
    QTableView *dataTable;
    QStandardItemModel *tableModel;
    QChartView *chartView;

    QNetworkAccessManager *networkManager;

    void setupUI();
    void setupTable();
    void setupChart();
    void filterTable(const QString &text); // Only declare here, no definition

    void loadDynamicData();
    void populateTable(const PollutantDataset &dataset);
    void populateChart(const PollutantDataset &dataset);

    // Add this declaration
    void populateBarChart(const PollutantDataset &dataset);

    void fetchRemoteData(const QUrl &url);
    void processRemoteData(const QJsonObject &jsonObject);

private slots:
    void handleTableSelection(const QModelIndex &index);
    void handlePointClick(const QPointF &point);
    void onNetworkReply(QNetworkReply *reply);
};

#endif // POLLUTANTOVERVIEWPAGE_HPP
