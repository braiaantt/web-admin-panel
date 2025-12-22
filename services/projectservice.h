#ifndef PROJECTSERVICE_H
#define PROJECTSERVICE_H

#include <QObject>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include "apiclient.h"
#include "project.h"
#include "feature.h"
#include "technicalinfo.h"

class ProjectService : public QObject
{
    Q_OBJECT
public:
    ProjectService(ApiClient *apiClient, QObject *parent = nullptr);

    void createProject(const Project &project);
    void getProjectCover(int projectId, const QString &coverSrc);
    void getProjectFeatures(int projectId);
    void getProjectTechnicalInfo(int projectId);
    void getProjectImagePaths(int projectId);
    void updateProject(const Project &project, int projectId);
    void deleteProject(int projectId);

private slots:
    void createProjectFinished();
    void getProjectCoverFinished();
    void getProjectFeaturesFinished();
    void getProjectTechnicalInfoFinished();
    void getImagePathsFinished();
    void updateProjectFinished();
    void deleteProjectFinished();

signals:
    void projectCreated(int projectId);
    void projectCoverReceipt(int projectId, const QPixmap &pixmap);
    void projectFeaturesReceipt(const QVector<Feature> &feats);
    void projectTechnicalInfoReceipt(const QVector<TechnicalInfo> &info);
    void imagePathsReceipt(const QVector<QString> &paths);
    void projectUpdated(const Project &project);
    void projectDeleted();
    void errorOcurred(const QString &message);

private:
    ApiClient *apiClient;

    //------ Helpers ------
    QHttpMultiPart* makeProjectMultiPart(const Project &project);
    QString projectToStrJson(const Project &project);

    //------ Reply Handlers ------
    int handleCreateProject(const QByteArray &data);
    void handleProjectFeaturesReceipt(QVector<Feature> &container, const QByteArray &data);
    void handleProjectTechnicalInfo(QVector<TechnicalInfo> &container, const QByteArray &data);
    void handleGetImagePaths(QVector<QString> &container, const QByteArray &data);
    Project handleUpdateProjectFinished(const QByteArray &data);
    QByteArray getProjectUpdateBody(const Project &project);
};

#endif // PROJECTSERVICE_H
