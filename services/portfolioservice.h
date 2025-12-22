#ifndef PORTFOLIOSERVICE_H
#define PORTFOLIOSERVICE_H

#include <QObject>
#include <QJsonArray>
#include <QHttpMultiPart>
#include "apiclient.h"
#include "portfolio.h"

class PortfolioService : public QObject
{
    Q_OBJECT
public:
    PortfolioService(ApiClient *apiClient, QObject *parent = nullptr);

    void getPortfolio();
    void getUserPhoto(const QString &path);
    void updatePortfolio(const Portfolio &portfolio);
    void updateUserPhoto(int portfolioId, const QString &srcPath);

signals:
    void portfolioReceipt(const Portfolio &portfolio);
    void userPhotoReceipt(const QPixmap& photo);
    void userPhotoUpdated(const QString &path);
    void portfolioUpdated();
    void errorOcurred(const QString &message);

private slots:
    void getPortfolioFinished();
    void updateUserPhotoFinished();
    void getUserPhotoFinished();
    void updatePortfolioFinished();

private:
    ApiClient *apiClient;

    //------ Request Handlers ------
    Portfolio handlePortfolioFinished(const QByteArray &data);
    QString handleUpdateUserPhoto(const QByteArray &data);

    //------ Helpers ------
    QHttpMultiPart* makeUserPhotoMultiPart(const QString &srcPath);
    QVector<Technology> getTechnologiesFromArray(const QJsonArray &array);
    QVector<Project> getProjectsFromArray(const QJsonArray &array);
    QByteArray getPortfolioUpdateBody(const Portfolio &portfolio);
};

#endif // PORTFOLIOSERVICE_H
