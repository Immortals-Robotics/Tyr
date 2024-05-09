#include "tree.h"

namespace Tyr::Soccer
{
Tree::Tree(const size_t t_max_nodes)
{
    m_nodes.reserve(t_max_nodes);
}

void Tree::reset()
{
    m_nodes.clear();
}

Node *Tree::AddNode(const Common::Vec2 s, Node *const parent)
{
    if (m_nodes.size() < m_nodes.capacity())
    {
        m_nodes.push_back({
            .parent = parent,
            .state  = s,
        });

        if (parent)
            parent->child_count++;

        return &m_nodes.back();
    }
    else
    {
        Common::logWarning("Tree node limit of {} reached", m_nodes.capacity());
        return nullptr;
    }
}

Node *Tree::NearestNeighbour(const Common::Vec2 s)
{
    if (m_nodes.empty())
        return nullptr;

    float d   = std::numeric_limits<float>::max();
    int   ans = 0;
    for (int i = 0; i < m_nodes.size(); i++)
    {
        float tmp_d = s.distanceSquaredTo(m_nodes[i].state);
        if (tmp_d < d)
        {
            ans = i;
            d   = tmp_d;
        }
    }

    return &m_nodes[ans];
}

int Tree::tree_size()
{
    return m_nodes.size();
}

Node *Tree::GetNode(const int idx)
{
    return &m_nodes[idx];
}

void Tree::draw(Common::Color t_color) const
{
    for (int i = 0; i < m_nodes.size(); i++)
    {
        if (m_nodes[i].parent == nullptr)
        {
            // TODO: draw sth to mark the root
        }
        else
        {
            Common::debug().draw(Common::LineSegment{m_nodes[i].state, m_nodes[i].parent->state}, t_color, 0.5f);
        }
    }
}
} // namespace Tyr::Soccer
