#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace megadesk {

class MegaDesk : public Component,  public uart::UARTDevice {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_max_height_sensor(sensor::Sensor *sens) { this->max_height_ = sens; }
  void set_min_height_sensor(sensor::Sensor *sens) { this->min_height_ = sens; }
  void set_raw_height_sensor(sensor::Sensor *sens) { this->raw_height_ = sens; }
  void set_height_cm_sensor(sensor::Sensor *sens) { this->height_cm_ = sens; }
  void set_height_inches_sensor(sensor::Sensor *sens) { this->height_inches_ = sens; }
 protected:
  void parseData_(uint8_t command, uint16_t position, uint8_t push_addr);
  void recvData_();
  float getInches(int rawHeightValue);
  float getCentimeters(int rawHeightValue);
  
  sensor::Sensor *max_height_{nullptr};
  sensor::Sensor *min_height_{nullptr};
  sensor::Sensor *raw_height_{nullptr};
  sensor::Sensor *height_cm_{nullptr};
  sensor::Sensor *height_inches_{nullptr};

  int32_t readdigits_();
  uint32_t digits_ = 0;
};

}  // namespace megadesk
}  // namespace esphome
