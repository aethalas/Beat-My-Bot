// Title  :  Graph.cpp
// Purpose:  Provides the BMB Program with a Graph for AI Purposes
// Author :  Jonathon H Tyson
// Date   :  13/10/2017

// Using the graph.h header file, staticmap.h header file for access to the
// StaticMap class, and the Renderer.h file for access to the Renderer class.
#include "graph.h"
#include "staticmap.h"
#include "Renderer.h"
#include <vector>
#include <algorithm>
#include "mydrawengine.h"
//#include <mutex>

//std::mutex mtx2;

// Initialise
unsigned int Node::nodeNumber = 0;
Graph* Graph::pInst = nullptr;


Graph::Graph()
{

} // Graph()


Graph::~Graph()
{

} // ~Graph()


Graph* Graph::GetInstance()				// Static
{
	if (pInst == nullptr)
	{
		pInst= new Graph();
	} // if
	return pInst;
} // GetInstance()


Node::Node()
{
 nodeNumber++;
} // Node()


std::list<Node*> Node::GetNeighbours()
{
	std::list<Node*> neighbours;
	std::list<Edge>::iterator itr;
	for (itr = edgeList.begin(); itr != edgeList.end(); ++itr)
	{
		neighbours.push_back((*itr).toNode);
	} // for
	return neighbours;
} // GetNeighbours()


void Graph::Partition(Rectangle2D r)
{
 // If the area of the Rectangle2D is less than X number, then return early in
 // order to exit Partition() recursion.
 if (r.GetArea() < 1000)
 {
  return;
 } // if
 // If the Rectangle2D being passed to this function has been placed within a
 // obstacle on the StaticMap, then we will do the partitioning of the Rectangle.
 else if (StaticMap::GetInstance()->IsInsideBlock(r))
 {
  // Firstly, create four seperate Rectangle2Ds to represent each corner of the
  // original rectangle being fed into the Partition() function. It appears as
  // follows:
  // ---------------------
  // -         -         -
  // -    r1   -    r2   -
  // -         -         -
  // ---------------------
  // -         -         -
  // -    r2   -    r3   -
  // -         -         -
  // ---------------------
  // It's worth noting that the Rectangle2D's are drawn firstly by taking the
  // bottom left vector of the Rectangle followed by the top right vector.

  Rectangle2D r1, r2, r3, r4;
  r1.PlaceAt((r.GetTopLeft() + r.GetBottomLeft()) / 2, ((r.GetTopRight() + r.GetTopLeft()) / 2));
  r2.PlaceAt(r1.GetBottomRight(), r.GetTopRight());
  r3.PlaceAt(r.GetBottomLeft(), r1.GetBottomRight());
  r4.PlaceAt(r3.GetBottomRight(), r2.GetBottomRight());
 
  // Run the Partition() function on itself four times. This will dissect the
  // Rectangle2D into four smaller parts, should there be any obstacles present.
  Partition(r1);
  Partition(r2);
  Partition(r3);
  Partition(r4);
 } // else
 else
 {
  // Should there be no obstacles inside of the Rectangle, then we are going to
  // add the centre of this Rectangle into the AddNode() function. We do this by
  // running the Rectangle2D's 'GetCentre()' function.
  Vector2D position = r.GetCentre();
  AddNode(position);
 } // else if
} // Partition()


void Graph::AddNode(Vector2D location)
{
 Node newNode;
 newNode.position = location;
 newNode.id = newNode.nodeNumber;
 NodeVector.push_back(newNode);
} // AddNode()


void Graph::DrawNodes()
{
 for (unsigned int i = 0; i < NodeVector.size(); ++i)
 {
  MyDrawEngine::GetInstance()->DrawPoint(NodeVector[i].position, _XRGB(122, 122, 122));
  //Renderer::GetInstance()->DrawDot(NodeVector[i].position, 1);
 } // for
} // DrawNodes()


void Graph::DrawEdges()
{
 std::vector<Node>::iterator itr;
 std::list<Edge>::iterator itr2;

 for (itr = NodeVector.begin(); itr != NodeVector.end(); ++itr)
 {
  for (itr2 = itr->edgeList.begin(); itr2 != itr->edgeList.end(); ++itr2)
  {
   Renderer::GetInstance()->DrawLine((*itr2).fromNode->position, (*itr2).toNode->position, _XRGB(107, 163, 52));
  } // nested-for
 } // for
} // DrawEdges()



void Graph::DrawPath(std::vector<Vector2D> path)
{
 for (unsigned int i = 0; i < path.size(); ++i)
 {
  Renderer::GetInstance()->DrawDot(path[i], _XRGB(52, 163, 163));
 } // for
} // DrawPath()



void Graph::AddEdge(Node* from, Node* to, float cost)
{
 Edge newEdge;
 newEdge.fromNode = from;
 newEdge.toNode = to;
 newEdge.cost = cost;
 from->edgeList.push_back(newEdge);
} // AddEdge()



void Graph::AnalyseMap()
{
 Rectangle2D r;
 r.PlaceAt(1600, -1513, -1600, 1613);
 Partition(r);

 //std::vector<Node>::iterator itr2;

 for (auto& iterateNodeVector : NodeVector)
 {
  for (auto& iterateNodeVector2 : NodeVector)
  {
   if (StaticMap::GetInstance()->IsLineOfSight(iterateNodeVector.position, iterateNodeVector2.position) && (GetDistance(&(iterateNodeVector), &(iterateNodeVector2))) < 250)
   {
    AddEdge(&(iterateNodeVector), &(iterateNodeVector2), GetDistance(&(iterateNodeVector), &(iterateNodeVector2)));
   } // if
  } // nested-for
 } // for
} // AnalyseMap()



