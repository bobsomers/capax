#include "Hardware.h"

// global variable for zeroing the imu
bool g_imu_zero = false;

Hardware::Hardware()
{
    // hook up the internal timer stuff
    timer_running = false;
    last_elapsed = 0;
    time = new QTime();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerTick()));

    // create the data logger and dashboard
    logger = new DataLogger();
    dashboard = new Dashboard();
    dashboard->show();

    // connect up the hardware to the data logger
    // TODO

    // connect up the hardware to the dashboard
    connect(this, SIGNAL(TmrTick(int)), dashboard, SLOT(TmrUpdate(int)));
    connect(this, SIGNAL(EcuStateChanged(ecustate_t)), dashboard, SLOT(EcuUpdate(ecustate_t)));
    connect(this, SIGNAL(GpsStateChanged(gpsstate_t)), dashboard, SLOT(GpsUpdate(gpsstate_t)));

    // THESE WILL NEED TO BE CHANGED... MAYBE... (add priming function with rpm trigger)
    connect(dashboard, SIGNAL(StartRun()), this, SLOT(TmrStart()));
    connect(dashboard, SIGNAL(StopRun()), this, SLOT(TmrStop()));

#ifndef DEVMODE
    // open serial ports to grab uart data
    OpenUarts();
#endif

    // kick everything off, here we go!
    timer->start(50);
}

Hardware::~Hardware()
{
    delete time;

#ifndef DEVMODE
    // close down serial ports
    CloseUarts();
#endif
}

void Hardware::TmrStart()
{
    if (!timer_running)
    {
        time->start();
        timer_running = true;
    }
}

void Hardware::TmrStop()
{
    if (timer_running)
    {
        timer_running = false;
        last_elapsed = 0;
    }
}

void Hardware::TimerTick()
{
    // generate 1 sec pulses
    if (timer_running)
    {
        if (last_elapsed == 0)
        {
            last_elapsed = time->elapsed();
        }
        else if (time->elapsed() > last_elapsed + 1000)
        {
            last_elapsed = time->elapsed();
            emit TmrTick(time->elapsed());
        }
    }

#ifndef DEVMODE
    // process hardware updates
    ProcessEcu();
    ProcessGps();
    ProcessImu();
    ProcessDrvr();
#endif
}

void Hardware::WlsDataSend(QByteArray data)
{
    // TODO
}

#ifndef DEVMODE
void Hardware::OpenUarts()
{
    QMessageBox msg;

    // open ecu com port
    hEcuUart = CreateFile(ECU_COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hEcuUart == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            msg.setText("ECU COM port does not exist!");
            msg.exec();
            QApplication::quit();
        }

        msg.setText("Error occurred while trying to open ECU COM port!");
        msg.exec();
        QApplication::quit();
    }

    // set ecu com port settings
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hEcuUart, &dcbSerialParams))
    {
        msg.setText("Could not get ECU COM port settings!");
        msg.exec();
        QApplication::quit();
    }
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hEcuUart, &dcbSerialParams))
    {
        msg.setText("Could not set ECU COM port settings!");
        msg.exec();
        QApplication::quit();
    }

    // set ecu com port timeouts
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hEcuUart, &timeouts))
    {
        msg.setText("Could not set ECU COM port timeouts!");
        msg.exec();
        QApplication::quit();
    }

    // open gps com port
    hGpsUart = CreateFile(GPS_COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hGpsUart == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            msg.setText("GPS COM port does not exist!");
            msg.exec();
            QApplication::quit();
        }

        msg.setText("Error occurred while trying to open GPS COM port!");
        msg.exec();
        QApplication::quit();
    }

    // set gps com port settings
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hGpsUart, &dcbSerialParams))
    {
        msg.setText("Could not get GPS COM port settings!");
        msg.exec();
        QApplication::quit();
    }
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hGpsUart, &dcbSerialParams))
    {
        msg.setText("Could not set GPS COM port settings!");
        msg.exec();
        QApplication::quit();
    }

    // set gps com port timeouts
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hGpsUart, &timeouts))
    {
        msg.setText("Could not set GPS COM port timeouts!");
        msg.exec();
        QApplication::quit();
    }

    // open imu com port
    hImuUart = CreateFile(IMU_COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hImuUart == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            msg.setText("IMU COM port does not exist!");
            msg.exec();
            QApplication::quit();
        }

        msg.setText("Error occurred while trying to open IMU COM port!");
        msg.exec();
        QApplication::quit();
    }

    // set imu com port settings
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hImuUart, &dcbSerialParams))
    {
        msg.setText("Could not get IMU COM port settings!");
        msg.exec();
        QApplication::quit();
    }
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hImuUart, &dcbSerialParams))
    {
        msg.setText("Could not set IMU COM port settings!");
        msg.exec();
        QApplication::quit();
    }

    // set imu com port timeouts
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hImuUart, &timeouts))
    {
        msg.setText("Could not set IMU COM port timeouts!");
        msg.exec();
        QApplication::quit();
    }

    // open xbee com port
    hXbeeUart = CreateFile(XBEE_COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hXbeeUart == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            msg.setText("XBee COM port does not exist!");
            msg.exec();
            QApplication::quit();
        }

        msg.setText("Error occurred while trying to open XBee COM port!");
        msg.exec();
        QApplication::quit();
    }

    // set xbee com port settings
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hXbeeUart, &dcbSerialParams))
    {
        msg.setText("Could not get XBee COM port settings!");
        msg.exec();
        QApplication::quit();
    }
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hXbeeUart, &dcbSerialParams))
    {
        msg.setText("Could not set XBee COM port settings!");
        msg.exec();
        QApplication::quit();
    }

    // set xbee com port timeouts
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hXbeeUart, &timeouts))
    {
        msg.setText("Could not set XBee COM port timeouts!");
        msg.exec();
        QApplication::quit();
    }

    // open driver com port
    hDrvrUart = CreateFile(DRVR_COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hDrvrUart == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            msg.setText("Driver COM port does not exist!");
            msg.exec();
            QApplication::quit();
        }

        msg.setText("Error occurred while trying to open Driver COM port!");
        msg.exec();
        QApplication::quit();
    }

    // set driver com port settings
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hDrvrUart, &dcbSerialParams))
    {
        msg.setText("Could not get Driver COM port settings!");
        msg.exec();
        QApplication::quit();
    }
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hDrvrUart, &dcbSerialParams))
    {
        msg.setText("Could not set Drvier COM port settings!");
        msg.exec();
        QApplication::quit();
    }

    // set driver com port timeouts
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hDrvrUart, &timeouts))
    {
        msg.setText("Could not set Driver COM port timeouts!");
        msg.exec();
        QApplication::quit();
    }
}

