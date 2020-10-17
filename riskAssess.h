#ifndef RISKASSESS_H
#define RISKASSESS_H
#pragma once

#include "analysegraph\bglgraph.h"
#include "graphIO.h"
#include "vulnerability.h"

#include <iostream>
#include <mutex>
#include <thread>

#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptors.hpp>

#include <QtConcurrent\qtconcurrentrun.h>
#include <QtCore\QCoreApplication>
#include <QtCore\QThread>
#include <QtCore\qfuturesynchronizer.h>

template <typename Container> struct container_hash {
  std::size_t operator()(Container const &c) const {
    return boost::hash_range(c.begin(), c.end());
  }
};

class LayerModel {
public:
  LayerModel::LayerModel(void);
  std::unordered_map<std::string, unsigned int> m_entity;
};

class RiskAssessment : public boost::default_dfs_visitor {

public:
  diGraph &m_G;
  std::vector<diGraph::vertex_descriptor> m_spath;
  std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *m_allPaths;

  int m_source = 0;
  int m_destination = 0;
  bool *m_result;
  int m_path_index = 0;
  int unsigned m_numPaths = 0;

  template <typename Vertex, typename Graph>
  void discover_vertex(Vertex u, const Graph &g) {
    diGraph::vertex_descriptor vertex;
    vertex = boost::vertex(u, g);

    m_spath.push_back(vertex);
    m_path_index++;
  }

  template <typename Vertex, typename Graph>
  void finish_vertex(Vertex u, const Graph &g) {
    diGraph::vertex_descriptor vertex;
    auto v_color_map = get(boost::vertex_color_t(), m_G);
    vertex = boost::vertex(u, g);

    if (u == m_destination) {
      *m_result = true;
      // save current path
      m_allPaths->insert(std::make_pair(m_numPaths, m_spath));
      m_numPaths++;
    }

    if (u == m_source) {
      for (auto vd : boost::make_iterator_range(vertices(g))) {
        put(v_color_map, vd, boost::black_color);
      }
    }

    for (int i = m_path_index; i > 1; i--) {
      vertex = boost::vertex(m_spath[i - 1], g);

      if (boost::gray_color == v_color_map[vertex]) {

        break;
      } else {
        put(v_color_map, vertex, boost::white_color);
        m_spath.pop_back();
        m_path_index--;
      }
    }
  }

  // constructor
  RiskAssessment(
      diGraph &G, int sourceVertex, int destinationVertex, bool *result,
      std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *paths);
};

void findAllPaths(int sourceVertex, int destinationVertex, diGraph G,
                  std::vector<std::pair<double, AttackPath>> &foundAttackPaths);

void genRiskPaths(diGraph SearchGraph, int source,
                  std::vector<unsigned int> destinations, bool transpose,
                  std::vector<std::pair<double, AttackPath>> &attackPathVector);

void genRiskPathsMT(
    diGraph SearchGraph, int source, std::vector<unsigned int> destinations,
    bool transpose,
    std::vector<std::pair<double, AttackPath>> &attackPathVector);

bool compare(std::pair<double, AttackPath> a, std::pair<double, AttackPath> b);

void createRanking(std::vector<std::pair<double, AttackPath>> &attackPathVector,
                   unsigned int severityScaling, LayerModel attackerModel,
                   std::string typeOfEntryPoint);

int checkPathsDuplicates(
    std::vector<std::vector<std::string>> &pathDescriptions);

#endif