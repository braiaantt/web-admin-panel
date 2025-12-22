#include "portfoliopage.h"
#include "ui_portfoliopage.h"
#include <QMessageBox>
#include "createproject.h"
#include "portfolioproject.h"
#include "utils.h"
#include "technologyhandler.h"
#include "technologyrelation.h"
#include "portfolioproject.h"

PortfolioPage::PortfolioPage(ServiceFactory *factory, QWidget *parent)
    : QWidget(parent), factory(factory)
    , ui(new Ui::PortfolioPage)
{
    ui->setupUi(this);
    init();
}

PortfolioPage::~PortfolioPage()
{
    delete ui;
}

//------ Initialization ------

void PortfolioPage::init()
{
    portfolioService = factory->makePortfolioService(this);
    technologyService = factory->makeTechnologyService(this);
    projectService = factory->makeProjectService(this);

    connectSignalsAndSlots();
    ui->scrollAreaTechnologies->init();
}

void PortfolioPage::connectSignalsAndSlots()
{
    connect(portfolioService, &PortfolioService::portfolioReceipt, this, &PortfolioPage::setPortfolio);
    connect(portfolioService, &PortfolioService::userPhotoUpdated, this, &PortfolioPage::userPhotoUpdated);
    connect(portfolioService, &PortfolioService::userPhotoReceipt, this, &PortfolioPage::setUserPhoto);
    connect(portfolioService, &PortfolioService::portfolioUpdated, this, &PortfolioPage::portfolioUpdated);
    connect(portfolioService, &PortfolioService::errorOcurred, this, &PortfolioPage::errorOcurred);

    connect(technologyService, &TechnologyService::techIconReceipt, this, &PortfolioPage::techIconReceipt);
    connect(technologyService, &TechnologyService::errorOcurred, this, &PortfolioPage::errorOcurred);

    connect(projectService, &ProjectService::projectCoverReceipt, this, &PortfolioPage::projectCoverReceipt);
    connect(projectService, &ProjectService::errorOcurred, this, &PortfolioPage::errorOcurred);
}

//------ UI Slots ------

void PortfolioPage::on_pushButtonUpdateName_clicked()
{
    QString newName = ui->lineEditName->text();
    Portfolio portfolio;
    portfolio.setUserName(newName);

    portfolioService->updatePortfolio(portfolio);
}

void PortfolioPage::on_pushButtonUpdateProfession_clicked()
{
    QString newProfession = ui->lineEditProfession->text();
    Portfolio portfolio;
    portfolio.setUserProfession(newProfession);

    portfolioService->updatePortfolio(portfolio);
}

void PortfolioPage::on_pushButtonUpdateAbout_clicked()
{
    QString newUserAbout = ui->plainTextAbout->toPlainText();
    Portfolio portfolio;
    portfolio.setUserAbout(newUserAbout);

    portfolioService->updatePortfolio(portfolio);
}

void PortfolioPage::on_pushButtonAddTechnology_clicked()
{
    int entityId = 1;
    QString entityType = "portfolio";
    TechnologyRelation dialog(factory, entityId, entityType, this);
    connect(&dialog, &TechnologyRelation::technologiesChanged, this, &PortfolioPage::refreshTechnologies);
    dialog.exec();
}

void PortfolioPage::on_pushButtonAddProject_clicked()
{
    CreateProject dialog(factory, this);
    if(dialog.exec() == QDialog::Rejected) return;

    QHBoxLayout *layout = (QHBoxLayout*)ui->scrollAreaProjectWidgetContents->layout();
    Project project = dialog.getProject();
    PortfolioProject *projectWidget = new PortfolioProject(nullptr, project);

    if(projectWidget && layout){
        layout->insertWidget(layout->count()-1, projectWidget);
        projectWidget->setParent(ui->scrollAreaProjectWidgetContents); // to avoid memory leak warning
        connect(projectWidget, &PortfolioProject::goToProject, this, &PortfolioPage::goToProject);
    } else {
        projectWidget->deleteLater();
    }
}

