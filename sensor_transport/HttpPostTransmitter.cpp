#include "HttpPostTransmitter.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <iomanip>

static const char *TAG = "HTTP_POST_TRANSMITTER";

HttpPostTransmitter::HttpPostTransmitter(const char* url)
    : _url(url)
{
    esp_http_client_config_t config = {};
    config.url = _url.c_str();
    config.method = HTTP_METHOD_POST;
    config.timeout_ms = 5000;

    _client = esp_http_client_init(&config);

    if (!_client)
    {
        ESP_LOGE(TAG, "Failed to initialise HTTP client!");
        return;
    }

    esp_http_client_set_header(
        _client,
        "Content-Type",
        "application/json"
    );
}

HttpPostTransmitter::~HttpPostTransmitter()
{
    if (_client)
    {
        esp_http_client_cleanup(_client);
        _client = nullptr;
    }
}

bool HttpPostTransmitter::transmit(
    const std::map<std::string, SensorValue>& sensor_values)
{
    if (!_client)
    {
        ESP_LOGE(TAG, "HTTP client not initialised!");
        return false;
    }

    std::ostringstream json;
    json.setf(std::ios::fixed, std::ios::floatfield);
    json.precision(1);

    json << "{";

    for (auto it = sensor_values.begin(); it != sensor_values.end(); ++it)
    {
        json << "\"" << it->first << "\": ";

        std::visit([&](auto&& arg)
        {
            json << arg;
        }, it->second);

        if (std::next(it) != sensor_values.end())
        {
            json << ", ";
        }
    }

    json << "}";

    std::string post_data = json.str();

    esp_err_t err = esp_http_client_set_post_field(
        _client,
        post_data.c_str(),
        static_cast<int>(post_data.size())
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set POST payload: %s",
                 esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "POST data: %s", post_data.c_str());

    err = esp_http_client_perform(_client);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "POST failed: %s",
                 esp_err_to_name(err));
        return false;
    }

    int status = esp_http_client_get_status_code(_client);

    ESP_LOGI(TAG, "HTTP status=%d", status);

    return status >= 200 && status < 300;
}