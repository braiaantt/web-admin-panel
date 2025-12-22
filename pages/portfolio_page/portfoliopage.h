#ifndef PORTFOLIOPAGE_H
#define PORTFOLIOPAGE_H

#include <QWidget>
#include "servicefactory.h"
#include "portfolioservice.h"
#include "technologyservice.h"
#include "projectservice.h"
#include "portfolio.h"

namespace Ui {
class PortfolioPage;
}

class PortfolioPage : public QWidget
{
    Q_OBJECT

public:
    explicit PortfolioPage(ServiceFactory *serviceFactory, QWidget *parent = nullptr);
    ~PortfolioPage();

    void loadPortfolio();

public slots:
    void deleteProject(int projectId);

private slots:
    void setPortfolio(const Portfolio &portfolio);
    void setUserPhoto(const QPixmap &pixmap);
    void userPhotoUpdated(const QString &path);
    void techIconReceipt(int techId, const QPixmap &pixmap);
    void refreshTechnologies();
    void projectCoverReceipt(int projectId, const QPixmap &pixmap);
    void portfolioUpdated();
    void goToProject(const Project &project);
    void errorOcurred(const QString &message);

    //------ UI Slots ------
    void on_pushButtonAddTechnology_clicked();

    void on_pushButtonAddProject_clicked();

    void on_pushButtonUpdatePhoto_clicked();

    void on_pushButtonHandleTechnologies_clicked();

    void on_pushButtonUpdateName_clicked();

    void on_pushButtonUpdateProfession_clicked();

    void on_pushButtonUpdateAbout_clicked();

signals:
    void projectClicked(const Project &project);

private:
    Ui::PortfolioPage *ui;
    ServiceFactory *factory;
    PortfolioService *portfolioService;
    TechnologyService *technologyService;
    ProjectService *projectService;

    //------ Initialization ------
    void init();
    void connectSignalsAndSlots();

    //------ Helpers ------
    void setTechnologyWidgets(const QVector<Technology> &techs);
    void setProjectWidgets(const QVector<Project> &projects);
};

#endif // PORTFOLIOPAGE_H
