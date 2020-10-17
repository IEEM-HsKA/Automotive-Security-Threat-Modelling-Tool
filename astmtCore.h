#ifndef ASTMT_CORE_H
#define ASTMT_CORE_H
#pragma once

#include "DataBaseHandler.h"
#include "analysegraph\bglgraph.h"
#include "analysegraph\graphSearch.h"
#include "architectureElements.h"
#include "graphIO.h"
#include "modelGenerator.h"
#include "riskAssess.h"
#include "vulnerability.h"

#include <QtConcurrent\qtconcurrentrun.h>
#include <QtCore\QCoreApplication>
#include <QtCore\QRegularExpression.h>
#include <QtCore\QThread>
#include <QtCore\qdebug.h>

#include <boost/algorithm/string.hpp>
//#include <boost/process.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace ASTMT {

bool startASTMTcore(
    diGraph &eeArchitecture,
    std::unordered_map<unsigned int, std::string> componentNames,
    std::vector<QList<QString>> sgmTable,
    std::vector<std::pair<std::string, std::string>> &stringResult);
}
#endif ASTMT_CORE_H
