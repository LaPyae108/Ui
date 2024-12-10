#include "MainWindow.hpp"
#include "dataset.hpp"
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    // Create a stacked widget for page navigation
    stackedWidget = new QStackedWidget(this);

    // Initialize pages
    dashboard = new Dashboard(this);
    dataPage = new DataPage(this);
    analysisPage = new AnalysisPage(this);
    overviewPage = new PollutantOverviewPage(this);

    // Load dataset and update the analysis page
    try {
        PollutantDataset dataset;
        dataset.loadData("/mnt/c/Users/lapya/Downloads/COMP2811UI24-25_-_CW3/cwk2_solution_final/solution/csv/Y-2024.csv"); // Update to your actual file path
        analysisPage->updateChart(dataset.getData());
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }

    // Add pages to the stacked widget
    stackedWidget->addWidget(dashboard);
    stackedWidget->addWidget(dataPage);
    stackedWidget->addWidget(analysisPage);
    stackedWidget->addWidget(overviewPage);

    // Set the stacked widget as the central widget
    setCentralWidget(stackedWidget);

    // Configure window
    setWindowTitle("Pollutant Monitor");
    resize(1024, 768);

    // Add navigation buttons
    setupNavigation();
}

void MainWindow::setupNavigation() {
    // Create a navigation bar
    QWidget *navBar = new QWidget(this);
    QHBoxLayout *navLayout = new QHBoxLayout(navBar);

    // Create navigation buttons
    QPushButton *dashboardButton = new QPushButton("Dashboard", this);
    QPushButton *dataPageButton = new QPushButton("Data Page", this);
    QPushButton *analysisPageButton = new QPushButton("Analysis Page", this);
    QPushButton *overviewPageButton = new QPushButton("Overview Page", this);

    // Add buttons to navigation layout
    navLayout->addWidget(dashboardButton);
    navLayout->addWidget(dataPageButton);
    navLayout->addWidget(analysisPageButton);
    navLayout->addWidget(overviewPageButton);

    // Connect buttons to switch pages in the stacked widget
    connect(dashboardButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(dashboard);
    });

    connect(dataPageButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(dataPage);
    });

    connect(analysisPageButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(analysisPage);
    });

    connect(overviewPageButton, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentWidget(overviewPage);
    });

    // Combine navigation bar with stacked widget in the main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(navBar);
    mainLayout->addWidget(stackedWidget);

    // Set main layout for the central widget
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}
