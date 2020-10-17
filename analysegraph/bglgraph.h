#ifndef BGLGRAPH_H
#define BGLGRAPH_H
#pragma once

#include <iostream>
#include <map>
#include <string>

#ifndef Q_MOC_RUN
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/directed_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <boost/unordered_map.hpp>
#endif


namespace graphProps {
struct VertexProps {
  unsigned int vertexID = 0;
  std::string vertexName = "";
  unsigned short compType = 0;
};

struct EdgeProps {
  unsigned int edgeID = 0;
  std::string edgeName = "";
  unsigned short connectType = 0;
};
}

typedef boost::property<
    boost::vertex_name_t, std::string,
    boost::property<
        boost::vertex_index_t, int,
        boost::property<boost::vertex_color_t, boost::default_color_type,
                        graphProps::VertexProps>>>
    vertex_properties;
typedef boost::property<
    boost::edge_weight_t, float,
    boost::property<boost::edge_index_t, int, graphProps::EdgeProps>>
    edge_properties;
typedef boost::adjacency_list<boost::multisetS, boost::vecS, boost::directedS,
                              vertex_properties, edge_properties>
    diGraph;


// setup up a simple BGL graph for reading the generated dot-file (from
// ITS-Tools)
struct Vertex {
  std::string label, ID;
};

struct Edge {
  std::string label;
};
// bidirectional coud be a problem!
typedef boost::property<boost::graph_name_t, std::string> graph_p;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              Vertex, Edge, graph_p>
    graph_t;


class BGLGraph {
public:
  BGLGraph();
  ~BGLGraph();

  void addVertex(unsigned int vertexID, std::string vertexName);
  void addVertex(unsigned int vertexID, std::string vertexName,
                 unsigned short computeType);
  void deleteVertex(unsigned int vertexID);
  void getVertexProperties();
  void addEdge(unsigned int edgeID, unsigned int srcVertexID,
               unsigned int dstVertexID, std::string edgeName);
  void addEdge(unsigned int edgeID, unsigned int srcVertexID,
               unsigned int dstVertexID, std::string edgeName,
               unsigned short connectionType);
  void deleteEdge(unsigned int srcVertexID, unsigned int dstVertexID);
  void getEdgeProperties();
  void printGraph();
  void printFoundpaths();
  void getAdjacencyMatrix();
  void getAdjacencyMatrix(std::vector<std::vector<int>> *adjacency);
  void findPaths(int start, int stop);
  void traceOutGoingEdges();
  diGraph getDiaGraph();

private:
  diGraph m_diGraph;
  diGraph m_paths;

  std::vector<std::string> m_verticesNames;
  std::vector<std::string> m_edgesNames;

  std::vector<float> m_verticesWeights;
  std::vector<float> m_edgesWeights;

  // Pair of (id of source item, id of target item)
  std::vector<std::pair<int, int>> m_conVertices;
  // Tuple of (id of source item, id of target item, id of connection-type)
  std::vector<std::tuple<int, int, int>> m_verticesEdges;

  int m_numberVertices;
  int m_numberEdges;
};

void getConnectingIDs(
    const diGraph &eeArchitecturetecture,
    std::vector<std::tuple<unsigned int, unsigned int>> &adjacency,
    std::unordered_map<unsigned int, std::string> &uniqueElements);

#endif BGLGRAPH_H