void Hardware::CloseUarts()
{
    CloseHandle(hEcuUart);
    CloseHandle(hGpsUart);
    CloseHandle(hImuUart);
    CloseHandle(hXbeeUart);
    CloseHandle(hDrvrUart);
}

void Hardware::ProcessEcu()
{
    static ecustate_t state;
    static int run_counter = 0;
    bool state_changed = false;

    // limit this to run twice per update
    run_counter++;
    if (run_counter < 2)
    {
        return;
    }
    run_counter = 0;

    // more information about the serial protocol
    // for communicating with the MegaSquirt II
    // ECU can be found here:
    // http://www.megamanual.com/ms2/code.htm

    // send request for data
    char request[3];
    request[0] = 'a';
    request[1] = 0;
    request[2] = 6;
    DWORD bytes_sent = 0;
    if (!WriteFile(hEcuUart, request, 3, &bytes_sent, NULL))
    {
        QMessageBox msg;
        msg.setText("Error requesting data from the ECU!");
        msg.exec();
    }

    // receive 112 bytes back
    char data[112];
    char buffer[112];
    int total_bytes_read = 0;
    int attempts = 0;
    DWORD bytes_read = 0;
    while (total_bytes_read < 112)
    {
        attempts++;
        if (attempts > ECU_MAX_ATTEMPTS)
        {
            break;
        }

        if (!ReadFile(hEcuUart, buffer, 112 - total_bytes_read, &bytes_read, NULL))
        {
            QMessageBox msg;
            msg.setText("Error reading response from ECU!");
            msg.exec();
        }

        // save the bytes read
        memcpy(data + total_bytes_read, buffer, bytes_read);
        total_bytes_read += bytes_read;

        if (total_bytes_read >= 112)
        {
            state_changed = true;
        }
    }

    if (state_changed)
    {
        // start parsing data out of the received block of bytes
        unsigned short rpm = 0;
        memcpy(&rpm, data + 6, 2);
        rpm = ntohs(rpm); // hacky way to swap byte order since MegaSquirt is big-endian
        state.rpm = rpm;

        short spark_adv_x10 = 0;
        memcpy(&spark_adv_x10, data + 8, 2);
        spark_adv_x10 = ntohs(spark_adv_x10);
        state.spark_adv = spark_adv_x10 / 10.0;

        unsigned char engine = 0;
        memcpy(&engine, data + 11, 1);
        state.cranking = (engine & (1 << 1));

        short map_x10 = 0;
        memcpy(&map_x10, data + 18, 2);
        map_x10 = ntohs(map_x10);
        state.map = map_x10 / 10.0;

        short mat_x10 = 0;
        memcpy(&mat_x10, data + 20, 2);
        mat_x10 = ntohs(mat_x10);
        state.mat = mat_x10 / 10.0;

        short clt_x10 = 0;
        memcpy(&clt_x10, data + 22, 2);
        clt_x10 = ntohs(clt_x10);
        state.clt = clt_x10 / 10.0;

        short tps_x10 = 0;
        memcpy(&tps_x10, data + 24, 2);
        tps_x10 = ntohs(tps_x10);
        state.tps = tps_x10 / 10.0;

        short batt_x10 = 0;
        memcpy(&batt_x10, data + 26, 2);
        batt_x10 = ntohs(batt_x10);
        state.batt = batt_x10 / 10.0;

        unsigned short maf_x10 = 0;
        memcpy(&maf_x10, data + 64, 2);
        maf_x10 = ntohs(maf_x10);
        state.maf = maf_x10 / 10.0;

        unsigned short tach_cnt = 0;
        memcpy(&tach_cnt, data + 104, 2);
        tach_cnt = ntohs(tach_cnt);
        state.tach_count = tach_cnt;

        state.timestamp = time->elapsed();

        emit EcuStateChanged(state);
    }
}

