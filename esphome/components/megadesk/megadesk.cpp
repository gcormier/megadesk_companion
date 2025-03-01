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
      int r;
      while ((r = read()) > 0)
      {
        if ((r < 0x30) || (r > 0x39))
        {
          // non-digit we're done, return what we have
          return this->digits_;
        }
        // it's a digit, add with base10 shift
        this->digits_ = 10 * this->digits_ + (r - 0x30);
        // keep reading...
      }
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
        if (this->raw_height_)
        {
          this->raw_height_->publish_state(position);
          //this->height_cm_->publish_state(getCentimeters(position));
          //this->height_inches_->publish_state(getInches(position));
        }
      }
      else if (command == 'R')
      {
        if (push_addr == 11)
        {
          if (this->min_height_)
          {
            this->min_height_->publish_state(position);
          }
        }
        else if (push_addr == 12)
        {
          if (this->max_height_)
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
      return ((((rawHeightValue - 299) * (47 - 23)) / (6914 - 299)) + 23);
    }

    float MegaDesk::getCentimeters(int rawHeightValue)
    {
      return ((((rawHeightValue - 299) * (119.38 - 58.42)) / (6914 - 299)) + 58.42);
    }
  } // namespace megadesk
} // namespace esphome
