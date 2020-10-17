#ifndef ARCHITECTUREELEMENTS_H
#define ARCHITECTUREELEMENTS_H
#pragma once

#include <cstdlib>
#include <map>
#include <string>
#include <unordered_map>

class ComputeUnits {
public:
  //<component name, compute type>
  std::unordered_map<std::string, unsigned int> m_mapping;
  //<compute type, isHmi>
  std::unordered_map<unsigned int, bool> m_hmi;
  ComputeUnits(void);
};

class Connections {
public:
  //<interface name, interface type>
  std::unordered_map<std::string, unsigned int> m_mapping;
  ////<interface type, isWireless>
  // std::unordered_map<unsigned int, bool> wireless;
  Connections(void);
};

#endif ARCHITECTUREELEMENTS_H
