#ifndef HARDWARE_H
#define HARDWARE_H

#include <QObject>
#include <QApplication>
#include <QTime>
#include <QTimer>
#include <QByteArray>
#include <QMessageBox>
#include <QRegExp>

#ifdef RUNNING_IN_CAR
    #include <windows.h>
    #include <winsock2.h>
#endif

#include "DataLogger.h"
#include "Dashboard.h"
#include "ucvtypes.h"

// com port settings
#define ECU_COM_PORT L"COM1"
#define GPS_COM_PORT L"COM3"
#define IMU_COM_PORT L"COM4"
#define XBEE_COM_PORT L"COM5"
#define DRVR_COM_PORT L"COM6"

// default imu calibration
#define IMU_DEFAULT_AX 512
#define IMU_DEFAULT_AY 512
#define IMU_DEFAULT_AZ 512
#define IMU_DEFAULT_GX 512
#define IMU_DEFAULT_GY 512
#define IMU_DEFAULT_GZ 512

// g's and degrees per second per dac step
// this is based on 1024 values (10-bit DAC)
// and +/- 3g range and +/- 300 deg/sec range
#define GS_PER_STEP 0.005859375
#define DPS_PER_STEP 0.5859375

// the number of attempts to make to read all
// 112 bytes from the ECU before aborting
// this prevents the dashboard from hanging if the
// ECU refuses to respond with data
#define ECU_MAX_ATTEMPTS 3

class Hardware : public QObject
{
    Q_OBJECT

public:
    Hardware();
    ~Hardware();

public slots:
    void WlsDataSend(QByteArray data);
    void TmrStart();
    void TmrStop();
    void TimerTick();

signals:
    void EcuStateChanged(ecustate_t state);
    void GpsStateChanged(gpsstate_t state);
    void ImuStateChanged(imustate_t state);
    void LtsStateChanged(ltsstate_t state);
    void TmrTick(int ms);
    void WlsDataArrived(QByteArray data);

private:
    bool timer_running;
    int last_elapsed;
    QTime *time;
    QTimer *timer;
    DataLogger *logger;
    Dashboard *dashboard;

#ifdef RUNNING_IN_CAR
    HANDLE hEcuUart;
    HANDLE hGpsUart;
    HANDLE hImuUart;
    HANDLE hXbeeUart;
    HANDLE hDrvrUart;

    void OpenUarts();
    void CloseUarts();
    void ProcessEcu();
    void ProcessGps();
    void ProcessImu();
    void ProcessDrvr();
#endif
};

#endif // HARDWARE_H
