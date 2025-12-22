#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H

#include <QWidget>
#include "project.h"
#include "servicefactory.h"
#include "technologyservice.h"
#include "entitytechservice.h"
#include "entityimageservice.h"
#include "projectservice.h"

namespace Ui {
class ProjectPage;
}

class ProjectPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectPage(ServiceFactory *factory, const Project &project, QWidget *parent = nullptr);
    ~ProjectPage();

private slots:
    void technologiesRelated(const QVector<Technology> &techs);
    void techIconReceipt(int techId, const QPixmap &pixmap);
    void projectUpdated(const Project &project);
    void projectDeleted();
    void errorOcurred(const QString &message);

    //------ UI Slots ------
    void on_pushButtonBack_clicked();

    void on_pushButtonAddTechnology_clicked();

    void on_pushButtonHandleGallery_clicked();

    void on_pushButtonHandleFeats_clicked();

    void on_pushButtonHandleTechnicalInf_clicked();

    void on_pushButtonDeleteProject_clicked();

    void on_pushButtonUpdateTitle_clicked();

    void on_pushButtonUpdateMyComment_clicked();

    void on_pushButtonUpdateSmallAbout_clicked();

    void on_pushButtonUpdateBigAbout_clicked();

signals:
    void backToPortfolio(ProjectPage *page);
    void refreshProject(const Project &project);
    void deleteProject(int projectId);

private:
    Ui::ProjectPage *ui;
    ServiceFactory *factory;
    TechnologyService *technologyService;
    EntityTechService *entityTechService;
    EntityImageService *entityImageService;
    ProjectService *projectService;
    Project project;
    bool projectChanged;

    //------ Initialization ------
    void init();
    void connectSignalsAndSlots();
};

#endif // PROJECTPAGE_H
