#include "megadesk.h"
#include "esphome/core/log.h"

namespace esphome {
namespace megadesk {

static const char *TAG = "megadesk";

int32_t MegaDesk::readdigits_() {
  int r;
  while ((r = read()) > 0) {
    if ((r < 0x30) || (r > 0x39)) {
      // non-digit we're done, return what we have
      return this->digits_;
    }
    // it's a digit, add with base10 shift
    this->digits_ = 10*this->digits_ + (r-0x30);
    // keep reading...
  }
  return -1;
}

void MegaDesk::recvData_() {
  const int numChars = 2;
  const int numFields = 4; // read/store all 4 fields for simplicity, use only the last 3.
  // static variables allows segmented/char-at-a-time decodes
  static uint16_t receivedBytes[numFields];
  static uint8_t ndx = 0;
  int r; // read char/digit

  // read 2 chars
  while ((ndx < numChars) && ((r = read()) != -1))
  {
    if ((ndx == 0) && (r != '>'))
    {
      // first char is not Tx, keep reading...
      continue;
    }
    receivedBytes[ndx] = r;
    ++ndx;
  }
  // read ascii digits
  while ((ndx >= numChars) && ((r = readdigits_()) != -1)) {
    receivedBytes[ndx] = r;
    this->digits_ = 0; // clear
    if (++ndx == numFields) {
      // thats all 4 fields. parse/process them now and break-out.
      parseData_(receivedBytes[1],
                 receivedBytes[2],
                 receivedBytes[3]);
      ndx = 0;
      return;
    }
  }
}

void MegaDesk::parseData_(uint8_t command, uint16_t position, uint8_t push_addr) {
  if (command == '=')
  {
    if (this->raw_height_) {
      this->raw_height_->publish_state(position);
    }
  } else if (command == 'R'){
    if (push_addr == 11){
      if (this->min_height_) {
        this->min_height_->publish_state(position);
      }
    } else if (push_addr == 12){
      if (this->max_height_) {
        this->max_height_->publish_state(position);
      }
    }
  }
}

void MegaDesk::loop() {
  while (available()) {
    recvData_();
  }
}

void MegaDesk::dump_config() {
}

}  // namespace megadesk
}  // namespace esphome
