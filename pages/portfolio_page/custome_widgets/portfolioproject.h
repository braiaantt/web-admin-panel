#ifndef PORTFOLIOPROJECT_H
#define PORTFOLIOPROJECT_H

#include <QWidget>
#include "project.h"

namespace Ui {
class PortfolioProject;
}

class PortfolioProject : public QWidget
{
    Q_OBJECT

public:
    explicit PortfolioProject(QWidget *parent = nullptr, const Project &project = Project());
    ~PortfolioProject();

    //------ Setters ------
    void setCoverImage(const QPixmap &pixmap);
    void setProject(const Project &project);

    //------ Getters ------
    const Project& getProject() const;

signals:
    void goToProject(const Project &project);

private slots:
    //------ UI Slots ------
    void on_pushButtonGoToProject_clicked();

private:
    Ui::PortfolioProject *ui;
    Project project;

    //------ Initialization ------
    void init();
};

#endif // PORTFOLIOPROJECT_H
