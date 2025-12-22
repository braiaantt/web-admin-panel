#include "technicalinfohandler.h"
#include "ui_technicalinfohandler.h"
#include "utils.h"

TechnicalInfoHandler::TechnicalInfoHandler(ServiceFactory *factory, int projectId, QWidget *parent)
    : QDialog(parent), factory(factory), projectId(projectId)
    , ui(new Ui::TechnicalInfoHandler)
{
    ui->setupUi(this);
    init();
}

TechnicalInfoHandler::~TechnicalInfoHandler()
{
    delete ui;
}

//------ Initialization ------

void TechnicalInfoHandler::init()
{
    setWindowTitle("Información técnica del proyecto");

    projectService = factory->makeProjectService(this);
    technicalInfoService = factory->makeTechnicalInfoService(this);
    connectSignalsAndSlots();

    ui->tableWidgetTechnicalInfo->hideColumn(0); // Id column
    ui->tableWidgetTechnicalInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetTechnicalInfo->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->plainTextEditTechnicalInfo->setFocus();

    projectService->getProjectTechnicalInfo(projectId);
}

void TechnicalInfoHandler::connectSignalsAndSlots()
{
    connect(projectService, &ProjectService::projectTechnicalInfoReceipt, this, &TechnicalInfoHandler::technicalInfoReceipt);
    connect(projectService, &ProjectService::errorOcurred, this, &TechnicalInfoHandler::errorOcurred);

    connect(technicalInfoService, &TechnicalInfoService::technicalInfoAdded, this, &TechnicalInfoHandler::technicalInfoAdded);
    connect(technicalInfoService, &TechnicalInfoService::infoDeleted, this, &TechnicalInfoHandler::technicalInfoDeleted);
    connect(technicalInfoService, &TechnicalInfoService::errorOcurred, this, &TechnicalInfoHandler::errorOcurred);
}

//------ UI Slots ------

void TechnicalInfoHandler::on_pushButtonDeleteTechnicalInfo_clicked()
{
    int row = ui->tableWidgetTechnicalInfo->currentRow();
    if(row < 0) return;

    int id = ui->tableWidgetTechnicalInfo->item(row, 0)->text().toInt();
    technicalInfoService->deleteTechnicalInfo(id);
}

void TechnicalInfoHandler::on_pushButtonAddTechnicalInfo_clicked()
{
    QString info = ui->plainTextEditTechnicalInfo->toPlainText();
    technicalInfoService->addTechnicalInfo(projectId, info);
}

//------ Private Slots ------

void TechnicalInfoHandler::technicalInfoReceipt(const QVector<TechnicalInfo> &infoList)
{
    for(const TechnicalInfo& info : infoList){
        int row = ui->tableWidgetTechnicalInfo->rowCount();
        ui->tableWidgetTechnicalInfo->insertRow(row);

        ui->tableWidgetTechnicalInfo->setItem(row, 0, new QTableWidgetItem(QString::number(info.getId())));
        ui->tableWidgetTechnicalInfo->setItem(row, 1, new QTableWidgetItem(info.getInfo()));
    }
}

void TechnicalInfoHandler::technicalInfoAdded(const TechnicalInfo &tech)
{
    QTableWidget *table = ui->tableWidgetTechnicalInfo;
    int row = table->rowCount();
    table->insertRow(row);

    QTableWidgetItem *itemId = new QTableWidgetItem(tech.getId());
    QTableWidgetItem *itemInfo= new QTableWidgetItem(tech.getInfo());
    ui->tableWidgetTechnicalInfo->setItem(row, 0, itemId);
    ui->tableWidgetTechnicalInfo->setItem(row, 1, itemInfo);
    ui->plainTextEditTechnicalInfo->clear();
}

void TechnicalInfoHandler::technicalInfoDeleted(int deletedId)
{
    QTableWidget *table = ui->tableWidgetTechnicalInfo;
    for(int i = 0; i<table->rowCount(); i++){
        int infoId = table->item(i, 0)->text().toInt();
        if(infoId == deletedId){
            table->removeRow(i);
            table->clearSelection();
            break;
        }
    }
}

void TechnicalInfoHandler::errorOcurred(const QString &message)
{
    Utils::showWarning(this, message);
}
