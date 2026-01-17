// =========================================================================================================================
// XNL Future Technologies R36 Linux Partition Sizer
// =========================================================================================================================
// Purpose: This program is basically an (EXTREMELY xD) over-engineered tool to simply modify the file expandtoexfat.sh
// and change a simple numeric value in it and then saving it again with UTF-8 format. Nothing which could not easily be
// done with Kate (KDE) for example, I just wanted to make a small program for this to make it easier for my community
// to set a custom Linux/ArkOS partition size.
//
// At the same time I wanted to take this opportunity to make a small C++ (With Qt) program with LOTS of comments in
// it so beginners might learn a something from it on how I did things and why I did certain things.
//
//---------------------------------------------------------------------------------------------------------
// Copyright: © 2025 XNL Future Technologies
// Website:   https://www.teamxnl.com/R36-Linux-Partition-Sizer
// YouTube:   https://www.youtube.com/XNLFutureTechnologies
// YouTube 2: https://www.youtube.com/XNLFutureTechnologies2
// License:   Source code is only intended for educational purposes and to give people
//            the ability to 'take a look' what this application is doing. It CAN be used
//            to learn from and use snippets to make (a different) program from them for yourself and
//            also for public release, but it's not allowed to modify this program and then re-release
//            it as an alternative version (more explained on why bellow)
//            for the full license please visit the website of this project/program (listed above)
//
//  ## Third-Party Software Notices
//  This application uses the Qt 5 framework, which is licensed under the 
//  GNU Lesser General Public License (LGPL) version 3.
//  
//  - Qt Copyright: © The Qt Company Ltd and other contributors
//  - Qt License: LGPL v3 (https://www.gnu.org/licenses/lgpl-3.0.html)
//  - Qt Source Code: https://www.qt.io/download-open-source
//  - Qt Version Used: [5.15.13]
//  
//  Qt libraries are dynamically linked
//  For more information about Qt licensing, visit: https://www.qt.io/licensing/
//---------------------------------------------------------------------------------------------------------
//
// Reason why I don't allow alternative versions of this program in the license:
// Because like stated at the top already: It's just an over-engineered tool which basically (in the end)
// does nothing more than just changing one numeric value in a text file (.sh actually).
// And I don't see any need why this needs to be forked and re-published/released dozens of times when
// this program just does what it needs to do.
//
// Q: So I can't adapt it so that it also works for my device (which uses different files and such)?
// A: NO, because that would defeat the whole purpose of my application which makes it super clear that
//    it's intended for the R36S and R36H (and most likely also usable for the RG351MP). If there would
//    be dozens of variations of my program with just minor changes in the code and some slight graphical
//    adjustments it would only clutter the internet with dozens of variations and cause confusion, or it
//    will result in lots of repositories which are all at a different version, alternative version etc
//    without being transparent about WHAT has changed or not compared to the original
//    (which happens more than enough already these days in my opinion).
//
// Q: But what if I COMPLETELY re-do the UI, make sure there is no confusion with this version etc?
// A: IF it is indeed for a completely different device which would otherwise not be supported by my
//    version (also not when holding shift while starting the application), AND you are going to re-do
//    the application UI, make your own icons/images, remove my logo/branding from it etc, THEN it
//    won't be a problem. That would be an exception to the license limitation(s) for the source code.
//    HOWEVER, you are then still required to include credits in your source-code, your version has
//    has to be SHARE-A-LIKE (and thus also be published WITH open-source), and the compiled version
//    will need to include credits to this version and a link to the website of this version (the teamxnl.com page)
//    in for example the about screen (for example something like: Based on the XNL R36 Linux Partition Sizer).
//
//    But in my honest opinion? Just don't clutter the internet with yet another version of this program and
//    instead just make your own version which is specifically designed for your device(s) inspired on/by this one :)
//
//    IMPORTANT Note: The license 'restrictions' described above obviously only apply to the source code of this program,
//    not to the used Qt Libraries/framework (which are not included in the download btw, but just want to make this clear)
// =========================================================================================================================

#include "mainwindow.h"
#include "frmabout.h"
#include "frmhelpandinfo.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QMessageBox>
#include <QStorageInfo>
#include <QCloseEvent>
#include <QFile>
#include <QPixmap>
#include <QEvent>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QProcess>
#include <QRegularExpression>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusConnection>
#include <QTextDocument>

#pragma region "Global Declarations"
// Beginner tip: Making these variables static will ensure they are only "global" to this particular source file (the .cpp)
// This also ensures that if you have a larger project and later-on accidentally have two (completely different) variables with the same
// name in different source files, and one IS supposed to be 'exposed externally' that these won't 'clash' with the one which is supposed to
// be 'exposed' :) So if you like/prefer working with 'file global declarations' like I do, then make sure to make it a habit to declare them
// as static for local instances :)
static QString BOOTPartitionMountPoint = "";         // Used to 'store' the Boot Partition Mountpoint, so that we know from which mountpoint we'll need to read (and modify) the files
static bool BootedInIgnoreCompatibility = false;     // Used to register if users have booted the application while holding down shift to ignore the R36 Only limitations, and thus taking the risk themselves that they might mess up their Linux Configuration
static qint64 MaxDriveSizeLimit = 0;                 // Global Variable which will be used to calculate and set the maximum allowed size (via the slider) for the Linux Partition
static qint64 DetectedDriveSize = 0;                 // Global variable which will be used to store the detected total Drive size
static qint64 CurLinuxArkOSPartSize = 0;             // Global variable which will be used to store the minimal Linux/ArkOS partition size (retrieved from the initial boot files and set by the (d)ArkOS Developer)
static qint64 BootDriveSize = 0;                     // Global variable which will be used to store the boot partition size
static qint64 ReservedRomPartition = 5368709120;     // Mandatory minimal size (5GB/GiB in Base-2! 1GB = 1024MB) for the ROMS Partition (to prevent possible issues with temp files, other apps/games etc)
static qint64 BaseCalc = 1024;                       // The Application is by default configured to use Base-2 (1KB = 1024bytes, 1GB = 1024MB etc), but this can be changed with the radio buttons by the user
static int TuxClicks = 0;                            // Used to count the amount of clicks on the Tux-SD-Card within the time limit set by the timer to enable 'pro-mode' (a "hidden" feature")
static bool UsedBackupScript = false;                // Used to set if we've used the initial value(s) of a detected present backup script or not (used when for example a user decided he/she wanted to edit the boot script again after saving)
static bool XRLPHasMountedBootPartition = false;     // Used to keep track if this application has mounted the boot partition so that it's able to offer to unmount it again after an error or when it's finished
static QString MountedBootDevicePath = "";           // Used to store the device path of the mounted BOOT partition so we can use that to later unmount it again using UDisk2 (If the user wants to)
static bool isCtrlPressed = false;                   // Used to keep track of if the Control key is being held down (for both the easter-egg and hidden 'pro mode activation')
static bool isShiftPressed = false;                  // Used to keep track of if the Shift key is being held down (for both the easter-egg and hidden 'pro mode activation')
#pragma endregion

#pragma region "Main Form Routines"
//==================================================================================================================
// The Creation/Initialization of the MainWindow
//==================================================================================================================
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Commandline help arguments handling
    if (QCoreApplication::arguments().contains("--help") || QCoreApplication::arguments().contains("-help")) {
        printf("==========================================================\n");
        printf("XNL R36 Linux Partition Sizer Help\n");
        printf("© 2025 XNL Future Technologies\n");
        printf("Website: https://www.teamxnl.com/r36-linux-partition-sizer/\n");
        printf("YouTube: https://www.youtube.com/@XNLFutureTechnologies/\n");
        printf("==========================================================\n");
        printf("This program is an GUI Application (Qt based under LGPL)\n");
        printf("Please run it as a normal (desktop) application.\n\n");
        printf("Advanced Feature:\n");
        printf("--ignore-compat Will make the XNL R36 Linux Partition Sizer skip the checks for R36/RG351MP Specific (d)ArkOS bootfiles.\n");
        printf("Do however ONLY use this function if you REALLY know what your doing, and do realize this is at your own risk!\n");
        XNLExit();
    }

    ui->setupUi(this);

    // Just making sure that the textboxes (in the help and about screen) aren't black with black text if a user would be using
    // dark mode for their DE. I however also did not wanted to force white as background since that would be annoyingly blinding to
    // darm-mode users, so I decided to 'pick the middle ground' and go for a light-gray.
    qApp->setStyleSheet("QLineEdit, QTextEdit { background-color: #e8e8e8; color: black; }");

    // Adding an event filter to QLabels (which are also used to display images), so they can be
    // clickable with an 'triggered event'. Because in Qt items like labels don't have click events etc.
    ui->imgXNLLogo->installEventFilter(this);
    ui->imgBootPartition->installEventFilter(this);
    ui->imgLinuxPartition->installEventFilter(this);
    ui->imgRomsPartition->installEventFilter(this);
    ui->imgSDCard->installEventFilter(this);
    ui->lblSdCardSize->installEventFilter(this);
    ui->lblCardSize->installEventFilter(this);
    ui->lblDriveLttr->installEventFilter(this);
    ui->lblCalculationMode->installEventFilter(this);
    ui->lblMinimalLinuxPartitionSize->installEventFilter(this);
    ui->imgWarningIcon->installEventFilter(this);

    // Additional Clickable Labels (used for the website links and such)
    ui->LlblAbout->installEventFilter(this);
    ui->LlblHelp->installEventFilter(this);
    ui->LlblWebsite->installEventFilter(this);
    ui->LlblWebsite_2->installEventFilter(this);
    ui->LlblYouTube1->installEventFilter(this);
    ui->LlblYouTube2->installEventFilter(this);

    WarningIcon = QPixmap(":/images/Images/WarningIcon.png");

    // Make the warning icon invisible/hidden if it's not needed (I can't find this property in the
    // Qt Creator Property window for some reason so I'll just do it here :)
    ui->imgWarningIcon->hide();
    ui->gbProMode->hide();

    // Setup the timer which can be used for the blinking warning icon (if ignore-compat mode had been activated)
    tmrWarningTimer = new QTimer(this);
    tmrWarningTimer->setInterval(500);
    tmrWarningTimer->setSingleShot(false);

    // This timer is activated when the user clicks on the 'Tux SD-Card' while holding down shift+ctrl so that they
    // have to click 10 times on it before 'Pro Mode' activates. Each time the user clicks the timer will be reset
    // and they have another 1.2 seconds to click again until the 'clicks counter' resets back to 0.
    tmrProMode = new QTimer(this);
    tmrProMode->setInterval(1200);
    tmrProMode->setSingleShot(false);


    // Setup connections for UI elements
    setupConnections();

    // A small feature for people who do still want to use this application but NOT for the R36S/R36H, however this is 100%
    // at their own responsibility. Because I have not (and WILL NOT) test the script edits for other devices, and can't (and won't) confirm
    // it will work for other boot-scripts. (This is a commandline argument activated 'feature' intended for testing or advanced users ONLY!)
    if (QCoreApplication::arguments().contains("--ignore-compat")) {
        BootedInIgnoreCompatibility = true;
        ui->imgWarningIcon->show();        
        tmrWarningTimer->start();
        QMessageBox::warning(nullptr, "XNL R36 Linux Partition Sizer - Security Override!", "You've started the XNL R36 Linux Partition Sizer in 'Ignore R36 Detection mode' by using the (commandline) argument<br><b>--ignore-compat</b>!<br><br>This means that the XNL R36 Linux Partition Sizer will not particularly check for matching files which belong to the R36 image. However this program still needs certain files to be (almost) the same to be able to function properly! So even while you told it to ignore the R36 checks, it is still possible that the program does not work for your device and/or it's boot files!<br><br><b>PROCEED AT YOUR OWN RISK!</b>");
    }    

    // Now that UI is set up, detect SD cards and partitions
    detectBootDrive();
}

