// Title  :  Graph.h
// Purpose:  Provides the BMB Program with a Graph for AI Purposes
// Author :  Jonathon H Tyson
// Date   :  13/10/2017

#pragma once
#include "shapes.h"
#include "rules.h"
#include <list>
#include <vector>

struct Node;
struct Edge
{
 Node* fromNode;
 Node* toNode;
 float cost;
};


struct Node
{
 Node();
 static unsigned int nodeNumber;
 int id;
 std::list<Edge> edgeList;
 Vector2D position;
 float g; // Movement Cost
 float f; // Heuristic + Cost
 Node* parentNode;
 std::list<Node*> GetNeighbours();
};


class Graph
{
private:
	Graph();

	Graph(const Graph& other);		// Disabled
	static Graph* pInst;
 std::vector<Node> NodeVector;
 std::list<Node*> openList;
 std::list<Node*> closedList;
 std::vector<Vector2D> Pathfind(Node* from, Node* to, Vector2D destination);
 Node* pCurrent;

public:
 ~Graph();
	static Graph* GetInstance();				// Thread-safe singleton
 void Partition(Rectangle2D r);
 void AddNode(Vector2D location);
 void DrawNodes();
 void DrawEdges();
 void DrawPath(std::vector<Vector2D> path);
 void AddEdge(Node* from, Node* to, float cost);
 void AnalyseMap();
 float GetDistance(Node* from, Node* to);
 Node* GetClosestNode(Vector2D position);
 float Heuristic(Vector2D from, Vector2D to);
 Node* LowestFValue();
 void Reset();
 bool openListContains(Node* neighbour);
 bool closedListContains(Node* neighbour);
 std::vector<Vector2D> GetPath(Node* current, Vector2D destination);
 std::vector<Vector2D> Pathfind(Vector2D from, Vector2D to);
};