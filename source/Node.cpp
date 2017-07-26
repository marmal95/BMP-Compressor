#include "Node.h"

Node::Node(std::pair<uint32_t, uint32_t> colorData)
	: left(nullptr), right(nullptr), colorData(colorData)
{}

Node::Node(Node * l, Node * r)
	: left(l), right(r), colorData(std::make_pair(-1, l->colorData.second + r->colorData.second))
{}

Node::~Node()
{
	delete left; 
	delete right;
}
