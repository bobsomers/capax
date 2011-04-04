#include "Dashboard.h"

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
{
    // set the window size
    setGeometry(50, 50, SCREEN_WIDTH, SCREEN_HEIGHT);

    QGroupBox *lap_timers = new QGroupBox("Lap Timing");
    QGroupBox *button_box = new QGroupBox("Controls");
    QGroupBox *tachspeed_box = new QGroupBox("Tach/Speed");

    QGridLayout *lap_timers_layout = new QGridLayout();
    QGridLayout *button_layout = new QGridLayout();
    QGridLayout *tachspeed_layout = new QGridLayout();
    QGridLayout *layout = new QGridLayout();

    // build lap timers
    for (int i = 0; i < NUMBER_OF_LAPS; i++)
    {
        lap_number[i] = new QLabel(QString("<h1>%1</h1>").arg(i + 1));

        int secs_per_lap = MAX_RUN_TIME_SECS / NUMBER_OF_LAPS;
        expected_lap_secs[i] = secs_per_lap * (i + 1);
        int lap_finish_mins = expected_lap_secs[i] / 60;
        int lap_finish_secs = expected_lap_secs[i] % 60;
        lap_expected_time[i] = new QLabel(QString("<h3>%1:%2</h3>").arg(lap_finish_mins).arg(lap_finish_secs, 2, 10, QChar('0')));

        lap_actual_time[i] = new QLabel("<h2><font color='grey'>0:00</font></h2>");

        lap_timers_layout->addWidget(lap_number[i], i, 0);
        lap_timers_layout->addWidget(lap_expected_time[i], i, 1);
        lap_timers_layout->addWidget(lap_actual_time[i], i, 2);
    }

    // control buttons
    run_status = new QLabel("<font color='red'>Stopped</font>");
    run_status->setAlignment(Qt::AlignCenter);
    run_status->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    start_button = new QPushButton("Start Run");
    start_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    next_lap_button = new QPushButton("Next Lap");
    next_lap_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    options_button = new QPushButton("Options");
    options_button->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    button_layout->addWidget(run_status, 0, 0);
    button_layout->addWidget(start_button, 0, 1);
    button_layout->addWidget(next_lap_button, 0, 2);
    button_layout->addWidget(options_button, 0, 3);

    // tach/speed
    atach = new QNeedleIndicator(this);
    atach->setRange(TACH_MIN, TACH_MAX);
    atach->setMajorTicks(TACH_MAX / 1000 + 1);
    atach->setMinorTicks(9);
    atach->setDigitFormat(QString("%.0f"));
    atach->setDigitFont(QFont("Fixed", 10, QFont::Bold));
    atach->setLabel(QString("RPM"));

    aspeed = new QNeedleIndicator(this);
    aspeed->setRange(SPEED_MIN, SPEED_MAX);
    aspeed->setMajorTicks(SPEED_MAX / 5 + 1);
    aspeed->setMinorTicks(4);
    aspeed->setDigitFormat(QString("%.0f"));
    aspeed->setDigitFont(QFont("Fixed", 10, QFont::Bold));
    aspeed->setLabel(QString("Speed"));

    dtach = new QLabel("0.0 rpm");
    dtach->setAlignment(Qt::AlignCenter);
    dtach->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    dspeed = new QLabel("0.0 mph");
    dspeed->setAlignment(Qt::AlignCenter);
    dspeed->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));
    davgspeed = new QLabel("0.0 avg");
    davgspeed->setAlignment(Qt::AlignCenter);
    davgspeed->setFont(QFont("Fixed", BUTTON_FONT_SIZE, QFont::Bold));


    tachspeed_layout->addWidget(atach, 0, 0);
    tachspeed_layout->addWidget(aspeed, 0, 1);
    tachspeed_layout->addWidget(dtach, 1, 0, 2, 1);
    tachspeed_layout->addWidget(dspeed, 1, 1);
    tachspeed_layout->addWidget(davgspeed, 2, 1);
    tachspeed_layout->setRowStretch(0, 1);

    // master layout
    lap_timers->setLayout(lap_timers_layout);
    button_box->setLayout(button_layout);
    tachspeed_box->setLayout(tachspeed_layout);
    layout->addWidget(lap_timers, 0, 0, 2, 1);
    layout->addWidget(button_box, 0, 1);
    layout->addWidget(tachspeed_box, 1, 1);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(1, 1);

    setLayout(layout);
    setWindowTitle("Urban Concept Dashboard");

    // variable initialization
    current_lap = 0;
    current_secs = 0;
    avgspeed_numerator = 0.0;
    avgspeed_denominator = 0;
    run_in_progress = false;

    // create options window
    options = new Options();

    // connect up signals and slots
    connect(start_button, SIGNAL(clicked()), this, SLOT(StartRunButtonClicked()));
    connect(next_lap_button, SIGNAL(clicked()), this, SLOT(NextLap()));
    connect(options_button, SIGNAL(clicked()), this, SLOT(OptionsButtonClicked()));

    // go full screen if in production mode
