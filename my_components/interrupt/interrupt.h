#pragma once

#include "esphome/core/component.h"
#include "esphome/components/esp32/gpio.h"

namespace esphome {
namespace interrupt {

  constexpr int RATE_SAMPLES = 10;
class Interrupt : public Component {
 public:
  void set_pin(esphome::esp32::ESP32InternalGPIOPin* pin) { pin_ = pin; }

  void setup() override;
  void loop() override;
  
  volatile uint32_t cnt = 0;
  volatile uint64_t dsum = 0;
  volatile uint32_t csum = 0;
  volatile float rate = 0.f;
 protected:
  esphome::esp32::ESP32InternalGPIOPin* pin_=NULL;
  volatile bool triggered_{false};
  volatile int64_t last_interrupt_us_ = 0;
  
  static Interrupt *instance_;
  static void IRAM_ATTR isr(void *arg);

    uint32_t rates[RATE_SAMPLES] = {};
    uint32_t rate_index = 0;
    uint32_t rate_sum = 0;
    uint32_t last_count = 0;
    uint32_t last_ms = 0;
    
    float get_rate(uint32_t count) {
      uint32_t now = esp_log_timestamp();
      uint32_t dt = now - last_ms;
    
      if (dt == 0)
        return 0;
    
      float rate = (count - last_count) * 1000.0f / dt;
    
      last_count = count;
      last_ms = now;
    
      rate_sum -= rates[rate_index];
      rates[rate_index] = rate;
      rate_sum += rate;
      rate_index = (rate_index + 1) % RATE_SAMPLES;
    
      return rate_sum / RATE_SAMPLES;
    }
};

}
}