#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include "Dashboard.hpp"
#include "DataPage.hpp"
#include "AnalysisPage.hpp"
#include "PollutantOverviewPage.hpp" // Include the PollutantOverviewPage header

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *stackedWidget;
    Dashboard *dashboard;
    DataPage *dataPage;
    AnalysisPage *analysisPage;
    PollutantOverviewPage *overviewPage; // Add PollutantOverviewPage

    void setupUI();
    void setupNavigation();
};

#endif // MAINWINDOW_HPP