//==================================================================================================================
// Managing the close event of the application (so we can for example offer to unmount the R36 Boot partition if needed)
//==================================================================================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
    // Offer to unmount the boot partition again (because we've mounted it with this program)
    offerUnmountBootPartition();
    
    // Call base class implementation to actually close the window
    QMainWindow::closeEvent(event);
}

//==================================================================================================
// Part of the Qt Framework when using UI's, this is a 'destructor' which basically cleans up the
// UI when the MainWindow is destroyed
//==================================================================================================
MainWindow::~MainWindow() {
    delete ui;
}

//==================================================================================================================
// The 'other' (but more Qt integrated) way of connecting UI components/widgets to the code
//==================================================================================================================
void MainWindow::setupConnections() {
    // Connect slider to update function
    connect(ui->tbSizeAdjuster, &QSlider::valueChanged, this, &MainWindow::onSliderValueChanged);
    
    // Connect buttons
    connect(ui->btnApply, &QPushButton::clicked, this, &MainWindow::onApplyClicked);
    connect(ui->btnExit, &QPushButton::clicked, this, &QWidget::close);

    // Connect the (Dynamically in code!) created timer(s)
    connect(tmrWarningTimer, &QTimer::timeout, this, &MainWindow::onWarningTimerTimeout);
    connect(tmrProMode, &QTimer::timeout, this, &MainWindow::onProModeTimerTimeout);

    // The RadioButtons (for switching between base2 and base10)
    connect(ui->rbBase2, &QRadioButton::toggled, this, &MainWindow::onBaseRadioChanged);
    connect(ui->rbBase10, &QRadioButton::toggled, this, &MainWindow::onBaseRadioChanged);
    // NOTE: I could also have added the 'Link Labels' here, but I decided to do it differently so this program includes two methods as an example.
}
#pragma endregion

#pragma region "Timers"
//==================================================================================================================
// The 'Blinking Timer' for the Warning Icon when the Ignore (R36) Compatibility mode has been activated
//==================================================================================================================
void MainWindow::onWarningTimerTimeout() {
    // Here I'm NOT making the entire control/widget (QLabel) hide/show alternating
    // but instead I'm setting and clearing the image on it alternating. The reason I'm doing it this way
    // is because it them remains clickable at all times, even while the icon is "invisible" to the user.
    static bool BlinkWarningIcon = false;               // This static bool will persist across calls so if it's set to true, it will remember this in the next time it's called
    BlinkWarningIcon = !BlinkWarningIcon;               // Simply just setting the value inverted to its own value :)
    if(BlinkWarningIcon){
        ui->imgWarningIcon->setPixmap(WarningIcon);     // Set/Re-set the image
    }else{
        ui->imgWarningIcon->clear();                    // Clear the image
    }
}

//==================================================================================================================
// The timer which ensures that users have to click the 'Tux SD-Card' 10 times fast to enable the 'Pro Mode'
//==================================================================================================================
void MainWindow::onProModeTimerTimeout() {
    // After the timer ended it will reset the amount of TuxClicks
    tmrProMode->stop();
    TuxClicks = 0;
}
#pragma endregion

