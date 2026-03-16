# IPC Dashboard – Electric Vehicle Instrument Panel Cluster

A Qt/QML-based instrument panel cluster (IPC) for an electric vehicle, designed to run on a **Raspberry Pi 4** with a **7-inch HDMI display** (1024 × 600).

---

## Features

| Gauge / Widget | Description |
|---|---|
| **Speedometer** | Circular arc gauge, 0–260 km/h, colour-coded (green → amber → red) |
| **RPM bar** | Vertical bar, 0–10 000 RPM |
| **Motor temperature** | Vertical bar, 20–100 °C, colour-coded |
| **Battery indicator** | Horizontal bar, 0–100 %, charging animation |
| **Range** | Estimated remaining range in km |
| **Gear indicator** | P / R / N / D selector with active-colour highlighting |
| **Turn signals** | Blinking arrow indicators (left & right) |
| **Clock** | Live HH:MM clock in the top bar |
| **Status bar** | Odometer, brake indicator, drive mode |

All values are driven by a `VehicleData` C++ backend model that currently runs a **built-in simulation** so the UI works without physical hardware.  
In a real deployment, replace `VehicleData::simulationTick()` with CAN-bus or shared-memory reads from the vehicle ECU.

---

## Prerequisites

| Dependency | Version |
|---|---|
| Qt | ≥ 5.15 or Qt 6 |
| Qt Quick | included with Qt |
| Qt Quick Controls 2 | included with Qt |
| Qt Quick Shapes | included with Qt |
| GCC / Clang | C++17 |

### Install Qt on Ubuntu / Raspberry Pi OS

```bash
sudo apt update
sudo apt install -y qt5-default qml-module-qtquick2 \
    qml-module-qtquick-controls2 qml-module-qtquick-shapes \
    qtdeclarative5-dev qtquickcontrols2-5-dev build-essential
```

---

## Build on the Raspberry Pi (native compile)

```bash
cd IPC_Dashboard

# Option A – qmake
qmake IPC_Dashboard.pro
make -j$(nproc)

# Option B – CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The resulting binary is `IPC_Dashboard` (or `build/IPC_Dashboard`).

---

## Cross-compile from a Linux host (optional)

1. Install the Pi sysroot and toolchain:
   ```bash
   sudo apt install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
   ```
2. Uncomment the toolchain lines in `IPC_Dashboard.pro`:
   ```
   QMAKE_CC  = arm-linux-gnueabihf-gcc
   QMAKE_CXX = arm-linux-gnueabihf-g++
   ```
3. Point `qmake` at your Pi Qt sysroot and build as usual.

---

## Running on Raspberry Pi 4

### Bare-metal / eglfs (no X11 or Wayland, recommended for dashboard)

```bash
export QT_QPA_PLATFORM=eglfs
./IPC_Dashboard
```

### Under X11 / Wayland

```bash
export DISPLAY=:0   # X11
./IPC_Dashboard
```

### Autostart on boot

Create `/etc/systemd/system/ipc-dashboard.service`:

```ini
[Unit]
Description=EV Instrument Panel Cluster
After=multi-user.target

[Service]
ExecStart=/opt/IPC_Dashboard/bin/IPC_Dashboard
Environment=QT_QPA_PLATFORM=eglfs
Restart=on-failure
User=pi

[Install]
WantedBy=multi-user.target
```

Then enable it:

```bash
sudo systemctl enable --now ipc-dashboard.service
```

---

## Project structure

```
IPC_Dashboard/
├── CMakeLists.txt          # CMake build file
├── IPC_Dashboard.pro       # qmake project file
├── main.cpp                # Application entry point
├── vehicledata.h           # VehicleData C++ model (header)
├── vehicledata.cpp         # VehicleData C++ model (implementation + simulation)
├── resources.qrc           # Qt resource bundle
└── qml/
    ├── main.qml            # Root window (1024 × 600, dark theme)
    ├── SpeedGauge.qml      # Circular speedometer component
    ├── BatteryIndicator.qml# Horizontal battery bar component
    ├── GearIndicator.qml   # P/R/N/D gear selector component
    └── TurnSignal.qml      # Blinking turn-signal arrow component
```

---

## Integrating with real vehicle data

`VehicleData` exposes Qt properties that QML automatically binds to:

```
speed        (int)    – vehicle speed in km/h
batteryLevel (int)    – battery state-of-charge in %
rpm          (int)    – motor RPM
motorTemp    (double) – motor temperature in °C
range        (int)    – estimated range in km
gear         (QString)– current gear: "P", "R", "N", or "D"
leftSignal   (bool)   – left turn-signal active
rightSignal  (bool)   – right turn-signal active
isCharging   (bool)   – vehicle is plugged in and charging
```

Call the corresponding `set*()` slots from a CAN-bus reader thread or a shared-memory consumer to feed live data into the UI.
