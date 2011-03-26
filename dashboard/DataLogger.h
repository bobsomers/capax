#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QDateTime>

#include "buildctrl.h"
#include "ucvtypes.h"

// record types (single byte identifiers)
#define ECU_RECORD 0x01
#define GPS_RECORD 0x02
#define IMU_RECORD 0x03
#define LTS_RECORD 0x04

class DataLogger : public QObject
{
    Q_OBJECT

public:
    DataLogger();
    ~DataLogger();

    void SetDirectory(QDir dir) { directory = dir; }

public slots:
    // hardware interface
    void EcuUpdate(ecustate_t state);
    void GpsUpdate(gpsstate_t state);
    void ImuUpdate(imustate_t state);
    void LtsUpdate(ltsstate_t state);
    void TmrUpdate(int ms);

    // control interface
    void LogStart();
    void LogStop();

signals:
    void LogStatusChanged(bool running);

private:
    bool logger_running;
    QDir directory;
    QFile *logfile;
    QDataStream *stream;
};

#endif // DATALOGGER_H
