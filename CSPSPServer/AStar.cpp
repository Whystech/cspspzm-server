#include "AStar.h"
#include "Grid.h"
#include "Person.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>

void AStar::Rebuild(std::vector<Node*>* nodes, Grid* grid)
{
	mNodes = *nodes;
	for (unsigned int i=0; i<mNodes.size(); i++) {
		mNodes[i]->mConnections.clear();
	}

	for (unsigned int i=0; i<mNodes.size(); i++) {
		for (unsigned int j=i+1; j<mNodes.size(); j++) {
			if (!grid->LineOfSight(mNodes[i]->mX,mNodes[i]->mY,mNodes[j]->mX,mNodes[j]->mY,true,true)) continue;
			mNodes[i]->mConnections.push_back(mNodes[j]);
			mNodes[j]->mConnections.push_back(mNodes[i]);
		}
	}
}

Node* AStar::GetClosestNode(Person* person, Grid* grid)
{
	return GetClosestNode(person->mX,person->mY,grid);
}

Node* AStar::GetClosestNode(float x, float y, Grid* grid)
{
	Node* closest = NULL;
	float closestDistance = (std::numeric_limits<float>::max)();
	for (unsigned int i=0; i<mNodes.size(); i++) {
		float dx = (float)mNodes[i]->mX-x;
		float dy = (float)mNodes[i]->mY-y;
		float distance = dx*dx+dy*dy;
		if (distance >= closestDistance) continue;
		if (!grid->LineOfSight(x,y,mNodes[i]->mX,mNodes[i]->mY,true,true)) continue;
		closest = mNodes[i];
		closestDistance = distance;
	}
	return closest;
}

std::vector<Node*> AStar::GetPath(Node* start, Node* end)
{
	std::vector<Node*> path;
	if (start == NULL || end == NULL) return path;
	if (start == end) {
		path.push_back(start);
		return path;
	}

	std::vector<Node*> open;
	std::vector<Node*> closed;
	std::map<Node*,Node*> parent;
	std::map<Node*,float> cost;
	open.push_back(start);
	cost[start] = 0.0f;

	while (!open.empty()) {
		unsigned int bestIndex = 0;
		float bestScore = (std::numeric_limits<float>::max)();
		for (unsigned int i=0; i<open.size(); i++) {
			float dx = (float)open[i]->mX-end->mX;
			float dy = (float)open[i]->mY-end->mY;
			float score = cost[open[i]]+sqrtf(dx*dx+dy*dy);
			if (score < bestScore) {
				bestScore = score;
				bestIndex = i;
			}
		}

		Node* current = open[bestIndex];
		open.erase(open.begin()+bestIndex);
		if (current == end) {
			for (Node* node=end; node != NULL; node=parent.count(node) ? parent[node] : NULL) {
				path.push_back(node);
				if (node == start) break;
			}
			std::reverse(path.begin(),path.end());
			return path;
		}
		closed.push_back(current);

		for (unsigned int i=0; i<current->mConnections.size(); i++) {
			Node* neighbor = current->mConnections[i];
			if (std::find(closed.begin(),closed.end(),neighbor) != closed.end()) continue;
			float dx = (float)neighbor->mX-current->mX;
			float dy = (float)neighbor->mY-current->mY;
			float nextCost = cost[current]+sqrtf(dx*dx+dy*dy);
			if (!cost.count(neighbor) || nextCost < cost[neighbor]) {
				cost[neighbor] = nextCost;
				parent[neighbor] = current;
				if (std::find(open.begin(),open.end(),neighbor) == open.end()) open.push_back(neighbor);
			}
		}
	}

	return path;
}