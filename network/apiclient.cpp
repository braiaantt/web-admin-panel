#include "apiclient.h"

ApiClient::ApiClient(QObject *parent) :
    QObject(parent),
    basicRequests(new BasicRequests(this))
{
    basicRequests->setAutoDeleteReply(false); // Configure BasicRequest for let us handle reply delete
}

//------ Requests ------
//-- Auth --
QNetworkReply* ApiClient::login(const QByteArray &body)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Content-Type", "application/json");

    return basicRequests->post(loginEndpoint, body, headers);
}

//-- Portfolio --
QNetworkReply* ApiClient::getPortfolio()
{
    return basicRequests->get(portfolioEndpoint);
}

QNetworkReply* ApiClient::updateUserPhoto(int portfolioId, QHttpMultiPart *multiPart)
{
    QString finalEndpoint = portfolioEndpoint + "/" + QString::number(portfolioId) + "/user-photo";
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->putMultipart(finalEndpoint, multiPart, headers);
}

QNetworkReply* ApiClient::updatePortfolio(const QByteArray &body)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->put(portfolioEndpoint, body, headers);
}

//-- Technology --

QNetworkReply* ApiClient::postTechnology(QHttpMultiPart *multiPart)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->postMultipart(technologyEndpoint, multiPart, headers);
}

QNetworkReply* ApiClient::getTechnologies()
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    return basicRequests->get(technologyEndpoint, headers);
}

QNetworkReply* ApiClient::deleteTechnology(int techId)
{
    QString resourcePath = technologyEndpoint + "/" +QString::number(techId);
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->deleteResource(resourcePath, headers);
}

//-- Entity Technology --

QNetworkReply* ApiClient::addTechRelation(const QByteArray &body)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    headers.insert("Content-Type", "application/json");

    return basicRequests->post(entityTechnologyEndpoint, body, headers);
}

QNetworkReply* ApiClient::removeTechRelation(const QString &queryParams)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->deleteResource(entityTechnologyEndpoint + queryParams, headers);
}

QNetworkReply* ApiClient::getRelations(const QString &queryParams)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->get(entityTechnologyEndpoint + queryParams, headers);
}

//-- Project --

QNetworkReply* ApiClient::postProject(QHttpMultiPart *multiPart)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->postMultipart(projectEndpoint, multiPart, headers);
}

QNetworkReply* ApiClient::getProjectFeatures(int projectId)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    QString finalEndpoint = projectEndpoint + "/" +QString::number(projectId) + featuresEndpoint;

    return basicRequests->get(finalEndpoint, headers);
}

QNetworkReply* ApiClient::getProjectTechnicalInfo(int projectId)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    QString finalEndpoint = projectEndpoint + "/" +QString::number(projectId) + technicalInfoEndpoint;

    return basicRequests->get(finalEndpoint, headers);
}

QNetworkReply* ApiClient::getProjectImagePaths(int projectId)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    QString finalEndpoint = projectEndpoint + "/" +QString::number(projectId) + "/images";

    return basicRequests->get(finalEndpoint, headers);
}

QNetworkReply* ApiClient::updateProject(const QByteArray &body, int projectId)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    QString finalEndpoint = projectEndpoint + "/" + QString::number(projectId);

    return basicRequests->put(finalEndpoint, body, headers);
}

QNetworkReply* ApiClient::deleteProject(int projectId)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    QString finalEndpoint = projectEndpoint + "/" +QString::number(projectId);

    return basicRequests->deleteResource(finalEndpoint, headers);
}

//-- Static --

QNetworkReply* ApiClient::getImage(const QString &path)
{
    return basicRequests->get(path);
}

//-- Project Images --

QNetworkReply *ApiClient::postProjectImage(QHttpMultiPart *multiPart)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->postMultipart(projectImageEndpoint, multiPart, headers);
}

QNetworkReply *ApiClient::deleteProjectImage(const QString &queryParams)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->deleteResource(projectImageEndpoint + queryParams, headers);
}

//-- Features --

QNetworkReply* ApiClient::postFeature(const QByteArray &body)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    headers.insert("Content-Type", "application/json");

    return basicRequests->post(featuresEndpoint, body, headers);
}

QNetworkReply* ApiClient::deleteFeature(int featureId)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->deleteResource(featuresEndpoint + "/" + QString::number(featureId), headers);
}

//-- Technical Info --

QNetworkReply* ApiClient::postTechnicalInfo(const QByteArray &body)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());
    headers.insert("Content-Type", "application/json");

    return basicRequests->post(technicalInfoEndpoint, body, headers);
}

QNetworkReply* ApiClient::deleteTechnicalInfo(int techId)
{
    QMap<QByteArray,QByteArray> headers;
    headers.insert("Authorization", ("Bearer " + accessToken).toUtf8());

    return basicRequests->deleteResource(technicalInfoEndpoint + "/" + QString::number(techId), headers);
}

//------ Setters ------

void ApiClient::setHostName(const QString &_hostName)
{
    hostName = _hostName;
    basicRequests->setHostName(_hostName);
}

void ApiClient::setLoginEndpoint(const QString &endpoint){
    loginEndpoint= endpoint;
}

void ApiClient::setPortfolioEndpoint(const QString &endpoint)
{
    portfolioEndpoint = endpoint;
}

void ApiClient::setTechnologyEndpoint(const QString &endpoint)
{
    technologyEndpoint = endpoint;
}

void ApiClient::setEntityTechnologyEndpoint(const QString &endpoint)
{
    entityTechnologyEndpoint = endpoint;
}

void ApiClient::setProjectImageEndpoint(const QString &endpoint)
{
    projectImageEndpoint = endpoint;
}

void ApiClient::setProjectEndpoint(const QString &endpoint){
    projectEndpoint = endpoint;
}

void ApiClient::setFeaturesEndpoint(const QString &endpoint){
    featuresEndpoint = endpoint;
}

void ApiClient::setTechnicalInfoEndpoint(const QString &endpoint)
{
    technicalInfoEndpoint = endpoint;
}

void ApiClient::setAccessToken(const QString &token){
    accessToken = token;
}

void ApiClient::setRefreshToken(const QString &token){
    refreshToken = token;
}
