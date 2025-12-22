#include "projectservice.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QPixmap>
#include "networkutils.h"

ProjectService::ProjectService(ApiClient *apiClient, QObject *parent)
    : QObject(parent)
    , apiClient(apiClient)
{

}

void ProjectService::createProject(const Project &project)
{
    QHttpMultiPart *multiPart = makeProjectMultiPart(project);
    QNetworkReply *reply = apiClient->postProject(multiPart);

    if(!reply){
        emit errorOcurred("ProjectService - CreateProject: Reply Null. Not Sended.");
        multiPart->deleteLater();
        return;
    }

    multiPart->setParent(reply);
    connect(reply, &QNetworkReply::finished, this, &ProjectService::createProjectFinished);
}

void ProjectService::getProjectCover(int projectId, const QString &coverSrc)
{
    QNetworkReply *reply = apiClient->getImage(coverSrc);

    if(!reply){
        emit errorOcurred("ProjectService - CreateProject: Reply Null. Not Sended.");
        return;
    }

    reply->setProperty("projectId", projectId);
    connect(reply, &QNetworkReply::finished, this, &ProjectService::getProjectCoverFinished);
}

void ProjectService::getProjectFeatures(int projectId)
{
    QNetworkReply *reply = apiClient->getProjectFeatures(projectId);
    if(!reply){
        emit errorOcurred("ProjectService - GetProjectFeatures. Reply Null. Not Sended");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &ProjectService::getProjectFeaturesFinished);
}

void ProjectService::getProjectTechnicalInfo(int projectId)
{
    QNetworkReply *reply = apiClient->getProjectTechnicalInfo(projectId);
    if(!reply){
        emit errorOcurred("ProjectService - GetTechnicalInfo: Reply Null. Not Sended");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &ProjectService::getProjectTechnicalInfoFinished);
}

void ProjectService::getProjectImagePaths(int projectId)
{
    QNetworkReply *reply = apiClient->getProjectImagePaths(projectId);
    if(!reply){
        emit errorOcurred("ProjectService - GetImagePaths: Reply Null. Not Sended");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &ProjectService::getImagePathsFinished);
}

void ProjectService::updateProject(const Project &project, int projectId)
{
    QByteArray body = getProjectUpdateBody(project);
    QNetworkReply *reply = apiClient->updateProject(body, projectId);
    if(!reply){
        emit errorOcurred("ProjectService - UpdateProject: Reply Null. Not Sended");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &ProjectService::updateProjectFinished);
}

void ProjectService::deleteProject(int projectId)
{
    QNetworkReply *reply = apiClient->deleteProject(projectId);
    if(!reply){
        emit errorOcurred("ProjectService - DeleteProject: Reply Null. Not Sended.");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &ProjectService::deleteProjectFinished);
}

//------ Private Slots ------

void ProjectService::createProjectFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("ProjectService - AddProject: " + errorMsg);
        reply->deleteLater();
        return;
    }

    int projectId = handleCreateProject(reply->readAll());
    emit projectCreated(projectId);

    reply->deleteLater();
}

void ProjectService::getProjectCoverFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("ProjectService - GetProjectCover: " + errorMsg);
        reply->deleteLater();
        return;
    }

    int projectId = reply->property("projectId").toInt();
    QPixmap pixmap;
    pixmap.loadFromData(reply->readAll());

    emit projectCoverReceipt(projectId, pixmap);

    reply->deleteLater();
}

void ProjectService::getProjectFeaturesFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("ProjectService - GetProjectFeatures: " + errorMsg);
        reply->deleteLater();
        return;
    }

    QVector<Feature> feats;
    handleProjectFeaturesReceipt(feats, reply->readAll());

    emit projectFeaturesReceipt(feats);
    reply->deleteLater();
}

void ProjectService::getProjectTechnicalInfoFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("ProjectService - GetProjectTechnicalInfo: " + errorMsg);
        reply->deleteLater();
        return;
    }

    QVector<TechnicalInfo> info;
    handleProjectTechnicalInfo(info, reply->readAll());

    emit projectTechnicalInfoReceipt(info);
    reply->deleteLater();
}

void ProjectService::getImagePathsFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("ProjectService - GetImagePaths: " + errorMsg);
        reply->deleteLater();
        return;
    }

    QVector<QString> paths;
    handleGetImagePaths(paths, reply->readAll());

    emit imagePathsReceipt(paths);
    reply->deleteLater();
}

