#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H
#pragma once

// class for properties of the BGL graph
#include "../architectureElements.h"
#include "bglgraph.h"

namespace graphMap {
struct nodes {
  int ID;
  std::string uuid;
  std::string nodName;
  std::string nodType;
};

struct findNodeUUID {
  std::string uuid;
  ;
  findNodeUUID(std::string uuid) : uuid(uuid) {}
  bool operator()(const nodes &m) const { return m.uuid == uuid; }
};

struct edges {
  int ID;
  std::string edName;
  unsigned int srcNode;
  unsigned int destNode;
  std::string edType;
};

struct findEdgeID {
  int ID;
  ;
  findEdgeID(int ID) : ID(ID) {}
  bool operator()(const edges &m) const { return m.ID == ID; }
};
} // namespace graphMap

// holds and manages the adjacency matrix
class GraphManager {

private:
  // boost BGL graph
  BGLGraph m_Graph;

  std::vector<graphMap::nodes> m_nodeMap;
  std::vector<graphMap::edges> m_edgeMap;
  std::map<std::pair<std::string, std::string>, unsigned int> m_mapping;
  unsigned int m_numNodes;
  unsigned int m_numEdges;

public:
  // constructor
  GraphManager();

  // deconstructor
  ~GraphManager();

  // add a new node
  void addNode(unsigned int nodeID, std::string nodeName);
  void addNode(unsigned int nodeID, std::string uuid, std::string nodeName);
  void addNode(unsigned int nodeID, std::string uuid, std::string nodeName,
               std::string nodeType);

  // delete existent node
  void deleteNode(std::string nodeUuid);

  // find unique node
  int findNode(std::string nodeUuid);
  void findNode(std::string nodeUuid, int &getID, std::string &getName);
  int findEdge(int edgeID);

  // add new edge between two nodes
  void addEdge(unsigned int edgeID, unsigned int srcNode, unsigned int destNode,
               std::string edgeName);
  void addEdge(unsigned int edgeID, std::string uuidSrcNode,
               std::string uuidDestNode, std::string edgeName);
  void addEdge(std::string uuidSrcNode, std::string uuidDestNode,
               std::string edgeName);
  void addEdge(std::string uuidSrcNode, std::string uuidDestNode,
               std::string edgeName, std::string edgeType);

  // delete existent node
  void deleteEdge(unsigned int edgeID);
  void deleteEdge(unsigned int srcNode, unsigned int destNode);
  void deleteEdge(std::string uuidSrcNode, std::string uuidDestNode);

  // build bgl graph form internal node and edge map
  void buildBGLGraph(void);

  void printGraph();

  BGLGraph getBGLGraph();

  std::vector<std::string> getNodes();

  std::vector<std::string> getEdges();
};

#endif GRAPH_MANAGER_H