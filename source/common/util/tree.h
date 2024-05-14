#pragma once
#include <vector>
#include <queue>

#include "../logging/macros.h"

namespace Tyr::Common
{
template <typename T>
struct Node
{
    T                                     value;
    std::vector<std::unique_ptr<Node<T>>> children;

    Node(T t_val) : value(t_val)
    {}
};

template <typename T>
class Tree
{
public:
    Tree() = default;

    inline void insert(T t_value, Node<T> *t_parent = nullptr)
    {
        auto node = new Node<T>(t_value);
        m_nodes.push_back(node);
        if (!m_root) {
            m_root = node;
        } else if (t_parent) {
            t_parent->children.push_back(node);
        } else {
            logError("Parent is Null");
        }
    }

    inline void clear() {
        m_nodes.clear();
        m_root.reset();
    }

    inline std::size_t size() const {
        return m_nodes.size();
    }

private:
    std::unique_ptr<Node<T>> m_root;
    std::vector<std::unique_ptr<Node<T>>> m_nodes;
};

} // namespace Tyr::Common
