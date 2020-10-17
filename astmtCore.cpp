#include "astmtCore.h"

namespace ASTMT {

/// <summary>Main application of the ASTMT</summary>
/// <returns>Identified attack paths and their risks</returns>  
bool startASTMTcore(
    diGraph &eeArchitecture,
    std::unordered_map<unsigned int, std::string> componentNames,
    std::vector<QList<QString>> sgmTable,
    std::vector<std::pair<std::string, std::string>> &stringResult) {

  bool transpose = true;
  bool result;
  std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> architecturePaths;
  diGraph reducedArchitecture;

  std::string threat;
  std::string hazard;
  std::string guideword;
  std::string element;
  std::string attackerTarget;
  std::string entryPoint;
  std::string severity;
  unsigned int severityScale = 1;
  std::unordered_map<unsigned int, Vulnerability> vulColection;
  std::vector<std::pair<unsigned int, std::vector<Vulnerability>>> vulMapping;
  std::unordered_map<unsigned int, std::string> uniqueElements, elementsReducedArchitecture;
  std::vector<std::tuple<unsigned int, unsigned int>> adjacency, adjacencyReducedArchitecture;
  std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> foundPaths;
  std::vector<QStringList> dataBaseVuls;
  std::unordered_map<unsigned int, Vulnerability> globalVulMap;
  std::map<std::string, std::string> compInstance;
  std::unordered_map<std::string, unsigned int> compVulMapping;
  std::vector<std::unordered_map<unsigned int, float>> pathsRisks;
  std::vector<std::pair<unsigned int, float>> ranking;

  if (boost::filesystem::exists("work//AT_Graph_0.dot")) {
    std::remove("work//AT_Graph_0.dot");
  }

  std::ofstream log("log.txt");
  
  // Record start time
  auto start = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed;
  
  // Read vulnerabilities from sql data base
  DatabaseHandler db("attack.db");
  ComputeUnits compute;
  Connections connect;
  db.readFromDataBase(dataBaseVuls);
  log << "Vulnerabilities in database: " << dataBaseVuls.size() << std::endl;
  db.readVulnerabilityFromDataBase(dataBaseVuls, compute, connect, globalVulMap);

  // based on the adjacency matrix and the exitent connections between the
  // components, the required set of components/buses is derived
  getConnectingIDs(eeArchitecture, adjacency, uniqueElements);

  for (int i = 0; i < sgmTable.size(); i++)
  {
    threat = sgmTable.at(i).at(0).toLocal8Bit().constData();
    hazard = sgmTable.at(i).at(1).toLocal8Bit().constData();
    guideword = sgmTable.at(i).at(2).toLocal8Bit().constData();
    element = sgmTable.at(i).at(3).toLocal8Bit().constData();
    attackerTarget = sgmTable.at(i).at(4).toLocal8Bit().constData();
    entryPoint = sgmTable.at(i).at(5).toLocal8Bit().constData();
    severity = sgmTable.at(i).at(6).toLocal8Bit().constData();

    if (attackerTarget.compare(entryPoint) == 0) {
      std::cout << "Error: Entry point and attacker target are equal" << std::endl;
      return false;
    }

    if (severity.compare("S1") == 0) {
      severityScale = 1;
    }
    if (severity.compare("S2") == 0) {
      severityScale = 2;
    }
    if (severity.compare("S3") == 0) {
      severityScale = 4;
    }

    // find attacker position and target
    unsigned int attackerPos = 0;
    for (std::pair<unsigned int, std::string> element : uniqueElements) {
      if (element.second == entryPoint) {
        attackerPos = element.first;
      }
    }

    // represents the goal of the attack and is used to build the specification
    unsigned int attackerGoal = 0;
    for (std::pair<unsigned int, std::string> element : uniqueElements) {
      if (element.second == attackerTarget) {
        attackerGoal = element.first;
      }
    }

    // Reduce overall architecture to the relevant parts
    search::findPaths(eeArchitecture, attackerPos, attackerGoal, &result, &architecturePaths);
    search::createReducedGraphFromPaths(&architecturePaths, eeArchitecture, reducedArchitecture);

    if (result == false) {
      std::cout << "No communication between entry point and attacker goal "
                   "possible! Stopping computation"
                << std::endl;
      log << "No communication between entry point and attacker goal possible! "
             "Stopping computation"
          << std::endl;
      return false;
    }

    boost::filesystem::ofstream outFile("eeArchitectureReduced.dot");
    boost::write_graphviz(
        outFile, reducedArchitecture,
        boost::make_label_writer(boost::get(
            &graphProps::VertexProps::vertexName, reducedArchitecture)),
        boost::make_label_writer(
            boost::get(&graphProps::EdgeProps::edgeName, reducedArchitecture)));
    outFile.close();
    getConnectingIDs(reducedArchitecture, adjacencyReducedArchitecture,
                     elementsReducedArchitecture);

    for (std::pair<unsigned int, std::string> element :
         elementsReducedArchitecture) {
      if (element.second == entryPoint) {
        attackerPos = element.first;
      }
    }

    for (std::pair<unsigned int, std::string> element :
         elementsReducedArchitecture) {
      if (element.second == attackerTarget) {
        attackerGoal = element.first;
      }
    }

    // map vulnerabilities to each component of the ee architecture
    vulnerabilityMapper(globalVulMap, entryPoint, reducedArchitecture,
                         vulMapping, componentNames, compInstance);
    log << "Vulnerabilities: " << globalVulMap.size() << std::endl;

    // generate gal model for model checking
    buildGALModel(reducedArchitecture, "attack_model.gal", compInstance,
                    vulMapping, elementsReducedArchitecture,
                    adjacencyReducedArchitecture, attackerPos, attackerGoal,
                    guideword, connect, compute);

    // Record end time
    auto finishCalc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedCalc = finishCalc - start;
    log << "Preparing architecture and vulnerability mapping took "
        << elapsedCalc.count() << "s" << std::endl;

    std::system("..//ITS-Tool//eclipsec.exe -i attack_model.gal -reach "
                "-timeout 50400 -itsflags \"-wgo AT_Graph\" > its-log.txt");

    bool success = boost::filesystem::exists("work//AT_Graph_0.dot");

    if (!success) {
      log << "No attack paths found " << std::endl;
      std::cout << "No attack paths found " << std::endl;
      return false;
    } else {

      log << "At least one attack path was found! Reading paths..."
          << std::endl;
      std::cout << "At least one attack path was found! Reading paths..."
                << std::endl;
      diGraph attackPathsIN;

      // Record end time
      auto finishCalc2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsedCalc2 = finishCalc2 - finishCalc;
      log << "Model-Checking and graph exporting took: " << elapsedCalc2.count()
          << "s" << std::endl;

      unsigned int rootNode;
      std::vector<unsigned int> endNodes;

      // read in attack paths
      readAttackPaths(reducedArchitecture, elementsReducedArchitecture,
                      globalVulMap, entryPoint, attackPathsIN, rootNode,
                      endNodes);
      log << "Reading attack paths finished and " << endNodes.size()
          << " destinations found. Starting risk assessment" << std::endl;
      log << "Graph consits of " << boost::num_vertices(attackPathsIN)
          << " vertices and " << boost::num_edges(attackPathsIN) << " edges."
          << std::endl;

      std::vector<std::pair<double, AttackPath>> attackPathVector;
      LayerModel AttackerModel;

      // generate risk values for each path
      if (transpose) {
        diGraph G_T;
        transpose_graph(attackPathsIN, G_T);
        genRiskPathsMT(G_T, rootNode, endNodes, transpose, attackPathVector);
        createRanking(attackPathVector, severityScale, AttackerModel,
                      entryPoint);

        // Record end time
        auto finishCalc3 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedCalc3 = finishCalc3 - finishCalc2;
        log << "Reading of attacks paths and risk assement took: "
            << elapsedCalc3.count() << "s" << std::endl;

        generateReport(attackPathVector, G_T, transpose, globalVulMap,
                       entryPoint, threat, hazard, stringResult);

        auto finishGlobal = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedGlobal = finishGlobal - start;
        log << "Total run took: " << elapsedGlobal.count() << "s" << std::endl;
      } else {
        genRiskPathsMT(attackPathsIN, rootNode, endNodes, transpose,
                       attackPathVector);
        createRanking(attackPathVector, severityScale, AttackerModel,
                      entryPoint);

        // Record end time
        auto finishCalc = std::chrono::high_resolution_clock::now();
        auto elapsedCalc = finishCalc - start;
        log << "Extracting Paths took: " << elapsedCalc.count() << "s"
            << std::endl;

        generateReport(attackPathVector, attackPathsIN, transpose, globalVulMap,
                       entryPoint, threat, hazard, stringResult);
        auto finishGlobal = std::chrono::high_resolution_clock::now();
        auto elapsedGlobal = finishGlobal - start;
        log << "Total run took: " << elapsedGlobal.count() << "s" << std::endl;
      }

      log.close();
    }
  }
  return true;
}
} // namespace ASTMT