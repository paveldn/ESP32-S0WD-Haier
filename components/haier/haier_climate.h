#ifndef _HAIER_CLIMATE_H
#define _HAIER_CLIMATE_H

#include <chrono>
#include "esphome/components/climate/climate.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace haier {

class HaierClimate :    public esphome::Component,
                        public esphome::climate::Climate,
                        public esphome::uart::UARTDevice
{
public:
    HaierClimate() = delete;
    HaierClimate(const HaierClimate&) = delete;
    HaierClimate& operator=(const HaierClimate&) = delete;
    HaierClimate(esphome::uart::UARTComponent* parent);
    ~HaierClimate();
    void setup() override;
    void loop() override;
    void control(const esphome::climate::ClimateCall &call) override;
    float get_setup_priority() const override { return esphome::setup_priority::HARDWARE ; }
    void set_send_wifi_signal(bool sendWifi) { mSendWifiSignal = sendWifi; };
    void set_outdoor_temperature_sensor(esphome::sensor::Sensor *sensor) { mOutdoorSensor = sensor; }

protected:
    esphome::climate::ClimateTraits traits() override;
    void sendData(const uint8_t * message, size_t size, bool withCrc = true);
    void processStatus(const uint8_t* packet, uint8_t size);
    void handleIncomingPacket();
    void getSerialData();
private:
    enum ProtocolPhases
    {
        // Initialization
        psSendingInit1 = 0,
        psWaitingAnswerInit1,
        psSendingInit2,
        psWaitingAnswerInit2,
        psSendingFirstStatusRequest,
        psWaitingFirstStatusAnswer,
        // Functional state
        psIdle,
        psSendingStatusRequest,
        psWaitingStatusAnswer,
        psSendingUpdateSignalRequest,
        psWaitingUpateSignalAnswer,
        psSendingSignalLevel,   // No answer to this command
    };
    ProtocolPhases      mPhase;
    SemaphoreHandle_t   mReadMutex;
    uint8_t*            mLastPacket;
    uint8_t             mFanModeFanSpeed;
    uint8_t             mOtherModesFanSpeed;
    bool                mSendWifiSignal;
    esphome::sensor::Sensor*                mOutdoorSensor;
    esphome::climate::ClimateTraits         mTraits;
    std::chrono::steady_clock::time_point   mLastByteTimestamp;         // For packet timeout
    std::chrono::steady_clock::time_point   mLastRequestTimestamp;      // For answer timeout
    std::chrono::steady_clock::time_point   mLastValidStatusTimestamp;  // For protocol timeout
    std::chrono::steady_clock::time_point   mLastStatusRequest; // To request AC status
    std::chrono::steady_clock::time_point   mLastSignalRequest; // To send WiFI signal level

};

} // namespace haier
} // namespace esphome


#endif // _HAIER_CLIMATE_H