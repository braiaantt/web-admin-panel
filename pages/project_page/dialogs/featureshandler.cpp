#include "featureshandler.h"
#include "ui_featureshandler.h"
#include "utils.h"

FeaturesHandler::FeaturesHandler(ServiceFactory *factory, int projectId, QWidget *parent)
    : QDialog(parent), factory(factory), projectId(projectId)
    , ui(new Ui::FeaturesHandler)
{
    ui->setupUi(this);
    init();
}

FeaturesHandler::~FeaturesHandler()
{
    delete ui;
}

//------ Initialization ------

void FeaturesHandler::init()
{
    setWindowTitle("Características del proyecto");

    projectService = factory->makeProjectService(this);
    featureService = factory->makeFeatureService(this);
    connectSignalsAndSlots();

    ui->tableWidgetFeatures->hideColumn(0); // Id column
    ui->tableWidgetFeatures->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetFeatures->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->plainTextEditFeature->setFocus();

    projectService->getProjectFeatures(projectId);
}

void FeaturesHandler::connectSignalsAndSlots()
{
    connect(projectService, &ProjectService::projectFeaturesReceipt, this, &FeaturesHandler::featuresReceipt);
    connect(projectService, &ProjectService::errorOcurred, this, &FeaturesHandler::errorOcurred);

    connect(featureService, &FeatureService::featureCreated, this, &FeaturesHandler::featureCreated);
    connect(featureService, &FeatureService::featureDeleted, this, &FeaturesHandler::featureDeleted);
    connect(featureService, &FeatureService::errorOcurred, this, &FeaturesHandler::errorOcurred);
}

//------ Private Slots ------

void FeaturesHandler::featuresReceipt(const QVector<Feature> &feats)
{
    for(const Feature& feat : feats){
        int row = ui->tableWidgetFeatures->rowCount();
        ui->tableWidgetFeatures->insertRow(row);

        ui->tableWidgetFeatures->setItem(row, 0, new QTableWidgetItem(QString::number(feat.getId())));
        ui->tableWidgetFeatures->setItem(row, 1, new QTableWidgetItem(feat.getFeature()));
    }
}

void FeaturesHandler::featureCreated(const Feature &feature)
{
    QTableWidget *table = ui->tableWidgetFeatures;
    int row = table->rowCount();
    table->insertRow(row);

    QTableWidgetItem *itemId = new QTableWidgetItem(feature.getId());
    QTableWidgetItem *itemFeat = new QTableWidgetItem(feature.getFeature());
    ui->tableWidgetFeatures->setItem(row, 0, itemId);
    ui->tableWidgetFeatures->setItem(row, 1, itemFeat);
    ui->plainTextEditFeature->clear();
}

void FeaturesHandler::featureDeleted()
{
    QTableWidget *table = ui->tableWidgetFeatures;
    for(int i = 0; i<table->rowCount(); i++){
        int featId = table->item(i, 0)->text().toInt();
        if(featId == cacheFeature.getId()){
            table->removeRow(i);
            table->clearSelection();
            break;
        }
    }
}

void FeaturesHandler::errorOcurred(const QString &message)
{
    Utils::showWarning(this, message);
}

//------ UI Slots ------

void FeaturesHandler::on_pushButtonAddFeat_clicked()
{
    QString feat = ui->plainTextEditFeature->toPlainText();
    cacheFeature.setFeature(feat);
    featureService->addFeature(projectId, feat);
}

void FeaturesHandler::on_pushButtonDeleteFeat_clicked()
{
    int row = ui->tableWidgetFeatures->currentRow();
    if(row < 0) return;

    int featId = ui->tableWidgetFeatures->item(row, 0)->text().toInt();
    cacheFeature.setId(featId);
    featureService->deleteFeature(featId);
}

