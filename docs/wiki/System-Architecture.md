# System Architecture

The ESP32 Smart Home Automation firmware is built using a professional, modular C++ architecture heavily leveraging FreeRTOS for concurrency.

## Core Concepts

The system is designed around **Separation of Concerns**. Each hardware component (sensor, actuator, or display) is encapsulated within its own C++ class. These classes are self-contained PlatformIO libraries located in the `firmware/lib/` directory.

Instead of a single, blocking `loop()`, the application spawns 9 independent FreeRTOS tasks.

## FreeRTOS Task Map

| Task name | Component Class | Stack Size | Run Mode | Description |
| :--- | :--- | :--- | :--- | :--- |
| `TempRead` | `TemperatureSensor` | 4096 bytes | Always | Reads DHT11 every 2 seconds. Pushes readings to `tempQueue` and `displayQueue`. |
| `AutoFan` | `FanController` | 4096 bytes | **Suspendable** | Waits on `tempQueue`. Energizes the fan relay if temp > 33°C. |
| `LightRead` | `LightSensor` | 4096 bytes | Always | Reads LDR every 2 seconds. Pushes to `lightQueue`. |
| `AutoLight` | `LightController` | 4096 bytes | **Suspendable** | Waits on `lightQueue`. Energizes bulb relay if ADC < 2200. |
| `SmokeDet` | `SmokeSensor` | 4096 bytes | Always | Reads MQ-2 every 1 second. Triggers buzzers and BT alerts on high gas. |
| `TouchDet` | `TouchSensor` | 4096 bytes | Always | Polls capacitive touch pin. Triggers intrusion alerts. |
| `Ultrasonic`| `UltrasonicSensor`| 4096 bytes | Always | Pulses HC-SR04. Lights presence LED when object < 20cm. |
| `OLED` | `OLEDDisplay` | 8192 bytes | Always | Refreshes the SSD1306 dashboard at 5 Hz. |
| `SwitchCtrl`| `SwitchController`| 4096 bytes | Always | Parses incoming BT commands; manages task suspension for Manual mode. |

> [!NOTE] 
> By default, `AutoFan` and `AutoLight` are suspended on boot, meaning the system starts in **Manual mode**.

## Inter-Task Communication

Because multiple tasks run concurrently, they need safe ways to share data. The architecture uses two primary FreeRTOS primitives:

### 1. Queues
When a sensor reading directly drives an actuator, Queues are used. For example, `TempRead` pushes a temperature value into `tempQueue`. The `AutoFan` task blocks (sleeps) until a value appears in the queue, meaning it consumes exactly zero CPU cycles while waiting.

### 2. Thread-Safe Shared State
For data that needs to be accessed globally (like updating the OLED display or broadcasting status over Bluetooth), the system uses the `SharedState` singleton. 

Located in `firmware/include/SystemState.h`, this class encapsulates a `SystemState` struct and protects it with a FreeRTOS Mutex (`SemaphoreHandle_t`).

**Example: Writing state safely**
```cpp
SharedState::update([](SystemState& state) {
    state.temperature = currentTemp;
});
```
*The lambda function ensures the mutex is automatically acquired before the update and released immediately after.*

**Example: Reading state safely**
```cpp
SystemState currentState = SharedState::snapshot();
```
*Creates a safe, thread-local copy of the state that can be used without holding up other tasks.*

## Component Layout

The repository uses PlatformIO's auto-discovered `lib/` folder to maintain modularity:

```text
firmware/
├── lib/
│   ├── TemperatureSensor/    # DHT11 driver
│   ├── FanController/        # Fan relay logic
│   ├── BluetoothManager/     # BT Classic singleton
│   └── ... (one folder per component)
├── src/
│   ├── main.cpp              # Task spawning & bootstrap
│   └── SystemState.cpp       # Singleton instantiation
└── include/
    ├── Config.h              # Global pins & thresholds
    └── SystemState.h         # Shared state definition
```
This structure ensures zero tight-coupling between components. The `TemperatureSensor` does not know that the `FanController` exists; they only interact via FreeRTOS queues.
