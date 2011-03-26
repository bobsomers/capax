#ifndef OPTIONS_H
#define OPTIONS_H

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QDir>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QMessageBox>
#include <QFile>

#ifndef DEVMODE
    #include <windows.h>
#endif

#include "buildctrl.h"
#include "ucvtypes.h"

class Options : public QWidget
{
    Q_OBJECT

public:
    Options(QWidget *parent = 0);
    ~Options();

public slots:
    void ShutdownButtonClicked();
    void RefreshFilesAndDrives();
    void CopyLogfile();
    void ZeroImu();

private:
    QLabel *title, *logfiles_explain, *flashdrive_explain, *zero_explain;
    QPushButton *close_button, *exit_button, *shutdown_button,
                *copy_button, *refresh_button, *zero_button;
    QListWidget *logfiles, *drives;
};

#endif // OPTIONS_H