#pragma region "User/Form/UI Controls (Handling) Section"
//============================================================================
// A small and simple helper which will FIRST ask the user if they actually
// do want to open a link (label) they clicked on in their browser, instead of
// just rudely opening a browser to one of my linked websites instead
//============================================================================
void MainWindow::OpenExternalLinkWithConfirm(const QString &MessageText, const QString &WebsiteURL)
{
    if (QMessageBox::question(this, "XNL R36 Linux Partition Sizer", MessageText, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        QDesktopServices::openUrl(QUrl(WebsiteURL));
    }
}

//============================================================================
// Triggered when the radio buttons (for Base-2 and Base-10) are changed
//============================================================================
void MainWindow::onBaseRadioChanged()
{
    // Set BaseCalc depending on which radio is checked
    if (ui->rbBase10->isChecked()){
        BaseCalc = 1000;
    }else{
        BaseCalc = 1024;
    }
    // Then update the UI again
    UpdateUI();
}

//==================================================================================================
// The function which is connected to the movement of the 'size slider', moving the slider will
// trigger this function and then re-calculate the values which is needs to show on screen
//==================================================================================================
void MainWindow::onSliderValueChanged() {
    // So that we can change the unit labels depending on the set base calculation mode
    QString GBLabel = "GiB";
    // If the program has been set to base-10, well also use the 'unit label'
    if (BaseCalc == 1000)
    {
        GBLabel = "GB";
    }
    // Here we'll convert the value/ticks of the slider to 1MB per tick (in base-10 to bytes)
    qint64 AddedSize = static_cast<qint64>(ui->tbSizeAdjuster->value()) * 1000000;

    // Then we'll add the 'new MB's' as bytes to the CurLinPartSize so we can display it and maybe use it for processing the file later
    qint64 CurLinPartSize = CurLinuxArkOSPartSize + AddedSize;

    // Show the current bytes in the textbox (if it's not hidden)
    ui->txtNewSize->setText(QString::number(CurLinPartSize));

    // Show the current (selected) Linux/ArkOS partition size (again using either base-10 or base-2)
    ui->lblLinuxPartSize->setText("<center>" + QString::number((double)CurLinPartSize / (BaseCalc * BaseCalc * BaseCalc), 'f', 2) + GBLabel + "</center>");
    
    // Next we'll calculate the new ROM's partition size with the following steps:
    // - First we'll take the full detected size of the drive and 'scale it down' to GB's (using the selected base method)
    // - Then we'll substract the current value from the slider from this full SD-Card size (because the value of the slider basically indicated the desired size of the Linux Partition)
    // - And then we also substract the size of the already pre-defined bootdrive (it's just about 110MB or so (at the moment of writing this), but still, just to keep it a bit more accurate)
    double RomsPartSize = ((double)DetectedDriveSize / (BaseCalc * BaseCalc * BaseCalc)) - ((double)CurLinPartSize / (BaseCalc * BaseCalc * BaseCalc)) - ((double)BootDriveSize / (BaseCalc * BaseCalc * BaseCalc));

    // And then we'll update the label underneath the "ROM's SD-Card" with the currently selected size of the ROM's Partition
    ui->lblRomsPartSize->setText("<center>" + QString::number(RomsPartSize, 'f', 2) + GBLabel + "</center>");
}

//==================================================================================================
// The Apply Button
//==================================================================================================
void MainWindow::onApplyClicked() {
    // NOTE: I'm doing SOME basic checks to ensure that the drive is still there, hasn't been swapped with another one etc,
    // but I will not go to 'large extends' into doing so. If the user on purpose decided to circumvent the 'failsafe/idiot-proofing'
    // then it is just his/her own fault if stuff goes wrong ;) (Let's be honest: If you on purpose remove or swap the SD-Card
    // after the application has started and done it's safety checks, then you VERY WELL know what you are trying to do!)

    // Ask the user if he/she is happy with the currently set configuration or not and if he/she wants to proceed to write the modifications to the initial bootfiles or (d)ArkOS
    if (QMessageBox::question(this, "XNL R36 Linux Partition Sizer", 
        QString("You are about to configure the initial boot scripts of your (d)ArkOS installation for the following sizes:<br>"
                "Boot Partition: <b>%1</b> (Fixed size)<br>"
                "Linux/(d)ArkOS Partition: <b>%2</b><br>"
                "ROM's Partition: <b>%3</b><br><br>"
                "Are you sure that this is the desired configuration?<br><br>"
                "NOTE:<br><b>Modifying the initial boot scripts is of course <u>at your own risk</u>!</b><br><br>"
                "A backup of the original boot script will be created at:<br>"
                "<b>%4expandtoexfat.xbck</b>")
            .arg(stripHtml(ui->lblBootPartSize->text()), stripHtml(ui->lblLinuxPartSize->text()), stripHtml(ui->lblRomsPartSize->text()), stripHtml(ui->lblDriveLttr->text())), 
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    // first I'm checkin if all the required files (to Match the R36S/R36H) are still present on the disk
    QString dtbFilePath = BOOTPartitionMountPoint + "/rk3326-rg351mp-linux.dtb";
    QString FirstBootScriptPath = BOOTPartitionMountPoint + "/firstboot.sh";
    QString ExpandtoExfatScriptPath = BOOTPartitionMountPoint + "/expandtoexfat.sh";
    QString ExpandtoExfatScriptXNLBackupPath = BOOTPartitionMountPoint + "/expandtoexfat.xbck";

    if ((QFile::exists(dtbFilePath) || BootedInIgnoreCompatibility) && QFile::exists(FirstBootScriptPath) && QFile::exists(ExpandtoExfatScriptPath)) {
        // Then I will once more load the partition size set by the developer of the (d)ArkOS image which was used to flash the SD-Card
        qint64 CurLinuxArkOSPartSizeCheck = GetCurrentArkOSPartSize(BOOTPartitionMountPoint + "/expandtoexfat.sh");

        // I will check if the SD-Card still has the same size as when the application was started
        // Get the base device from MountedBootDevicePath (e.g., "/dev/sdb1" -> "/dev/sdb")
        QString baseDevicePath = MountedBootDevicePath;
        QRegularExpression re("^(/dev/[a-z]+)");
        QRegularExpressionMatch match = re.match(MountedBootDevicePath);
        if (match.hasMatch()) {
            baseDevicePath = match.captured(1);
        }

        // Now get the total size of the SD card
        qint64 DetectedDriveSizeCheck = 0;
        if (!baseDevicePath.isEmpty()) {
            QProcess lsblkSize;
            lsblkSize.start("lsblk", QStringList() << "-b" << "-o" << "SIZE" << "-n" << "-d" << baseDevicePath);
            lsblkSize.waitForFinished();
            DetectedDriveSizeCheck = lsblkSize.readAllStandardOutput().trimmed().toLongLong();
        }

        // When it turns out that physical the SD-Card size "magically changed" ;)
        if (DetectedDriveSizeCheck != DetectedDriveSize)
        {
            // Well tell the user that we refuse to continue because basically 'something fishy is going on'
            QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "The SD-Card size does not seem to match with the size of the SD-Card which was detected upon starting the XNL R36 Linux Partition Sizer!\n\nI don't know what you are trying to do, but this program is ONLY intended to be used with (d)ArkOS for use on the R36S/R36H for a reason.\n\nIf you just wanted to do another SD-Card for a different R36S/R36H then please just restart the XNL R36 Linux Partition Sizer to start over with a fresh set of data.\n\nSave operation canceled.");
            return;
        }

        // When it turns out that data in the initial bootfile "magically changed" compared to the data we've detected during the start of our application 
        // BUT, if the application has been started while we loaded the data (partially) from a backup already created by this program, then we'll ignore this check
        if (CurLinuxArkOSPartSizeCheck != CurLinuxArkOSPartSize && !UsedBackupScript)
        {
            QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "The data in the initial boot scripts for (d)ArkOS doesn't seem to match with the data which was detected upon starting the XNL R36 Linux Partition Sizer!\n\nI don't know what you are trying to do, but this program is ONLY intended to be used with (d)ArkOS for use on the R36S/R36H for a reason.\n\nIf you just wanted to do another SD-Card for a different R36S/R36H then please just restart the XNL R36 Linux Partition Sizer to start over with a fresh set of data.\n\nSave operation canceled.");
            return;
        }

        // If we haven't created a backup of the original boot script yet, we will do so now, otherwise we will just leave it as is
        // because we don't want multiple overwrites if a user for example keeps changing his/her mind with the partition size he/she wants to allocate
        if(!QFile::exists(ExpandtoExfatScriptXNLBackupPath)){
            // We'll try to make a backup of the original script, and if that fails we will bail out.
            if (!QFile::copy(ExpandtoExfatScriptPath, ExpandtoExfatScriptXNLBackupPath)) {
                // And if that fails, we'll notify the user AND refuse to resume (since we don't want to mess with the
                // bootfile/scripts if we can't even properly make a backup of the original!)
                QMessageBox::critical(this, "XNL R36 Linux Partition Sizer", QString("Could not create a backup of the original initial bootscript:<br><b>%1</b><br><br>Save operation canceled.").arg(ExpandtoExfatScriptPath));
                return;
            }
        }

        if (ModifyBootScript(ExpandtoExfatScriptPath, ui->txtNewSize->text(), this)) {
            // We'll also create a file on the boot partition called .xnlft-linpatext, this is a hidden file which stands for: 
            // XNL Ft. Linux Partition Ext
            // In this file we'll store the size with which we've increased the Linux partition,
            // this can then be used by my other programs running on the R36 like the XNL Package Manager for example to determine IF
            // it would be 'safe' to install very large packages without 'stealing' reserved storage space from future updates of (d)ArkOS
            qint64 AddedSize = static_cast<qint64>(ui->tbSizeAdjuster->value()) * 1000000;
            QFile outFile(BOOTPartitionMountPoint + ".xnlft-linpatext");
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                outFile.write(QString::number(AddedSize).toUtf8() + "\n");
                outFile.close();
                // NOTE: I did not included error handling if this extra file fails to write,
                // because it's not that big of deal and ONLY affects certain of my OWN projects, and thus NOT other users
                // in general. Thefor I don't want to bother others with errors about this.
            }
            
            // Tell the user we have successfully managed to update the bootscript
            QMessageBox::information(this, "XNL R36 Linux Partition Sizer", "The Initial Boot Files for your (d)ArkOS installation have successfully been updated.\n\nAfter unmounting you can remove the SD-Card from your computer, insert it into your R36S/R36H and (d)ArkOS will do the rest for you (re-sizing the partitions) upon it's first boot.\n\nThe XNL R36 Linux Partition Sizer will now exit.");
            // Offer to unmount the partition if we have mounted it with this application
            offerUnmountBootPartition();
            // And then just "XNL Exit" the application directly
            XNLExit();
        }else{
            // If we've failed to update the bootscript, we'll tell the user about this situation and what they MIGHT need to try
            QMessageBox::critical(this, "XNL R36 Linux Partition Sizer", "An unknown error occurred while trying to update the initial partition creation bootscript!\n\nPlease proceed with caution or manually restore the created backup expandtoexfat.xbck by renaming it back to expandtoexfat.sh\n\nIt might also be needed to just re-flash the entire (d)ArkOS image to your SD-Card (worse-case-scenario though).");
        }
    }else{
        QMessageBox::critical(nullptr, "XNL R36 Linux Partition Sizer", "For some reason the expected bootfiles for the R36S/R36H are no longer being detected as present on the SD-Card!\n\nI don't know what you are trying to do, but this program is ONLY intended to be used with (d)ArkOS for use on the R36S/R36H for a reason.\n\nIf you just wanted to do another SD-Card for a different R36S/R36H then please just restart the XNL R36 Linux Partition Sizer to start over with a fresh set of data.\n\nSave opperation canceled.");
        return;
    }
}

