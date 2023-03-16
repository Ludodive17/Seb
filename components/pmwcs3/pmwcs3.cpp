#include "pmwcs3.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pmwcs3 {

static const char *const TAG = "pmwcs3";
	
	
void write_i2c_address_register(uint8_t address){
  if (!this->write_bytes(PMWCS3_REG_READ_START, nullptr, 0)) {
      this->status_set_warning();
      ESP_LOGW(TAG, "couldn't start a new reading with  PMWCS3_REG_READ_START registers");
      return;
    }
  delay(100);		
	
}
void PMWCS3Component::setup() {
   ESP_LOGCONFIG(TAG, "Setting up PMWCS3...");
}
	
void PMWCS3Component::update() { this->read_data_(); }

float PMWCS3Component::get_setup_priority() const { return setup_priority::DATA; }
		
void PMWCS3Component::dump_config() {
  ESP_LOGCONFIG(TAG, "PMWCS3");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with PMWCS3 failed!");
  }
  ESP_LOGI(TAG, "%s", this->is_failed() ? "FAILED" : "OK");
	
  LOG_UPDATE_INTERVAL(this);
	
  LOG_SENSOR("  ", "e25", this->e25_sensor_);	
  LOG_SENSOR("  ", "ec", this->ec_sensor_);
  LOG_SENSOR("  ", "temperatue", this->temperature_sensor_);
  LOG_SENSOR("  ", "vwc", this->vwc_sensor_);
}

void PMWCS3Component::read_data_() {
  uint8_t data[8];
  float e25, ec, temperature, vwc;
  	
 /////// Super important !!!! first activate reading PMWCS3_REG_READ_START (if not, return always the same values) ////
	
  if (!this->write_bytes(PMWCS3_REG_READ_START, nullptr, 0)) {
      this->status_set_warning();
      ESP_LOGW(TAG, "couldn't start a new reading with  PMWCS3_REG_READ_START registers");
      return;
    }
  delay(100);	
	
  if (!this->read_bytes(PMWCS3_REG_GET_DATA, (uint8_t *) &data, 8)){
     ESP_LOGW(TAG, "Error reading  PMWCS3_REG_GET_DATA registers");
     this->mark_failed();
     return;	  
  }
  if (this->e25_sensor_ != nullptr) {
	  e25 = ((data[1] << 8) | data[0])/100.0;
	  this->e25_sensor_->publish_state(e25);
	  ESP_LOGD(TAG, "e25: data[0]=%d, data[1]=%d, result=%f", data[0] , data[1] , e25);
  }
  if (this->ec_sensor_ != nullptr) {
	  ec = ((data[3] << 8) | data[2])/10.0;
	  this->ec_sensor_->publish_state(ec);
	  ESP_LOGD(TAG, "ec: data[2]=%d, data[3]=%d, result=%f", data[2] , data[3] , ec);
  }
  if (this->temperature_sensor_ != nullptr) {
	  temperature = ((data[5] << 8) | data[4])/100.0;
	  this->temperature_sensor_->publish_state(temperature);
	  ESP_LOGD(TAG, "temp: data[4]=%d, data[5]=%d, result=%f", data[4] , data[5] , temperature); 
  }
  if (this->vwc_sensor_ != nullptr) {
	  vwc = ((data[7] << 8) | data[6])/10.0;
	  this->vwc_sensor_->publish_state(vwc);
	  ESP_LOGD(TAG, "vwc: data[6]=%d, data[7]=%d, result=%f", data[6] , data[7] , vwc);
  }	
}

}  // namespace pmwcs3
}  // namespace esphome
