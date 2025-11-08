// =========================================================================================================================
// XNL R36 Linux Partition Sizer - MainWindow header file
// =========================================================================================================================
// Copyright: © 2025 XNL Future Technologies
// Website:   https://www.teamxnl.com/R36-Linux-Partition-Sizer
// YouTube:   https://www.youtube.com/XNLFutureTechnologies
// For more (project and License) information please check mainwindow.cpp
// =========================================================================================================================
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStorageInfo>
#include <QMessageBox>
#include <QTimer>
#include <QString>
#include <QMap>
#include <QPixmap>
#include <QCloseEvent>
#include <QSharedMemory>

extern QSharedMemory* GlobalsharedMemory;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;     // The event filter which can be used instead of the connect slots for example
    void keyPressEvent(QKeyEvent *event) override;              // Used to detect keypress events   
    void keyReleaseEvent(QKeyEvent *event) override;            // Used to detec keyrelease events
    void changeEvent(QEvent *event) override;                   // Used to detect event changes (like the form getting focus for example)
    void closeEvent(QCloseEvent *event) override;               // Used to be able to add code to the "Application Exit Event"

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OpenExternalLinkWithConfirm(const QString &MessageText, const QString &WebsiteURL);
    int tryMountBootPartition(const QString &devicePath);
    void onApplyClicked();
    void CheckPigeonEgg();
    void onWarningTimerTimeout();
    void onProModeTimerTimeout();
    void UpdateUI();
    void onBaseRadioChanged();
    void XNLExit();

private:
    Ui::MainWindow *ui;
    QTimer *tmrWarningTimer;
    QTimer *tmrProMode;
    QPixmap WarningIcon;

    // Methods
    void detectBootDrive();
    qint64 getDiskSize(const QString &mountPoint);
    qint64 GetCurrentArkOSPartSize(const QString &filePath);
    bool ModifyBootScript(const QString &filePath, const QString &newSizeValue, QWidget *parent);
    void setupConnections();
    QString EstimateSDCardSize(qint64 DetectedSizeInBytes);
    void onSliderValueChanged();
    QString stripHtml(const QString &html);
    void offerUnmountBootPartition();
};

#endif // MAINWINDOW_H



