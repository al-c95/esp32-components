#pragma once

#include <string>
#include <map>
#include "esp_http_client.h"
#include "ISensorDataTransmitter.h"

class HttpPostTransmitter : public ISensorDataTransmitter
{
    public:
        explicit HttpPostTransmitter(const char* url);
        ~HttpPostTransmitter() override;

        HttpPostTransmitter(const HttpPostTransmitter&) = delete;
        HttpPostTransmitter& operator=(const HttpPostTransmitter&) = delete;

        bool transmit(
            const std::map<std::string, SensorValue>& sensor_values
        ) override;

    private:
        std::string _url;
        esp_http_client_handle_t _client = nullptr;
};