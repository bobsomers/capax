#include "Options.h"

// global variable for zeroing the imu
extern bool g_imu_zero;

Options::Options(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *imu_box = new QGroupBox("IMU Calibration");
    QGroupBox *logfiles_box = new QGroupBox("Data Logging Management");
    QGroupBox *exitshutdown_box = new QGroupBox("Exit/Shutdown");

    QGridLayout *imu_layout = new QGridLayout();
    QGridLayout *logfiles_layout = new QGridLayout();
    QGridLayout *exitshutdown_layout = new QGridLayout();
    QGridLayout *layout = new QGridLayout();

    title = new QLabel(QString("<h1>Options</h1>"));
    close_button = new QPushButton("Done");
    close_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));

    // imu calibration
    zero_explain = new QLabel("Zero the IMU measurements to calibrate:");
    zero_button = new QPushButton("Zero IMU");
    zero_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    imu_layout->addWidget(zero_explain, 0, 0);
    imu_layout->addWidget(zero_button, 0, 1);

    // logfiles management
    logfiles_explain = new QLabel("Log files on the system:");
    logfiles_explain->setFont(QFont("Fixed", LABEL_FONT_SIZE, QFont::Bold));
    logfiles = new QListWidget();
    logfiles->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    flashdrive_explain = new QLabel("Select a destination drive:");
    flashdrive_explain->setFont(QFont("Fixed", LABEL_FONT_SIZE, QFont::Bold));
    drives = new QListWidget();
    drives->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    refresh_button = new QPushButton("Refresh");
    refresh_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    copy_button = new QPushButton("Copy Log File");
    copy_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    logfiles_layout->addWidget(logfiles_explain, 0, 0);
    logfiles_layout->addWidget(logfiles, 1, 0);
    logfiles_layout->addWidget(flashdrive_explain, 0, 1);
    logfiles_layout->addWidget(drives, 1, 1);
    logfiles_layout->addWidget(refresh_button, 2, 0);
    logfiles_layout->addWidget(copy_button, 2, 1);

    // exit/shutdown box
    exit_button = new QPushButton("Exit to Windows");
    exit_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    shutdown_button = new QPushButton("Shutdown Car");
    shutdown_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    exitshutdown_layout->addWidget(exit_button, 0, 0);
    exitshutdown_layout->addWidget(shutdown_button, 0, 1);

    // master layout
    imu_box->setLayout(imu_layout);
    logfiles_box->setLayout(logfiles_layout);
    exitshutdown_box->setLayout(exitshutdown_layout);
    layout->addWidget(title, 0, 0);
    layout->addWidget(close_button, 0, 1);
    layout->addWidget(imu_box, 1, 0, 1, 2);
    layout->addWidget(logfiles_box, 2, 0, 1, 2);
    layout->addWidget(exitshutdown_box, 3, 0, 1, 2);

    setLayout(layout);
    setWindowTitle("Options");

    // connect up signals/slots
    connect(close_button, SIGNAL(clicked()), this, SLOT(hide()));
    connect(exit_button, SIGNAL(clicked()), QApplication::instance(), SLOT(closeAllWindows()));
    connect(shutdown_button, SIGNAL(clicked()), this, SLOT(ShutdownButtonClicked()));
    connect(refresh_button, SIGNAL(clicked()), this, SLOT(RefreshFilesAndDrives()));
    connect(copy_button, SIGNAL(clicked()), this, SLOT(CopyLogfile()));
    connect(zero_button, SIGNAL(clicked()), this, SLOT(ZeroImu()));

    RefreshFilesAndDrives();
}

Options::~Options()
{
    // nothing yet
}

void Options::ShutdownButtonClicked()
{
#ifdef RUNNING_IN_CAR
    LUID luid;
    TOKEN_PRIVILEGES privs;
    HANDLE token;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);
    LookupPrivilegeValue(NULL, L"SeShutdownPrivilege", &luid);

    privs.PrivilegeCount = 1;
    privs.Privileges[0].Luid = luid;
    privs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(token, false, &privs, 0, NULL, NULL);

    ExitWindowsEx(EWX_POWEROFF, 0);
#endif
}

void Options::RefreshFilesAndDrives()
{
    logfiles->clear();
    drives->clear();

    QDir cwd;
    QStringList files = cwd.entryList(QStringList("*.ucv"));
    for (int i = 0; i < files.size(); i++)
    {
        logfiles->addItem(files[i]);
    }

    QFileInfoList drive_list = QDir::drives();
    for (int i = 0; i < drive_list.size(); i++)
    {
        drives->addItem(drive_list.at(i).filePath());
    }
}

void Options::CopyLogfile()
{
    QListWidgetItem *logfile = logfiles->currentItem();
    QListWidgetItem *drive = drives->currentItem();
    QMessageBox msg;

    if (logfile == NULL)
    {
        msg.setWindowTitle("Error");
        msg.setText("Please select a log file first.");
        msg.exec();
        return;
    }

    if (drive == NULL)
    {
        msg.setWindowTitle("Error");
        msg.setText("Please select a destination drive first.");
        msg.exec();
        return;
    }

    QDir cwd;
    QString src = cwd.absoluteFilePath(logfile->text());
    QDir drv(drive->text());
    QString dest = drv.absoluteFilePath(logfile->text());

    // copy the file
    QFile::copy(src, dest);

    msg.setWindowTitle("Complete");
    msg.setText(QString("Copied to %1.").arg(dest));
    msg.exec();
}

void Options::ZeroImu()
{
    g_imu_zero = true;
}
