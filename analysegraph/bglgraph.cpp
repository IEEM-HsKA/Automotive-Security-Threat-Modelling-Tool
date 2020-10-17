#include "bglgraph.h"
#include "graphSearch.h"

BGLGraph::BGLGraph() {}

BGLGraph::~BGLGraph() {}

/// <summary>Adds a vertex(node) to the graph structure</summary>
void BGLGraph::addVertex(unsigned int vertexID, std::string vertexName) {
  // add vertex and intialize
  auto vert = boost::add_vertex(m_diGraph);
  m_diGraph[vert].vertexID = vertexID;
  m_diGraph[vert].vertexName = vertexName;

  m_verticesNames.push_back(vertexName);
  m_numberVertices = boost::num_vertices(m_diGraph);
}

/// <summary>Adds a vertex(node) to the graph structure</summary>
void BGLGraph::addVertex(unsigned int vertexID, std::string vertexName,
                         unsigned short computeType) {
  // add vertex and intialize
  auto vert = boost::add_vertex(m_diGraph);
  m_diGraph[vert].vertexID = vertexID;
  m_diGraph[vert].vertexName = vertexName;
  m_diGraph[vert].compType = computeType;
}

/// <summary>Removes a vertex(node) form the graph structure</summary>
void BGLGraph::deleteVertex(unsigned int vertexID) {
  diGraph::vertex_descriptor vert;
  vert = boost::vertex(vertexID, m_diGraph);
  boost::remove_vertex(vert, m_diGraph);

  m_numberVertices = boost::num_vertices(m_diGraph);
}

void BGLGraph::getVertexProperties() {}

/// <summary>Adds an edge(line) between two vertices(nodes) in the graph structure</summary>
void BGLGraph::addEdge(unsigned int edgeID, unsigned int srcVertexID,
                       unsigned int dstVertexID, std::string edgeName) {
  //(1) get vertex descriptor for source and destination vertex
  diGraph::vertex_descriptor src, dst;
  src = boost::vertex(srcVertexID, m_diGraph);
  dst = boost::vertex(dstVertexID, m_diGraph);
  // set edge properties between source and destination vertex
  auto e = boost::add_edge(src, dst, m_diGraph);
  // initialize edge properties
  m_diGraph[e.first].edgeID = edgeID;
  m_diGraph[e.first].edgeName = edgeName;

  m_edgesNames.push_back(edgeName);
  m_numberEdges = boost::num_edges(m_diGraph);
}

/// <summary>Adds an edge(line) between two vertices(nodes) in the graph structure</summary>
void BGLGraph::addEdge(unsigned int edgeID, unsigned int srcVertexID,
                       unsigned int dstVertexID, std::string edgeName,
                       unsigned short connectionType) {
  //(1) get vertex descriptor for source and destination vertex
  diGraph::vertex_descriptor src, dst;
  src = boost::vertex(srcVertexID, m_diGraph);
  dst = boost::vertex(dstVertexID, m_diGraph);

  // set edge properties between source and destination vertex
  auto edgeDesc = boost::add_edge(src, dst, m_diGraph);
  m_diGraph[edgeDesc.first].edgeID = edgeID;
  m_diGraph[edgeDesc.first].edgeName = edgeName;
  m_diGraph[edgeDesc.first].connectType = connectionType;
}

void BGLGraph::getEdgeProperties() {}

/// <summary>Removes an edge(line) between two vertices(nodes) in the graph structure</summary>
void BGLGraph::deleteEdge(unsigned int srcVertexID, unsigned int dstVertexID) {
  diGraph::vertex_descriptor src, dst;
  src = boost::vertex(srcVertexID, m_diGraph);
  dst = boost::vertex(dstVertexID, m_diGraph);
  boost::remove_edge(src, dst, m_diGraph);

  m_numberEdges = boost::num_edges(m_diGraph);
}

/// <summary>Function for plotting the graph structure as DOT file which will be converted to an PDF</summary>
/// <returns>DOT file containing the graph structure</returns>  
void BGLGraph::printGraph() {

  std::ofstream f("Graph.dot");
  write_graphviz(f, m_diGraph,
                 boost::make_label_writer(boost::get(
                     &graphProps::VertexProps::vertexName, m_diGraph)),
                 boost::make_label_writer(
                     boost::get(&graphProps::EdgeProps::edgeName, m_diGraph)));
  f.close();
  system("renderGraph.bat");
}

/// <summary>Function for plotting the graph structure as DOT file which will be converted to an PDF</summary>
/// <returns>DOT file containing the graph structure</returns> 
void BGLGraph::printFoundpaths() {
  // Write graph to file
  std::ofstream f("SearchGraph.dot");
  write_graphviz(f, m_paths,
                 boost::make_label_writer(
                     boost::get(&graphProps::VertexProps::vertexName, m_paths)),
                 boost::make_label_writer(
                     boost::get(&graphProps::EdgeProps::edgeName, m_paths)));

  f.close();
  system("renderSearchGraph.bat");
}

/// <summary>Function converts the Boost BGL graph to an adjaceny matrix</summary>
/// <returns>Adjacency matrix</returns> 
void BGLGraph::getAdjacencyMatrix() {
  typedef boost::graph_traits<diGraph>::edge_iterator edge_iterator;
  std::pair<edge_iterator, edge_iterator> ei = edges(m_diGraph);

  for (edge_iterator edge_iter = ei.first; edge_iter != ei.second;
       ++edge_iter) {
    std::cout << "(" << source(*edge_iter, m_diGraph) << ", "
              << target(*edge_iter, m_diGraph) << ")\n";
  }
  unsigned int N = num_edges(m_diGraph);
  std::vector<std::vector<int>> mat(N, std::vector<int>(N));

  for (edge_iterator edge_iter = ei.first; edge_iter != ei.second;
       ++edge_iter) {
    int a = source(*edge_iter, m_diGraph);
    int b = target(*edge_iter, m_diGraph);
    mat[a][b] = 1;
    mat[b][a] = 1;
  }
}

