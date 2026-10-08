#include "interrupt.h"

#include "driver/gpio.h"
#include "esp_log.h"

namespace esphome {
namespace interrupt {

static const char *TAG = "interrupt";

Interrupt *Interrupt::instance_ = nullptr;

void Interrupt::setup() {
  instance_ = this;
    
  const int gpio = pin_->get_pin();
  gpio_config_t io_conf = {};
  //io_conf.pin_bit_mask = 1ULL << pin_;
  io_conf.pin_bit_mask = 1ULL << gpio;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.intr_type = GPIO_INTR_POSEDGE;

  gpio_config(&io_conf);

  gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
  gpio_isr_handler_add(static_cast<gpio_num_t>(gpio), isr, nullptr);
}

void IRAM_ATTR Interrupt::isr(void *arg) {
  if (instance_ == nullptr)
    return;

  const int64_t now = esp_timer_get_time();
  int64_t d = now - instance_->last_interrupt_us_;
  if (d < 100000)
    return;
    
  instance_->last_interrupt_us_ = now;
  instance_->cnt++;
  instance_->csum++;
  instance_->dsum+=d;
  instance_->triggered_ = true;
}

void Interrupt::loop() {
  if (!triggered_)
    return;

  triggered_ = false;
  if(csum>=5){
      rate = 60.*1000.*1000./(dsum/(double)csum);
      dsum=0;
      csum=0;
  }
  ESP_LOGD(TAG, "Interrupt received");
}

}
}