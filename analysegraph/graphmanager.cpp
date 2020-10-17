#include "graphmanager.h"

GraphManager::GraphManager() {
  m_numEdges = 0;
  m_numNodes = 0;
}

GraphManager::~GraphManager() {}

/// <summary>Add a new node to ee/architecture</summary>
void GraphManager::addNode(unsigned int nodeID, std::string nodeName) {
  graphMap::nodes newNode = {nodeID, "NULL", nodeName};
  m_nodeMap.push_back(newNode);
}

/// <summary>Add a new node to ee/architecture</summary>
void GraphManager::addNode(unsigned int nodeID, std::string nodeUuid,
                           std::string nodeName) {
  graphMap::nodes newNode = {nodeID, nodeUuid, nodeName};
  m_nodeMap.push_back(newNode);
}

/// <summary>Add a new node to ee/architecture</summary>
void GraphManager::addNode(unsigned int nodeID, std::string nodeUuid,
                           std::string nodeName, std::string nodeType) {
  graphMap::nodes newNode = {nodeID, nodeUuid, nodeName, nodeType};
  m_nodeMap.push_back(newNode);
}

/// <summary>Delete existent node from ee/architecture</summary>
void GraphManager::deleteNode(std::string nodeUuid) {
  // convert from uuid to nodeID -> search in std vector
  auto it = std::find_if(m_nodeMap.begin(), m_nodeMap.end(),
                         graphMap::findNodeUUID(nodeUuid));
  m_Graph.deleteVertex(it->ID);
}

/// <summary>Find unique node based on its UUID and return its integer ID</summary>
/// <returns>Integer number for the found node</returns>
int GraphManager::findNode(std::string nodeUuid) {
  // find the relevant node in the vector and return it
  auto it = std::find_if(m_nodeMap.begin(), m_nodeMap.end(),
                         graphMap::findNodeUUID(nodeUuid));
  return it->ID;
}

/// <summary>Find unique node based on its UUID and return all properties of the node</summary>
/// <returns>All properties of a node</returns>
void GraphManager::findNode(std::string nodeUuid, int &getID,
                            std::string &getName) {
  // find the relevant node in the vector and return it
  auto it = std::find_if(m_nodeMap.begin(), m_nodeMap.end(),
                         graphMap::findNodeUUID(nodeUuid));
  getID = it->ID;
  getName = it->nodName;
}

/// <summary>Add new connection in ee/architecture</summary>
void GraphManager::addEdge(unsigned int edgeID, unsigned int srcNode,
                           unsigned int destNode, std::string edgeName) {

  graphMap::edges newEdge = {edgeID, edgeName, srcNode, destNode};
  m_edgeMap.push_back(newEdge);
}

/// <summary>Add new connection in ee/architecture</summary>
void GraphManager::addEdge(unsigned int edgeID, std::string uuidSrcNode,
                           std::string uuidDestNode, std::string edgeName) {
  int srcNode = 0;
  int destNode = 0;

  // find the integer ID of the source and destination node and add an edge to
  // the boost graph
  srcNode = findNode(uuidSrcNode) - 1;
  destNode = findNode(uuidDestNode) - 1;

  // create a pair of the source an destination node(plug)
  std::pair<std::string, std::string> edge(uuidSrcNode, uuidDestNode);
  // add the created pair to the std map with its unique index for each added
  // bgl edge
  m_mapping.insert(std::make_pair(edge, edgeID));
  graphMap::edges newEdge = {edgeID, edgeName, srcNode, destNode};
  m_edgeMap.push_back(newEdge);

  m_numEdges++;

  std::cout << "Source " << srcNode << " to destination " << destNode
            << std::endl;
}

/// <summary>Add new connection in ee/architecture</summary>
void GraphManager::addEdge(std::string uuidSrcNode, std::string uuidDestNode,
                           std::string edgeName) {
  int srcNode = 0;
  int destNode = 0;

  // find the integer ID of the source and destination node and add an edge to
  // the boost graph
  srcNode = findNode(uuidSrcNode) - 1;
  destNode = findNode(uuidDestNode) - 1;

  // create a pair of the source an destination node(plug)
  std::pair<std::string, std::string> edge(uuidSrcNode, uuidDestNode);
  // add the created pair to the std map with its unique index for each added
  // bgl edge
  m_mapping.insert(std::make_pair(edge, m_numEdges));
  graphMap::edges newEdge = {m_numEdges, edgeName, srcNode, destNode};
  m_edgeMap.push_back(newEdge);
  m_numEdges++;

  std::cout << "Source " << srcNode << " to destination " << destNode
            << std::endl;
}

