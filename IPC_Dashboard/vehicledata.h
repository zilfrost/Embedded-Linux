#ifndef VEHICLEDATA_H
#define VEHICLEDATA_H

#include <QObject>
#include <QTimer>

/**
 * @brief VehicleData – backend model for the EV instrument panel cluster.
 *
 * Exposes vehicle state (speed, battery, RPM, temperature, range, gear,
 * turn-signal activity) to QML via Q_PROPERTY bindings.
 *
 * In a real deployment the setter slots are driven by CAN-bus frames or a
 * shared-memory region updated by the vehicle's ECU.  Here a QTimer drives
 * a simple simulation so the UI can be validated on the desktop / Pi without
 * external hardware.
 */
class VehicleData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int    speed       READ speed       WRITE setSpeed       NOTIFY speedChanged)
    Q_PROPERTY(int    batteryLevel READ batteryLevel WRITE setBatteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(int    rpm         READ rpm         WRITE setRpm         NOTIFY rpmChanged)
    Q_PROPERTY(double motorTemp   READ motorTemp   WRITE setMotorTemp   NOTIFY motorTempChanged)
    Q_PROPERTY(int    range       READ range       WRITE setRange       NOTIFY rangeChanged)
    Q_PROPERTY(QString gear       READ gear        WRITE setGear        NOTIFY gearChanged)
    Q_PROPERTY(bool   leftSignal  READ leftSignal  WRITE setLeftSignal  NOTIFY leftSignalChanged)
    Q_PROPERTY(bool   rightSignal READ rightSignal WRITE setRightSignal NOTIFY rightSignalChanged)
    Q_PROPERTY(bool   isCharging  READ isCharging  WRITE setIsCharging  NOTIFY isChargingChanged)

public:
    explicit VehicleData(QObject *parent = nullptr);

    int     speed()        const { return m_speed; }
    int     batteryLevel() const { return m_batteryLevel; }
    int     rpm()          const { return m_rpm; }
    double  motorTemp()    const { return m_motorTemp; }
    int     range()        const { return m_range; }
    QString gear()         const { return m_gear; }
    bool    leftSignal()   const { return m_leftSignal; }
    bool    rightSignal()  const { return m_rightSignal; }
    bool    isCharging()   const { return m_isCharging; }

public slots:
    void setSpeed(int speed);
    void setBatteryLevel(int level);
    void setRpm(int rpm);
    void setMotorTemp(double temp);
    void setRange(int range);
    void setGear(const QString &gear);
    void setLeftSignal(bool active);
    void setRightSignal(bool active);
    void setIsCharging(bool charging);

signals:
    void speedChanged();
    void batteryLevelChanged();
    void rpmChanged();
    void motorTempChanged();
    void rangeChanged();
    void gearChanged();
    void leftSignalChanged();
    void rightSignalChanged();
    void isChargingChanged();

private slots:
    void simulationTick();

private:
    int     m_speed        = 0;
    int     m_batteryLevel = 85;
    int     m_rpm          = 0;
    double  m_motorTemp    = 25.0;
    int     m_range        = 320;
    QString m_gear         = "P";
    bool    m_leftSignal   = false;
    bool    m_rightSignal  = false;
    bool    m_isCharging   = false;

    QTimer *m_simTimer     = nullptr;
    int     m_simStep      = 0;
};

#endif // VEHICLEDATA_H
