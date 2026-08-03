# esp32-components

A small collection of reusable components for ESP32 projects (ESP-IDF): I2C master helper, SNTP time synchronization helpers, and a simple WiFi helper.

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

**Notes**
- These components are minimal helpers — adapt them to your project's error handling and event model.
- Build and flash using ESP-IDF tooling as usual.
