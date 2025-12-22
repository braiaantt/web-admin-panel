#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <basicrequests.h>
#include <QHttpMultiPart>

class ApiClient : public QObject
{
    Q_OBJECT
public:
    ApiClient(QObject *parent = nullptr);

    //------ Requests ------
    //-- Auth --
    QNetworkReply* login(const QByteArray &body);

    //-- Portfolio --
    QNetworkReply* getPortfolio();
    QNetworkReply* updateUserPhoto(int portfolioId, QHttpMultiPart *multiPart);
    QNetworkReply* updatePortfolio(const QByteArray &body);

    //-- Technology --
    QNetworkReply* postTechnology(QHttpMultiPart *multipart);
    QNetworkReply* getTechnologies();
    QNetworkReply* deleteTechnology(int techId);

    //-- Entity Technology --
    QNetworkReply* addTechRelation(const QByteArray &body);
    QNetworkReply* removeTechRelation(const QString &queryParams);
    QNetworkReply* getRelations(const QString &queryParams);

    //-- Project --
    QNetworkReply* postProject(QHttpMultiPart *multiPart);
    QNetworkReply* getProjectFeatures(int projectId);
    QNetworkReply* getProjectTechnicalInfo(int projectId);
    QNetworkReply* getProjectImagePaths(int projectId);
    QNetworkReply* updateProject(const QByteArray &body, int projectId);
    QNetworkReply* deleteProject(int projectId);

    //-- Project Images --
    QNetworkReply *postProjectImage(QHttpMultiPart *multiPart);
    QNetworkReply *deleteProjectImage(const QString &queryParams);

    //-- Features --
    QNetworkReply* postFeature(const QByteArray &body);
    QNetworkReply* deleteFeature(int featureId);

    //-- Technical Info --
    QNetworkReply* postTechnicalInfo(const QByteArray &body);
    QNetworkReply* deleteTechnicalInfo(int infoId);

    //-- Static --
    QNetworkReply* getImage(const QString &path);

    //------ Setters ------
    void setHostName(const QString &hostName);
    void setLoginEndpoint(const QString &loginEndpoint);
    void setPortfolioEndpoint(const QString &portfolioEndpoint);
    void setTechnologyEndpoint(const QString &technologyEndpoint);
    void setEntityTechnologyEndpoint(const QString &entityTechnologyEndpoint);
    void setProjectImageEndpoint(const QString &projectImageEndpoint);
    void setProjectEndpoint(const QString &projectEndpoint);
    void setFeaturesEndpoint(const QString &featuresEndpoint);
    void setTechnicalInfoEndpoint(const QString &technicalInfoEndpoint);
    void setAccessToken(const QString &accesstoken);
    void setRefreshToken(const QString &refreshToken);

private:
    BasicRequests *basicRequests;
    QString hostName;
    QString accessToken;
    QString refreshToken;

    //------ Endpoints ------
    QString loginEndpoint;
    QString portfolioEndpoint;
    QString technologyEndpoint;
    QString staticEndpoint;
    QString entityTechnologyEndpoint;
    QString projectImageEndpoint;
    QString projectEndpoint;
    QString featuresEndpoint;
    QString technicalInfoEndpoint;
};

#endif // APICLIENT_H
