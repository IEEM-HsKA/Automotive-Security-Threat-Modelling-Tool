#include "architectureElements.h"

ComputeUnits::ComputeUnits(void) {
  m_mapping.insert({"CD", 23});
  m_mapping.insert({"DVD", 24});
  m_mapping.insert({"OBD", 25});
  m_mapping.insert({"Camera", 26});
  m_mapping.insert({"Radar", 27});
  m_mapping.insert({"LiDAR", 28});
  m_mapping.insert({"ECU", 29});
  m_mapping.insert({"Gateway", 30});
  m_mapping.insert({"Aktor", 31});
  m_mapping.insert({"Sensor", 32});
  m_mapping.insert({"Wireless-Entry-Point", 33});
  m_mapping.insert({"Physical-Enrty-Point", 34});
}

Connections::Connections(void) {
  m_mapping.insert({"WLAN", 1});
  m_mapping.insert({"Bluetooth", 2});
  m_mapping.insert({"Cellular", 3});
  m_mapping.insert({"DVB", 4});
  m_mapping.insert({"DAB", 5});
  m_mapping.insert({"RDS", 6});
  m_mapping.insert({"TPMS", 7});
  m_mapping.insert({"RFID", 8});
  m_mapping.insert({"Infrared", 9});
  m_mapping.insert({"D-Bus", 10});
  m_mapping.insert({"CAN", 11});
  m_mapping.insert({"CAN-FD", 12});
  m_mapping.insert({"LIN", 13});
  m_mapping.insert({"MOST", 14});
  m_mapping.insert({"Flexray", 15});
  m_mapping.insert({"Ethernet", 16});
  m_mapping.insert({"P2P", 17});
  m_mapping.insert({"JTAG", 18});
  m_mapping.insert({"I2C", 19});
  m_mapping.insert({"Debug", 20});
  m_mapping.insert({"USB", 21});
  m_mapping.insert({"AUX", 22});
}