//============================================================================
// This is the customly assigned event filter which helps making QLabels clickable
// This is basically 'the manual way' instead of using connect. I PERSONALLY
// prefer using this method instead of the (more modern) 'connect system' if
// its for lots of smaller functions like help labels for example
//============================================================================
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Mouse Button Release Event
    if(event->type() == QEvent::MouseButtonRelease){
        // The XNL Ft Logo
        if (obj == ui->imgXNLLogo) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to my main YouTube Channel:\nhttps://www.youtube.com/@XNLFutureTechnologies\n\nDo you want to continue to visit my Main YouTube Channel?", "https://www.youtube.com/@XNLFutureTechnologies");
            return true;
        }

        // Opens a new dialog which is used as the About Dialog
        if (obj == ui->LlblAbout) {
            frmAbout aboutDialog(this);                             // 'this' sets the parent
            aboutDialog.setWindowModality(Qt::ApplicationModal);    // Optional: makes it modal
            aboutDialog.exec();                                     // Shows the dialog modally (like ShowDialog in C# for example)
            return true;
        }

        // Opens a new dialog which is used as the Help Dialog
        if (obj == ui->LlblHelp) {
            frmHelpAndInfo helpDialog(this);                        // 'this' sets the parent
            helpDialog.setWindowModality(Qt::ApplicationModal);     // Optional: makes it modal
            helpDialog.exec();                                      // Shows the dialog modally (like ShowDialog in C# for example)
            return true;
        }

        // Opens a browser to my main YouTube Channel (after first confirming IF the user would like to do so)
        if (obj == ui->LlblYouTube1) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to my main YouTube Channel:\nhttps://www.youtube.com/@XNLFutureTechnologies\n\nDo you want to continue to visit my Main YouTube Channel?", "https://www.youtube.com/@XNLFutureTechnologies");
            return true;
        }

        // Opens a browser to my Secondary YouTube Channel (after first confirming IF the user would like to do so)
        // BUT it's also including a small Pigeon (Easter) Egg :)
        if (obj == ui->LlblYouTube2) {
            if(ui->LlblYouTube2->text().contains("PIGEONS ARE AWESOME!!")){
                OpenExternalLinkWithConfirm("I KNOW RIGHT!? :D\n\nWe have a taken in a non-releasable rescue pigeon, and we're having an amazing time with her and her adventures.\nLots of my electronics and 3D projects are also for her or inspired by her btw :)\n\nWould you like to visit Sophie's own YouTube Channel?\n\nhttps://www.youtube.com/@SophieThePigeon", "https://www.youtube.com/@SophieThePigeon");
            }else{
                OpenExternalLinkWithConfirm("This will open a (new) browser window to my Secondary YouTube Channel:\nhttps://www.youtube.com/@XNLFutureTechnologies2\n\nDo you want to continue to visit my Secondary YouTube Channel?", "https://www.youtube.com/@XNLFutureTechnologies2");
            }
            return true;
        }

        // Opens a browser to the R36 Linux Partition sizer page (after first confirming IF the user would like to do so)
        if (obj == ui->LlblWebsite) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to the XNL R36 Linux Partition Sizer Website:\nhttps://www.teamxnl.com/R36-Linux-Partition-Sizer\n\nDo you want to continue to visit the XNL R36 Linux Partition Sizer Website?", "https://www.teamxnl.com/R36-Linux-Partition-Sizer");
            return true;
        }

        // Opens a browser to the R36 Central Page (after first confirming IF the user would like to do so)
        if (obj == ui->LlblWebsite_2) {
            OpenExternalLinkWithConfirm("This will open a (new) browser window to my 'R36S & R36H (d)ArkOS Central Page', this page basically contains all kinds of resources, information, links (both my own and external) and a collection of releases I've made for the R36S and R36H:\nhttps://www.teamxnl.com/r36s-r36h-arkos-central/\n\nDo you want to continue to visit the XNL R36S & R36H (d)ArkOS Central Page?", "https://www.teamxnl.com/r36s-r36h-arkos-central/");
            return true;
        }

        // Help/Info section of the Boot Partition Size
        if (obj == ui->imgBootPartition) {
            QMessageBox::information(this, "XNL R36 Linux Partition Sizer", "This SD-Card represents the BOOT Partition of your SD-Card, this partition basically contains the (like the name suggests) boot files needed to boot Linux/(d)ArkOS.\n\nThis is also the partition on which your dtb files (and \"display drivers\" simply put) are located.\n\nNOTE: You can't change the size of this boot partition, and neither should you use it for anything else.");
            return true;
        }

        // Help/Info section of the Linux Partition Size and also the 'hidden' "Pro-Mode" enabler
        if (obj == ui->imgLinuxPartition) {
            // First we'll see if we need to process the 'hidden feature clicks' when the user is clicking this
            // image/label while holding CTRL+Shift at the same time
            if(isShiftPressed && isCtrlPressed){
                // If there are less than 10 "TuxClicks"....
                if (TuxClicks < 10)
                {
                    // add +1 to the times the user has clicked tux
                    TuxClicks++;

                    // Reset the timer so we have another 1.2 seconds till the next required click
                    tmrProMode->stop();
                    tmrProMode->start();

                    if (TuxClicks >= 10){
                        // Timer can be disabled now
                        tmrProMode->stop();
                        
                        // Setting the value so high that this block won't activate a second time at all anymore
                        TuxClicks = 100;

                        // Showing the 'pro-mode' textbox and label (in a groupbox)
                        ui->gbProMode->show();

                        // Show the message that pro-mode has been enabled
                        QMessageBox::information(this, "XNL R36 Linux Partition Sizer - Hidden Feature", "'Pro-mode' Enabled!\n\nYou can now copy the bytes from the text field that appeared into the expandtoexfat.sh file yourself.\n\nWARNING: Do make sure you know what you are doing! and PLEASE make sure to read the help about editing that file if you want to make sure you won't mess it up!");
                    }
                }else{
                    // If the amount of clicks on Tux are already registered to be more than 10 (100 due to our 'lock' above) then we'll show a message that Pro-mode is already active
                    QMessageBox::information(this, "XNL R36 Linux Partition Sizer - Hidden Feature", "'Pro-mode' has already been enabled.\n\nYou can now copy the bytes from the text field that appeared into the expandtoexfat.sh file yourself.\n\nWARNING: Do make sure you know what you are doing! and PLEASE make sure to read the help about editing that file if you want to make sure you won't mess it up!");
                }
            }else{
                // And if the user isn't holding down ctrl+shift while clicking on the image/label,
                // we'll just show the normal informational dialog/popup
                QMessageBox::information(this, "XNL R36 Linux Partition Sizer", "This SD-Card represents the selected size of the Linux/(d)ArkOS Partition of your SD-Card after you save the changes with the XNL R36 Linux Partition Sizer.\n\nThe minimum allowed size for this partition has been set by the developer of the (d)ArkOS image you've used to flash this SD-Card.\n\nTIP: Click on help at the top right if you want some general tips/recommendations about the size options for your Linux partition.");
            }
            return true;
        }

        // Help/Info section of the Roms Partition Size
        if (obj == ui->imgRomsPartition) {
            QMessageBox::information(this, "XNL R36 Linux Partition Sizer", "This SD-Card represents the selected size of the ROM's partition (often labled EASYROMS) after you save the changes with the XNL R36 Linux Partition Sizer.\n\nWhen you increase the size of the Linux/(d)ArkOS partition (to for example make/reserve more space for additional software, mods, drivers etc), you will basically 'sacrifice' the extra amount you are assigning to Linux/(d)ArkOS from the ROM's partition.\n\nThe XNL Linux Partition Sizer has a hardcoded limit which prevents you shrinking the ROM's partition bellow 5GB. This to prevent possible compatibility issues with other applications, games, scripts etc.\n\nNOTE: This limit is set with base-2 (1GB = 1024MB).");
            return true;
        }

        // Help/Info section of the Displayed SD-Card Image (Top Left)
        if (obj == ui->imgSDCard || obj == ui->lblSdCardSize) {
            QMessageBox::information(this, "XNL R36 Linux Partition Sizer", "This is just an illustration, the size shown is an calculated estimation based on the detected (usable) size of your SD-Card.");
            return true;
        }

        // Help/Info section of the Total SD-Card size label
        if (obj == ui->lblCardSize) {
            QMessageBox::information(this, "XNL R36 Linux Partition Sizer", "This is the detected total size of the (d)ArkOS SD-Card you've flashed the image to.\n\nNOTE: It is normal if the drive shows less space than the card is advertised with (for example showing 31GB for a 32GB Card).\n\nThis simply put has to do with filesystem overhead and other technical things ;) (or the calculation method you've selected bellow).\n\nIf you noticed that the sizes don't add up perfectly (especially when using base-10), then please read the help to understand why.");
            return true;
        }

        // Help/Info section of the Minimal Linux Partition Size Label
        if (obj == ui->lblMinimalLinuxPartitionSize){
            QMessageBox::information(this, "XNL R36 Linux Partition Sizer", "This is the minimal size the Linux/ArkOS partition needs to be.\n\nThis size has been set by the developer of the (d)ArkOS Image you've used to flash your SD-Card with.\n\nYou are not be able make the Linux partition smaller than this value.");
            return true;
        }

        // YES! I know, it's NOT a drive letter in Linux ;), Just a name of the label which is left from the Windows port
        if (obj == ui->lblDriveLttr) {
            // Create a "Question Message Dialog" with two buttons (Yes and No), and then setting the No button as the default selected button.
            // however, this question dialog is created as an 'if block' because we directly 'read back' the dialog result (and thus which button the user clicked)
            if (QMessageBox::question(this, "XNL R36 Linux Partition Sizer", "This is the detected boot partition path.\n\nWould you like to open this location in your file manager to view the contents?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
            {
                // This method ensures it opens the path in the default system file browser, instead of for example
                // making it open Dolphin (which would only work on KDE based Distro's)
                QDesktopServices::openUrl(QUrl::fromLocalFile(ui->lblDriveLttr->text()));
            }
            return true;
        }

        // Help/Info section of the Calculation Mode Label (explaining the radio buttons)
        if (obj == ui->lblCalculationMode) {
            QMessageBox::information(this, "XNL R36 Linux Partition Sizer", "There are basically two ways to calculate storage size: base-10 and base-2.\n\nStorage manufacturers use base-10 when telling you that you for example bought a 128GB SD-Card.\n\nOperating systems like Windows and Linux actually use base-2 to calculate storage sizes.\n\nBase-10 (Decimal System):\nIn base-10 1GB = 1000MB\n\nBase-2 (Binary System):\nIn base-2 1GB = 1024MB\nNOTE: The official label for base-2 GB's is actually GiB.\n\nWhich is also the reason why you would for example only see/'get' about 119.2GB in Windows for an SD-Card labled as 128GB.");
            return true;
        }

        // The blinking warning icon (which is only visible in ignore (R36) compatibility mode)
        if (obj == ui->imgWarningIcon){
            QMessageBox::warning(this, "XNL R36 Linux Partition Sizer - Ignore Compatibility Mode ", "The XNL R36 Linux Partition Sizer has been started in a mode where it's told to ignore the checks for R36 speciffic boot files!\n\nIf you are able to see this message then it does mean that the program did still detected the firstboot.sh and expandtoexfat.sh scripts AND that your bootdrive is indeed also labeled BOOT, but this does NOT guarantee that the XNL R36 Linux Partition Sizer will work as expected for your device!\n\nUSE WITH CAUTION, PROCEED AT YOUR OWN RISK!");
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
#pragma endregion

#pragma region "Pigeon/Easter Egg Section"
//============================================================================
// Pigeon (Easter) Egg Section
//============================================================================
void MainWindow::CheckPigeonEgg()
{
    // If one of the keys is no longer being held down, then we'll revert back to the normal link
    if (!isCtrlPressed || !isShiftPressed)
    {
        ui->LlblYouTube2->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:8pt; text-decoration: underline; color:#2980b9;\">XNL Future Technologies YouTube (Secondary)</span></p></body></html>");
    }
}

//==================================================================================================
// These trigger key events (key releases in this instance)
// NOTE: Do not forget to declare the (protected) overrides in the header (.h) file when doing so :)
//==================================================================================================
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Checking if the Control key is being released
    if (event->key() == Qt::Key_Control){
        isCtrlPressed = false;  // If so, we'll register it's no longer being held
    }
    // Same as above but then for the shift key
    if (event->key() == Qt::Key_Shift){
        isShiftPressed = false;
    }

    // Update the "Pigeon Easter Egg"
    CheckPigeonEgg();

    // And then "returning the event to the system" for proper handling
    QMainWindow::keyReleaseEvent(event);
}

//==================================================================================================
// These trigger key events (key presses in this instance)
// NOTE: Do not forget to declare the (protected) overrides in the header (.h) file when doing so :)
//==================================================================================================
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Checking if the Control key is being pressed...
    if (event->key() == Qt::Key_Control){
        isCtrlPressed = true;   // And if so, register it as being held down
    }
    // Same as above but for the shift key
    if (event->key() == Qt::Key_Shift){
        isShiftPressed = true;
    }

    // Set the label with the PIGEONS ARE AWESOME easter egg text, including
    // html styling of the label
    if (isCtrlPressed && isShiftPressed) {
        ui->LlblYouTube2->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:8pt; text-decoration: underline; color:#2980b9;\">PIGEONS ARE AWESOME!!</span></p></body></html>");
    }

    // And then "returning the event to the system" for proper handling
    QMainWindow::keyPressEvent(event); // Call base class implementation
}

//==================================================================================================
// Simple 'Activation Change Detection' to detect focus change/loss of the form
// so we can reset the 'hidden' easter egg button state
//==================================================================================================
void MainWindow::changeEvent(QEvent *event)
{
    // If the application activation has changed...
    if (event->type() == QEvent::ActivationChange) {
        // We'll check if the application/current form is the active window...
        if (this->isActiveWindow()) {
            // If so, then we'll reset the states of the pressed buttons.
            // This prevents the 'easter egg' to become stuck as permanently activated after
            // it has shown the pop-up dialog/messagebox
            isCtrlPressed = false;
            isShiftPressed = false;
            CheckPigeonEgg();
        }
    }

    // Then 'hand over' the event back to 'the system' so they will (all) will be processed normally
    QMainWindow::changeEvent(event);
}
#pragma endregion

#pragma region "XNL Program Functions"
//==================================================================================================
// Helper function which can (well should be able to ;)) 'auto mount' The SD-Card partitions in the
// same way the filemanager of the distro does it (for example Dolphin on an KDE distro).
// Using this method SHOULD (as far as I know) work on nearly all desktop distro's without
// having to rely on creating mount points manually (which could cause problems in some cases)
//==================================================================================================
int MainWindow::tryMountBootPartition(const QString &devicePath)
{
    // Show question dialog
    QString dialogText = QString("It SEEMS like the XNL R36 Linux Partitioner has found a usable R36 SD-Card: <b>%1</b>.<br><br>However, this BOOT partition isn't mounted yet. Mounting this partition is needed to check for the correct files and to be able to modify the files on it.<br><br>Would you like to (try and) mount the BOOT partition using the XNL R36 Linux Partitioner?<br><br>NOTE: This will not make any changes to your system, nor will it make any changes to your SD-Card (yet).").arg(devicePath);
    QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "BOOT Partition Mount Required", dialogText, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    // And try to mount of the reply to this message/question dialog was Yes...
    if (reply == QMessageBox::Yes) {
        // Prepare the device path (from the passed path as parameter)
        QString devName = devicePath.split("/").last();
        QString udisksObjPath = "/org/freedesktop/UDisks2/block_devices/" + devName;

        // Create the interface which we will use to mount the partition
        QDBusInterface iface(
            "org.freedesktop.UDisks2",
            udisksObjPath,
            "org.freedesktop.UDisks2.Filesystem",
            QDBusConnection::systemBus()
        );

        // Try to request the mounting process and store the result/reply in a variable
        QDBusReply<QString> mountReply = iface.call("Mount", QVariantMap());
        
        // Finally we'll check if the mount has completed correctly or not
        if (mountReply.isValid()) {
            BOOTPartitionMountPoint = mountReply.value();
            return 0;   // The return code if mounting has succeeded 
        } else {
            return 50;  // The error code I'm using if mounting has failed
        }
    }else{
        return 100; // The 'error code' I'm using when the user has refused to mount
    }
}

//==================================================================================================
// The function which tries to accurately detect the R36 SD-Card and it's partitions, and also
// tries to auto mount it if needed, and reads the config/scripts on it when they are present
// Yes, It's called detectBootDrive while it also checks for other partitions to be present, but
// that is because we are basically only working with the bootdrive/partition and not with the
// other partitions
//==================================================================================================
void MainWindow::detectBootDrive() {
    int correctBootDrivesFound = 0;

    QProcess lsblk;
    lsblk.start("lsblk", QStringList() << "-b" << "-o" << "NAME,LABEL,FSTYPE,SIZE" << "-P");
    lsblk.waitForFinished();
    QString output = lsblk.readAllStandardOutput();

    QRegularExpression partRx("NAME=\"([^\"]+)\" LABEL=\"([^\"]*)\" FSTYPE=\"([^\"]*)\" SIZE=\"([^\"]+)\"");
    QMap<QString, QSet<QString>> drivePartitions;
    QMap<QString, QMap<QString, QString>> driveLabels;
    QMap<QString, QMap<QString, QString>> driveFstypes;
    QMap<QString, QMap<QString, qint64>> driveSizes;

    QRegularExpressionMatchIterator it = partRx.globalMatch(output);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString name = match.captured(1);
        QString label = match.captured(2);
        QString fstype = match.captured(3);
        qint64 size = match.captured(4).toLongLong();

        QRegularExpression driveRx("^(sd[a-z]+)");
        QRegularExpressionMatch driveMatch = driveRx.match(name);
        if (driveMatch.hasMatch()) {
            QString drive = driveMatch.captured(1);
            drivePartitions[drive].insert(name);
            driveLabels[drive][name] = label;
            driveFstypes[drive][name] = fstype;
            driveSizes[drive][name] = size;
        }
    }
    qint64 CurrentSetUserSize = 0;

    for (const QString &drive : drivePartitions.keys()) {
        bool hasBoot = false, hasRoot = false, hasEasyroms = false;
        for (const QString &part : drivePartitions[drive]) {
            QString label = driveLabels[drive][part];
            QString fstype = driveFstypes[drive][part];
            QString DetectedMountPoint = "";
            if (label == "BOOT" && fstype == "vfat"){

                BootDriveSize = driveSizes[drive][part];

                // part is the device name, e.g. "sdd1"
                QString devicePath = "/dev/" + part;
                bool isMounted = false;
                // Checking if the detected device is actively mounted or not (so we can access the files)
                for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
                    if (storage.device() == devicePath) {
                        isMounted = true;
                        DetectedMountPoint = storage.rootPath();    // IF it's already mounted we will store the current mountpoint into a string
                        MountedBootDevicePath = devicePath;
                        break;
                    }
                }
                
                // Create the variable as 'noninitialized'
                int MountAttemptResult = -1;
                if (!isMounted) {
                    // Partition is not mounted, and thus ask the user if they want to try and automount or not
                    MountAttemptResult = tryMountBootPartition(devicePath);
                    
                    if(MountAttemptResult == 0){   // Mounting succeeded
                        XRLPHasMountedBootPartition = true; // Register that we have mounted the partition so that we can offer to unmount it again when we're done (or on an error where we can't proceed)
                        MountedBootDevicePath = devicePath;
                        QMessageBox::information(nullptr, "Mount Success", "R36 BOOT partition mounted at: <b>" + BOOTPartitionMountPoint + "</b><br><br>The XNL R36 Linux Partitioner will now check for the required R36 Boot Files (and the other partitions which need to be present).");
                    }
                    
                    if(MountAttemptResult == 50){  // Mounting failed (when user did allowed/asked to mount it)
                        QMessageBox::critical(nullptr, "Mount Failed", "Could not mount the BOOT partition.\n\nMake sure you have permission to mount removable drives.");
                    }
                    
                    if(MountAttemptResult == 100){  // Mounting failed due to the user refusing XNL R36 Linux Partitioner Mounting it
                        QMessageBox::critical(nullptr, "Mount Required To Proceed", "Boot drive not mounted, please keep in mind that you will NEED to mount the boot drive/partition to be able to configure the (d)ArkOS installation script.\n\nThe XNL R36 Linux Partition Sizer will now resume searching for more BOOT partitions, but if it doesn't find any it will exit due to the fact it doesn't have any (mounted) boot drives to work with.");
                    }
                }else{
                    BOOTPartitionMountPoint = DetectedMountPoint;
                    // Even though we didn't auto mounted it ourselves, we do register it as a successful mount so that the program
                    // knows it can (safely) resume checking for the files to be present (and their contents)
                    MountAttemptResult = 0;
                }

                if(MountAttemptResult == 0){
                    // Even though we did made it "all the way through" the BOOT Partition detection, we're not there yet ;)
                    // We will still need to detect IF it actually has the bootfiles which belong to the R36/RG351MP/Fresh (d)ArkOS Image
                    
                    // So first we'll check if the dtb file is present or not, which should have this name if it's the rg351mp image which has
                    // been used as explained in my tutorial (since this tool is also intended to be used with that image, and my tutorial ;) )
                    // NOTE: This specific check for the files CAN be ignored if desired, but is COMPLETELY at own risk of the user and NOT recommended!
                    QString dtbFilePath = BOOTPartitionMountPoint + "/rk3326-rg351mp-linux.dtb";
                    QString FirstBootScriptPath = BOOTPartitionMountPoint + "/firstboot.sh";
                    QString ExpandtoExfatScriptPath = BOOTPartitionMountPoint + "/expandtoexfat.sh";
                    QString ExpandtoExfatScriptXNLBackupPath = BOOTPartitionMountPoint + "/expandtoexfat.xbck";
                    if (QFile::exists(dtbFilePath) || BootedInIgnoreCompatibility) {
                        if (QFile::exists(FirstBootScriptPath) && QFile::exists(ExpandtoExfatScriptPath)){
                            // Now we'll check if BOTH the firstboot.sh and expandtoexfat.sh script are (still) present on the boot partition of the SD-Card,
                            // if this isn't the case, then this card most likely has already been used to boot an R36/RG351MP etc, and thus we will no longer be able
                            // to manipulate the (d)ArkOS "first initialization script" which resizes the partitions

                            // Detect if the SD-Card already contains a backup of the original boot files, this would indicate that the
                            // user had previously edited the files already but for example decided to make some additional adjustments.
                            // this is for example needed to prevent that the minimal size won't be locked to their manually set size
                            if (QFile::exists(ExpandtoExfatScriptXNLBackupPath))
                            {
                                // Get the currently configured Linux Partition Size (from the backupped Initial Boot Scripts of (d)ArkOS)
                                CurLinuxArkOSPartSize = GetCurrentArkOSPartSize(BOOTPartitionMountPoint + "/expandtoexfat.xbck");
                                // Register that we are working with a value from the backup script instead of the 'main script'
                                // otherwise safety checks during saving will fail!
                                UsedBackupScript = true;

                                // We'll ALSO load the already modified file but this is so that we can 'reload' the slider position to the
                                // position the user had previously set it to.
                                CurrentSetUserSize = GetCurrentArkOSPartSize(BOOTPartitionMountPoint + "/expandtoexfat.sh");
                            }
                            else
                            {
                                // Get the currently configured Linux Partition Size (from the ORIGINAL Initial Boot Scripts of (d)ArkOS)
                                CurLinuxArkOSPartSize = GetCurrentArkOSPartSize(BOOTPartitionMountPoint + "/expandtoexfat.sh");
                            }
                            if (CurLinuxArkOSPartSize > 0)
                            {
                                // We'll register that we've found a matching boot drive that is (well should be) ready to modify
                                hasBoot = true;
                                
                                // This is NOT just the boot partition or the (d)ArkOS partition but the entire SDCard it's size!
                                QProcess lsblkSize;
                                lsblkSize.start("lsblk", QStringList() << "-b" << "-o" << "SIZE" << "-n" << "-d" << QString("/dev/%1").arg(drive));
                                lsblkSize.waitForFinished();
                                DetectedDriveSize = lsblkSize.readAllStandardOutput().trimmed().toLongLong();

                                ui->lblSdCardSize->setText("<html><head/><body><p><span style=\" font-size:24pt; color:#ead4f1;\">" + EstimateSDCardSize(DetectedDriveSize) + "</span></p></body></html>");
                            
                                // NOTE For those comparing (or having previously used) the C# version of my program:
                                // The "Three Partitions Count Test" is no longer done here in this port but a little bit further
                                // around the line: if (hasBoot && hasRoot && hasEasyroms){
                                // HOWEVER, this C++ port no longer explicitly states that it's missing one of the other partitions required to identify the
                                // R36 SD-Card, but just says it didn't found a suitable bootdrive (which in my opinion is perfectly fine also for this purpose)
                            }else
                            {
                                // The error when parsing ('converting') the detected string can't be converted properly to a numeric value
                                if (CurLinuxArkOSPartSize == -2)
                                {
                                    // Show a message box which will explain that we could not properly parse the data from the script file from the SD-Card.
                                    // This could be due to an incompatible script, modified script, corrupted file, basically about anything.
                                    QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Sorry, can't continue!\n\nThere was an error while trying to parse the required Linux partition size from the expandtoexfat script.\n\nThis could either mean that your script is not compatible with this program (anymore), that you are trying to modify a bootdrive which this program isn't intended for, or that for example the (d)ArkOS Developer(s) have changed the file so drastically that The XNL R36 Linux Partition Sizer now needs an update to be compatible (again).\n\nThe XNL R36 Linux Partition Sizer will now exit.\n");
                                    XNLExit();
                                }
                                // The error when when the required startMarker or endMarker could not be found in the expandtoexfat script (most likely indicating that the developer has changed the script significantly)
                                if (CurLinuxArkOSPartSize == -3)
                                {
                                    // Show a message box which will explain that we could not detect the required data in the expandtoextfat script
                                    QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Sorry, can't continue!\n\nCould not detect the required data in the expandtoexfat script.\n\nThis could most likely mean that the (d)ArkOS developer has changed the script and/or it's internal commands in such a way that the XNL R36 Linux Partition Sizer first requires an update before it is compatible again.\n\nIt COULD however also mean that you are trying to modify a boot drive which this application is not compatible with.\n\nThe XNL R36 Linux Partition Sizer will now exit.\n");
                                    XNLExit();
                                }

                                // Show a message box which will explain that we encountered an unknown error while trying to process the expandtoexfat script
                                QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Sorry, can't continue!\n\nAn unknown exception occurred while trying to process the expandtoexfat script from the SD-Card.\n\nPlease remove the SD-Card, reinsert it and try again. If the problem persists, then you could also try to re-flash the (d)ArkOS image to your SD-Card.\n\nThe XNL R36 Linux Partition Sizer will now exit.\n");
                                XNLExit();
                            }
                        }else{
                            // If we DID found the expected dtb file but we did NOT find the firstboot files, then this means
                            // that this card has already been setup by the (d)ArkOS and thus the partitioning already took place
                            // In this instance we will just outright refuse to modify the partitions, because we don't want to be
                            // the possible cause for data and/or savegame loss etc.
                            QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Sorry, can't continue!\n\nWhile the correct drive and boot file(s) where detected for the R36S/R36H, it seems like it that this SD-Card has already been through the initial boot process!\n\nThe XNL R36 Linux Partition Sizer needs to be used DIRECTLY after you've flashed the image to the SD-Card, and BEFORE you insert it into the R36S/R36H for the first time.\n\nThe XNL R36 Linux Partition Sizer will now exit.\n");
                            
                            // Offer to unmount the boot partition again (because we've mounted it with this program)
                            offerUnmountBootPartition();
                            XNLExit();
                        }
                    }else{
                            // If we did NOT found the expected dtb after mounting (or while the partition was already mounted), then we will also bail out
                            // unless the program is started in --ignore-compat mode. However even in ignore-compat, the program still needs the two sh scripts to exist!
                            QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Sorry, can't continue!<br><br>While the drive structure seems to be correct, the XNL R36 Linux Partition Sizer can't seem to find the expected device tree blob (DTB): <b>rk3326-rg351mp-linux.dtb</b><br><br>The XNL R36 Linux Partition Sizer will now exit.<br>");
                            
                            // Offer to unmount the boot partition again (because we've mounted it with this program)
                            offerUnmountBootPartition();
                            XNLExit();
                    }
                    
                    ui->lblDriveLttr->setText(BOOTPartitionMountPoint);

                }
            }else if ((label == "EASYROMS" && fstype == "ntfs") || (label == "EASYROMS" && fstype == "vfat")){
                hasEasyroms = true;
            }else if (fstype == "ext4" || fstype == "btrfs"){ // Added btrfs for dArkOS support!
                hasRoot = true;
            }
        }

        // If the SD-Card has all three required partitions to qualify, we'll count it as a valid SD-Card/BootDrive
        if (hasBoot && hasRoot && hasEasyroms){
            correctBootDrivesFound++;
        }
    }

    if (correctBootDrivesFound < 1)
    {
        if(BootedInIgnoreCompatibility){
            QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Could not detect a (suitable) R36S/R36H SD-Card and/or boot partition!<br><br>To use the XNL R36 Linux Partition Sizer you will need to run this program DIRECTLY after flashing the (d)ArkOS image to the SD-Card and BEFORE inserting (and booting) it into the R36S/R36H for the first time.<br><br>Note: If you are experiencing detection issues, you might try to remove the SD-Card from the computer, re-insert it and then try to run the XNL R36 Linux Partition Sizer again.<br><br>The XNL R36 Linux Partition Sizer will now exit.<br>");
        }else{
            QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Could not detect a (suitable) R36S/R36H SD-Card and/or boot partition!<br><br>To use the XNL R36 Linux Partition Sizer you will need to run this program DIRECTLY after flashing the (d)ArkOS image to the SD-Card and BEFORE inserting (and booting) it into the R36S/R36H for the first time.<br><br>Note: If you are experiencing detection issues, you might try to remove the SD-Card from the computer, re-insert it and then try to run the XNL R36 Linux Partition Sizer again.<br><br>NOTE 2: This program does NOT support card readers which use a device path like /dev/mmcblk...<br><br>IF you are trying to use this program for another device than the R36, then you can TRY to force it to continue anyway by starting this program from the terminal using the argument:<br><b>--ignore-compat</b><br><br>The XNL R36 Linux Partition Sizer will now exit.<br>");
        }

        // Offer to unmount the boot partition again (because we've mounted it with this program)
        // This call SHOULD not be needed here, but as safety fallback I did included it here
        offerUnmountBootPartition();
        XNLExit();
        return;
    }
    else if (correctBootDrivesFound > 1)
    {
        QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "More than one suitable R36S/R36H modifiable (and mounted) SD-Card detected!\n\nThe XNL R36 Linux Partition Sizer will only allow one R36S/R36H modifiable SD-Card to be inserted in your computer/card-reader at the same time to prevent possible issues or incorrect editing of boot scripts!\n\nPlease make sure that you only have one R36S/R36H SD-Card inserted and then restart the XNL R36 Linux Partition Sizer.\n\nThe XNL R36 Linux Partition Sizer will now exit.\n");
        // Offer to unmount the boot partition again (because we've mounted it with this program)
        // This call SHOULD not be needed here since it should already bail-out when a boot-drive has been mounted but the file structure doesn't match, but as safety fallback I did included it here
        offerUnmountBootPartition();
        XNLExit();
        return;
    }
    else{
        // Making sure that the SD-Card (it's total size) is atleast 32GB to ensure we do have enough 'headroom'
        // to alter the size and still (at least) have 5GB for roms (because we won't allow to turn the entire card into Linux with my tool)
        if (DetectedDriveSize < 30089095168)
        {
            // Yup, I DO realize that we are actually checking for 30GB here and not exactly 32GB, however this is to account for
            // allocation tables, 10-base vs 2-base etc. And considering that the COMMON SD-Card sizes are:
            // 2GB, 4GB, 8GB, 16GB, 32GB, 64GB, 128GB, 256GB, 512GB, 1TB etc etc, this method should not cause problems for 99.5% of the users
            QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Sorry, but The XNL R36 Linux Partition Sizer <b>needs at least a 32GB SD-Card</b> to have a decent (and usable) effect!<br><br>It is however recommended for more serious use of these options that you would use a 64GB or even a 128GB SD-Card.<br><br>The XNL R36 Linux Partition Sizer will now exit.<br>");

            // Offer to unmount the boot partition again (because we've mounted it with this program)
            // This call SHOULD not be needed here since it should already bail-out when a boot-drive has been mounted but the file structure doesn't match, but as safety fallback I did included it here
            offerUnmountBootPartition();
            XNLExit();
            return;
        }else{
            // Update the user interface/GUI
            UpdateUI();

            // If we detected that we are using data from a backup script (indicating another round of editing)
            if (UsedBackupScript)
            {
                // And the "re-loaded" user set value is loaded correctly AND the loaded original (d)ArkOS partition size (set by the image Developer) are correct
                if (CurrentSetUserSize >= 0 && CurLinuxArkOSPartSize > 0)
                {
                    // then we'll going to make sure that the slider (re-)loads at the correct position again.
                    long SliderValue = (CurrentSetUserSize - CurLinuxArkOSPartSize) / 1000000;
                    ui->tbSizeAdjuster->setValue(static_cast<int>(SliderValue));                    
                }
                else
                {
                    // If we detected a possible issue with the back up, then we won't allow re-editing of the file(s) either anymore!
                    QMessageBox::critical(nullptr, "XNL R36 Linux Partition Resizer - Can't Continue", "Sorry, but it seems like it that something is going wrong!\n\nThe XNL R36 Linux Partition Sizer isn't able to properly (re-)load of your previous edit of this boot drive!\n\nYou could manually try to revert the back up by removing expandtoexfat.sh and renaming expandtoexfat.xbck to expandtoexfat.sh after that, but it might also be required to re-flash the (d)ArkOS image to your SD-Card.\n\nThe XNL R36 Linux Partition Sizer will now exit.\n");

                    // Offer to unmount the boot partition again (because we've mounted it with this program)
                    // This call SHOULD not be needed here since it should already bail-out when a boot-drive has been mounted but the file structure doesn't match, but as safety fallback I did included it here
                    offerUnmountBootPartition();
                    XNLExit();
                }
            }
        }
    }
}