void Hardware::ProcessGps()
{
    static gpsstate_t state;
    static QByteArray buffer;
    static int lock_state = 0;
    int pos = 0;
    bool state_changed = false;

    char buf[161] = {0};
    DWORD bytes_read = 0;
    if (!ReadFile(hGpsUart, buf, 160, &bytes_read, NULL))
    {
        QMessageBox msg;
        msg.setText("Error reading from GPS UART!");
        msg.exec();
    }

    buffer.append(buf, bytes_read);

    // make sure the buffer starts with the first dollar sign
    QRegExp dollar("\\$");
    pos = dollar.indexIn(buffer);
    if (pos >= 0)
    {
        // shift the buffer to align it with the beginning of the message
        buffer = buffer.mid(pos);
    }

    // search for a GGA message match
    QRegExp ggare("^\\$GPGGA,([0-9.]+),([0-9.]+),(N|S),([0-9.]+),(W|E),([0-9]),[0-9]+,[0-9.]+,([0-9.]+),");
    pos = ggare.indexIn(buffer);
    if (pos >= 0)
    {
        QString utc_time = ggare.cap(1);
        QString latitude = ggare.cap(2);
        QString lat_ns = ggare.cap(3);
        QString longitude = ggare.cap(4);
        QString long_ew = ggare.cap(5);
        QString gps_lock = ggare.cap(6);
        QString altitude = ggare.cap(7);

        bool ok = false;
        state.utc_hrs = utc_time.mid(0, 2).toInt(&ok);
        state.utc_mins = utc_time.mid(2, 2).toInt(&ok);
        state.utc_secs = utc_time.mid(4, 6).toDouble(&ok);
        if (latitude.length() == 9)
        {
            state.pos.lat_deg = latitude.mid(0, 2).toInt(&ok);
            state.pos.lat_mins = latitude.mid(2).toDouble(&ok);
        }
        else
        {
            state.pos.lat_deg = latitude.mid(0, 3).toInt(&ok);
            state.pos.lat_mins = latitude.mid(3).toDouble(&ok);
        }
        state.pos.lat_dir = lat_ns.at(0).toAscii();
        if (longitude.length() == 9)
        {
            state.pos.long_deg = longitude.mid(0, 2).toInt(&ok);
            state.pos.long_mins = longitude.mid(2).toDouble(&ok);
        }
        else
        {
            state.pos.long_deg = longitude.mid(0, 3).toInt(&ok);
            state.pos.long_mins = longitude.mid(3).toDouble(&ok);
        }
        state.pos.long_dir = long_ew.at(0).toAscii();
        state.alt = altitude.toDouble(&ok);
        state.timestamp = time->elapsed();
        state_changed = true;

        // check the lock state
        int cur_lock_state = gps_lock.toInt(&ok);
        if (cur_lock_state != lock_state)
        {
            // state changed, update status led
            char update_buf[3];
            update_buf[0] = 'g';
            update_buf[1] = '=';
            update_buf[2] = (cur_lock_state) ? '1' : '0';
            DWORD bytes_written = 0;
            if (!WriteFile(hImuUart, update_buf, 3, &bytes_written, NULL))
            {
                // silently fail, non-critical error
            }

            lock_state = cur_lock_state;
        }

        // shift the buffer down
        buffer = buffer.mid(pos + ggare.matchedLength());

        state_changed = true;
    }

    // search for a VTG match
    QRegExp vtgre("^\\$GPVTG,([0-9.]+),[^,]*,[^,]*,[^,]*,[0-9.]+,[^,]*,([0-9.]+)");
    pos = vtgre.indexIn(buffer);
    if (pos >= 0)
    {
        QString heading = vtgre.cap(1);
        QString speed = vtgre.cap(2);

        bool ok = false;
        state.heading = heading.toDouble(&ok);
        state.speed = speed.toDouble(&ok) * 0.621371192; // convert km/h to mph
        state.timestamp = time->elapsed();
        state_changed = true;

        // shift the buffer down
        buffer = buffer.mid(pos + vtgre.matchedLength());
    }

    if (state_changed)
    {
        // update anybody listening
        emit GpsStateChanged(state);
    }
}

