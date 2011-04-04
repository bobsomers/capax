#ifndef UCVTYPES_H
#define UCVTYPES_H

#define BUTTON_FONT_SIZE 16
#define LABEL_FONT_SIZE 14

typedef struct ecustate_struct {
    int timestamp; // in ms since timer was started
    int rpm;
    double spark_adv; // in degrees
    bool cranking;
    double map; // in kPa
    double mat; // in degrees F
    double clt; // in degrees F
    double tps; // in percent
    double batt; // in volts
    double maf; // in mg/sec
    int tach_count;
} ecustate_t;

typedef char gpsdir_t;
#define GPS_NORTH 'N'
#define GPS_SOUTH 'S'
#define GPS_EAST 'E'
#define GPS_WEST 'W'

typedef struct gpspos_struct {
    int lat_deg; // in whole degrees
    double lat_mins; // in compound minutes
    gpsdir_t lat_dir;
    int long_deg; // in whole degrees
    double long_mins; // in compound minutes
    gpsdir_t long_dir;
} gpspos_t;

typedef struct gpsstate_struct {
    int timestamp; // in ms since timer was started
    int utc_hrs;
    int utc_mins;
    double utc_secs;
    gpspos_t pos;
    double alt; // in m
    double speed; // in mph
    double heading; // in degrees
} gpsstate_t;

typedef struct imustate_struct {
    int timestamp; // in ms since the timer was started
    double ax; // in g's
    double ay; // in g's
    double az; // in g's
    double gx; // in degs/sec
    double gy; // in degs/sec
    double gz; // in degs/sec
} imustate_t;

typedef struct ltsstate_struct {
    int timestamp; // in ms since the timer was started
    bool headlights;
    bool brakelights;
    bool left_turn;
    bool right_turn;
    bool hazards;
} ltsstate_t;

#endif // UCVTYPES_H
