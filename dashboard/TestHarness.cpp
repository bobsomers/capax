#include "TestHarness.h"

TestHarness::TestHarness(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *ecu_box = new QGroupBox("Engine Control Unit");
    QGroupBox *gps_box = new QGroupBox("GPS");
    QGroupBox *imu_box = new QGroupBox("Inertial Measurement Unit");
    QGroupBox *lts_box = new QGroupBox("Lighting System");
    QGroupBox *wls_box = new QGroupBox("Wireless Radio");
    QGroupBox *tmr_box = new QGroupBox("Timer/Logger");

    QGridLayout *ecu_layout = new QGridLayout();
    QGridLayout *gps_layout = new QGridLayout();
    QGridLayout *imu_layout = new QGridLayout();
    QGridLayout *lts_layout = new QGridLayout();
    QGridLayout *wls_layout = new QGridLayout();
    QGridLayout *tmr_layout = new QGridLayout();
    QGridLayout *layout = new QGridLayout();

    // ecu box
    ecu_rpm_label = new QLabel("RPM:");
    ecu_rpm_edit = new QLineEdit("2000");
    ecu_spark_label = new QLabel("Spark Advance:");
    ecu_spark_edit = new QLineEdit("15.0");
    ecu_cranking_label = new QLabel("Cranking:");
    ecu_cranking_edit = new QCheckBox("Yes");
    ecu_map_label = new QLabel("Manifold Air Pres:");
    ecu_map_edit = new QLineEdit("1500.0");
    ecu_mat_label = new QLabel("Manifold Air Temp:");
    ecu_mat_edit = new QLineEdit("15.0");
    ecu_clt_label = new QLabel("Coolant Temp:");
    ecu_clt_edit = new QLineEdit("90.0");
    ecu_tps_label = new QLabel("Throttle Position:");
    ecu_tps_edit = new QLineEdit("50.0");
    ecu_batt_label = new QLabel("Battery:");
    ecu_batt_edit = new QLineEdit("11.9");
    ecu_maf_label = new QLabel("Mass Airflow:");
    ecu_maf_edit = new QLineEdit("900.0");
    ecu_tc_label = new QLabel("Tach Count:");
    ecu_tc_edit = new QLineEdit("100");
    ecu_update_button = new QPushButton("Update");

    ecu_layout->addWidget(ecu_rpm_label, 0, 0);
    ecu_layout->addWidget(ecu_rpm_edit, 0, 1);
    ecu_layout->addWidget(ecu_spark_label, 1, 0);
    ecu_layout->addWidget(ecu_spark_edit, 1, 1);
    ecu_layout->addWidget(ecu_cranking_label, 2, 0);
    ecu_layout->addWidget(ecu_cranking_edit, 2, 1);
    ecu_layout->addWidget(ecu_map_label, 3, 0);
    ecu_layout->addWidget(ecu_map_edit, 3, 1);
    ecu_layout->addWidget(ecu_mat_label, 4, 0);
    ecu_layout->addWidget(ecu_mat_edit, 4, 1);
    ecu_layout->addWidget(ecu_clt_label, 5, 0);
    ecu_layout->addWidget(ecu_clt_edit, 5, 1);
    ecu_layout->addWidget(ecu_tps_label, 6, 0);
    ecu_layout->addWidget(ecu_tps_edit, 6, 1);
    ecu_layout->addWidget(ecu_batt_label, 7, 0);
    ecu_layout->addWidget(ecu_batt_edit, 7, 1);
    ecu_layout->addWidget(ecu_maf_label, 8, 0);
    ecu_layout->addWidget(ecu_maf_edit, 8, 1);
    ecu_layout->addWidget(ecu_tc_label, 9, 0);
    ecu_layout->addWidget(ecu_tc_edit, 9, 1);
    ecu_layout->addWidget(ecu_update_button, 10, 0, 1, 2);
    ecu_layout->setRowStretch(11, 1);

    // gps box
    gps_time_label = new QLabel("UTC Time:");
    gps_colon1_label = new QLabel(":");
    gps_colon2_label = new QLabel(":");
    gps_hrs_edit = new QLineEdit("12");
    gps_mins_edit = new QLineEdit("01");
    gps_secs_edit = new QLineEdit("01.001");
    gps_lat_label = new QLabel("Latitude:");
    gps_deg1_label = new QLabel("deg");
    gps_min1_label = new QLabel("min");
    gps_latdeg_edit = new QLineEdit("35");
    gps_latmin_edit = new QLineEdit("15.653229");
    gps_latdir_edit = new QLineEdit("N");
    gps_long_label = new QLabel("Longitude:");
    gps_deg2_label = new QLabel("deg");
    gps_min2_label = new QLabel("min");
    gps_longdeg_edit = new QLineEdit("120");
    gps_longmin_edit = new QLineEdit("39.2362974");
    gps_longdir_edit = new QLineEdit("W");
    gps_alt_label = new QLabel("Altitude:");
    gps_alt_edit = new QLineEdit("500.0");
    gps_speed_label = new QLabel("Speed:");
    gps_speed_edit = new QLineEdit("25.0");
    gps_heading_label = new QLabel("Heading:");
    gps_heading_edit = new QLineEdit("187.5");
    gps_update_button = new QPushButton("Update");

    gps_layout->addWidget(gps_time_label, 0, 0);
    gps_layout->addWidget(gps_hrs_edit, 0, 1);
    gps_layout->addWidget(gps_colon1_label, 0, 2);
    gps_layout->addWidget(gps_mins_edit, 0, 3);
    gps_layout->addWidget(gps_colon2_label, 0, 4);
    gps_layout->addWidget(gps_secs_edit, 0, 5);
    gps_layout->addWidget(gps_lat_label, 1, 0);
    gps_layout->addWidget(gps_latdeg_edit, 1, 1);
    gps_layout->addWidget(gps_deg1_label, 1, 2);
    gps_layout->addWidget(gps_latmin_edit, 1, 3);
    gps_layout->addWidget(gps_min1_label, 1, 4);
    gps_layout->addWidget(gps_latdir_edit, 1, 5);
    gps_layout->addWidget(gps_long_label, 2, 0);
    gps_layout->addWidget(gps_longdeg_edit, 2, 1);
    gps_layout->addWidget(gps_deg2_label, 2, 2);
    gps_layout->addWidget(gps_longmin_edit, 2, 3);
    gps_layout->addWidget(gps_min2_label, 2, 4);
    gps_layout->addWidget(gps_longdir_edit, 2, 5);
    gps_layout->addWidget(gps_alt_label, 3, 0);
    gps_layout->addWidget(gps_alt_edit, 3, 1, 1, 5);
    gps_layout->addWidget(gps_speed_label, 4, 0);
    gps_layout->addWidget(gps_speed_edit, 4, 1, 1, 5);
    gps_layout->addWidget(gps_heading_label, 5, 0);
    gps_layout->addWidget(gps_heading_edit, 5, 1, 1, 5);
    gps_layout->addWidget(gps_update_button, 6, 0, 1, 6);
    gps_layout->setRowStretch(7, 1);

    // imu box
    imu_ax_label = new QLabel("Accel X:");
    imu_ax_edit = new QLineEdit("0.0");
    imu_ay_label = new QLabel("Accel Y:");
    imu_ay_edit = new QLineEdit("1.0");
    imu_az_label = new QLabel("Accel Z:");
    imu_az_edit = new QLineEdit("0.0");
    imu_gx_label = new QLabel("Gyro X:");
    imu_gx_edit = new QLineEdit("0.0");
    imu_gy_label = new QLabel("Gyro Y:");
    imu_gy_edit = new QLineEdit("0.0");
    imu_gz_label = new QLabel("Gyro Z:");
    imu_gz_edit = new QLineEdit("0.0");
    imu_update_button = new QPushButton("Update");

    imu_layout->addWidget(imu_ax_label, 0, 0);
    imu_layout->addWidget(imu_ax_edit, 0, 1);
    imu_layout->addWidget(imu_ay_label, 1, 0);
    imu_layout->addWidget(imu_ay_edit, 1, 1);
    imu_layout->addWidget(imu_az_label, 2, 0);
    imu_layout->addWidget(imu_az_edit, 2, 1);
    imu_layout->addWidget(imu_gx_label, 3, 0);
    imu_layout->addWidget(imu_gx_edit, 3, 1);
    imu_layout->addWidget(imu_gy_label, 4, 0);
    imu_layout->addWidget(imu_gy_edit, 4, 1);
    imu_layout->addWidget(imu_gz_label, 5, 0);
    imu_layout->addWidget(imu_gz_edit, 5, 1);
    imu_layout->addWidget(imu_update_button, 6, 0, 1, 2);
    imu_layout->setRowStretch(7, 1);

    // lights box
    lts_head_label = new QLabel("Headlights:");
    lts_head_edit = new QCheckBox("On");
    lts_brake_label = new QLabel("Brakelights:");
    lts_brake_edit = new QCheckBox("On");
    lts_left_label = new QLabel("Left Turn Signal:");
    lts_left_edit = new QCheckBox("On");
    lts_right_label = new QLabel("Right Turn Signal:");
    lts_right_edit = new QCheckBox("On");
    lts_hazards_label = new QLabel("Hazards:");
    lts_hazards_edit = new QCheckBox("On");
    lts_update_button = new QPushButton("Update");

    lts_layout->addWidget(lts_head_label, 0, 0);
    lts_layout->addWidget(lts_head_edit, 0, 1);
    lts_layout->addWidget(lts_brake_label, 1, 0);
    lts_layout->addWidget(lts_brake_edit, 1, 1);
    lts_layout->addWidget(lts_left_label, 2, 0);
    lts_layout->addWidget(lts_left_edit, 2, 1);
    lts_layout->addWidget(lts_right_label, 3, 0);
    lts_layout->addWidget(lts_right_edit, 3, 1);
    lts_layout->addWidget(lts_hazards_label, 4, 0);
    lts_layout->addWidget(lts_hazards_edit, 4, 1);
    lts_layout->addWidget(lts_update_button, 5, 0, 1, 2);
    lts_layout->setRowStretch(6, 1);

    // wireless box
    wls_rx_label = new QLabel("Received from the radio:");
    wls_rx_edit = new QTextEdit();
    wls_tx_label = new QLabel("Transmit to the radio:");
    wls_tx_edit = new QTextEdit();
    wls_update_button = new QPushButton("Update");

    wls_layout->addWidget(wls_rx_label, 0, 0);
    wls_layout->addWidget(wls_rx_edit, 1, 0);
    wls_layout->addWidget(wls_tx_label, 2, 0);
    wls_layout->addWidget(wls_tx_edit, 3, 0);
    wls_layout->addWidget(wls_update_button, 4, 0);
    wls_layout->setRowStretch(5, 1);

    // timer box
    tmr_time_display = new QLCDNumber(8);
    tmr_time_display->setSegmentStyle(QLCDNumber::Flat);
    tmr_start_button = new QPushButton("Start Timer");
    tmr_stop_button = new QPushButton("Stop Timer");
    log_start_button = new QPushButton("Start Data Logger");
    log_stop_button = new QPushButton("Stop Data Logger");

    tmr_layout->addWidget(tmr_time_display, 0, 0);
    tmr_layout->addWidget(tmr_start_button, 1, 0);
    tmr_layout->addWidget(tmr_stop_button, 2, 0);
    tmr_layout->addWidget(log_start_button, 3, 0);
    tmr_layout->addWidget(log_stop_button, 4, 0);
    tmr_layout->setRowStretch(5, 1);

    // master layout
    ecu_box->setLayout(ecu_layout);
    gps_box->setLayout(gps_layout);
    imu_box->setLayout(imu_layout);
    lts_box->setLayout(lts_layout);
    wls_box->setLayout(wls_layout);
    tmr_box->setLayout(tmr_layout);
    layout->addWidget(ecu_box, 0, 0);
    layout->addWidget(gps_box, 0, 1);
    layout->addWidget(imu_box, 0, 2);
    layout->addWidget(lts_box, 1, 0);
    layout->addWidget(wls_box, 1, 1);
    layout->addWidget(tmr_box, 1, 2);

    setLayout(layout);
    setWindowTitle("Hardware Interface Test Harness");

    // connect up the ui to internal slots
    connect(tmr_start_button, SIGNAL(clicked()), this, SLOT(TmrStart()));
    connect(tmr_stop_button, SIGNAL(clicked()), this, SLOT(TmrStop()));
    connect(this, SIGNAL(TmrTick(int)), tmr_time_display, SLOT(display(int)));
    connect(ecu_update_button, SIGNAL(clicked()), this, SLOT(UpdateEcu()));
    connect(gps_update_button, SIGNAL(clicked()), this, SLOT(UpdateGps()));
    connect(imu_update_button, SIGNAL(clicked()), this, SLOT(UpdateImu()));
    connect(lts_update_button, SIGNAL(clicked()), this, SLOT(UpdateLts()));
    connect(wls_update_button, SIGNAL(clicked()), this, SLOT(UpdateWls()));

    // done initializing ui, set up some internal stuff
    timer_running = false;
    time = new QTime();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerTick()));

    // debug
    logger = new DataLogger();
    dashboard = new Dashboard();
    dashboard->show();

    // connect up the test harness to the data logger
    connect(log_start_button, SIGNAL(clicked()), logger, SLOT(LogStart()));
    connect(log_stop_button, SIGNAL(clicked()), logger, SLOT(LogStop()));
    connect(this, SIGNAL(EcuStateChanged(ecustate_t)), logger, SLOT(EcuUpdate(ecustate_t)));
    connect(this, SIGNAL(GpsStateChanged(gpsstate_t)), logger, SLOT(GpsUpdate(gpsstate_t)));
    connect(this, SIGNAL(ImuStateChanged(imustate_t)), logger, SLOT(ImuUpdate(imustate_t)));
    connect(this, SIGNAL(LtsStateChanged(ltsstate_t)), logger, SLOT(LtsUpdate(ltsstate_t)));
    connect(this, SIGNAL(TmrTick(int)), logger, SLOT(TmrUpdate(int)));

    // connect up the test harness to the dashboard
    connect(this, SIGNAL(TmrTick(int)), dashboard, SLOT(TmrUpdate(int)));
    connect(this, SIGNAL(EcuStateChanged(ecustate_t)), dashboard, SLOT(EcuUpdate(ecustate_t)));
    connect(this, SIGNAL(GpsStateChanged(gpsstate_t)), dashboard, SLOT(GpsUpdate(gpsstate_t)));
    connect(this, SIGNAL(ImuStateChanged(imustate_t)), dashboard, SLOT(ImuUpdate(imustate_t)));
    connect(this, SIGNAL(LtsStateChanged(ltsstate_t)), dashboard, SLOT(LtsUpdate(ltsstate_t)));
}

