// =========================================================================================================================
// XNL R36 Linux Partition Sizer - frmHelpandInfo code file
// =========================================================================================================================
// Copyright: © 2025 XNL Future Technologies
// Website:   https://www.teamxnl.com/R36-Linux-Partition-Sizer
// YouTube:   https://www.youtube.com/XNLFutureTechnologies
// For more (project and License) information please check mainwindow.cpp
// =========================================================================================================================

// NOTE: This is minimal (to not) commented, since it's basically just a form with a pre-defined rich textbox
//       using Qt Designer, only "actual code" going on here is opening/initializing the window and closing it ;)

#include "frmhelpandinfo.h"
#include "ui_frmhelpandinfo.h"

frmHelpAndInfo::frmHelpAndInfo(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::frmHelpAndInfo)
{
    ui->setupUi(this);
}

frmHelpAndInfo::~frmHelpAndInfo()
{
    delete ui;
}

void frmHelpAndInfo::on_cmdClose_clicked()
{
    this->close();
}

