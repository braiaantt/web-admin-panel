#include "projectpage.h"
#include "ui_projectpage.h"
#include <QMessageBox>
#include "technologyrelation.h"
#include "handleimages.h"
#include "featureshandler.h"
#include "technicalinfohandler.h"
#include "utils.h"

ProjectPage::ProjectPage(ServiceFactory *factory, const Project &project, QWidget *parent)
    : QWidget(parent), factory(factory), project(project)
    , ui(new Ui::ProjectPage)
{
    ui->setupUi(this);
    init();
}

ProjectPage::~ProjectPage()
{
    delete ui;
}

//------ Initialization ------

void ProjectPage::init()
{
    technologyService = factory->makeTechnologyService(this);
    entityTechService = factory->makeEntityTechService(this);
    entityImageService = factory->makeEntityImageService(this);
    projectService = factory->makeProjectService(this);

    ui->lineEditTitle->setText(project.getName());
    ui->plainTextEditSmallAbout->setPlainText(project.getSmallAbout());
    ui->plainTextEditBigAbout->setPlainText(project.getBigAbout());
    ui->plainTextEditMyComments->setPlainText(project.getUserComments());
    ui->scrollAreaTechnologies->init();

    connectSignalsAndSlots();

    QString entityType = "project";
    entityTechService->getRelations(project.getId(), entityType);
}

void ProjectPage::connectSignalsAndSlots()
{
    connect(entityTechService, &EntityTechService::technologiesRelated, this, &ProjectPage::technologiesRelated);
    connect(entityTechService, &EntityTechService::errorOcurred, this, &ProjectPage::errorOcurred);

    connect(technologyService, &TechnologyService::techIconReceipt, this, &ProjectPage::techIconReceipt);
    connect(technologyService, &TechnologyService::errorOcurred, this, &ProjectPage::errorOcurred);

    connect(projectService, &ProjectService::projectDeleted, this, &ProjectPage::projectDeleted);
    connect(projectService, &ProjectService::projectUpdated, this, &ProjectPage::projectUpdated);
    connect(projectService, &ProjectService::errorOcurred, this, &ProjectPage::errorOcurred);
}

//------ UI Slots ------

void ProjectPage::on_pushButtonBack_clicked()
{
    if(projectChanged)
        emit refreshProject(project);

    emit backToPortfolio(this);
}

void ProjectPage::on_pushButtonUpdateTitle_clicked()
{
    QString newTitle = ui->lineEditTitle->text();
    Project p;
    p.setName(newTitle);

    projectService->updateProject(p, project.getId());
}

void ProjectPage::on_pushButtonUpdateMyComment_clicked()
{
    QString newUserComment = ui->plainTextEditMyComments->toPlainText();
    Project p;
    p.setUserComments(newUserComment);

    projectService->updateProject(p, project.getId());
}

void ProjectPage::on_pushButtonUpdateSmallAbout_clicked()
{
    QString newSmallAbout = ui->plainTextEditSmallAbout->toPlainText();
    Project p;
    p.setSmallAbout(newSmallAbout);

    projectService->updateProject(p, project.getId());
}

void ProjectPage::on_pushButtonUpdateBigAbout_clicked()
{
    QString newBigAbout = ui->plainTextEditBigAbout->toPlainText();
    Project p;
    p.setBigAbout(newBigAbout);

    projectService->updateProject(p, project.getId());
}

void ProjectPage::on_pushButtonAddTechnology_clicked()
{
    QString entityType = "project";
    TechnologyRelation dialog(factory, project.getId(), entityType, this);
    dialog.exec();
}

void ProjectPage::on_pushButtonHandleGallery_clicked()
{
    HandleImages dialog(factory, project.getId(), this);
    dialog.exec();;
}


void ProjectPage::on_pushButtonHandleFeats_clicked()
{
    FeaturesHandler dialog(factory, project.getId(), this);
    dialog.exec();
}

void ProjectPage::on_pushButtonHandleTechnicalInf_clicked()
{
    TechnicalInfoHandler dialog(factory, project.getId(), this);
    dialog.exec();
}

void ProjectPage::on_pushButtonDeleteProject_clicked()
{
    projectService->deleteProject(project.getId());
}

//------ Private Slots ------

void ProjectPage::technologiesRelated(const QVector<Technology> &techs)
{
    for(const Technology &tech : techs){
        TechnologyWidget *techWidget = new TechnologyWidget(nullptr, tech);
        technologyService->getTechIcon(tech.getId(), tech.getImgPath());
        ui->scrollAreaTechnologies->addTechnologyWidget(techWidget);
    }
}

void ProjectPage::techIconReceipt(int techId, const QPixmap &pixmap)
{
    ui->scrollAreaTechnologies->setTechIcon(techId, pixmap);
}

void ProjectPage::projectUpdated(const Project &updated)
{
    project.setName(updated.getName());
    project.setSmallAbout(updated.getSmallAbout());
    project.setBigAbout(updated.getBigAbout());
    project.setUserComments(updated.getUserComments());

    projectChanged = true;
    QMessageBox::information(this, "Mensaje", "El proyecto fue actualizado correctamente!");
}

void ProjectPage::projectDeleted()
{
    emit deleteProject(project.getId());
    emit backToPortfolio(this);
}

void ProjectPage::errorOcurred(const QString &message)
{
    Utils::showWarning(this, message);
}
