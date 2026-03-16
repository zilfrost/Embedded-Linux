#include "vehicledata.h"
#include <QtMath>

VehicleData::VehicleData(QObject *parent)
    : QObject(parent)
    , m_simTimer(new QTimer(this))
{
    connect(m_simTimer, &QTimer::timeout, this, &VehicleData::simulationTick);
    /* Update every 100 ms – smooth enough for a dashboard needle */
    m_simTimer->start(100);
}

/* ---- property setters ---- */

void VehicleData::setSpeed(int speed)
{
    speed = qBound(0, speed, 260);
    if (m_speed == speed) return;
    m_speed = speed;
    emit speedChanged();
}

void VehicleData::setBatteryLevel(int level)
{
    level = qBound(0, level, 100);
    if (m_batteryLevel == level) return;
    m_batteryLevel = level;
    emit batteryLevelChanged();
}

void VehicleData::setRpm(int rpm)
{
    rpm = qBound(0, rpm, 10000);
    if (m_rpm == rpm) return;
    m_rpm = rpm;
    emit rpmChanged();
}

void VehicleData::setMotorTemp(double temp)
{
    if (qFuzzyCompare(m_motorTemp, temp)) return;
    m_motorTemp = temp;
    emit motorTempChanged();
}

void VehicleData::setRange(int range)
{
    range = qBound(0, range, 600);
    if (m_range == range) return;
    m_range = range;
    emit rangeChanged();
}

void VehicleData::setGear(const QString &gear)
{
    if (m_gear == gear) return;
    m_gear = gear;
    emit gearChanged();
}

void VehicleData::setLeftSignal(bool active)
{
    if (m_leftSignal == active) return;
    m_leftSignal = active;
    emit leftSignalChanged();
}

void VehicleData::setRightSignal(bool active)
{
    if (m_rightSignal == active) return;
    m_rightSignal = active;
    emit rightSignalChanged();
}

void VehicleData::setIsCharging(bool charging)
{
    if (m_isCharging == charging) return;
    m_isCharging = charging;
    emit isChargingChanged();
}

/* ---- simulation ---- */

void VehicleData::simulationTick()
{
    ++m_simStep;

    /* Simulate a drive cycle (0-120 km/h acceleration then coast, repeat) */
    const int cycleLength = 400; /* steps == 40 s per cycle */
    int phase = m_simStep % cycleLength;

    int targetSpeed = 0;
    if (phase < 100)       targetSpeed = phase * 1;          /* 0→100 */
    else if (phase < 200)  targetSpeed = 100;                /* hold  */
    else if (phase < 300)  targetSpeed = 100 - (phase - 200);/* 100→0 */
    else                   targetSpeed = 0;                   /* idle  */

    int currentSpeed = m_speed;
    if (currentSpeed < targetSpeed) currentSpeed = qMin(currentSpeed + 2, targetSpeed);
    else                            currentSpeed = qMax(currentSpeed - 3, targetSpeed);
    setSpeed(currentSpeed);

    /* RPM roughly proportional to speed (EV single-speed) */
    setRpm(static_cast<int>(currentSpeed * 60.0));

    /* Motor temperature rises with load, cools slowly */
    double load = static_cast<double>(currentSpeed) / 100.0;
    double newTemp = m_motorTemp + (load * 0.08) - 0.02;
    newTemp = qBound(20.0, newTemp, 95.0);
    setMotorTemp(newTemp);

    /* Battery drains while driving, would charge when plugged in */
    if (!m_isCharging && currentSpeed > 0 && (m_simStep % 50 == 0)) {
        int newLevel = qMax(0, m_batteryLevel - 1);
        setBatteryLevel(newLevel);
        setRange(static_cast<int>(newLevel * 3.76));
    }

    /* Automatically engage Drive gear when moving */
    if (currentSpeed > 0 && m_gear == "P") setGear("D");
    if (currentSpeed == 0 && phase > 300)  setGear("P");
}
