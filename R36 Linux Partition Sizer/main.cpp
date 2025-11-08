// =========================================================================================================================
// XNL R36 Linux Partition Sizer - Main Code
//
// NOTE: This is actually the "initializer" or "start motor" if you will of the application :)
//       This code basically fires-up the Mainwindow
// =========================================================================================================================
// Copyright: © 2025 XNL Future Technologies
// Website:   https://www.teamxnl.com/R36-Linux-Partition-Sizer
// YouTube:   https://www.youtube.com/XNLFutureTechnologies
// For more (project and License) information please check mainwindow.cpp
// =========================================================================================================================
#include <QApplication>
#include <QMessageBox>
#include <QIcon>
#include "mainwindow.h"

// A GLOBAL variable/pointer to the shared memory "file" we're about to make during start up
// This is what I often use to make an application single instance if it's important to do so.
QSharedMemory* GlobalsharedMemory = nullptr;

//==================================================================================================================
// The main routine which starts, creates a SharedMemoryFile (more about that below), it then
// initializes the GUI and after that properly cleanup after itself
//==================================================================================================================
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Here I'm using/introducing a single-instance-check where I ensure that only one XNL R36 Linux Partition Sizer
    // program can be active at the same time (to prevent file I/O and mounting issues)
    // Do however keep in mind that this method can cause an 'issue' where the programs claims there is already
    // another instance running if that instance was terminated abnormally (for example starting it from the terminal and then killing it using ctrl+c).
    // This however isn't too much of a big deal, because after the first 'error' of it saying there is already another instance
    // running, you can simply start the program again and it will run as normal (because that one then has actually cleaned up after itself)
    GlobalsharedMemory = new QSharedMemory("XNLFtR36LinuxPartitionSizerSingleInstanceSharedMemoryLock2110");
    
    // First we check if we can attach (connect) to the SharedMemory...
    if (GlobalsharedMemory->attach(QSharedMemory::ReadOnly)) {
        // If this is the case, then it means that the shared memory actually already exists and thus the program is already active!
        // So we now detach again (THIS instance which just started)        
        GlobalsharedMemory->detach();
        // Tell the user we can't run multiple instances of this application
        QMessageBox::warning(nullptr, "Program Already Active", "XNL R36 Linux Partition Sizer is already running, you can only run one instance of this program at the same time");
        // And then return 0 (effectively quiting the application during the startup process)
        return 0;
    }
    
    // Then we'll try to create the shared memory....
    if (!GlobalsharedMemory->create(1)) {
        // But if this fails, then it is most likely that it already exists, and thus that the application is already running
        // but that the previous check somehow failed.

        // So we yet again 'bail-out' in the same way as above...
        QMessageBox::warning(nullptr, "Program Already Active", "XNL R36 Linux Partition Sizer is already running, you can only run one instance of this program at the same time");
        return 0;
    }

    // Setting the application icon
    QIcon appIcon(":/images/Images/AppIcon.ico");
    app.setWindowIcon(appIcon);
    
    // Setting the main window and showing it
    MainWindow w;
    w.show();

    // Store the 'exit result' in a variable, we're doing that here so that the
    // code here waits until we've reached that point in the program, so that we
    // AFTER the GUI is done can clean up the SharedMemoryFile we've created :)
    // Otherwise this would been the last line: return app.exec();
    // And thus we would not get a chance to do our MemoryFileCleanup part ;)
    int result = app.exec();
    
    // On a normal exit of the application we'll check if the shared memory is
    // attached to our program (which it should of course)....
    if (GlobalsharedMemory->isAttached()) {
        // And if so, we'll detach it so we don't get errors about the application
        // is already running if we start it again (this also explicitly destroys the sharedmemory mapping we've created)
        GlobalsharedMemory->detach();
    }    
    
    // Clean up the memory we've reserved for the Shared memory 
    delete GlobalsharedMemory;
    GlobalsharedMemory = nullptr;
    return result;
}
