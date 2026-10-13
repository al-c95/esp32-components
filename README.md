# esp32-components

A small collection of reusable C++ components for ESP32 projects (ESP-IDF): I2C master, SNTP time synchronization, WiFi connectivity, and HTTP sensor-data transmission helpers.

**Requirements:**
- ESP-IDF (tested with recent releases)

**Components & quick examples**

**I2CMaster:** A thin C++ wrapper around the ESP-IDF I2C master APIs.

Basic usage:
```cpp
#include "I2CMaster.h"

// Create I2C master on port 0, SDA=21, SCL=22, 400kHz, internal pullups enabled
I2CMaster i2c(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22, 400000, true);

// Write to device at address 0x40
uint8_t buf[] = { 0x01, 0x02 };
esp_err_t err = i2c.write(0x40, buf, sizeof(buf));

// Read 4 bytes
uint8_t in[4];
err = i2c.read(0x40, in, sizeof(in));
```

See the implementation in [I2CMaster/I2CMaster.cpp](I2CMaster/I2CMaster.cpp#L1).

**time_sync:** Simple SNTP starter and helpers to wait for a valid RTC time.

Basic usage:
```cpp
#include "time_sync.h"

// Start SNTP
time_sync_start();

// Wait up to 10 seconds for a valid time
bool ok = time_sync_wait_until_synced(10);
if (ok) {
	int64_t now_ms = time_sync_utc_now_ms();
	// use now_ms
}
```

See the implementation in [time_sync/time_sync.cpp](time_sync/time_sync.cpp#L1).

**WiFi:** Small helper class to initialise WiFi, register connection callbacks and connect to an AP.

Basic usage:
```cpp
#include "WiFi.h"
#include "esp_netif.h"

// Called when connected
void on_connected(const ip4_addr_t *ip)
{
	// ip->addr contains IPv4 address (network byte order)
}

// Called when disconnected
void on_disconnected()
{
}

WiFi wifi;
wifi.init(on_connected, on_disconnected);
wifi.connect("MySSID", "MyPassword");
```

See the implementation in [WiFi/WiFi.cpp](WiFi/WiFi.cpp#L1).

**sensor_transport:** A simple HTTP POST transmitter for sensor readings. `HttpPostTransmitter` implements `ISensorDataTransmitter` and serializes a map of named sensor values into a JSON object. Supported value types are `float`, `int`, and `int64_t`.

Basic usage (after WiFi is connected):
```cpp
#include "HttpPostTransmitter.h"

HttpPostTransmitter transmitter("http://192.168.1.104:3000/sensor-data/wind");

std::map<std::string, SensorValue> values = {
    {"timestampUtc", int64_t{1791608178}},
    {"speed", 1.7f},
    {"direction", 233}
};

bool ok = transmitter.transmit(values);
```

The transmitter sends `Content-Type: application/json` and returns `true` for an HTTP 2xx response, or `false` if the request fails or receives a non-2xx status. The caller is responsible for establishing network connectivity and obtaining valid timestamps.

The component depends on ESP-IDF's `esp_http_client`. Its shared headers are `HttpPostTransmitter.h`, `ISensorDataTransmitter.h`, and `SensorReading.h`.

See the implementation in [sensor_transport/HttpPostTransmitter.cpp](sensor_transport/HttpPostTransmitter.cpp#L1).

**Using a component from this repository**

Add the desired component to your project's `main/idf_component.yml`, for example:

```yaml
dependencies:
  sensor_transport:
    git: "https://github.com/al-c95/esp32-components"
    path: "sensor_transport"
    version: "master"
```

Then run `idf.py reconfigure` and `idf.py build`. You can add the other components in the same way, using their respective directory names as `path` values.

**Notes**
- These components are minimal helpers — adapt them to your project's error handling and event model.
- The current `sensor_transport` implementation uses a fixed 256-byte buffer for its JSON payload, so long payloads may be truncated. This will be addressed in a later change.
- Build and flash using ESP-IDF tooling as usual.
