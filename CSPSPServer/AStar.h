#ifndef _ASTAR_H_
#define _ASTAR_H_

#include <vector>
#include "Node.h"

class Grid;
class Person;

class AStar
{
private:
	std::vector<Node*> mNodes;

public:
	void Rebuild(std::vector<Node*>* nodes, Grid* grid);
	Node* GetClosestNode(Person* person, Grid* grid);
	Node* GetClosestNode(float x, float y, Grid* grid);
	std::vector<Node*> GetPath(Node* start, Node* end);
};

#endif