#ifdef RUNNING_IN_CAR
    showFullScreen();
#endif
}

Dashboard::~Dashboard()
{
    // nothing yet
}

void Dashboard::EcuUpdate(ecustate_t state)
{
    // update the tachometer
    atach->setValue(state.rpm);
    dtach->setText(QString("<h1>%1 rpm</h1>").arg(state.rpm));
}

void Dashboard::GpsUpdate(gpsstate_t state)
{
    // update the speedometer
    aspeed->setValue((int)(state.speed));
    dspeed->setText(QString("%1 mph").arg(state.speed, 2, 'f', 1, QChar('0')));

    // update the average speed
        // IMPORTANT NOTE: this assumes the updates are coming in from the gps
        // at fixed, regular intervals! it simply averages all the datapoints,
        // rather than trying to factor in time travelling at that speed
    // only updates when a run is in progress
    if (run_in_progress)
    {
        avgspeed_numerator += state.speed;
        avgspeed_denominator++;
        if (avgspeed_denominator != 0)
        {
            double avgspeed = avgspeed_numerator / (double)avgspeed_denominator;
            davgspeed->setText(QString("%1 avg").arg(avgspeed, 2, 'f', 1, QChar('0')));
        }
    }
}

void Dashboard::TmrUpdate(int ms)
{
    current_secs = ms / 1000;

    // update the current lap timer
    int mins = current_secs / 60;
    int secs = current_secs % 60;
    QString lap_time_str;
    if (current_secs <= expected_lap_secs[current_lap])
    {
        // we're on target
        lap_time_str = QString("<h2><font color='green'>%1:%2</font></h2>").arg(mins).arg(secs, 2, 10, QChar('0'));
    }
    else
    {
        // we're behind schedule
        lap_time_str = QString("<h2><font color='red'>%1:%2</font></h2>").arg(mins).arg(secs, 2, 10, QChar('0'));
    }
    lap_actual_time[current_lap]->setText(lap_time_str);
}

void Dashboard::OptionsButtonClicked()
{
#ifndef RUNNING_IN_CAR
    options->show();
#else
    options->showFullScreen();
#endif
    options->activateWindow();
}

void Dashboard::StartRunButtonClicked()
{
    if (!run_in_progress)
    {
        // start the run        
        start_button->setText("Stop Run");
        run_status->setText("<font color='green'>Running</font>");
        for (int i = 0; i < NUMBER_OF_LAPS; i++)
        {
            lap_actual_time[i]->setText("<h2><font color='grey'>0:00</font></h2>");
        }
        run_in_progress = true;
        current_lap = 0;
        avgspeed_numerator = 0.0;
        avgspeed_denominator = 0;
        emit StartRun();
    }
    else
    {
        // stop the run
        start_button->setText("Start Run");
        run_status->setText("<font color='red'>Stopped</font>");
        run_in_progress = false;
        emit StopRun();
    }
}

void Dashboard::NextLap()
{
    if (!run_in_progress) return;

    current_lap++;

    if (current_lap >= NUMBER_OF_LAPS)
    {
        // stop the run
        start_button->setText("Start Run");
        run_status->setText("<font color='red'>Stopped</font>");
        run_in_progress = false;
        emit StopRun();
    }

    // calculate the delta time for this lap
    int delta_secs = expected_lap_secs[current_lap - 1] - current_secs;
    int delta_m = delta_secs / 60;
    int delta_s = delta_secs % 60;
    QString lap_time_str;
    if (delta_secs < 0)
    {
        // behind schedule!
        delta_m *= -1;
        delta_s *= -1;
        lap_time_str = QString("<h2><font color='red'>+%1:%2</font></h2>").arg(delta_m).arg(delta_s, 2, 10, QChar('0'));
    }
    else
    {
        lap_time_str = QString("<h2><font color='green'>-%1:%2</font></h2>").arg(delta_m).arg(delta_s, 2, 10, QChar('0'));
    }
    lap_actual_time[current_lap - 1]->setText(lap_time_str);
}
