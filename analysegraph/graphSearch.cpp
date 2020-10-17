#include "graphSearch.h"

colormap vertex_coloring;

PathFinder::PathFinder(
    int sourceVertex, int destinationVertex, bool *result,
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *paths)
{
  m_source = sourceVertex;
  m_destination = destinationVertex;
  m_result = result;
  m_allPaths = paths;
}

// function finds all paths between a source and a destination vertex
// algorithm is a modified depht-first-search
void search::findPaths(
    diGraph SearchGraph, int source, int destination, bool *foundDestination,
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *paths)
{
  PathFinder visitor(source, destination, foundDestination, paths);
  boost::depth_first_search(SearchGraph, visitor,
                            boost::make_assoc_property_map(vertex_coloring),
                            source);
}

void search::printAllPaths(
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *foundPaths,
    diGraph origGraph, diGraph &pathGraph)
{
  std::vector<std::pair<diGraph::vertex_descriptor, diGraph::vertex_descriptor>>
      tempEdges;
  diGraph::vertex_descriptor src, dest;

  for (std::map<unsigned int, std::vector<diGraph::vertex_descriptor>>::iterator
           iter = foundPaths->begin();
       iter != foundPaths->end(); ++iter)
  {
    int path = iter->first;
    if (iter->second.size() <= 1)
    {
      //
    }
    else
    {
      for (unsigned int i = 0; i < iter->second.size() - 1; i++)
      {
        src = iter->second.at(i);
        dest = iter->second.at(i + 1);
        auto p = std::make_pair(src, dest);
        // find duplicates
        if (std::find(tempEdges.begin(), tempEdges.end(), p) !=
            tempEdges.end())
        {
          //
        }
        else
        {
          tempEdges.push_back(p);
        }
      }
    }
  }
  for (std::vector<std::pair<diGraph::vertex_descriptor,
                             diGraph::vertex_descriptor>>::const_iterator it = tempEdges.begin();
       it != tempEdges.end(); it++)
  {
    src = it->first;
    dest = it->second;

    auto newEdge = boost::add_edge(src, dest, pathGraph).first;
    auto oldEdge = boost::edge(boost::vertex(src, origGraph),
                               boost::vertex(dest, origGraph), origGraph)
                       .first;
    // copy vertex
    pathGraph[src].vertexName = origGraph[src].vertexName;
    pathGraph[dest].vertexName = origGraph[dest].vertexName;
    pathGraph[dest].compType = origGraph[dest].compType;

    // copy edge
    pathGraph[newEdge].edgeID = origGraph[oldEdge].edgeID;
    pathGraph[newEdge].edgeName = origGraph[oldEdge].edgeName;
    pathGraph[newEdge].connectType = origGraph[oldEdge].connectType;
  }
}

// create create graph from several unique paths
void search::createReducedGraphFromPaths(
    std::map<unsigned int, std::vector<diGraph::vertex_descriptor>> *foundPaths,
    diGraph origGraph, diGraph &pathGraph)
{
  std::vector<std::pair<diGraph::vertex_descriptor, diGraph::vertex_descriptor>>
      tempEdges;
  diGraph::vertex_descriptor src, dest;
  unsigned int newSrc = 0;
  unsigned int newDest = 0;
  unsigned int counter = 0;
  std::unordered_map<unsigned int, unsigned> vertexMapper;
  std::unordered_map<unsigned int, unsigned> destMapper;

  for (std::map<unsigned int, std::vector<diGraph::vertex_descriptor>>::iterator
           iter = foundPaths->begin();
       iter != foundPaths->end(); ++iter)
  {
    int path = iter->first;
    if (iter->second.size() <= 1)
    {
      //
    }
    else
    {
      for (unsigned int i = 0; i < iter->second.size() - 1; i++)
      {
        src = iter->second.at(i);
        dest = iter->second.at(i + 1);
        auto p = std::make_pair(src, dest);
        // find duplicates
        if (std::find(tempEdges.begin(), tempEdges.end(), p) !=
            tempEdges.end())
        {
          //
        }
        else
        {
          tempEdges.push_back(p);
        }
      }
    }
  }
  counter = tempEdges.size() + 1;

  for (std::vector<std::pair<diGraph::vertex_descriptor,
                             diGraph::vertex_descriptor>>::const_iterator it =
           tempEdges.begin();
       it != tempEdges.end(); it++)
  {

    src = it->first;
    dest = it->second;

    auto findSrc = vertexMapper.find(src);
    if (findSrc != vertexMapper.end())
    {
      // vertex already added
      newSrc = findSrc->second;
    }
    else
    {
      // add vertex and intialize
      auto srcVert = boost::add_vertex(pathGraph);

      // remember new vertex in the reduced graph
      newSrc = srcVert;
      vertexMapper.insert(std::make_pair(src, newSrc));

      // copy properties of destination vertex
      // pathGraph[newSrc].vertexID = origGraph[src].vertexID;
      std::string bug = origGraph[src].vertexName;
      pathGraph[newSrc].vertexID = newSrc;
      pathGraph[newSrc].vertexName = origGraph[src].vertexName;
      pathGraph[newSrc].compType = origGraph[src].compType;
    }

    auto findDest = vertexMapper.find(dest);
    if (findDest != vertexMapper.end())
    {
      newDest = findDest->second;
    }
    else
    {
      // add vertex and intialize
      auto destVert = boost::add_vertex(pathGraph);

      // remeber new vertex in the reduced graph
      newDest = destVert;
      vertexMapper.insert(std::make_pair(dest, newDest));

      // copy properties of destination vertex
      // pathGraph[newDest].vertexID = origGraph[dest].vertexID;
      std::string bug = origGraph[dest].vertexName;
      pathGraph[newDest].vertexID = newDest;
      pathGraph[newDest].vertexName = origGraph[dest].vertexName;
      pathGraph[newDest].compType = origGraph[dest].compType;
    }

    auto newEdge = boost::add_edge(newSrc, newDest, pathGraph).first;
    auto oldEdge = boost::edge(boost::vertex(src, origGraph),
                               boost::vertex(dest, origGraph), origGraph)
                       .first;

    // copy edge
    // pathGraph[newEdge].edgeID = origGraph[oldEdge].edgeID;
    pathGraph[newEdge].edgeID = counter;
    pathGraph[newEdge].edgeName = origGraph[oldEdge].edgeName;
    pathGraph[newEdge].connectType = origGraph[oldEdge].connectType;
    counter++;
  }
}