void ProjectService::updateProjectFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("ProjectService - UpdateProject: " + errorMsg);
        reply->deleteLater();
        return;
    }

    Project project = handleUpdateProjectFinished(reply->readAll());
    emit projectUpdated(project);
    reply->deleteLater();
}

void ProjectService::deleteProjectFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString errorMsg;
    if(!NetworkUtils::checkError(reply, errorMsg)){
        emit errorOcurred("ProjectService - DeleteProject: " + errorMsg);
        reply->deleteLater();
        return;
    }

    emit projectDeleted();
    reply->deleteLater();
}

//------ Reply Handlers ------

int ProjectService::handleCreateProject(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject()) return -1;

    QJsonObject obj = doc.object();
    if(!obj.contains("id")) return -1;

    return obj["id"].toInt();
}

//------ Helpers ------

QHttpMultiPart* ProjectService::makeProjectMultiPart(const Project &project)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //-- JSON part --
    QHttpPart jsonPart;
    jsonPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"project\""));
    jsonPart.setBody(projectToStrJson(project).toUtf8());

    //-- File part --
    QFile *file = new QFile(project.getCoverPath());
    file->open(QIODevice::ReadOnly);

    QHttpPart filePart;
    filePart.setHeader(
        QNetworkRequest::ContentDispositionHeader,
        QVariant("form-data; name=\"file\"; filename=\"" + file->fileName() +"\"")
        );
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "image/*");
    filePart.setBodyDevice(file);
    file->setParent(multiPart);

    //-- Appends --
    multiPart->append(jsonPart);
    multiPart->append(filePart);

    return multiPart;
}

QString ProjectService::projectToStrJson(const Project &project)
{
    QJsonObject obj;
    obj["name"] = project.getName();
    obj["small_about"] = project.getSmallAbout();
    obj["big_about"] = project.getBigAbout();
    obj["user_comment"] = project.getUserComments();

    QJsonDocument doc(obj);
    QString projectJson = QString::fromUtf8(doc.toJson());
    return projectJson;
}

void ProjectService::handleProjectFeaturesReceipt(QVector<Feature> &container, const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isArray()) return;

    QJsonArray array = doc.array();
    for(const QJsonValue &value : std::as_const(array)){
        if(!value.isObject()) continue;

        QJsonObject obj = value.toObject();
        int id = obj["id"].toInt();
        int projectId = obj["project_id"].toInt();
        QString feat = obj["feat"].toString();

        Feature feature(id, projectId, feat);
        container.append(feature);
    }
}

void ProjectService::handleProjectTechnicalInfo(QVector<TechnicalInfo> &container, const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isArray()) return;

    QJsonArray array = doc.array();
    for(const QJsonValue &value : std::as_const(array)){
        if(!value.isObject()) continue;

        QJsonObject obj = value.toObject();
        int id = obj["id"].toInt();
        int projectId = obj["project_id"].toInt();
        QString info = obj["info"].toString();

        container.append(TechnicalInfo(id, projectId, info));
    }
}

void ProjectService::handleGetImagePaths(QVector<QString> &container, const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isArray()) return;

    QJsonArray array = doc.array();
    for(const QJsonValue &value : std::as_const(array)){
        if(!value.isString()) continue;

        container.append(value.toString());
    }
}

Project ProjectService::handleUpdateProjectFinished(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    Project project;

    project.setName(obj["name"].toString());
    project.setSmallAbout(obj["small_about"].toString());
    project.setBigAbout(obj["big_about"].toString());
    project.setUserComments(obj["user_comment"].toString());

    return project;
}

QByteArray ProjectService::getProjectUpdateBody(const Project &project)
{
    QString title = project.getName();
    QString userComment = project.getUserComments();
    QString smallAbout = project.getSmallAbout();
    QString bigAbout = project.getBigAbout();
    QJsonObject obj;

    if(!title.isEmpty()){
        obj["name"] = title;
    }

    if(!userComment.isEmpty()){
        obj["user_comment"] = userComment;
    }

    if(!smallAbout.isEmpty()){
        obj["small_about"] = smallAbout;
    }

    if(!bigAbout.isEmpty()){
        obj["big_about"] = bigAbout;
    }

    return QJsonDocument(obj).toJson();
}

