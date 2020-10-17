#include "riskAssess.h"

struct vulPath {
  vulPath() : exploit(0), name() {}
  vulPath(double expl, std::string newName) : exploit(expl), name(newName) {}
  double exploit;
  std::string name;
};

LayerModel::LayerModel(void) {

  m_entity.insert({"WLAN", 1});
  m_entity.insert({"Bluetooth", 1});
  m_entity.insert({"Cellular", 1});
  m_entity.insert({"DVB", 1});
  m_entity.insert({"DAB", 1});
  m_entity.insert({"RDS", 1});
  m_entity.insert({"TPMS", 1});
  m_entity.insert({"RFID", 1});
  m_entity.insert({"Infrared", 1});
  m_entity.insert({"Camera", 1});
  m_entity.insert({"Radar", 1});
  m_entity.insert({"LiDAR", 1});
  m_entity.insert({"USB", 2});
  m_entity.insert({"AUX", 2});
  m_entity.insert({"CD", 2});
  m_entity.insert({"DVD", 2});
  m_entity.insert({"OBD", 2});
  m_entity.insert({"ECU", 3});
  m_entity.insert({"Gateway", 3});
  m_entity.insert({"Aktor", 3});
  m_entity.insert({"Sensor", 3});
  m_entity.insert({"D-Bus", 4});
  m_entity.insert({"CAN-Bus", 4});
  m_entity.insert({"CAN-FD-Bus", 4});
  m_entity.insert({"LIN-Bus", 4});
  m_entity.insert({"MOST-Bus", 4});
  m_entity.insert({"Flexray", 4});
  m_entity.insert({"Ethernet", 4});
  m_entity.insert({"P2P", 5});
  m_entity.insert({"JTAG", 6});
  m_entity.insert({"I2C", 6});
  m_entity.insert({"Debug", 6});
}

// constructor
RiskAssessment::RiskAssessment(
    diGraph &G, int sourceVertex, int destinationVertex, bool *result,
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *paths)
    : m_source(sourceVertex), m_destination(destinationVertex),
      m_result(result), m_allPaths(paths), m_G(G) {}

AttackPath::AttackPath(unsigned int pathId, double riskValue,
                       std::vector<diGraph::vertex_descriptor> pathElements,
                       std::vector<std::string> pathEdges)
    : m_pathId(pathId), m_riskValue(riskValue), m_pathElements(pathElements),
      m_pathEdges(pathEdges) {}

/// <summary>Function calculates the overall risk value for each path</summary>
/// <returns>Risk values of attack paths</returns>
void computeRiskValues(
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *foundPaths,
    diGraph &origGraph, diGraph &pathGraph,
    std::unordered_map<unsigned int, double> &pathRisks) {
  double pathRisk = 1;
  double resPathRisk = 0.0;
  double tempRisk = 0.0;
  unsigned int n = 0;
  diGraph::vertex_descriptor src, dest;
  std::vector<std::pair<diGraph::vertex_descriptor, diGraph::vertex_descriptor>>
      tempEdges;
  std::cout << "Number of found paths: " << foundPaths->size() << std::endl;

  for (std::map<unsigned int, std::vector<diGraph::vertex_descriptor>>::iterator
           iter = foundPaths->begin();
       iter != foundPaths->end(); ++iter) {
    int path = iter->first;

    // only one vertex
    if (iter->second.size() <= 1) {
      continue;
    } else {

      for (unsigned int i = 0; i < iter->second.size() - 1; i++) {
        src = iter->second.at(i);
        dest = iter->second.at(i + 1);

        // get risk value for edge
        auto edge = boost::edge(boost::vertex(src, origGraph),
                                boost::vertex(dest, origGraph), origGraph)
                        .first;
        tempRisk = get(boost::edge_weight_t(), origGraph, edge);

        // assess only (real) vunlerabilities [0:1)
        if (tempRisk > 0) {
          pathRisk = pathRisk + tempRisk;
          n++;
        }
      }

      if (n > 0) {
        resPathRisk = (pathRisk / n);
        n = 0;
      } else {
        resPathRisk = 0;
      }
      pathRisks.insert(std::make_pair(path, resPathRisk));
      resPathRisk = 0;
      pathRisk = 0;
    }
  }
}


