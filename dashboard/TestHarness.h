#ifndef TESTHARNESS_H
#define TESTHARNESS_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QLCDNumber>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QByteArray>

#include "buildctrl.h"
#include "DataLogger.h"
#include "Dashboard.h"
#include "ucvtypes.h"

class TestHarness : public QWidget
{
    Q_OBJECT

public:
    TestHarness(QWidget *parent = 0);
    ~TestHarness();

public slots:
    // hardware interface emulation
    void WlsDataSend(QByteArray data);
    void LtsHeadlights(bool headlights);
    void LtsBrakelights(bool brakelights);
    void LtsLeftTurn(bool left_turn);
    void LtsRightTurn(bool right_turn);
    void LtsHazards(bool hazards);
    void TmrStart();
    void TmrStop();

    // gui and internal use slots
    void TimerTick();
    void UpdateEcu();
    void UpdateGps();
    void UpdateImu();
    void UpdateLts();
    void UpdateWls();

signals:
    // hardware interface emulation
    void EcuStateChanged(ecustate_t state);
    void GpsStateChanged(gpsstate_t state);
    void ImuStateChanged(imustate_t state);
    void LtsStateChanged(ltsstate_t state);
    void TmrTick(int ms);
    void WlsDataArrived(QByteArray data);

private:
    QLabel *ecu_rpm_label, *ecu_spark_label, *ecu_cranking_label,
           *ecu_map_label, *ecu_mat_label, *ecu_clt_label,
           *ecu_tps_label, *ecu_batt_label, *ecu_maf_label,
           *ecu_tc_label, *gps_time_label, *gps_colon1_label,
           *gps_colon2_label, *gps_lat_label, *gps_deg1_label,
           *gps_min1_label, *gps_long_label, *gps_deg2_label,
           *gps_min2_label, *gps_alt_label, *gps_speed_label,
           *gps_heading_label, *imu_ax_label, *imu_ay_label,
           *imu_az_label, *imu_gx_label, *imu_gy_label,
           *imu_gz_label, *lts_head_label, *lts_brake_label,
           *lts_left_label, *lts_right_label, *lts_hazards_label,
           *wls_rx_label, *wls_tx_label;
    QLineEdit *ecu_rpm_edit, *ecu_spark_edit, *ecu_map_edit,
              *ecu_mat_edit, *ecu_clt_edit, *ecu_tps_edit,
              *ecu_batt_edit, *ecu_maf_edit, *ecu_tc_edit,
              *gps_hrs_edit, *gps_mins_edit, *gps_secs_edit,
              *gps_latdeg_edit, *gps_latmin_edit, *gps_longdeg_edit,
              *gps_longmin_edit, *gps_latdir_edit, *gps_longdir_edit,
              *gps_alt_edit, *gps_speed_edit, *gps_heading_edit,
              *imu_ax_edit, *imu_ay_edit, *imu_az_edit,
              *imu_gx_edit, *imu_gy_edit, *imu_gz_edit;
    QCheckBox *ecu_cranking_edit, *lts_head_edit, *lts_brake_edit,
              *lts_left_edit, *lts_right_edit, *lts_hazards_edit;
    QTextEdit *wls_rx_edit, *wls_tx_edit;
    QLCDNumber *tmr_time_display;
    QPushButton *ecu_update_button, *gps_update_button, *imu_update_button,
                *lts_update_button, *wls_update_button, *tmr_start_button,
                *tmr_stop_button, *log_start_button, *log_stop_button;

    bool timer_running;
    QTime *time;
    QTimer *timer;

    // debug
    DataLogger *logger;
    Dashboard *dashboard;
};

#endif // TESTHARNESS_H
