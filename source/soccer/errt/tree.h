#pragma once

namespace Tyr::Soccer
{
struct Node
{
    Node *parent      = nullptr;
    int   child_count = 0;

    Common::Vec2 state;
};

class Tree
{
private:
    std::vector<Node> m_nodes;

public:
    Tree(size_t t_max_nodes);
    void reset();

    Node *AddNode(Common::Vec2 s, Node *parent);
    Node *NearestNeighbour(Common::Vec2 s);
    int   tree_size();
    Node *GetNode(int idx);

    void draw(Common::Color t_color = Common::Color::black().transparent()) const;
};
} // namespace Tyr::Soccer