void PortfolioPage::on_pushButtonUpdatePhoto_clicked()
{
    QString imgPath = Utils::selectImageFile();
    if(imgPath.isNull()) return;

    int portfolioId = 1;
    portfolioService->updateUserPhoto(portfolioId, imgPath);
}

void PortfolioPage::on_pushButtonHandleTechnologies_clicked()
{
    TechnologyHandler dialog(technologyService, this);
    dialog.exec();
}

//------ Public Methods------

void PortfolioPage::loadPortfolio()
{
    portfolioService->getPortfolio();
}

//------ Public Slots ------

void PortfolioPage::deleteProject(int projectId)
{
    QHBoxLayout *layout = (QHBoxLayout*)ui->scrollAreaProjectWidgetContents->layout();
    for(int i = 0; i<layout->count(); i++){
        PortfolioProject *projectWidget = qobject_cast<PortfolioProject*>(layout->itemAt(i)->widget());
        if(projectWidget && projectWidget->getProject().getId() == projectId){
            layout->removeWidget(projectWidget);
            projectWidget->deleteLater();
            break;
        }
    }
}

//------ Private Slots ------

void PortfolioPage::setPortfolio(const Portfolio &portfolio)
{
    ui->lineEditName->setText(portfolio.getUserName());
    ui->lineEditProfession->setText(portfolio.getUserProfession());
    ui->plainTextAbout->appendPlainText(portfolio.getUserAbout());
    setTechnologyWidgets(portfolio.getTechnologies());
    setProjectWidgets(portfolio.getProjects());

    portfolioService->getUserPhoto(portfolio.getUserPhotoPath());
}

void PortfolioPage::setUserPhoto(const QPixmap &pixmap)
{
    QPixmap rounded = Utils::roundedPixmap(pixmap, ui->labelPhoto->size());
    ui->labelPhoto->setPixmap(rounded);
}

void PortfolioPage::techIconReceipt(int techId, const QPixmap &pixmap)
{
    ui->scrollAreaTechnologies->setTechIcon(techId, pixmap);
}

void PortfolioPage::refreshTechnologies()
{
    /*
    Future implementation with service provider to avoid
    interferences between PortfolioPage and TechnologyRelation dialog,
    because they use the same service to get technologies
    */
}

void PortfolioPage::projectCoverReceipt(int projectId, const QPixmap &pixmap)
{
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(ui->scrollAreaProjectWidgetContents->layout());
    if(!layout) return;

    for(int i = 0; i<layout->count(); i++){
        PortfolioProject *projectWidget = qobject_cast<PortfolioProject*>(layout->itemAt(i)->widget());
        if(projectWidget && projectWidget->getProject().getId() == projectId){
            projectWidget->setCoverImage(pixmap);
            break;
        }
    }
}

void PortfolioPage::goToProject(const Project &project)
{
    emit projectClicked(project);
}

void PortfolioPage::userPhotoUpdated(const QString &path)
{
    portfolioService->getUserPhoto(path);
}

void PortfolioPage::portfolioUpdated()
{
    QMessageBox::information(this, "Mensaje", "El portfolio se actualizó correctamente.");
}

void PortfolioPage::errorOcurred(const QString &message)
{
    Utils::showWarning(this, message);
}

//------ Helpers ------

void PortfolioPage::setTechnologyWidgets(const QVector<Technology> &techs)
{
    for(const Technology &tech : techs){
        TechnologyWidget *techWidget = new TechnologyWidget(nullptr, tech);
        technologyService->getTechIcon(tech.getId(), tech.getImgPath());
        ui->scrollAreaTechnologies->addTechnologyWidget(techWidget);
    }
}

void PortfolioPage::setProjectWidgets(const QVector<Project> &projects)
{
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(ui->scrollAreaProjectWidgetContents->layout());
    if(!layout) return;

    for(const Project &project : projects){
        PortfolioProject *widget = new PortfolioProject(nullptr, project);
        projectService->getProjectCover(project.getId(), project.getCoverPath());
        layout->insertWidget(layout->count()-1, widget);
        connect(widget, &PortfolioProject::goToProject, this, &PortfolioPage::goToProject);
    }
}