/// <summary>Function converts the Boost BGL graph to an adjaceny matrix</summary>
/// <returns>Adjacency matrix</returns> 
void BGLGraph::getAdjacencyMatrix(std::vector<std::vector<int>> *adjacency) {
  typedef boost::graph_traits<diGraph>::edge_iterator edge_iterator;
  std::pair<edge_iterator, edge_iterator> ei = edges(m_diGraph);

  unsigned int N = num_edges(m_diGraph);
  std::vector<std::vector<int>> mat(N, std::vector<int>(N));

  for (edge_iterator edge_iter = ei.first; edge_iter != ei.second;
       ++edge_iter) {
    int a = source(*edge_iter, m_diGraph);
    int b = target(*edge_iter, m_diGraph);
    mat[a][b] = 1;
    mat[b][a] = 1;
  }
  *adjacency = mat;
}

/// <summary>Function identifies and print all paths between a start and end point in the graph structure</summary>
/// <returns>Adjacency matrix</returns> 
void BGLGraph::findPaths(int start, int stop) {
  bool searchState = false;
  std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> foundPaths;

  search::findPaths(m_diGraph, start, stop, &searchState, &foundPaths);
  if (searchState) {
    search::printAllPaths(&foundPaths, m_diGraph, m_paths);
  } else {
    std::cout << "Search ends without finding a path" << std::endl;
  }
}

/// <summary>Function finds and prints all outgoing edges of a vertex</summary>
/// <returns>Outgoing edges</returns> 
void BGLGraph::traceOutGoingEdges() {
  diGraph::vertex_iterator vertexIt, vertexEnd;
  diGraph::out_edge_iterator outedgeIt, outedgeEnd;
  boost::tie(vertexIt, vertexEnd) = vertices(m_diGraph);

  for (; vertexIt != vertexEnd; ++vertexIt) {
    std::cout << "outgoing edges for " << *vertexIt << ": ";
    boost::tie(outedgeIt, outedgeEnd) =
        out_edges(*vertexIt, m_diGraph); //  Similar to incoming edges
    for (; outedgeIt != outedgeEnd; ++outedgeIt) {
      std::cout << " " << *outedgeIt;
    }
    std::cout << std::endl;
  }
}

/// <summary>Based on the adjacency matrix the function generates tupples of (src,edge) and (edge,dst)</summary>
/// <returns>Vertex/Edge tupples of (src,edge) or (edge,dst)</returns> 
void getConnectingIDs(
    const diGraph &eeArchitecturetecture,
    std::vector<std::tuple<unsigned int, unsigned int>> &adjacency,
    std::unordered_map<unsigned int, std::string> &uniqueElements) {
  typedef boost::graph_traits<diGraph>::edge_iterator edge_iterator;
  typename boost::graph_traits<diGraph>::out_edge_iterator ei2, ei_end2;
  // required to check if the connection is a bus with several members
  std::unordered_map<std::string, unsigned int> busMap;
  std::pair<edge_iterator, edge_iterator> ei =
      boost::edges(eeArchitecturetecture);
  unsigned int vID_src, vID_dst, eID;
  diGraph::vertex_descriptor src, dst;

  for (edge_iterator edge_iter = ei.first; edge_iter != ei.second;
       ++edge_iter) {

    src = boost::vertex(source(*edge_iter, eeArchitecturetecture),
                        eeArchitecturetecture);
    dst = boost::vertex(target(*edge_iter, eeArchitecturetecture),
                        eeArchitecturetecture);
    // auto edge = boost::edge(src, dst, eeArchitecturetecture).first;

    vID_src = eeArchitecturetecture[src].vertexID;
    vID_dst = eeArchitecturetecture[dst].vertexID;
    eID = eeArchitecturetecture[*edge_iter].edgeID;

    // Bus detection
    auto busMap_it = busMap.find(eeArchitecturetecture[*edge_iter].edgeName);
    if (busMap_it != busMap.end()) {
      // if a bus connection was identified, force one id instead of n ids for n
      // bus members
      if (busMap_it->second != eID) {
        eID = busMap_it->second;
      }
    } else {
      busMap.insert(
          std::make_pair(eeArchitecturetecture[*edge_iter].edgeName, eID));
    }

    // store id of source and destination vertex tuple of form (src,edge) and
    // (edge,dst) due to the fact that edges also components
    adjacency.push_back(std::make_tuple(vID_src, eID));
    adjacency.push_back(std::make_tuple(eID, vID_dst));

    auto uniqueElements_it = uniqueElements.find(vID_src);
    if (uniqueElements_it != uniqueElements.end()) {
      //
    } else {
      uniqueElements.insert(
          std::make_pair(vID_src, eeArchitecturetecture[src].vertexName));
    }

    auto uniqueElements_it2 = uniqueElements.find(vID_dst);
    if (uniqueElements_it2 != uniqueElements.end()) {
      //
    } else {
      uniqueElements.insert(
          std::make_pair(vID_dst, eeArchitecturetecture[dst].vertexName));
    }

    auto uniqueElements_it3 = uniqueElements.find(eID);
    if (uniqueElements_it3 != uniqueElements.end()) {
      //
    } else {
      uniqueElements.insert(
          std::make_pair(eID, eeArchitecturetecture[*edge_iter].edgeName));
    }
  }
}
diGraph BGLGraph::getDiaGraph() { return m_diGraph; }