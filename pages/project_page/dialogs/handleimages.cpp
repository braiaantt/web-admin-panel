#include "handleimages.h"
#include "ui_handleimages.h"
#include "utils.h"
#include "imagewidget.h"

HandleImages::HandleImages(ServiceFactory *factory, int projectId, QWidget *parent)
    : QDialog(parent), factory(factory), projectId(projectId)
    , ui(new Ui::HandleImages)
{
    ui->setupUi(this);
    init();
}

HandleImages::~HandleImages()
{
    delete ui;
}

//------ Initialization ------

void HandleImages::init()
{
    setWindowTitle("Imágenes del proyecto");

    projectService = factory->makeProjectService(this);
    entityImageService = factory->makeEntityImageService(this);

    connectSignalsAndSlots();
    projectService->getProjectImagePaths(projectId);
}

void HandleImages::connectSignalsAndSlots()
{
    connect(projectService, &ProjectService::imagePathsReceipt, this, &HandleImages::pathsReceipt);
    connect(projectService, &ProjectService::errorOcurred, this, &HandleImages::errorOcurred);

    connect(entityImageService, &EntityImageService::imageReceipt, this, &HandleImages::imageReceipt);
    connect(entityImageService, &EntityImageService::imageCreated, this, &HandleImages::imageCreated);
    connect(entityImageService, &EntityImageService::deleteImageSuccess, this, &HandleImages::deleteImageSuccess);
    connect(entityImageService, &EntityImageService::errorOcurred, this, &HandleImages::errorOcurred);
}

//------ Private Slots ------

void HandleImages::imageCreated(const QString &imgPath)
{
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    if(!layout){
        Utils::showWarning(this, "No Layout - Cannot Add New Image");
        return;
    }

    QPixmap pixmap(cachedPath);
    ImageWidget *imageWidget = new ImageWidget(nullptr);
    imageWidget->setImagePath(imgPath);
    imageWidget->setPixMap(pixmap);

    connect(imageWidget, &ImageWidget::removeImage, this, &HandleImages::removeImage);
    layout->insertWidget(layout->count()-1, imageWidget);
}

void HandleImages::pathsReceipt(const QVector<QString> &paths)
{
    for(const QString &path : paths){
        entityImageService->getImage(path);
    }
}

void HandleImages::imageReceipt(const QString &imgPath, const QPixmap &pixmap)
{
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    if(!layout) return;

    ImageWidget *imageWidget = new ImageWidget(nullptr);
    imageWidget->setImagePath(imgPath);
    imageWidget->setPixMap(pixmap);

    connect(imageWidget, &ImageWidget::removeImage, this, &HandleImages::removeImage);
    layout->insertWidget(layout->count()-1, imageWidget);
}

void HandleImages::removeImage(const QString &path)
{
    cachedPath = path;
    entityImageService->deleteImage(projectId, path);
}

void HandleImages::deleteImageSuccess()
{
    QLayout *layout = ui->scrollAreaWidgetContents->layout();
    if(!layout) return;

    for(int i = 0; i<layout->count(); i++){
        ImageWidget *imageWidget = qobject_cast<ImageWidget*>(layout->itemAt(i)->widget());
        if(!imageWidget) continue;

        if(imageWidget->getImagePath() == cachedPath){
            layout->removeWidget(imageWidget);
            imageWidget->deleteLater();
            break;
        }
    }
}

void HandleImages::errorOcurred(const QString &message)
{
    Utils::showWarning(this, message);
}

//------ UI Slots ------

void HandleImages::on_pushButton_clicked()
{
    cachedPath = Utils::selectImageFile();
    entityImageService->addImage(projectId, cachedPath);
}

