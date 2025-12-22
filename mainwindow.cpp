#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "page_names.h"
#include "utils.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//------ Initialization ------

void MainWindow::init()
{
    QTimer::singleShot(50, this, [this]{
        if(!serviceFactory.init()){
            Utils::showWarning(this, "Error Loading Config");
            return;
        }
        initPages();
        connectSignalsAndSlots();
    });
}

void MainWindow::initPages()
{
    QHash<QString, QWidget*> pages;

    loginPage = new LoginPage(&serviceFactory, this);
    portfolioPage = new PortfolioPage(&serviceFactory, this);

    pages.insert(PageName::LOGIN, loginPage);
    pages.insert(PageName::PORTFOLIO, portfolioPage);

    for(auto it = pages.cbegin(); it != pages.cend(); ++it){

        QWidget* stackedWidgetPage = ui->stackedWidgetPages->findChild<QWidget*>(it.key());
        if(stackedWidgetPage && stackedWidgetPage->layout()){
            stackedWidgetPage->layout()->addWidget(it.value());
        } else {
            qDebug()<<"Null: "<<it.key()<<" or layout";
        }
    }
}

void MainWindow::connectSignalsAndSlots()
{
    connect(loginPage, &LoginPage::loginComplete, this, &MainWindow::loginComplete);

    connect(portfolioPage, &PortfolioPage::projectClicked, this, &MainWindow::projectClicked);
}

//------ Private Slots ------

void MainWindow::loginComplete()
{
    QWidget* page = ui->stackedWidgetPages->findChild<QWidget*>(PageName::PORTFOLIO);
    if(!page){
        qDebug()<<"Page '"<<PageName::PORTFOLIO<<"' Not Found";
        return;
    }

    portfolioPage->loadPortfolio();
    ui->stackedWidgetPages->setCurrentWidget(page);
}

void MainWindow::projectClicked(const Project &project)
{
    ProjectPage *projectPage = new ProjectPage(&serviceFactory, project, this);

    QWidget* stackedWidgetPage = ui->stackedWidgetPages->findChild<QWidget*>(PageName::PROJECT);
    if(stackedWidgetPage && stackedWidgetPage->layout()){
        connect(projectPage, &ProjectPage::backToPortfolio, this, &MainWindow::backToPortfolioPage);
        connect(projectPage, &ProjectPage::refreshProject, portfolioPage, &PortfolioPage::refreshProject);
        connect(projectPage, &ProjectPage::deleteProject, portfolioPage, &PortfolioPage::deleteProject);
        stackedWidgetPage->layout()->addWidget(projectPage);
        ui->stackedWidgetPages->setCurrentWidget(stackedWidgetPage);
    } else {
        projectPage->deleteLater();
    }
}

void MainWindow::backToPortfolioPage(ProjectPage *page)
{
    QWidget *stackedWidgetPage = ui->stackedWidgetPages->findChild<QWidget*>(PageName::PROJECT);
    QLayout *layout = stackedWidgetPage->layout();
    if(!layout) return;

    layout->removeWidget(page); //Remove project page widget
    page->deleteLater();

    QWidget *widgetPortfolio= ui->stackedWidgetPages->findChild<QWidget*>(PageName::PORTFOLIO);
    ui->stackedWidgetPages->setCurrentWidget(widgetPortfolio);
}
