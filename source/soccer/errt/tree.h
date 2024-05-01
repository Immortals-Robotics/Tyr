#pragma once

namespace Tyr::Soccer
{
struct Node
{
    Node        *parent;
    unsigned int childs_num;
    Common::Vec2        state;
};

#define MAX_NODES 1000

class Tree
{
    Node node[MAX_NODES];

    unsigned int nodes_num;

public:
    Tree();
    void         reset();
    Node        *AddNode(Common::Vec2 &s, Node *p);
    Node        *NearestNeighbour(Common::Vec2 &s);
    unsigned int tree_size();
    Node        *GetNode(unsigned int num);
};
} // namespace Tyr::Soccer
