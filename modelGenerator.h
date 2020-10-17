#ifndef MODELGENERATOR_H
#define MODELGENERATOR_H
#pragma once

#include "analysegraph\bglgraph.h"
#include "vulnerability.h"

#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>

void buildGALModel(
    diGraph eeArchitecturetecture, std::string filename,
    std::map<std::string, std::string> compInstance,
    std::vector<std::pair<unsigned int, std::vector<Vulnerability>>> vulMapping,
    std::unordered_map<unsigned int, std::string> uniqueElements,
    std::vector<std::tuple<unsigned int, unsigned int>> adjacency,
    unsigned int AttackerPos, unsigned int AttackerGoal, std::string guideword,
    Connections connect, ComputeUnits compute);

#endif