//==================================================================================================
// A simple helper function which will trigger a question dialog when the program is about to close
// if the program had previously mounted the BOOT partition of the R36 asking the user if he/she
// would like to unmount the partition again upon exit.
// NOTE: This obviously only works for the current program session, meaning that if you start XRLPS
// and then close it while answering no upon the 'Want to unmount request' and then restart XRLPS,
// that it then won't ask if you want to unmount (because it was basically already mounted during
// the start of the program)
//==================================================================================================
void MainWindow::offerUnmountBootPartition()
{
    // IF the program has mounted the R36 Boot Partition AND we know where it has mounted it (from)...
    if (XRLPHasMountedBootPartition && !MountedBootDevicePath.isEmpty()) {
        // Then ask if the user wants to unmount it again after we're done (or after an error)
        if (QMessageBox::question(
                nullptr,
                "XNL R36 Linux Partition Sizer - Unmount Boot Partition",
                "The XNL R36 Linux Partition Sizer mounted the BOOT partition for you.<br><br>"
                "Would you like to try and unmount it again?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes) == QMessageBox::Yes)
        {
            // And if the user has replied yes, we'll use udisksctl to request to unmount the path we previously stored in memory
            QProcess unmountProc;
            unmountProc.start("udisksctl", QStringList() << "unmount" << "-b" << MountedBootDevicePath);
            // Wait (halt the application) until the unmounting has finished
            unmountProc.waitForFinished();

            // Then we'll check the return/exit code to see if the unmounting has succeeded or not...
            if (unmountProc.exitCode() == 0) {
                // If unmounting succeeded we'll tell the user the partition has been unmounted
                QMessageBox::information(nullptr, "XNL R36 Linux Partition Sizer - R36 Boot Unmounted", "The BOOT partition was successfully unmounted.");
            } else {
                // And if it failed we'll notify the user he/she might need to unmount it themselves manually
                QMessageBox::warning(nullptr, "XNL R36 Linux Partition Sizer - R36 Unmounted Failed", "Failed to unmount the BOOT partition.\n\nYou may need to unmount it manually.");
            }
        }
    }
}

//==================================================================================================
// This is basically the update function which updates the labels on the GUI/Form so that the user
// can see what the current (slider) settings will result in when they would save/update the script
//==================================================================================================
void MainWindow::UpdateUI()
{
    // So that we can change the unit labels depending on the set base calculation mode
    QString MBLabel = "MiB";
    QString GBLabel = "GiB";
    // If the program has been set to base-10, well also use the 'unit label'
    if (BaseCalc == 1000)
    {
        MBLabel = "MB";
        GBLabel = "GB";
    }
    
    // This function is called to update the User Interface after the use has either moved the slider,
    // or when he/she has changed the 'calculation method' (base-10 vs base-2)
    MaxDriveSizeLimit = DetectedDriveSize - BootDriveSize - ReservedRomPartition;
    double SliderMax = (long)(MaxDriveSizeLimit - CurLinuxArkOSPartSize) / 1000000;
    ui->tbSizeAdjuster->setMaximum((int)SliderMax);

    // CURRENTLY not even necessary, but here we'll check if the boot drive should
    // be labeled with MB or GB. (Although I don't suspect it will EVER needs to be GB, but just future proofing it)
    double BootPartSize = (double)BootDriveSize / (BaseCalc * BaseCalc);
    if (BootPartSize >= BaseCalc)
    {
        ui->lblBootPartSize->setText("<center>" + QString::number(BootPartSize / BaseCalc, 'f', 0) + GBLabel + "</center>");
    }
    else
    {
        ui->lblBootPartSize->setText("<center>" + QString::number(BootPartSize, 'f', 0) + MBLabel + "</center>");
    }
    
    // Show the current Minimal Linux/ArkOS partition size (and represent it on screen using the set calculation method base-10 or base-2)
    ui->lblMinimalLinuxPartitionSize->setText(QString::number((double)CurLinuxArkOSPartSize / (BaseCalc * BaseCalc * BaseCalc), 'f', 2) + GBLabel);

    // If the user choose to use base 10 configuration then we'll add a plus-minus infront of the detected size to indicate that it's
    // not fully accurate due to the calculation methods used (conversions from long to integers etc)
    QString PrefixDetectedSize = "";
    if (BaseCalc == 1000) { PrefixDetectedSize = "±"; }

    // Show the detected (usable) card size in either base-10 or base-2 depending on the selected setting
    ui->lblCardSize->setText(PrefixDetectedSize + QString::number((double)DetectedDriveSize / (BaseCalc * BaseCalc * BaseCalc), 'f', 2) + GBLabel);
    qint64 AddedSize = static_cast<qint64>(ui->tbSizeAdjuster->value()) * 1000000;

    // Then we'll add the 'new MB's' as bytes to the CurLinPartSize so we can display it and maybe use it for processing the file later
    qint64 CurLinPartSize = CurLinuxArkOSPartSize + AddedSize;
    
    // Show the current bytes in the textbox (if it's not hidden)
    ui->txtNewSize->setText(QString::number(CurLinPartSize));
    
    // Show the current (selected) Linux/ArkOS partition size (again using either base-10 or base-2)
    ui->lblLinuxPartSize->setText("<center>" + QString::number((double)CurLinPartSize / (BaseCalc * BaseCalc * BaseCalc), 'f', 2) + GBLabel + "</center>");
    
    // Next we'll calculate the new ROM's partition size with the following steps:
    // - First we'll take the full detected size of the drive and 'scale it down' to GB's (using the selected base method)
    // - Then we'll substract the current value from the slider from this full SD-Card size (because the value of the slider basically indicated the desired size of the Linux Partition)
    // - And then we also substract the size of the already pre-defined bootdrive (it's just about 110MB or so (at the moment of writing this), but still, just to keep it a bit more accurate)
    double RomsPartSize = ((double)DetectedDriveSize / (BaseCalc * BaseCalc * BaseCalc)) - ((double)CurLinPartSize / (BaseCalc * BaseCalc * BaseCalc)) - ((double)BootDriveSize / (BaseCalc * BaseCalc * BaseCalc));    

    // And then we'll update the label underneath the "ROM's SD-Card" with the currently selected size of the ROM's Partition
    ui->lblRomsPartSize->setText("<center>" + QString::number(RomsPartSize, 'f', 2) + GBLabel + "</center>");
}

//==================================================================================================
// This is a small and simple 'helper function' which quickly returns the estimated SD-Card size
// in human readable format (32GB for example) when the function is passed the detected size in bytes.
// Reason why I'm using quite a 'large range' to detect/guess the SD-Card size, is because I want to
// account for SD-Cards which report a different size than expected (even when takin base-10 / base-2 into account).
//
// NOTE: We DO NOT have to change the values here based on when the user selects base-10 or base-2, because these
// calculations are done only once during the start of the application and for this 'estimation' raw-byte values are used
//==================================================================================================
QString MainWindow::EstimateSDCardSize(qint64 DetectedSizeInBytes)
{
    // This one basically reports that it's (most likely!) an 32GB SD-Card when the passed SizeInBytes is larger or
    // equal to 30000000000 bytes (a "tad" under 32GB) and smaller than 60000000000 bytes (nearing 64GB) 
    if (DetectedSizeInBytes >= 30000000000LL && DetectedSizeInBytes < 60000000000LL)
    {
        return "32GB";
    }

    // And the same basically happens for each following block with increasing steps
    // Won't comment on all of them though, that's just not neccesary here ;)
    if (DetectedSizeInBytes >= 60000000000LL && DetectedSizeInBytes < 120000000000LL)
    {
        return "64GB";
    }
    if (DetectedSizeInBytes >= 120000000000LL && DetectedSizeInBytes < 240000000000LL)
    {
        return "128GB";
    }
    if (DetectedSizeInBytes >= 240000000000LL && DetectedSizeInBytes < 500000000000LL)
    {
        return "256GB";
    }
    if (DetectedSizeInBytes >= 500000000000LL && DetectedSizeInBytes < 1000000000000LL)
    {
        return "512GB";
    }
    if (DetectedSizeInBytes >= 1000000000000LL && DetectedSizeInBytes < 1400000000000LL)
    {
        return "1TB";
    }
    if (DetectedSizeInBytes >= 1400000000000LL && DetectedSizeInBytes < 1900000000000LL)
    {
        return "1.5TB";
    }
    if (DetectedSizeInBytes >= 1900000000000LL && DetectedSizeInBytes < 2300000000000LL)
    {
        return "2TB";
    }

    // If none of the above matched, then we'll just return >2TB (meaning bigger than 2TB)
    // so this is currently/basically just the 'hardcap limit' of this application.
    // Which is in my opinion MORE than enough for the consoles for which this application is intended for ;) 
    return ">2TB";
}

//==================================================================================================
// This function is used to read the minimal set Linux/ArkOS partition size from the file
// expandtoexfat.sh on the BOOT Partition. This file is used during the initial boot of (d)ArkOS
// after flashing the (d)ArkOS Image to the SD-Card. The size read from this file is set by the developer
// of the (d)ArkOS image that has been used to flash this SD-Card, and thus we will not allow the users of
// XNL R36 Linux Partition Sizer to make the Linux/ArkOS partition smaller than the size set by the (d)ArkOS/Image
// developer in that file.
//==================================================================================================
qint64 MainWindow::GetCurrentArkOSPartSize(const QString &filePath)
{
    // We'll read the file (which has been passed as parameter when we called this function) into a string/"memory"
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return -1; // If ANY unexpected error occurred, we'll just return our default error code (-1)
    }
    QString fileContent = QString::fromUtf8(file.readAll());
    file.close();
    // The start marker of the section in between we want to find the pre-set Linux Partition size (in bytes)
    QString startMarker = "newExtSizePct=$(printf %.2f \"$((10**4 * ";
    // The end marker/position of the section in between we want to find the pre-set Linux Partition size (in bytes)
    QString endMarker = "/$maxSize))e-4\")";
    // Find the positions of markers in the string
    int startIndex = fileContent.indexOf(startMarker);
    int endIndex = fileContent.indexOf(endMarker, startIndex);
    // If BOTH have returned a valid position (meaning both 'markers' have been found) AND the EndMarker
    // was found in the string/file at a later position than the start marker, THEN we will continue to
    // read the 
    if ((startIndex != -1 && endIndex != -1) && endIndex > startIndex)
    {
        // Here will add the length of the startMarker string to the startIndex position where this string was found
        // this basically means that if we from here on use startIndex, that it won't start at the beginning of the
        // 'search string'/startMarker, but that it will start at the end of that startMarker. Because we only want
        // the number value between the startMarker and the endMarker, we do not also want to include the entire
        // "startMarker".
        startIndex += startMarker.length();
        // Now we'll subtract a substring from the mainstring (which contains the entire file), where we will
        // 'start extracting' at the (now adjusted) startIndex, and to know the length of the string between the
        // start and endMarker we'll simply subtract the startIndex from the endIndex, and then you basically have
        // the amount of characters between these two markers :)   lastly we'll add .trimmed() to the 'command' which
        // will basically ensure that there are no trailing or leading whitespaces (spaces, tabs, newlines etc) in the substring.
        QString numberString = fileContent.mid(startIndex, endIndex - startIndex).trimmed();
        // Next we'll try to convert the found substring into an actual numeric value (a long)
        // and if this fails we'll just return -2, which we are using as error code in this Application
        bool ok = false;
        qint64 extractedValue = numberString.toLongLong(&ok);
        if (ok)
        {
            return extractedValue;  // If it succeeded we return the found value
        }
        else
        {
            return -2;              // If 'conversion' failed, we'll return a 'custom error code' (-2)
        }
    }
    else
    {
        // If we could not detect the correct startMarker or endMarker, then we'll return a 'custom error code' (-3) so that
        // we can inform the user that we suspect that this is either not a valid (d)ArkOS initial boot file, or that the file for
        // example might have been updated/changed and that this Application is no longer compatible with this file.
        return -3;
    }
}

//==================================================================================================
// This function actually updates the partition size in the boot script and injects cleanup logic
// for the backup file made by the XNL R36 Linux Partition Sizer (so that the backup is also cleaned
// up after the R36 has 'done it's thing' during first boot)
//==================================================================================================
bool MainWindow::ModifyBootScript(const QString &filePath, const QString &newSizeValue, QWidget *parent)
{
    // These are the markers between which we'll set the new partition size value
    QString startMarker = "newExtSizePct=$(printf %.2f \"$((10**4 * ";
    QString endMarker = "/$maxSize))e-4\")";

    // This is the code that will clean up the backup of the init bootscript file.
    // If this line is able to run, it means the overwritten file worked, so the backup is no longer needed.
    QString insertText =
        "# Clean up the expandtoexfat back-up from XNL R36 Linux Partition Sizer\n"
        "sudo rm -v /boot/expandtoexfat.xbck\n";

    QString fileContent;

    // Read the entire original file into memory
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // If reading (well opening) the file failed, we'll throw an error for the user...
        QMessageBox::warning(parent, "XNL R36 Linux Partition Sizer", "Error while trying to read the data from the original boot script!\n\nThe XNL R36 Linux Partition Sizer was unable to apply your changes to the boot script!");
        // Return we've failed to edit (open in this instance) the file
        return false;
    }
    // If opening the file succeeded, we'll read all the contents into the fileContent string
    fileContent = QString::fromUtf8(file.readAll());
    // Close the file once it's in memory (since from hereon we'll work in memory instead until we're ready to save again)
    file.close();

    // Find the two markers where we want to set the new Linux Partition size
    int startIndex = fileContent.indexOf(startMarker);
    int endIndex = fileContent.indexOf(endMarker, startIndex);

    // If we found valid locations for both markers, continue editing the file
    if ((startIndex != -1 && endIndex != -1) && endIndex > startIndex) {
        // Find the initially set Linux Partition size (see also GetCurrentArkOSPartSize)
        startIndex += startMarker.length();
        QString originalNumber = fileContent.mid(startIndex, endIndex - startIndex).trimmed();

        // Replace the original number with the new size value
        fileContent.replace(originalNumber, newSizeValue);

        // Check if the backup cleanup line is already present (means file was edited previously)
        if (!fileContent.contains("sudo rm -v /boot/expandtoexfat.xbck")) {
            // Look for the line where we want to inject our cleanup code
            int insertIndex = fileContent.indexOf("if [ $exitcode -eq 0 ]; then");
            if (insertIndex != -1) {
                // Insert the cleanup code and comment above the found line
                fileContent.insert(insertIndex, insertText);
            } else {
                // Next we'll try to inject the cleanup line for dArkOS if the (d)ArkOS script fails
                insertIndex = fileContent.indexOf("if [ \"$exitcode\" == \"0\" ]; then");
                if (insertIndex != -1) {
                    // Find the end of the line (newline)
                    int endOfLine = fileContent.indexOf('\n', insertIndex);
                    if (endOfLine != -1) {
                        // Insert the cleanup code and comment underneath the found line
                        fileContent.insert(endOfLine + 1, insertText);
                    }
                }else{
                    // If we didn't find that line, warn the user to clean up the backup manually
                    QMessageBox::warning(parent, "XNL R36 Linux Partition Sizer", "Sorry, XNL R36 Linux Partition Sizer was unable to add the automatic clean-up lines to remove the expandtoexfat.xbck after the device has successfully booted into Linux/(d)ArkOS.\n\nThis means that you will have to remove the expandtoexfat.xbck file yourself after your device has fully completed the partitioning.\n");
                }
            }
        }

        // Setting the path for the file we're going to use as output (outFile) 
        QFile outFile(filePath);
        // Now we're opening the file for WRITEOnline, as text and it will truncate,
        // meaning that if the file already exists that the original content will be erased (so that the
        // edited file will basically take it's place)
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QMessageBox::critical(parent, "XNL R36 Linux Partition Sizer", QString("Sorry, there was an unknown error while trying to write the file back to your SD-Card:\n%1\n").arg(outFile.errorString()));
            // Return we've failed to edit the file
            return false;
        }

        // Write to the file in UTF-8 format
        outFile.write(fileContent.toUtf8());
        
        // Close the file (finalize it)
        outFile.close();

        // Return that the edit has succeeded
        return true;
    } else {
        // If we didn't find the markers, return false (could add a warning here if desired)
        return false;
    }
}
  
//==================================================================================================
// Helper function to strip HTML tags from text (so we can for example read/use mark-upped text
// from labels as information elsewhere, like in messageboxes without issues)
//==================================================================================================
QString MainWindow::stripHtml(const QString &html)
{
    QTextDocument doc;
    doc.setHtml(html);
    return doc.toPlainText();
}

//==================================================================================================
// This uses a sharedmemory mapping to ensure that this program is a single instance without needing
// to create a lock-file or without the need of creating a 'localserver' (which requires yet another
// include). HOWEVER! Since we actually need to detach the memory mapping to ensure we prevent issues
// when starting the program again after (properly) closing it, we will need to detach this mapping
// first before we can for example use exit(0) to ABRUPTLY force exit the program. And thus why
// I created the XNLExit substitute for the "exit(0) calls"
//==================================================================================================
void MainWindow::XNLExit(){
    if (GlobalsharedMemory && GlobalsharedMemory->isAttached())
    {
        GlobalsharedMemory->detach();
    }
    exit(0);
}
#pragma endregion