/// <summary>Add new connection in ee/architecture</summary>
void GraphManager::addEdge(std::string uuidSrcNode, std::string uuidDestNode,
                           std::string edgeName, std::string edgeType) {
  int srcNode = 0;
  int destNode = 0;

  // find the integer ID of the source and destination node and add an edge to
  // the boost graph
  srcNode = findNode(uuidSrcNode) - 1;
  destNode = findNode(uuidDestNode) - 1;

  // create a pair of the source an destination node(plug)
  std::pair<std::string, std::string> edge(uuidSrcNode, uuidDestNode);
  // add the created pair to the std map with its unique index for each added
  // bgl edge
  m_mapping.insert(std::make_pair(edge, m_numEdges));
  m_Graph.addEdge(m_numEdges, srcNode, destNode, edgeName);
  graphMap::edges newEdge = {m_numEdges, edgeName, srcNode, destNode, edgeType};
  m_edgeMap.push_back(newEdge);
  m_numEdges++;

  std::cout << "Source " << srcNode << " to destination " << destNode
            << " and communicationtype: " << edgeName << std::endl;
}

// delete existent node
void GraphManager::deleteEdge(unsigned int edgeID) {}

// delete existent node
void GraphManager::deleteEdge(unsigned int srcNode, unsigned int destNode) {}

void GraphManager::deleteEdge(std::string uuidSrcNode,
                              std::string uuidDestNode) {
  auto edgeID = std::make_pair(uuidSrcNode, uuidDestNode);
  unsigned int pos = m_mapping[edgeID];

  std::cout << "Edge with position " << pos << " is deleted" << std::endl;
}

/// <summary>Todo: Find coonection in ee/architecture</summary>
int GraphManager::findEdge(int edgeID) { return 0; }

/// <summary>Print current modeled ee/architecture</summary>
void GraphManager::printGraph() {
  // Update and print graph
  buildBGLGraph();
  m_Graph.printGraph();
}

/// <summary>Create a BGL graph which is based on the modeled ee/architecture</summary>
void GraphManager::buildBGLGraph() {
  BGLGraph tempGraph;
  unsigned int vertexCount = 0;
  unsigned int edgeCount = 0;

  ComputeUnits computeElements;
  Connections connectionElements;

  for (auto itEdge = std::begin(m_nodeMap); itEdge != std::end(m_nodeMap);
       ++itEdge) {
    unsigned int compType = 0;
    auto findCompType = computeElements.m_mapping.find(itEdge->nodType);
    if (findCompType != computeElements.m_mapping.end()) {
      compType = findCompType->second;
    } else {
      compType = 0;
    }
    tempGraph.addVertex(vertexCount, itEdge->nodName, compType);
    vertexCount++;
  }

  for (auto itVertex = std::begin(m_edgeMap); itVertex != std::end(m_edgeMap);
       ++itVertex) {
    unsigned int connectType = 0;
    auto findConnectpType = connectionElements.m_mapping.find(itVertex->edType);
    if (findConnectpType != connectionElements.m_mapping.end()) {
      connectType = findConnectpType->second;
    } else {
      connectType = 0;
    }
    tempGraph.addEdge(edgeCount, itVertex->srcNode, itVertex->destNode,
                      itVertex->edName, connectType);
    edgeCount++;
  }

  m_Graph = tempGraph;
}

/// <summary>Function returns the current BGL graph</summary>
/// <returns>Reference to current BGL graph</returns>
BGLGraph GraphManager::getBGLGraph() { return m_Graph; }

/// <summary>Function returns all nodes in the ee/architecture</summary>
/// <returns>All nodes in the ee/architecture</returns>
std::vector<std::string> GraphManager::getNodes() {
  std::vector<std::string> stringNodeList;

  for (auto it = m_nodeMap.begin(); it != m_nodeMap.end(); ++it) {

    stringNodeList.push_back(it->nodName);
  }

  return stringNodeList;
}

/// <summary>Function returns all connections in the ee/architecture</summary>
/// <returns>All connections in the ee/architecture</returns>
std::vector<std::string> GraphManager::getEdges() {
  std::vector<std::string> stringEdgeList;

  for (auto it = m_edgeMap.begin(); it != m_edgeMap.end(); ++it) {

    stringEdgeList.push_back(it->edName);
  }

  return stringEdgeList;
}