void findAllPathsRek(
    diGraph::vertex_descriptor vertex, int destinationVertex,
    diGraph &searchGraph, std::vector<diGraph::vertex_descriptor> &searchPath,
    unsigned int &numPaths,
    boost::vec_adj_list_any_vertex_pa::bind_<
        boost::vertex_color_t, diGraph, vertex_properties>::type &v_color_map,
    double probability,
    std::vector<std::pair<double, AttackPath>> &foundAttackPaths,
    std::vector<std::string> pathEdges) {
  // switch between all paths and most relevant paths
  bool fullSearch = false;
  double tempProba = 0;

  if (probability <= 1e-300) {
    return;
  }

  // mark vertex as visited
  put(v_color_map, vertex, boost::black_color);
  searchPath.push_back(vertex);

  // if vertex is destination, store path to it
  if (vertex == destinationVertex) {
    // build attack path object with relevant information
    AttackPath path(numPaths, probability, searchPath, pathEdges);
    foundAttackPaths.push_back(std::make_pair(probability, path));
    numPaths++;
    probability = 1;
  } else {
    // typename boost::graph_traits<diGraph>::adjacency_iterator vi, vi_end;
    typename boost::graph_traits<diGraph>::out_edge_iterator ei, ei_end;
    std::map<diGraph::vertex_descriptor, vulPath> vertexDuplicatesV2;

    if (fullSearch) {

      for (boost::tie(ei, ei_end) = boost::out_edges(vertex, searchGraph);
           ei != ei_end; ++ei) {
        tempProba = boost::get(boost::edge_weight_t(), searchGraph, *ei);
        probability *= tempProba;
        std::string vul = searchGraph[*ei].edgeName;
        pathEdges.push_back(vul);

        findAllPathsRek(boost::target(*ei, searchGraph), destinationVertex,
                        searchGraph, searchPath, numPaths, v_color_map,
                        probability, foundAttackPaths, pathEdges);
      }
    } else {

      // Guided search (only the edge with the highest Probability)
      for (boost::tie(ei, ei_end) = boost::out_edges(vertex, searchGraph);
           ei != ei_end; ++ei) {
        diGraph::vertex_descriptor adja_vertex =
            boost::target(*ei, searchGraph);
        tempProba = boost::get(boost::edge_weight_t(), searchGraph, *ei);
        std::string vul = searchGraph[*ei].edgeName;

        auto it = vertexDuplicatesV2.find(adja_vertex);
        if (it == vertexDuplicatesV2.end()) {
          // not found
          vertexDuplicatesV2[adja_vertex] = vulPath(tempProba, vul);
        } else {
          // found
          double tempProbaV2 =
              boost::get(boost::edge_weight_t(), searchGraph, *ei);
          if (tempProbaV2 < tempProba) {
            vertexDuplicatesV2[adja_vertex] = vulPath(tempProbaV2, vul);
          }
        }
      }

      for (std::map<diGraph::vertex_descriptor, vulPath>::iterator it =
               vertexDuplicatesV2.begin();
           it != vertexDuplicatesV2.end(); it++) {
        // rekursive check for all adjacent vertices
        if (boost::white_color == v_color_map[it->first]) {
          // probability *= boost::get(boost::edge_weight_t(), searchGraph,
          // *ei);
          probability *= it->second.exploit;
          pathEdges.push_back(it->second.name);
          findAllPathsRek(it->first, destinationVertex, searchGraph, searchPath,
                          numPaths, v_color_map, probability, foundAttackPaths,
                          pathEdges);
        }
      }
    }
  }

  put(v_color_map, vertex, boost::white_color);
  searchPath.pop_back();
  pathEdges.pop_back();
}