TestHarness::~TestHarness()
{
    delete time;
}

void TestHarness::TmrStart()
{
    if (!timer_running)
    {
        time->start();
        timer->start(1000);
        timer_running = true;
    }
}

void TestHarness::TmrStop()
{
    if (timer_running)
    {
        timer->stop();
        timer_running = false;
    }
}

void TestHarness::TimerTick()
{
    emit TmrTick(time->elapsed());
}

void TestHarness::WlsDataSend(QByteArray data)
{
    wls_tx_edit->setPlainText(QString(data));
}

void TestHarness::LtsHeadlights(bool headlights)
{
    lts_head_edit->setChecked(headlights);
}

void TestHarness::LtsBrakelights(bool brakelights)
{
    lts_brake_edit->setChecked(brakelights);
}

void TestHarness::LtsLeftTurn(bool left_turn)
{
    lts_left_edit->setChecked(left_turn);
}

void TestHarness::LtsRightTurn(bool right_turn)
{
    lts_right_edit->setChecked(right_turn);
}

void TestHarness::LtsHazards(bool hazards)
{
    lts_hazards_edit->setChecked(hazards);
}

void TestHarness::UpdateEcu()
{
    ecustate_t state;

    state.rpm = ecu_rpm_edit->text().toInt();
    state.spark_adv = ecu_spark_edit->text().toDouble();
    state.cranking = ecu_cranking_edit->isChecked();
    state.map = ecu_map_edit->text().toDouble();
    state.mat = ecu_mat_edit->text().toDouble();
    state.clt = ecu_clt_edit->text().toDouble();
    state.tps = ecu_tps_edit->text().toDouble();
    state.batt = ecu_batt_edit->text().toDouble();
    state.maf = ecu_maf_edit->text().toDouble();
    state.tach_count = ecu_tc_edit->text().toInt();
    state.timestamp = (timer_running) ? time->elapsed() : 0;

    emit EcuStateChanged(state);

    QMessageBox msg_box;
    msg_box.setText("ECU state update signal has been dispatched.");
    msg_box.exec();
}