void Hardware::ProcessImu()
{
    static int neutral_ax = IMU_DEFAULT_AX;
    static int neutral_ay = IMU_DEFAULT_AY;
    static int neutral_az = IMU_DEFAULT_AZ;
    static int neutral_gx = IMU_DEFAULT_GX;
    static int neutral_gy = IMU_DEFAULT_GY;
    static int neutral_gz = IMU_DEFAULT_GZ;
    static imustate_t state;
    static QByteArray buffer;
    int pos = 0;
    bool zeroed = false;

    char buf[161] = {0};
    DWORD bytes_read = 0;
    if (!ReadFile(hImuUart, buf, 160, &bytes_read, NULL))
    {
        QMessageBox msg;
        msg.setText("Error reading from IMU UART!");
        msg.exec();
    }

    buffer.append(buf, bytes_read);

    // make sure the buffer starts with the first dollar sign
    QRegExp dollar("\\$");
    pos = dollar.indexIn(buffer);
    if (pos >= 0)
    {
        // shift the buffer to align it with the beginning of the message
        buffer = buffer.mid(pos);
    }

    // search for an imu message match
    QRegExp imumsg("^\\$([0-9]{4})\\s+([0-9]{4})\\s+([0-9]{4})\\s+([0-9]{4})\\s+([0-9]{4})\\s+([0-9]{4})");
    pos = imumsg.indexIn(buffer);
    if (pos >= 0)
    {
        // refer to mainboard schematic, imu microcontroller just outputs
        // the numbers in channel order 0-6
        QString gy_str = imumsg.cap(1);
        QString gx_str = imumsg.cap(2);
        QString gz_str = imumsg.cap(3);
        QString az_str = imumsg.cap(4);
        QString ay_str = imumsg.cap(5);
        QString ax_str = imumsg.cap(6);

        bool ok = false;
        int iax = ax_str.toInt(&ok);
        int iay = ay_str.toInt(&ok);
        int iaz = az_str.toInt(&ok);
        int igx = gx_str.toInt(&ok);
        int igy = gy_str.toInt(&ok);
        int igz = gz_str.toInt(&ok);

        if (g_imu_zero)
        {
            // update the new zero values
            neutral_ax = iax;
            neutral_ay = iay;
            neutral_az = iaz;
            neutral_gx = igx;
            neutral_gy = igy;
            neutral_gz = igz;
            zeroed = true;
        }

        // convert to g's and degs per sec based on zero val
        // also, remap coordinate axes (right handed) of the car as follows:
        //      CAR X : - driver left, + driver right = imu -ay
        //      CAR Y : - down, + up = imu az
        //      CAR Z : - forward, + backward = imu ax
        state.ax = (iay - neutral_ay) * GS_PER_STEP * -1;
        state.ay = (iaz - neutral_az) * GS_PER_STEP;
        state.az = (iax - neutral_ax) * GS_PER_STEP;
        state.gx = (igx - neutral_gx) * DPS_PER_STEP * -1;
        state.gy = (igz - neutral_gz) * DPS_PER_STEP;
        state.gz = (igy - neutral_gy) * DPS_PER_STEP;
        state.timestamp = time->elapsed();

        // shift the buffer down
        buffer = buffer.mid(pos + imumsg.matchedLength());

        // update anybody listening
        emit ImuStateChanged(state);
    }

    if (zeroed)
    {
        QMessageBox msg;
        msg.setWindowTitle("Complete");
        msg.setText("IMU has been calibrated.");
        msg.exec();
        g_imu_zero = false;
    }
}

void Hardware::ProcessDrvr()
{

}

#endif
