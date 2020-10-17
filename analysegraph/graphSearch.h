#ifndef GRAPHSEARCH_H
#define GRAPHSEARCH_H
#pragma once

#include "bglgraph.h"

typedef std::map<diGraph::vertex_descriptor, boost::default_color_type>
    colormap;
extern colormap vertex_coloring;

namespace search {

void findPaths(
    diGraph SearchGraph, int source, int destination, bool *foundDestination,
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *paths);
void printAllPaths(
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *foundPaths,
    diGraph origGraph, diGraph &pathGraph);
void createReducedGraphFromPaths(
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *foundPaths,
    diGraph origGraph, diGraph &pathGraph);

} // namespace search

class PathFinder : public boost::default_dfs_visitor {

public:
  std::vector<diGraph::vertex_descriptor> m_spath;
  std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *m_allPaths;
  int m_source;
  int m_destination;
  bool *m_result;
  int m_path_index = 0;
  int unsigned m_numPaths = 0;

  PathFinder(
      int sourceVertex, int destinationVertex, bool *result,
      std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *paths);

  template <typename Vertex, typename Graph>
  void discover_vertex(Vertex u, const Graph &g) {
    diGraph::vertex_descriptor vertex;
    vertex = boost::vertex(u, g);

    boost::default_color_type color = vertex_coloring[vertex];

    m_spath.push_back(vertex);
    m_path_index++;
  }

  template <typename Vertex, typename Graph>
  void finish_vertex(Vertex u, const Graph &g) {
    diGraph::vertex_descriptor vertex;
    vertex = boost::vertex(u, g);

    if (u == m_destination) {
      *m_result = true;
      // save current path
      m_allPaths->insert(std::make_pair(m_numPaths, m_spath));
      m_numPaths++;
    }

    if (u == m_source) {
      for (auto vd : boost::make_iterator_range(vertices(g))) {
        vertex_coloring[vd] = boost::black_color;
      }
    }

    for (int i = m_path_index; i > 1; i--) {
      vertex = boost::vertex(m_spath[i - 1], g);
      if (vertex_coloring[vertex] == boost::gray_color) {
        break;
      } else {
        vertex_coloring[vertex] = boost::white_color;
        m_spath.pop_back();
        m_path_index--;
      }
    }
  }
};

#endif GRAPHSEARCH_H