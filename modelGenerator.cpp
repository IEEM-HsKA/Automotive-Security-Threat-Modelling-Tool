#include "modelGenerator.h"

typedef std::pair<unsigned int, unsigned int> uPair;

struct pairHash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &uPair) const {
    return std::hash<T1>()(uPair.first) ^ std::hash<T2>()(uPair.second);
  }
};

/// <summary>Gerates GAL model for model checking with ITS-Tools</summary>
/// <returns>Textfile with GAL model</returns>
void buildGALModel(
    diGraph eeArchitecturetecture, std::string filename,
    std::map<std::string, std::string> compInstance,
    std::vector<std::pair<unsigned int, std::vector<Vulnerability>>> vulMapping,
    std::unordered_map<unsigned int, std::string> uniqueElements,
    std::vector<std::tuple<unsigned int, unsigned int>> adjacency,
    unsigned int AttackerPos, unsigned int AttackerGoal, std::string guideword,
    Connections connect, ComputeUnits compute) {
  std::unordered_map<std::string, std::string> guidewords;

  guidewords["disclosure of information"] =
      "(*:acc_priv >= 3 || *:jump >= 3) && *:c == 1;";
  guidewords["reset of component"] =
      "(*:acc_priv ==2 || *:acc_priv >= 4) *:a == 1;";
  guidewords["manipulation of message"] =
      "(*:acc_priv >= 4 || *:jump >= 4)&& *:i == 1;";
  guidewords["manipulation of firmware"] =
      "(*:acc_priv >= 4 || *:jump >= 4) && *:i == 1;";
  guidewords["disconnection of component"] =
      "(*:acc_priv ==2 || *:acc_priv >= 4) *:a == 1;";
  guidewords["delay of message or signal"] =
      "(*:acc_priv ==2 || *:acc_priv >= 4) *:a == 1;";
  guidewords["deletion of message or signal"] =
      "(*:acc_priv >= 4 || *:jump >= 4) && *:i == 1;";
  guidewords["stopping of component"] = "(*:acc_priv ==2 || *:acc_priv >= 4 || "
                                        "*:jump == 2 || *:jump >= 4) *:a == 1;";
  guidewords["activation of(diagnostic) function"] =
      "(*:acc_priv >= 2 || *:jump >= 2) && *: i == 1;";
  guidewords["insertion of message or signal"] =
      "(*:acc_priv >= 4 || *:jump >= 4) && *:i == 1;";

  //<component instance, component type>
  std::fstream f;
  f.open(filename, std::ios::out);

  unsigned int ID = 0;
  unsigned int vulRef = 0;
  unsigned int compVuls = 4;
  unsigned int src = 0;
  unsigned int dst = 0;
  unsigned int compID = 0;
  std::string nameVul = "test";
  std::string compType = "NA";
  std::string compName = "NA";
  int req_priv = 0;
  int acc_priv = 0;
  unsigned int entry = AttackerPos;
  // number of elements which are (unique) connected
  unsigned int nComponents = 0;

  std::unordered_map<unsigned int, std::string> mappingIDComp;
  std::unordered_map<std::string, unsigned int>::iterator compVul_it;
  std::unordered_map<unsigned int, std::string> tempIDNamePair;
  std::unordered_map<std::string, bool> duplicates;

  // build specification
  std::string specification;

  auto findGuideword = guidewords.find(guideword);
  if (findGuideword != guidewords.end()) {
    specification = findGuideword->second;
    boost::replace_all(specification, "*",
                       eeArchitecturetecture[AttackerGoal].vertexName);
  } else {
    return;
  }

  // check if specification is related zu availabilty
  bool integrity = false;
  if (boost::algorithm::contains(specification, "i == 1")) {
    integrity = true;
  }

  for (std::pair<unsigned int, std::string> element : uniqueElements) {
    if (element.first >= nComponents) {
      nComponents = element.first;
    }
  }

  // setup components
  f << "$MAXID = " << nComponents << "; \n";
  f << "$MAXREQ = 5; \n";
  f << "typedef req_t = 0 .. $MAXREQ; \n";
  f << "typedef id_t = 0 .. $MAXID; \n\n";

  // attacker

  f << "gal Attacker($A = 0) { \n";
  f << "\tint req = 0; \n";
  f << "\tint pos = $A; \n\n";

  f << "\ttransition attPos(id_t $id)[pos == $id] label \"attPos\"($id) { \n";
  f << "\t\t//no action \n\t} \n\n";

  f << "\ttransition attPos1(id_t $id)[pos == $id && req == 1] label "
       "\"attPos1\"($id) { \n";
  f << "\t\t//no action \n\t} \n\n";

  f << "\ttransition attPos2(id_t $id)[pos == $id && req == 2] label "
       "\"attPos2\"($id) { \n";
  f << "\t\t//no action \n\t} \n\n";

  f << "\ttransition attPos3(id_t $id)[pos == $id && req == 3] label "
       "\"attPos3\"($id) { \n";
  f << "\t\t//no action \n\t} \n\n";

  f << "\ttransition attPos4(id_t $id)[pos == $id && req == 4] label "
       "\"attPos4\"($id) { \n";
  f << "\t\t//no action \n\t} \n\n";

  f << "\ttransition attPos5(id_t $id)[pos == $id && req == 5] label "
       "\"attPos5\"($id) { \n";
  f << "\t\t//no action \n\t} \n\n";

  f << "\ttransition moveAttPos(id_t $src, id_t $dst, req_t $req)[pos == $src] "
       "label \"moveAttPos\"($src, $dst, $req) { \n";
  f << "\t\tpos = $dst; \n";
  f << "\t\treq = $req; \n";
  f << "\t} \n\n";
  f << "} \n";

  /*connections and their vulnerabilities
  only vulnerability class 1 (component -> component) and 4 (component ->
  connection) relevant*/
  for (auto it = uniqueElements.begin(); it != uniqueElements.end(); it++) {
    auto findCompType = compInstance.find(it->second);
    if (findCompType != compInstance.end()) {
      compType = findCompType->second;
    } else {
      compType = "NA";
    }

    // get component id based on the component name
    // temporal map with component id and component name
    compID = it->first;
    mappingIDComp.insert(std::make_pair(compID, it->second));

    // check if the element is a connection
    if (connect.m_mapping.find(compType) != connect.m_mapping.end()) {
      // check if component type was already added to the model
      auto findDuplicates = duplicates.find(it->second);
      if (findDuplicates == duplicates.end()) {
        duplicates.insert(std::make_pair(it->second, true));

        f << "\n\ngal " << it->second + "_"
          << "($ID=0) \n"
          << "{ \n";
        f << "\tint acc_priv = 0; \n"
          << "\tint id = $ID; \n"
          << "\tint c = 0; \n"
          << "\tint i = 0; \n"
          << "\tint a = 0; \n"
          << "\tint ia = 0; \n"
          << "\tint jump = 0; \n"
          << "\tint visit = 0; \n"
          << "\tint access = 0; \n\n";

        f << "\ttransition attAccess (req_t $req) [access == 0] label "
             "\"attAccess\" ($req) { \n";
        f << "\t\tacc_priv = 1; \n"; // on connections there can only be a
                                     // priveledge level of 1
        f << "\t\taccess = 1; \n \t} \n";

        // get all relevant vulnerabilities based on the component id
        std::vector<Vulnerability> temp;

        for (std::vector<std::pair<unsigned int, std::vector<Vulnerability>>>::
                 iterator it = vulMapping.begin();
             it != vulMapping.end(); it++) {
          if (it->first == compID) {
            temp = it->second;
          }
        }

        for (std::vector<Vulnerability>::iterator temp_it = temp.begin();
             temp_it != temp.end(); temp_it++) {
          /*class 2&4: connection -> component
          connection -> connection*/
          if ((temp_it->m_vulClass == 2) || (temp_it->m_vulClass == 3)) {
            // get relevant vulnerability information
            acc_priv = temp_it->m_accPriveleges;
            req_priv = temp_it->m_reqPriveleges;
            nameVul = std::to_string(temp_it->m_ID);

            f << "\ttransition "
              << "vul_" << nameVul << " [access == 1 && acc_priv == 1] { \n";

            if (temp_it->m_confidentiality) {
              f << "\t\tc = 1; \n";
            }

            if (temp_it->m_integrity || temp_it->m_authentication ||
                temp_it->m_authorization) {
              f << "\t\ti = 1; \n";
            }

            if ((temp_it->m_availability) && (!integrity)) {
              f << "\t\ta = 1; \n";
            }

            // from connection to component
            if (temp_it->m_vulClass == 2) {
              f << "\t\tjump = " << acc_priv << "; \n"
                << "\t\tself.\"moveAttPos" << temp_it->m_accPriveleges
                << "\"; \n";
            }

            f << "\t} \n";
          }
        }

        f << "\n\ttransition moveAttPos1[jump == 1] label \"moveAttPos1\" { \n";
        f << "\t\tacc_priv  = " << 0 << "; //prevent backward jumps\n";
        f << "\t\t//nothing to do \n\t} \n";

        f << "\n\ttransition moveAttPos2[jump == 2] label \"moveAttPos2\" { \n";
        f << "\t\tacc_priv  = " << 0 << "; //prevent backward jumps\n";
        f << "\t\t//nothing to do \n\t} \n";

        f << "\n\ttransition moveAttPos3[jump == 3] label \"moveAttPos3\" { \n";
        f << "\t\tacc_priv  = " << 0 << "; //prevent backward jumps\n";
        f << "\t\t//nothing to do \n\t} \n";

        f << "\n\ttransition moveAttPos4[jump == 4] label \"moveAttPos4\" { \n";
        f << "\t\tacc_priv  = " << 0 << "; //prevent backward jumps\n";
        f << "\t\t//nothing to do \n\t} \n";

        f << "\n\ttransition moveAttPos5[jump == 5] label \"moveAttPos5\" { \n";
        f << "\t\tacc_priv  = " << 0 << "; //prevent backward jumps\n";
        f << "\t\t//nothing to do \n\t} \n";
        f << "}\n";
      }
    }

    // components and their vulnerabilities
    if (compute.m_mapping.find(compType) != compute.m_mapping.end()) {
      // check if component type was already added to the model
      auto findDuplicates = duplicates.find(it->second);
      if (findDuplicates == duplicates.end()) {
        duplicates.insert(std::make_pair(it->second, true));

        f << "\n\ngal " << it->second + "_"
          << "($ID=0) \n"
          << "{ \n";
        f << "\tint acc_priv = 0; \n"
          << "\tint id = $ID; \n"
          << "\tint c = 0; \n"
          << "\tint i = 0; \n"
          << "\tint a = 0; \n"
          << "\tint ia = 0; \n"
          << "\tint jump = 0; \n"
          << "\tint visit = 0; \n"
          << "\tint access = 0; \n\n";
        f << "\ttransition attAccess (req_t $req) [access == 0"
          << " && visit == 0 "
          << "] label \"attAccess\" ($req) { \n";
        f << "\t\tacc_priv = $req; \n";
        f << "\t\taccess = 1; \n \t} \n";

        // get all relevant vulnerabilities based on the component id
        std::vector<Vulnerability> temp;

        for (std::vector<std::pair<unsigned int, std::vector<Vulnerability>>>::
                 iterator it = vulMapping.begin();
             it != vulMapping.end(); it++) {
          if (it->first == compID) {
            temp = it->second;
          }
        }

        for (std::vector<Vulnerability>::iterator temp_it = temp.begin();
             temp_it != temp.end(); temp_it++) {
          if ((temp_it->m_vulClass == 1) || (temp_it->m_vulClass == 4)) {
            // get relevant vulnerability information
            acc_priv = temp_it->m_accPriveleges;
            req_priv = temp_it->m_reqPriveleges;
            nameVul = std::to_string(temp_it->m_ID);

            f << "\ttransition "
              << "vul_" << nameVul
              << " [access == 1 && acc_priv == " << req_priv << "&& visit == 0"
              << " ] { \n";
            f << "\t\tacc_priv = " << acc_priv << "; \n";

            if (temp_it->m_confidentiality) {
              f << "\t\tc = 1; \n";
            }

            if (temp_it->m_integrity || temp_it->m_authentication ||
                temp_it->m_authorization) {
              f << "\t\ti = 1; \n";
            }

            if ((temp_it->m_availability) && (!integrity)) {
              f << "\t\ta = 1; \n";
            }

            // from component to connection (acquired priviledges == 1)
            if (temp_it->m_vulClass == 4) {
              f << "\t\tjump = " << acc_priv << "; \n"
                << "\t\tself.\"moveAttPos" << temp_it->m_accPriveleges
                << "\"; \n";
            }

            if (acc_priv == 5) {

              f << "\t\tjump = 5;\n"
                << "\t\tself.\"moveAttPos5\"; \n";
              f << "\t\tvisit = 1; \n";
            }

            f << "\t} \n";
          }
        }

        f << "\n\ttransition moveAttPos1[jump == 1] label \"moveAttPos1\" { \n";
        f << "\t\tvisit=1;// \n";
        f << "\t\tacc_priv = 0; \n\t } \n";

        f << "\n\ttransition moveAttPos2[jump == 2] label \"moveAttPos2\" { \n";
        f << "\t\tvisit=1;// \n";
        f << "\t\tacc_priv = 0; \n\t } \n";

        f << "\n\ttransition moveAttPos3[jump == 3] label \"moveAttPos3\" { \n";
        f << "\t\tvisit=1;// \n";
        f << "\t\tacc_priv = 0; \n\t } \n";

        f << "\n\ttransition moveAttPos4[jump == 4] label \"moveAttPos4\" { \n";
        f << "\t\tvisit=1;// \n";
        f << "\t\tacc_priv = 0; \n\t } \n";

        f << "\n\ttransition moveAttPos5[jump == 5] label \"moveAttPos5\" { \n";
        f << "\t\tvisit=1;// \n";
        f << "\t\tacc_priv = 0; \n\t } \n";
        f << "}\n";
      }
    } else {
      continue;
    }
  }

  // mapping of e/e-architecture
  f << "\ncomposite eeArchitecturetecture \n"
    << "{\n";
  f << "\tAttacker att($A=" << AttackerPos << "); \n";

  std::unordered_map<std::string, unsigned int> instances;

  for (std::vector<std::tuple<unsigned int, unsigned int>>::iterator it =
           adjacency.begin();
       it != adjacency.end(); it++) {
    std::tuple<unsigned int, unsigned int> temp = *it;

    unsigned int compID = get<0>(temp);
    unsigned int compID2 = get<1>(temp);

    auto compID_it = mappingIDComp.find(compID);
    if (compID_it != mappingIDComp.end()) {
      std::string compName = compID_it->second;

      auto copy_it1 = instances.find(compName);
      if (copy_it1 != instances.end()) {
        // component instance already exists in model -> nothing to add
      } else {
        // component instance does not exist in model -> add to model
        instances.insert(std::make_pair(compName, compID));

        f << "\t" << compName + "_"
          << " " << compName << " ($ID=" << compID << "); \n";
        tempIDNamePair.insert(std::make_pair(compID, compName));
      }
    }

    auto compID_it2 = mappingIDComp.find(compID2);
    if (compID_it2 != mappingIDComp.end()) {
      std::string connectName = compID_it2->second;

      auto copy2_it = instances.find(connectName);
      if (copy2_it != instances.end()) {
        // component already exists in model -> nothing to add
      } else {
        // component does not exist in model -> add to model
        instances.insert(std::make_pair(connectName, compID2));
        // get name of componet which belongs to the actual (node) ID

        f << "\t" << connectName + "_"
          << " " << connectName << " ($ID=" << compID2 << "); \n";
        tempIDNamePair.insert(std::make_pair(compID2, connectName));
      }
    }
  }

  f << "\n";

  f << "\tsynchronization startOfAttack { \n";
  f << "\t\tatt.\"attPos\"(" << AttackerPos << "); \n";

  std::string entryPoint = uniqueElements.at(AttackerPos);

  auto search = compInstance.find(entryPoint);
  if (search != compInstance.end()) {

    if (compute.m_mapping.find(search->second) != compute.m_mapping.end()) {
      // Entry point is a component
      f << "\t\t" << entryPoint << ".\"attAccess\"(2); \n\t}\n";
    } else {
      // Entry point is a connection
      f << "\t\t" << entryPoint << ".\"attAccess\"(1); \n\t}\n";
    }
  }

  // synchronization attacker position
  for (std::unordered_map<unsigned int, std::string>::iterator it =
           tempIDNamePair.begin();
       it != tempIDNamePair.end(); it++) {
    f << "\tsynchronization attackerPosition" << it->first << "_1 { \n";
    f << "\t\tatt.\"attPos1\"(" << it->first << "); \n";
    f << "\t\t" << it->second << ".\"attAccess\"(1); \n\t}\n";

    f << "\tsynchronization attackerPosition" << it->first << "_2 { \n";
    f << "\t\tatt.\"attPos2\"(" << it->first << "); \n";
    f << "\t\t" << it->second << ".\"attAccess\"(2); \n\t}\n";

    f << "\tsynchronization attackerPosition" << it->first << "_3 { \n";
    f << "\t\tatt.\"attPos3\"(" << it->first << "); \n";
    f << "\t\t" << it->second << ".\"attAccess\"(3); \n\t}\n";

    f << "\tsynchronization attackerPosition" << it->first << "_4 { \n";
    f << "\t\tatt.\"attPos4\"(" << it->first << "); \n";
    f << "\t\t" << it->second << ".\"attAccess\"(4); \n\t}\n";

    f << "\tsynchronization attackerPosition" << it->first << "_5 { \n";
    f << "\t\tatt.\"attPos5\"(" << it->first << "); \n";
    f << "\t\t" << it->second << ".\"attAccess\"(5); \n\t}\n";
  }
  f << "\n";

  // Second measure to identify and sort out of equal edges. This is the case
  // when bussytems like CAN are used
  std::unordered_map<uPair, bool, pairHash> uniqueEdges;

  for (std::vector<std::tuple<unsigned int, unsigned int>>::iterator it =
           adjacency.begin();
       it != adjacency.end(); it++) {
    std::string srcName, dstName;
    std::tuple<unsigned int, unsigned int> temp = *it;
    src = get<0>(temp);
    dst = get<1>(temp);

    auto uniqueEdges_it = uniqueEdges.find({src, dst});
    if (uniqueEdges_it != uniqueEdges.end()) {
      // edge already in model
      continue;
    } else {
      // Add new edge
      uniqueEdges.insert({{src, dst}, true});

      // instances
      auto IDComp_it = mappingIDComp.find(src);
      if (IDComp_it != mappingIDComp.end()) {
        srcName = IDComp_it->second;
      } else {
        srcName = "NA" + src;
      }

      auto IDComp_it2 = mappingIDComp.find(dst);
      if (IDComp_it2 != mappingIDComp.end()) {
        dstName = IDComp_it2->second;
      } else {
        dstName = "NA" + dst;
      }

      f << "\tsynchronization jump" << src << "_" << dst << "_1{ \n";
      f << "\t\tatt.\"moveAttPos\"(" << src << "," << dst << ",1); \n";
      f << "\t\t" << srcName << ".\"moveAttPos1\"; \n\t}\n";

      f << "\tsynchronization jump" << src << "_" << dst << "_2{ \n";
      f << "\t\tatt.\"moveAttPos\"(" << src << "," << dst << ",2); \n";
      f << "\t\t" << srcName << ".\"moveAttPos2\"; \n\t}\n";

      f << "\tsynchronization jump" << src << "_" << dst << "_3{ \n";
      f << "\t\tatt.\"moveAttPos\"(" << src << "," << dst << ",3); \n";
      f << "\t\t" << srcName << ".\"moveAttPos3\"; \n\t}\n";

      f << "\tsynchronization jump" << src << "_" << dst << "_4{ \n";
      f << "\t\tatt.\"moveAttPos\"(" << src << "," << dst << ",4); \n";
      f << "\t\t" << srcName << ".\"moveAttPos4\"; \n\t}\n";

      f << "\tsynchronization jump" << src << "_" << dst << "_5{ \n";
      f << "\t\tatt.\"moveAttPos\"(" << src << "," << dst << ",5); \n";
      f << "\t\t" << srcName << ".\"moveAttPos5\"; \n\t}\n";
    }
  }
  f << "}\n\n";
  f << "main eeArchitecturetecture; \n";

  f << "property p1 [never] : " << specification;

  f.close();
}