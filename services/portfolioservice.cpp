#include "portfolioservice.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPixmap>
#include <QFile>
#include "networkutils.h"

PortfolioService::PortfolioService(ApiClient *apiClient, QObject *parent) :
    QObject(parent)
    , apiClient(apiClient)
{
    //Important delete reply after finished, autoDeleteReply desactivated at ApiClient initialization
}

void PortfolioService::getPortfolio()
{
    QNetworkReply *reply = apiClient->getPortfolio();
    if(!reply){
        emit errorOcurred("PortfolioService - GetPortfolio: Null Reply. Not Sended");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &PortfolioService::getPortfolioFinished);
}

void PortfolioService::updateUserPhoto(int portfolioId, const QString &srcPath)
{
    QHttpMultiPart *multiPart = makeUserPhotoMultiPart(srcPath);
    QNetworkReply *reply = apiClient->updateUserPhoto(portfolioId, multiPart);
    if(!reply){
        emit errorOcurred("PortfolioService - UpdateUserPhoto. Reply Null. Not Sended");
        multiPart->deleteLater();
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &PortfolioService::updateUserPhotoFinished);
}

void PortfolioService::getUserPhoto(const QString &path)
{
    QNetworkReply *reply = apiClient->getImage(path);
    if(!reply){
        emit errorOcurred("PortfolioService - GetPhoto: Null Reply. Not Sended");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &PortfolioService::getUserPhotoFinished);
}

void PortfolioService::updatePortfolio(const Portfolio &portfolio)
{
    QByteArray body = getPortfolioUpdateBody(portfolio);
    QNetworkReply *reply = apiClient->updatePortfolio(body);

    if(!reply){
        emit errorOcurred("PortfolioService - UpdatePortfolio: Null Reply. Not Sended");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &PortfolioService::updatePortfolioFinished);
}

//------ Private Slots ------

void PortfolioService::getPortfolioFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if(!reply){
        emit errorOcurred("PortfolioService - GetPortfolio: Null Reply");
        return;
    }

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("PortfolioService - GetPortfolio: " + errorMsg);
        reply->deleteLater();
        return;
    }

    Portfolio portfolio = handlePortfolioFinished(reply->readAll());
    emit portfolioReceipt(portfolio);

    reply->deleteLater();
}

void PortfolioService::getUserPhotoFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply){
        emit errorOcurred("PortfolioService - GetPhoto: Null Reply");
        return;
    }

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("PortfolioService - GetPhoto: " + errorMsg);
        reply->deleteLater();
        return;
    }

    QPixmap photo;
    photo.loadFromData(reply->readAll());
    emit userPhotoReceipt(photo);

    reply->deleteLater();
}

void PortfolioService::updateUserPhotoFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("PortfolioService - UpdateUserPhoto" + errorMsg);
        reply->deleteLater();
        return;
    }

    emit userPhotoUpdated(handleUpdateUserPhoto(reply->readAll()));
    reply->deleteLater();
}

void PortfolioService::updatePortfolioFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("PortfolioService - UpdatePortfolio" + errorMsg);
        reply->deleteLater();
        return;
    }

    emit portfolioUpdated();
    reply->deleteLater();
}

//------ Request Handlers ------

Portfolio PortfolioService::handlePortfolioFinished(const QByteArray &data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if(!jsonDoc.isObject()) return Portfolio();
    QJsonObject obj = jsonDoc.object();

    Portfolio portfolio;
    portfolio.setUserName(obj["user_name"].toString());
    portfolio.setUserProfession(obj["user_profession"].toString());
    portfolio.setUserPhotoPath(obj["user_photo"].toString());
    portfolio.setUserAbout(obj["user_about"].toString());
    portfolio.setTechnologies(getTechnologiesFromArray(obj["techs"].toArray()));
    portfolio.setProjects(getProjectsFromArray(obj["projects"].toArray()));

    return portfolio;
}

QString PortfolioService::handleUpdateUserPhoto(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject()) return "";

    QJsonObject obj = doc.object();
    return obj["path"].toString();
}

//------ Helpers ------

QVector<Technology> PortfolioService::getTechnologiesFromArray(const QJsonArray &array)
{
    QVector<Technology> techs;

    for(const QJsonValue &value : array){
        if(!value.isObject()) continue;

        QJsonObject obj = value.toObject();
        int techId = obj["id"].toInt();
        QString name = obj["name"].toString();
        QString iconSrc = obj["icon_src"].toString();

        Technology tech(techId, name, iconSrc);
        techs.append(tech);

    }

    return techs;
}

QVector<Project> PortfolioService::getProjectsFromArray(const QJsonArray &array)
{
    QVector<Project> projects;

    for(const QJsonValue &value : array){
        if(!value.isObject()) continue;

        QJsonObject obj = value.toObject();
        int projectId = obj["id"].toInt();
        QString name = obj["name"].toString();
        QString smallAbout = obj["small_about"].toString();
        QString bigAbout = obj["big_about"].toString();
        QString userComment = obj["user_comment"].toString();
        QString coverSrc = obj["cover_src"].toString();

        Project project(projectId, name, smallAbout, bigAbout, userComment, coverSrc);
        projects.append(project);
    }

    return projects;
}

QHttpMultiPart* PortfolioService::makeUserPhotoMultiPart(const QString &srcPath)
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QFile* file = new QFile(srcPath);
    file->open(QIODevice::ReadOnly);

    QHttpPart filePart;
    filePart.setHeader(
        QNetworkRequest::ContentDispositionHeader,
        QVariant("form-data; name=\"file\"; filename=\"" + file->fileName().toUtf8() + "\"")
        );
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "image/*");
    filePart.setBodyDevice(file);

    multiPart->append(filePart);

    return multiPart;
}

QByteArray PortfolioService::getPortfolioUpdateBody(const Portfolio &portfolio)
{
    QString name = portfolio.getUserName();
    QString profession = portfolio.getUserProfession();
    QString about = portfolio.getUserAbout();
    QJsonObject obj;

    if(!name.isEmpty()){
        obj["user_name"] = name;
    }

    if(!profession.isEmpty()){
        obj["user_profession"] = profession;
    }

    if(!about.isEmpty()){
        obj["user_about"] = about;
    }

    return QJsonDocument(obj).toJson();
}
