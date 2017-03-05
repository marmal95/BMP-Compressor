#ifndef NODE_H
#define NODE_H

#include <fstream>

class Node
{
private:
	Node *left;
	Node *right;
	std::pair<uint32_t, uint32_t> colorData;

public:
	Node(std::pair<uint32_t, uint32_t> colorData); // Remarks: const reference is slower
	Node(Node *l, Node *r);
	~Node();

	friend class Huffman;
	friend struct NodeCmp;
};

struct NodeCmp
{
	bool operator() (const Node *lhs, const Node *rhs) const
	{
		return lhs->colorData.second > rhs->colorData.second;
	}
};

#endif // ! NODE_H

