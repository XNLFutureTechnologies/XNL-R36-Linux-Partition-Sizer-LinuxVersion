// =========================================================================================================================
// XNL R36 Linux Partition Sizer - frmHelpAndInfo header file
// =========================================================================================================================
// Copyright: © 2025 XNL Future Technologies
// Website:   https://www.teamxnl.com/R36-Linux-Partition-Sizer
// YouTube:   https://www.youtube.com/XNLFutureTechnologies
// For more (project and License) information please check mainwindow.cpp
// =========================================================================================================================

#ifndef FRMHELPANDINFO_H
#define FRMHELPANDINFO_H

#include <QDialog>

namespace Ui {
class frmHelpAndInfo;
}

class frmHelpAndInfo : public QDialog
{
    Q_OBJECT

public:
    explicit frmHelpAndInfo(QWidget *parent = nullptr);
    ~frmHelpAndInfo();

private slots:
    void on_cmdClose_clicked();

private:
    Ui::frmHelpAndInfo *ui;
};

#endif // FRMHELPANDINFO_H
