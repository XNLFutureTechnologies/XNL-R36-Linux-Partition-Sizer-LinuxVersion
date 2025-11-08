// =========================================================================================================================
// XNL R36 Linux Partition Sizer - frmAbout header file
// =========================================================================================================================
// Copyright: © 2025 XNL Future Technologies
// Website:   https://www.teamxnl.com/R36-Linux-Partition-Sizer
// YouTube:   https://www.youtube.com/XNLFutureTechnologies
// For more (project and License) information please check mainwindow.cpp
// =========================================================================================================================
#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QDialog>

namespace Ui {
class frmAbout;
}

class frmAbout : public QDialog
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

public:
    explicit frmAbout(QWidget *parent = nullptr);
    ~frmAbout();

private slots:
    void on_btnClose_clicked();
    void OpenExternalLinkWithConfirm(const QString &MessageText, const QString &WebsiteURL);

private:
    Ui::frmAbout *ui;
};

#endif // FRMABOUT_H
