#ifndef _NODE_H_
#define _NODE_H_

#include <vector>

class Node
{
public:
	int mX;
	int mY;
	std::vector<Node*> mConnections;

	Node(int x, int y): mX(x), mY(y) {}
};

#endif