float Graph::GetDistance(Node* from, Node* to)
{
 // Returns the magnitude between two nodes.
 return (to->position - from->position).magnitude();
} // GetDistance()



Node* Graph::GetClosestNode(Vector2D position)
{
 std::vector<Node>::iterator itr;
 Node* answer = nullptr;
 float bestDistance = FLT_MAX;

 for (auto& nodeVector : NodeVector)
 {
  if (((nodeVector).position - position).magnitude() < bestDistance)
  {
   answer = &(nodeVector);
   bestDistance = ((nodeVector).position - position).magnitude();
  }
 }
 return answer;
} // GetClosestNode()


float Graph::Heuristic(Vector2D from, Vector2D to)
{
 // Uses the 'Manhattan' method in order to decide the Heuristics. This method
 // can 'overestimate,' however, which will sometimes lead to A* not picking
 // the fastest path but avoids having to use sqrt.
 return (from.XValue - to.XValue) + (from.YValue - to.YValue);
} // Heuristic()


Node* Graph::LowestFValue()
{
 std::list<Node*>::iterator itr;
 std::list<Node*>::iterator itr2 = openList.begin();

 Node* lowestNode = *itr2;

 for (itr = openList.begin(); itr != openList.end(); ++itr)
 {
  if (lowestNode->f > (*itr2)->f)
  {
   lowestNode = (*itr2);
  }
 }
 return lowestNode;
} // LowestFValue()


void Graph::Reset()
{
 // Loop through the NodeVector and set the f and g values of all nodes to 0.
 for (auto& reset : NodeVector)
 {
  (reset).f = 0.0f;
  (reset).g = 0.0f;
  (reset).parentNode = nullptr;
 }

 // Start by emptying both of the lists before performing any A* searches. This
 // is to ensure that the A* search doesn't have any junk information remaining
 // from previous searches.
 closedList.clear();
 openList.clear();
} // Reset()


bool Graph::openListContains(Node* neighbour)
{
 for (/*std::list<Node*>::iterator itr = openList.begin(); itr != openList.end(); ++itr*/auto& iterateOpenList : openList)
 {
	 if (neighbour == (iterateOpenList))
	 {
		 return true;
	 }
 }
 return false;
} // openListContains()


bool Graph::closedListContains(Node* neighbour)
{
 for (auto& iterateClosedList : closedList)
 {
  if (neighbour == (iterateClosedList))
  {
   return true;
  }
 }
 return false;
} // openListContains()


std::vector<Vector2D> Graph::Pathfind(Node* from, Node* to, Vector2D destination)
{
 // Resets the algorithm.
 Reset();

 // Create the neighbour list.
 // std::list<Node*> neighbourList;

 // Create the starting node, populate it with information and push it to the
 // frontier list. Remember:
 // g(n) = the exact cost of the path from starting point to any other node.
 // h(n) = the heuristic estimate.
 // f(n) = g(n) + h(n).
 // This initial node will be pushed to the Tree instantaneously in while().
 pCurrent = nullptr;
 from->parentNode = nullptr;
 from->g = 0.0f;
 from->f = from->g + Heuristic(from->position, to->position);
 openList.push_back(from);

 // Runs until either the frontier has been emptied, or the goal has been found.
 while (!openList.empty())
 {
  // neighbourList.clear();
  pCurrent = LowestFValue(); // Set pCurrent to the lowest F value in Frontier.

  if (pCurrent == to) 
  {
   return GetPath(pCurrent, destination);
  } 
  openList.remove(pCurrent); // Remove pCurrent from the Frontier.
  closedList.push_back(pCurrent); // Add pCurrent to the tree.

  std::list<Node*> neighbourList;

  neighbourList = pCurrent->GetNeighbours(); // Adds the neighbours of the node to a list.

  //std::list<Node*>::iterator neighbour;
  for (auto& nearbyNeighbours : neighbourList)
  {
   if (!closedListContains((nearbyNeighbours)))
   {
    float new_g_score = pCurrent->g + GetDistance(pCurrent, (nearbyNeighbours));
	   if ((new_g_score < (nearbyNeighbours)->g) || !openListContains((nearbyNeighbours)))
	   {
	    (nearbyNeighbours)->parentNode = pCurrent;
	    (nearbyNeighbours)->g = new_g_score;
	    (nearbyNeighbours)->f = new_g_score + Heuristic((nearbyNeighbours)->position, to->position);
	    if (!openListContains((nearbyNeighbours)))
	    {
	     openList.push_back((nearbyNeighbours));
	    }
	   }
   }
  }
 }
  std::vector<Vector2D> temp;
 return GetPath(pCurrent, Vector2D(0,0));
} // Pathfind()


std::vector<Vector2D> Graph::Pathfind(Vector2D from, Vector2D to)
{
 Node* a = GetClosestNode(from);
 Node* b = GetClosestNode(to);
 return Pathfind(a, b, to);
} // Pathfind()


std::vector<Vector2D> Graph::GetPath(Node* current, Vector2D destination)
{
 std::vector<Vector2D> temp;
 temp.push_back(destination);
 while (current->parentNode != nullptr)
 {
  temp.push_back(current->position);
  current = current->parentNode;
 }
 temp.push_back(current->position);
 return temp;
} // GetPath()