/// <summary>Function extracts singele attack paths from the counterexample based on a modified DFS</summary>
/// <returns>Attack paths and risk values</returns>
void findAllPaths(
    int sourceVertex, int destinationVertex, diGraph G,
    std::vector<std::pair<double, AttackPath>> &foundAttackPaths) {
  unsigned int numPaths = foundAttackPaths.size();
  std::vector<diGraph::vertex_descriptor> searchPath;
  std::vector<std::string> pathEdges;
  boost::vec_adj_list_any_vertex_pa::bind_<
      boost::vertex_color_t, diGraph, vertex_properties>::type v_color_map =
      get(boost::vertex_color_t(), G);
  double probability = 1;

  // mark all vertices as unvisted
  diGraph::vertex_iterator v, vend;
  for (boost::tie(v, vend) = vertices(G); v != vend; ++v) {
    put(v_color_map, *v, boost::white_color);
  }

  findAllPathsRek(sourceVertex, destinationVertex, G, searchPath, numPaths,
                  v_color_map, probability, foundAttackPaths, pathEdges);
}

/// <summary>Function identifies (singlethreaded) all paths between a source and a destination vertex and prepares the risk computation based on a modified DFS</summary>
/// <returns>Attack paths and risk values</returns>
void genRiskPaths(
    diGraph SearchGraph, int source, std::vector<unsigned int> destinations,
    bool transpose,
    std::vector<std::pair<double, AttackPath>> &attackPathVector) {
  bool foundDestination;

  // iterate over the number of source-destenation pairs
  for (std::vector<unsigned int>::iterator it = destinations.begin();
       it != destinations.end(); ++it) {
    foundDestination = false;
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> paths;
    std::vector<std::pair<double, AttackPath>> tempAttackPaths;

    // iterate over all end nodes to get all paths and their related risk values
    if (transpose) {
      // if graph is transposed (direction of edges are inverted) than
      // destinations are the sources
      qDebug() << "Start DFS for destination: " << *it;
      findAllPaths(*it, source, SearchGraph, tempAttackPaths);
    } else {
      qDebug() << "Start DFS for destination: " << *it;
      findAllPaths(source, *it, SearchGraph, tempAttackPaths);
    }

    std::cout << "Number of assessed attack paths: " << tempAttackPaths.size()
              << std::endl;

    if (tempAttackPaths.size() > 0)
      foundDestination = true;

    if (foundDestination) {
      std::cout << "Path search finished successfully for destination " << *it
                << " . Starting risk assessment." << std::endl;
    }

    attackPathVector.insert(std::end(attackPathVector),
                            std::begin(tempAttackPaths),
                            std::end(tempAttackPaths));
  }
  std::cout << "Number of total assessed attack paths: "
            << attackPathVector.size() << std::endl;
}

/// <summary>Multithreaded worker for risk assessment</summary>
std::vector<std::pair<double, AttackPath>>
workerV2(diGraph SearchGraph, int source,
         std::vector<unsigned int> &destinations, bool transpose) {
  std::vector<std::pair<double, AttackPath>> foundAttackPaths;
  std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> tempPaths;

  qDebug() << "Threat" << QThread::currentThreadId()
           << " is starting computation.";

  // iterate over all end nodes to get all paths and their related risk values
  // (multi-threaded?)

  for (std::vector<unsigned int>::iterator it = destinations.begin();
       it != destinations.end(); it++) {

    if (transpose) {
      // if graph is transposed (direction of edges are inverted) than
      // destinations are the sources
      qDebug() << "Start DFS for destination: " << *it;
      findAllPaths(*it, source, SearchGraph, foundAttackPaths);
    } else {
      qDebug() << "Start DFS for destination: " << *it;
      findAllPaths(source, *it, SearchGraph, foundAttackPaths);
    }
  }
  qDebug() << "Number of assessed attack paths: " << foundAttackPaths.size();

  return foundAttackPaths;
}

