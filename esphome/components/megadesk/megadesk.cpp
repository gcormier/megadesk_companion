#include "megadesk.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace megadesk
  {

    static const char *TAG = "megadesk";

    void MegaDesk::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up MegaDesk...");
      // Read initial values
      this->write_str("<C0.0.");
      delay(1000);
      this->write_str("<R0.11.");
      delay(1000);
      this->write_str("<R0.12.");
      delay(1000);
    }

    int32_t MegaDesk::readdigits_()
    {
      // Only read what is already buffered. Calling read() past the buffered
      // data blocks the ESPHome main loop on the UART read timeout (~100ms)
      // and logs "Reading from UART timed out at byte 0!". The accumulated
      // value in this->digits_ persists across calls, so a field split across
      // multiple loop() iterations is resumed on the next call.
      while (available())
      {
        int r = read();
        if ((r < 0x30) || (r > 0x39))
        {
          // non-digit we're done, return what we have
          return this->digits_;
        }
        // it's a digit, add with base10 shift
        this->digits_ = 10 * this->digits_ + (r - 0x30);
        // keep reading...
      }
      // ran out of buffered data; resume on the next loop()
      return -1;
    }

    void MegaDesk::recvData_()
    {
      const int numChars = 2;
      const int numFields = 4; // read/store all 4 fields for simplicity, use only the last 3.
      // static variables allows segmented/char-at-a-time decodes
      static uint16_t receivedBytes[numFields];
      static uint8_t ndx = 0;
      int r; // read char/digit

      // read 2 chars (only what is buffered, to avoid a blocking read)
      while ((ndx < numChars) && available())
      {
        r = read();
        if ((ndx == 0) && (r != '>'))
        {
          // first char is not Tx, keep reading...
          continue;
        }
        receivedBytes[ndx] = r;
        ++ndx;
      }
      // read ascii digits
      while ((ndx >= numChars) && ((r = readdigits_()) != -1))
      {
        receivedBytes[ndx] = r;
        this->digits_ = 0; // clear
        if (++ndx == numFields)
        {
          // thats all 4 fields. parse/process them now and break-out.
          parseData_(receivedBytes[1],
                     receivedBytes[2],
                     receivedBytes[3]);
          ndx = 0;
          return;
        }
      }
    }

    void MegaDesk::parseData_(uint8_t command, uint16_t position, uint8_t push_addr)
    {
      if (command == '=')
      {
        // Guard each sensor independently: any of them may be unconfigured
        // (nullptr). Dereferencing an unconfigured sensor here caused a
        // null-pointer store-access-fault crash on the first height report.
        if (this->raw_height_ != nullptr)
        {
          this->raw_height_->publish_state(position);
        }
        if (this->height_cm_ != nullptr)
        {
          this->height_cm_->publish_state(getCentimeters(position));
        }
        if (this->height_inches_ != nullptr)
        {
          this->height_inches_->publish_state(getInches(position));
        }
      }
      else if (command == 'R')
      {
        if (push_addr == 11)
        {
          if (this->min_height_ != nullptr)
          {
            this->min_height_->publish_state(position);
          }
        }
        else if (push_addr == 12)
        {
          if (this->max_height_ != nullptr)
          {
            this->max_height_->publish_state(position);
          }
        }
      }
    }

    void MegaDesk::loop()
    {
      while (available())
      {
        recvData_();
      }
    }

    void MegaDesk::dump_config()
    {
    }

    float MegaDesk::getInches(int rawHeightValue)
    {
      return ((((rawHeightValue - 299.0) * (47.0 - 23.0)) / (6914.0 - 299.0)) + 23.0);
    }

    float MegaDesk::getCentimeters(int rawHeightValue)
    {
      return ((((rawHeightValue - 299.0) * (119.38 - 58.42)) / (6914.0 - 299.0)) + 58.42);
    }
  } // namespace megadesk
} // namespace esphome
