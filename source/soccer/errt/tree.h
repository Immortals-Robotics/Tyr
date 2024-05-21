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
public:
    Tree(const size_t t_max_nodes)
    {
        m_nodes.reserve(t_max_nodes);
    }

    void reset()
    {
        m_nodes.clear();
    }

    Node *AddNode(const Common::Vec2 s, Node *const parent)
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

    Node *NearestNeighbour(const Common::Vec2 s)
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

        return GetNode(ans);
    }

    int tree_size()
    {
        return m_nodes.size();
    }

    Node *GetNode(const int idx)
    {
        return &m_nodes[idx];
    }

    void draw(const Common::Color t_color = Common::Color::black().transparent()) const
    {
        for (int i = 0; i < m_nodes.size(); i++)
        {
            if (m_nodes[i].parent == nullptr)
            {
                // TODO: draw sth to mark the root
            }
            else
            {
                Common::debug().draw(Common::LineSegment{m_nodes[i].state, m_nodes[i].parent->state}, t_color, 8.0f);
            }
        }
    }

private:
    std::vector<Node> m_nodes;
};
} // namespace Tyr::Soccer