/// <summary>Function identifies (multithreaded) all paths between a source and a destination vertex and prepares the risk computation based on a modified DFS</summary>
/// <returns>Attack paths and risk values</returns>
void genRiskPathsMT(
    diGraph SearchGraph, int source, std::vector<unsigned int> destinations,
    bool transpose,
    std::vector<std::pair<double, AttackPath>> &attackPathVector) {
  QFuture<std::vector<std::pair<double, AttackPath>>> *myThreadPool = NULL;
  std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> paths;
  QFutureSynchronizer<void> synchronizer;
  unsigned int index = 0;
  unsigned int threads = (QThread::idealThreadCount() / 2);
  unsigned int dests = destinations.size();
  unsigned int destsPerThread = dests / threads;

  myThreadPool =
      new QFuture<std::vector<std::pair<double, AttackPath>>>[threads];

  qDebug() << "Create" << threads << " threads";

  for (unsigned int i = 0; i < threads; i++) {
    std::vector<unsigned int> tempDestinations;

    if (i == (threads - 1)) {
      unsigned int startOfBlock = i * destsPerThread;
      std::copy(destinations.begin() + startOfBlock, destinations.end(),
                std::back_inserter(tempDestinations));
    } else {
      unsigned int startOfBlock = i * destsPerThread;
      unsigned int endOfBlock = ((i * destsPerThread) + destsPerThread);
      std::copy(destinations.begin() + startOfBlock,
                destinations.begin() + endOfBlock,
                std::back_inserter(tempDestinations));
    }

    synchronizer.addFuture(myThreadPool[i] = QtConcurrent::run(
                               workerV2, diGraph(SearchGraph), int(source),
                               std::vector<unsigned int>(tempDestinations),
                               bool(transpose)));
  }

  synchronizer.waitForFinished();

  // gather results from all threads
  for (unsigned int i = 0; i < threads; i++) {
    std::vector<std::pair<double, AttackPath>> tempRes =
        myThreadPool[i].result();
    attackPathVector.insert(std::end(attackPathVector), std::begin(tempRes),
                            std::end(tempRes));
  }

  qDebug() << "Total of computed risk paths: " << attackPathVector.size();
}

bool compare(std::pair<double, AttackPath> a, std::pair<double, AttackPath> b) {
  return (a.first > b.first);
}

void createRanking(std::vector<std::pair<double, AttackPath>> &attackPathVector,
                   unsigned int severityScaling, LayerModel attackerModel,
                   std::string typeOfEntryPoint) {
  double attackerEffort = 1;

  auto findType = attackerModel.m_entity.find(typeOfEntryPoint);
  if (findType != attackerModel.m_entity.end()) {
    if (findType->second == 1) {
      attackerEffort = 1;
    }

    if (findType->second == 2) {
      attackerEffort = 0.7;
    }

    if (findType->second == 3) {
      attackerEffort = 0.5;
    }

    if (findType->second == 4) {
      attackerEffort = 0.4;
    }

    if (findType->second == 5) {
      attackerEffort = 0.3;
    }

    if (findType->second == 6) {
      attackerEffort = 0.2;
    }
  }

  for (std::vector<std::pair<double, AttackPath>>::iterator it =
           attackPathVector.begin();
       it != attackPathVector.end(); it++) {
    it->first = it->first * severityScaling * attackerEffort;
  }
  std::stable_sort(attackPathVector.begin(), attackPathVector.end(), compare);
}

/// <summary>Function checks for duplicated paths</summary>
/// <returns>Number for duplicated attack paths</returns>
int checkPathsDuplicates(
    std::vector<std::vector<std::string>> &pathDescriptions) {
  std::unordered_map<std::vector<std::string>, int,
                     container_hash<std::vector<std::string>>>
      testMap;
  std::vector<std::vector<std::string>> cleanedPaths;
  int count = 0;

  for (std::vector<std::vector<std::string>>::iterator it =
           pathDescriptions.begin();
       it != pathDescriptions.end(); it++) {

    auto findDuplicates = testMap.find(*it);
    if (findDuplicates == testMap.end()) {
      testMap.insert(std::make_pair(*it, count));
      cleanedPaths.push_back(*it);
    } else {
      count++;
    }
  }
  pathDescriptions = cleanedPaths;

  return count;
}
