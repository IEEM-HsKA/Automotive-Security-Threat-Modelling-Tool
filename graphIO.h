#ifndef GRAPHIO_H
#define GRAPHIO_H
#pragma once
#include "analysegraph\bglgraph.h"
#include "vulnerability.h"

#include <regex>

#include <QtCore\QRegularExpression.h>
#include <QtCore\qdebug.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptors.hpp>

class AttackPath {
public:
  unsigned int m_pathId;
  double m_riskValue;
  std::vector<diGraph::vertex_descriptor> m_pathElements;
  std::vector<std::string> m_pathEdges;
  AttackPath(unsigned int pathId, double riskValue,
             std::vector<diGraph::vertex_descriptor> pathElements,
             std::vector<std::string> pathEdges);
};

bool readAttackPaths(
    diGraph eeArchitecturetecture,
    std::unordered_map<unsigned int, std::string> uniqueElements,
    std::unordered_map<unsigned int, Vulnerability> globalVulMap,
    std::string entryPoint, diGraph &attackPaths, unsigned int &rootNode,
    std::vector<unsigned int> &endNodes);

void generateReport(
    std::vector<std::pair<double, AttackPath>> attackPathVector,
    diGraph SearchGraph, bool transpose,
    std::unordered_map<unsigned int, Vulnerability> globalVulMap,
    std::string entryPoint, std::string threat, std::string hazard);

void generateReport(
    std::vector<std::pair<double, AttackPath>> attackPathVector,
    diGraph SearchGraph, bool transpose,
    std::unordered_map<unsigned int, Vulnerability> globalVulMap,
    std::string entryPoint, std::string threat, std::string hazard,
    std::vector<std::pair<std::string, std::string>> &strAttackPaths);

bool isNumber(std::string token);

#endif GRAPHIO_H