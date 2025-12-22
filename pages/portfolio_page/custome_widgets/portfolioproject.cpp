#include "portfolioproject.h"
#include "ui_portfolioproject.h"

PortfolioProject::PortfolioProject(QWidget *parent, const Project &project)
    : QWidget(parent), project(project)
    , ui(new Ui::PortfolioProject)
{
    ui->setupUi(this);
    init();
}

PortfolioProject::~PortfolioProject()
{
    delete ui;
}

//------ Initialization ------

void PortfolioProject::init()
{
    if(!project.getCoverPath().isEmpty()){
        ui->labelImage->setPixmap(QPixmap(project.getCoverPath()));
    }

    ui->labelTitle->setText(project.getName());
    ui->labelSmallAbout->setText(project.getSmallAbout());
}

//------ Setters ------

void PortfolioProject::setCoverImage(const QPixmap &pixmap)
{
    ui->labelImage->setPixmap(pixmap);
}

void PortfolioProject::setProject(const Project &_project)
{
    project = _project;
    init();
}

//------ Getters ------

const Project& PortfolioProject::getProject() const
{
    return project;
}

//------ UI Slots ------

void PortfolioProject::on_pushButtonGoToProject_clicked()
{
    emit goToProject(project);
}
