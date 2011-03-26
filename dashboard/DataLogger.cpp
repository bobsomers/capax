#include "DataLogger.h"

DataLogger::DataLogger()
{
    logger_running = false;
    logfile = NULL;
}

DataLogger::~DataLogger()
{
    // don't delete me without flushing my data to disk!
    if (logger_running) LogStop();

    if (logfile != NULL)
    {
        delete logfile;
    }
}

void DataLogger::EcuUpdate(ecustate_t state)
{
    // discard it if the logger isn't running
    if (!logger_running || stream == NULL) return;

    // output the record type
    *stream << (quint8)ECU_RECORD;

    // output the state data
    *stream << (qint32)state.timestamp;
    *stream << (qint32)state.rpm;
    *stream << state.spark_adv;
    *stream << state.cranking;
    *stream << state.map;
    *stream << state.mat;
    *stream << state.clt;
    *stream << state.tps;
    *stream << state.batt;
    *stream << state.maf;
    *stream << (qint32)state.tach_count;
}

void DataLogger::GpsUpdate(gpsstate_t state)
{
    // discard it if the logger isn't running
    if (!logger_running || stream == NULL) return;

    // output the record type
    *stream << (quint8)GPS_RECORD;

    // output the state data
    *stream << (qint32)state.timestamp;
    *stream << (qint32)state.utc_hrs;
    *stream << (qint32)state.utc_mins;
    *stream << state.utc_secs;
    *stream << (qint32)state.pos.lat_deg;
    *stream << state.pos.lat_mins;
    *stream << (qint8)state.pos.lat_dir;
    *stream << (qint32)state.pos.long_deg;
    *stream << state.pos.long_mins;
    *stream << (qint8)state.pos.long_dir;
    *stream << state.alt;
    *stream << state.speed;
    *stream << state.heading;
}

void DataLogger::ImuUpdate(imustate_t state)
{
    // discard it if the logger isn't running
    if (!logger_running || stream == NULL) return;

    // output the record type
    *stream << (quint8)IMU_RECORD;

    // output the state data
    *stream << (qint32)state.timestamp;
    *stream << state.ax;
    *stream << state.ay;
    *stream << state.az;
    *stream << state.gx;
    *stream << state.gy;
    *stream << state.gz;
}

void DataLogger::LtsUpdate(ltsstate_t state)
{
    // discard it if the logger isn't running
    if (!logger_running || stream == NULL) return;

    // output the record type
    *stream << (quint8)LTS_RECORD;

    // output the state data
    *stream << (qint32)state.timestamp;
    *stream << state.headlights;
    *stream << state.brakelights;
    *stream << state.left_turn;
    *stream << state.right_turn;
    *stream << state.hazards;
}

void DataLogger::TmrUpdate(int ms)
{
    static int counter = 0;

    // discard it if the logger isn't running
    if (!logger_running) return;

    // flush the data to disk every 10 ticks (10 seconds)
    counter++;
    if (counter > 9)
    {
        if (logfile != NULL) logfile->flush();
        counter = 0;
    }
}

void DataLogger::LogStart()
{
    // can't start it if it's already running
    if (logger_running) return;

    // create the filename based on the current date and time
    QDateTime now = QDateTime::currentDateTime();
    QString filename = now.toString("yyyy-MM-dd_hh-mm-ss");
    filename += ".ucv";
    QString filepath = directory.absoluteFilePath(filename);

    // open the log file for writing
    logfile = new QFile(filepath);
    logfile->open(QIODevice::WriteOnly);

    // prepare the data stream
    stream = new QDataStream(logfile);
    stream->setVersion(QDataStream::Qt_4_5);

    // the logger is now running
    logger_running = true;
    emit LogStatusChanged(logger_running);
}

void DataLogger::LogStop()
{
    // can't stop it if it's not running
    if (!logger_running) return;

    // close the log file to flush it to disk
    logfile->close();

    // clean up the data stream
    delete stream;
    delete logfile;

    // the logger is no longer running
    logger_running = false;
    emit LogStatusChanged(logger_running);
}
