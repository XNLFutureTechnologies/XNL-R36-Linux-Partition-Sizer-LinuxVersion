// =========================================================================================================================
// XNL R36 Linux Partition Sizer - frmAbout code file
// =========================================================================================================================
// Copyright: © 2025 XNL Future Technologies
// Website:   https://www.teamxnl.com/R36-Linux-Partition-Sizer
// YouTube:   https://www.youtube.com/XNLFutureTechnologies
// For more (project and License) information please check mainwindow.cpp
// =========================================================================================================================

#include "frmabout.h"
#include "ui_frmabout.h"
#include <QLabel>
#include <QPixmap>
#include <QEvent>
#include <QDesktopServices>
#include <QMessageBox>

frmAbout::frmAbout(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::frmAbout)
{
    ui->setupUi(this);

    // Assigning the Pixmaps to the ImageLabel (essentially turning it into an 'image box')
    // This is an example of how to set an image to an label via code instead of using the Qt Designer :)
    QPixmap AboutImage(":/images/Images/AboutImg.jpg"); // First Creating the pixmap
    ui->imgAbout->setPixmap(AboutImage);                // Then setting the QLabel it's pixmap
    ui->imgAbout->setFrameShape(QFrame::NoFrame);       // Then removing the border/frame which I had set in the designer (for easier alignment)


    ui->LinkLabel1->installEventFilter(this);
    ui->LinkLabel2->installEventFilter(this);
    ui->LinkLabel3->installEventFilter(this);
    ui->LinkLabel4->installEventFilter(this);
    ui->LinkLabel5->installEventFilter(this);
    ui->LinkLabel6->installEventFilter(this);
    ui->LinkLabel7->installEventFilter(this);
    ui->LinkLabel8->installEventFilter(this);
    ui->LinkLabel9->installEventFilter(this);
    ui->LinkLabel10->installEventFilter(this);
}

frmAbout::~frmAbout()
{
    delete ui;
}

//============================================================================
// A small and simple helper which will FIRST ask the user if they actually
// do want to open a link (label) they clicked on in their browser, instead of
// just rudely opening a browser to one of my linked websites instead
//============================================================================
void frmAbout::OpenExternalLinkWithConfirm(const QString &MessageText, const QString &WebsiteURL)
{
    if (QMessageBox::question(this, "XNL R36 Linux Partition Sizer", MessageText, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        QDesktopServices::openUrl(QUrl(WebsiteURL));
    }
}

// These are simply the "LinkLabels" in the about dialog
bool frmAbout::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease){
        if (obj == ui->LinkLabel1) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to the website of the XNL R36 Linux Partition Sizer:\nhttps://www.teamxnl.com/R36-Linux-Partition-Sizer", "https://www.teamxnl.com/R36-Linux-Partition-Sizer");
            return true;
        }

        if (obj == ui->LinkLabel2) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to my Main Channel On YouTube:\nhttps://www.youtube.com/@XNLFutureTechnologies", "https://www.youtube.com/@XNLFutureTechnologies");
            return true;
        }

        if (obj == ui->LinkLabel3) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to my Second Channel On YouTube:\nhttps://www.youtube.com/@XNLFutureTechnologies2", "https://www.youtube.com/@XNLFutureTechnologies2");
            return true;
        }

        if (obj == ui->LinkLabel4) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to my Patreon Page:\nhttps://www.patreon.com/xnlfuturetechnologies", "https://www.patreon.com/xnlfuturetechnologies");
            return true;
        }

        if (obj == ui->LinkLabel5) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to the XNL R36S & R36H ArkOS Central, a 'central hub' I've created with a collection of information in regards to the R36S/H and a collection of links to my releases for the R36 units:\nhttps://www.teamxnl.com/r36s-r36h-arkos-central/", "https://www.teamxnl.com/r36s-r36h-arkos-central/");
            return true;
        }

        if (obj == ui->LinkLabel6) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to my (very detailed) tutorial on how to install or Update ArkOS for the R36S and R36H, which (near the end of the tutorial) also includes a section about this program:\nhttps://www.teamxnl.com/installing-or-updating-arkos-r36s-r36h/", "https://www.teamxnl.com/installing-or-updating-arkos-r36s-r36h/");
            return true;
        }

        if (obj == ui->LinkLabel7) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to the Official ArkOS page, wiki and downloads on GitHub:\nhttps://github.com/christianhaitian/arkos/wiki", "https://github.com/christianhaitian/arkos/wiki");
            return true;
        }

        if (obj == ui->LinkLabel8) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to the Community Maintained version of ArkOS for the R36S/R36H, maintained by AeolusUX:\nhttps://github.com/AeolusUX/ArkOS-R3XS", "https://github.com/AeolusUX/ArkOS-R3XS");
            return true;
        }

        if (obj == ui->LinkLabel9) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to my ArkOS Installation/Update tutorial but it will point directly to the DTB/'Display' Files section:\nhttps://www.teamxnl.com/installing-or-updating-arkos-r36s-r36h/#Step24dtbs", "https://www.teamxnl.com/installing-or-updating-arkos-r36s-r36h/#Step24dtbs");
            return true;
        }

        if (obj == ui->LinkLabel10) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to an excellent source of information when it comes to spotting fake/clone R36S units:\nhttps://handhelds.miraheze.org/wiki/R36S_Clones", "https://handhelds.miraheze.org/wiki/R36S_Clones");
            return true;
        }

    }
    return QWidget::eventFilter(obj, event);
}

void frmAbout::on_btnClose_clicked()
{
    this->close();
}