void TestHarness::UpdateGps()
{
    gpsstate_t state;

    state.utc_hrs = gps_hrs_edit->text().toInt();
    state.utc_mins = gps_mins_edit->text().toInt();
    state.utc_secs = gps_secs_edit->text().toDouble();
    state.pos.lat_deg = gps_latdeg_edit->text().toInt();
    state.pos.lat_mins = gps_latmin_edit->text().toDouble();
    state.pos.lat_dir = gps_latdir_edit->text().at(0).toAscii();
    state.pos.long_deg = gps_longdeg_edit->text().toInt();
    state.pos.long_mins = gps_longmin_edit->text().toDouble();
    state.pos.long_dir = gps_longdir_edit->text().at(0).toAscii();
    state.alt = gps_alt_edit->text().toDouble();
    state.speed = gps_speed_edit->text().toDouble();
    state.heading = gps_heading_edit->text().toDouble();
    state.timestamp = (timer_running) ? time->elapsed() : 0;

    emit GpsStateChanged(state);

    QMessageBox msg_box;
    msg_box.setText("GPS state update signal has been dispatched.");
    msg_box.exec();
}

void TestHarness::UpdateImu()
{
    imustate_t state;

    state.ax = imu_ax_edit->text().toDouble();
    state.ay = imu_ay_edit->text().toDouble();
    state.az = imu_az_edit->text().toDouble();
    state.gx = imu_gx_edit->text().toDouble();
    state.gy = imu_gy_edit->text().toDouble();
    state.gz = imu_gz_edit->text().toDouble();
    state.timestamp = (timer_running) ? time->elapsed() : 0;

    emit ImuStateChanged(state);

    QMessageBox msg_box;
    msg_box.setText("IMU state update signal has been dispatched.");
    msg_box.exec();
}

void TestHarness::UpdateLts()
{
    ltsstate_t state;

    state.headlights = lts_head_edit->isChecked();
    state.brakelights = lts_brake_edit->isChecked();
    state.left_turn = lts_left_edit->isChecked();
    state.right_turn = lts_right_edit->isChecked();
    state.hazards = lts_hazards_edit->isChecked();
    state.timestamp = (timer_running) ? time->elapsed() : 0;

    emit LtsStateChanged(state);

    QMessageBox msg_box;
    msg_box.setText("Lighting state update signal has been dispatched.");
    msg_box.exec();
}

void TestHarness::UpdateWls()
{
    emit WlsDataArrived(wls_rx_edit->toPlainText().toAscii());

    QMessageBox msg_box;
    msg_box.setText("Wireless data arrived signal has been dispatched.");
    msg_box.exec();
}
