#ifndef DICTINDEX_H
#define DICTINDEX_H

/*
 * Reference: [Compressing dictionaries with a DAWG](http://stevehanov.ca/blog/index.php?id=115)
 */

#include <algorithm>
#include <stack>
#include <stdio.h>
#include <string.h>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include <QtCore/qglobal.h>

#ifdef Q_OS_LINUX
#include <malloc.h>
#endif

template<typename Key, typename Value>
struct DictIndexNode
{
    using KeyIter = typename std::iterator_traits<typename Key::iterator>::value_type;

    DictIndexNode(const KeyIter &key = KeyIter(), const Value &value = Value())
        : _key(key)
        , _value(value)
    {}

    inline void addChild(DictIndexNode *child)
    {
        // `_children` is in order because `addEntry` is called with ordered data.
        _children.push_back(child);
    }
    DictIndexNode *findChild(const KeyIter &key) const
    {
        DictIndexNode node(key);
        auto pos = std::lower_bound(_children.begin(), _children.end(), &node, [](const auto &lhs, const auto &rhs) {
            return lhs->_key < rhs->_key;
        });
        if (pos == _children.end() || (*pos)->_key != key)
            return nullptr;
        return *pos;
    }

    KeyIter _key;
    Value _value;
    std::vector<DictIndexNode *> _children;
};

template<typename Key, typename Value>
inline bool operator==(const DictIndexNode<Key, Value> &lhs, const DictIndexNode<Key, Value> &rhs)
{
    return lhs._key == rhs._key && lhs._children == rhs._children;
}

template<typename Key, typename Value>
struct std::hash<DictIndexNode<Key, Value>>
{
    using KeyIter = typename std::iterator_traits<typename Key::iterator>::value_type;
    using IndexNode = DictIndexNode<Key, Value>;
    std::size_t operator()(const IndexNode &n) const noexcept
    {
        return std::hash<KeyIter>{}(n._key) ^ std::hash<std::vector<IndexNode *>>{}(n._children);
    }
};

template<typename Key, typename Value>
class DictIndex
{
    using KeyIter = typename std::iterator_traits<typename Key::iterator>::value_type;
    using IndexNode = DictIndexNode<Key, std::vector<Value>>;
    using Size = uint32_t;

public:
    DictIndex() { m_uncheckedNodes.push_back(&m_rootNode); };
    IndexNode *addEntry(const Key &key, const Value &value)
    {
        if (key < m_prevKey)
            return nullptr;

        IndexNode *previousNode = &m_rootNode;
        int n = key.size();
        int index = 0;
        for (; index < n; ++index) {
            // Since it's sorted, we just need to look at the rightest child.
            if (previousNode->_children.empty() || previousNode->_children.back()->_key != key[index])
                break;
            previousNode = previousNode->_children.back();
        }
        // minimize(index);
        IndexNode *ret = previousNode;
        for (; index < n; ++index) {
            IndexNode *child = new IndexNode(key[index]);
            previousNode->addChild(child);
            ++m_nodeCount;
            previousNode = child;
            // m_uncheckedNodes.push_back(child);
        }
        previousNode->_value.push_back(value);
        ++m_entryCount;
        m_prevKey = key;
        return ret;
    }
    IndexNode *findEntry(const Key &key) const
    {
        IndexNode const *previousNode = &m_rootNode;
        int n = key.length();
        int index = 0;
        for (; index < n; ++index) {
            IndexNode *child = previousNode->findChild(key[index]);
            if (!child)
                break;
            previousNode = child;
        }
        if (index < n || previousNode == &m_rootNode)
            return nullptr;
        return const_cast<IndexNode *>(previousNode);
    }
    std::vector<std::pair<Key, IndexNode *>> allEntries() const
    {
        std::vector<std::pair<Key, IndexNode *>> entries;
        IndexNode *levelMarker = nullptr;
        std::vector<KeyIter> keySequence;
        std::stack<const IndexNode *> s;
        s.push(&m_rootNode);
        while (!s.empty()) {
            IndexNode *node = const_cast<IndexNode *>(s.top());
            s.pop();
            if (node == levelMarker) {
                keySequence.pop_back();
                continue;
            }
            keySequence.push_back(node->_key);
            if (!node->_value.empty()) {
                Key key;
                for (auto it = ++keySequence.begin(); it != keySequence.end(); ++it)
                    key.push_back(*it);
                entries.push_back({key, node});
            }
            s.push(levelMarker);
            for (auto it = node->_children.rbegin(); it != node->_children.rend(); ++it)
                s.push(*it);
        }
        return entries;
    }
    void clear()
    {
        IndexNode *node;
        std::stack<IndexNode *> s;
        for (const auto &child : m_rootNode._children)
            s.push(child);
        while (!s.empty()) {
            node = s.top();
            s.pop();
            for (const auto &child : node->_children)
                s.push(child);
            delete node;
        }

        m_rootNode._children.clear();
        m_rootNode._children.shrink_to_fit();
        m_uncheckedNodes.clear();
        m_uncheckedNodes.shrink_to_fit();
        m_checkedNodes.clear();
        m_prevKey = Key();
        m_nodeCount = 0;
        m_entryCount = 0;
#ifdef Q_OS_LINUX
        malloc_trim(0); // release memory back to OS
#endif
    }
    void minimize(int upTo)
    {
        for (auto it = m_uncheckedNodes.end() - 1; it > m_uncheckedNodes.begin() + upTo; --it) {
            auto node = m_checkedNodes.find(**it);
            if (node == m_checkedNodes.end())
                m_checkedNodes[**it] = *it;
            else
                (*node).second->_value.insert((*node).second->_value.end(), (*it)->_value.begin(), (*it)->_value.end());
            IndexNode *parent = *(it - 1);
            auto pos = std::lower_bound(parent->_children.begin(),
                                        parent->_children.end(),
                                        *it,
                                        [](const auto &lhs, const auto &rhs) { return lhs->_key < rhs->_key; });
            *pos = m_checkedNodes[**it];
            if (node != m_checkedNodes.end())
                delete *it;
            m_uncheckedNodes.pop_back();
        }
    };
    void finish() { minimize(0); }
    size_t serialize(FILE *fp)
    {
        size_t bytes = byteCount();
        unsigned char *buf = (unsigned char *) malloc(bytes);
        unsigned char *start = buf;

        IndexNode *node;
        std::stack<IndexNode *> s;
        s.push(&m_rootNode);
        while (!s.empty()) {
            node = s.top();
            s.pop();
            _serialize(start, *node);
            for (auto it = node->_children.rbegin(); it != node->_children.rend(); ++it)
                s.push(*it);
        }
        fwrite(&bytes, sizeof(bytes), 1, fp);
        fwrite(buf, bytes, 1, fp);
        free(buf);
        return bytes;
    }
    size_t deserialize(FILE *fp)
    {
        size_t bytes;
        fread(&bytes, sizeof(bytes), 1, fp);
        unsigned char *buf = (unsigned char *) malloc(bytes);
        fread(buf, bytes, 1, fp);
        unsigned char *start = buf;

        IndexNode *node;
        std::stack<IndexNode *> ancestors;
        std::stack<IndexNode *> s;
        s.push(&m_rootNode);
        while (!s.empty()) {
            node = s.top();
            s.pop();
            _deserialize(start, *node);
            if (!ancestors.empty()) {
                ancestors.top()->_children.push_back(node);
                if (ancestors.top()->_children.size() == ancestors.top()->_children.capacity())
                    ancestors.pop();
            }
            if (node->_children.capacity() > 0)
                ancestors.push(node);
            for (size_t i = 0; i < node->_children.capacity(); ++i)
                s.push(new IndexNode);
        }
        free(buf);
        return 0;
    }
    inline size_t nodeCount() const { return m_nodeCount; }
    inline size_t entryCount() const { return m_entryCount; }
    size_t byteCount() const
    {
        size_t bytes = (nodeCount() + 1 /* m_rootNode */)
                           * (sizeof(KeyIter) + sizeof(uint8_t /* num of values */)
                              + sizeof(uint16_t /* num of children */))
                       + entryCount() * sizeof(Value);
        return bytes;
    }

private:
    std::vector<IndexNode *> m_uncheckedNodes;
    std::unordered_map<IndexNode, IndexNode *> m_checkedNodes;
    IndexNode m_rootNode;
    Key m_prevKey;
    size_t m_nodeCount = 0;
    size_t m_entryCount = 0;
};

template<typename T>
inline void _serialize(unsigned char *&data, const T &object)
{
    memcpy(data, &object, sizeof(T));
    data += sizeof(T);
}

template<typename T>
inline void _deserialize(unsigned char *&data, T &object)
{
    memcpy(&object, data, sizeof(T));
    data += sizeof(T);
}

template<typename T>
inline void _serialize(unsigned char *&data, const std::vector<T> &vec)
{
    uint8_t size = vec.size();
    _serialize(data, size);
    for (int i = 0; i < size; ++i)
        _serialize(data, vec[i]);
}

template<typename T>
inline void _deserialize(unsigned char *&data, std::vector<T> &vec)
{
    uint8_t size;
    _deserialize(data, size);
    vec.resize(size);
    for (int i = 0; i < size; ++i)
        _deserialize(data, vec[i]);
}

template<typename T1, typename T2, typename T3>
inline void _serialize(unsigned char *&data, const std::tuple<T1, T2, T3> &t)
{
    for (int i = 0; i < 3; ++i)
        _serialize(data, std::get<i>(t));
}

template<typename T1, typename T2, typename T3>
inline void _deserialize(unsigned char *&data, std::tuple<T1, T2, T3> &t)
{
    for (int i = 0; i < 3; ++i)
        _deserialize(data, std::get<i>(t));
}

template<typename T1, typename T2>
inline void _serialize(unsigned char *&data, const std::pair<T1, T2> &pair)
{
    _serialize(data, pair.first);
    _serialize(data, pair.second);
}

template<typename T1, typename T2>
inline void _deserialize(unsigned char *&data, std::pair<T1, T2> &pair)
{
    _deserialize(data, pair.first);
    _deserialize(data, pair.second);
}

template<typename Key, typename Value>
inline void _serialize(unsigned char *&data, const DictIndexNode<Key, Value> &node)
{
    _serialize(data, node._key);
    _serialize(data, node._value);
    _serialize(data, static_cast<uint16_t>(node._children.size()));
}

template<typename Key, typename Value>
inline void _deserialize(unsigned char *&data, DictIndexNode<Key, Value> &node)
{
    _deserialize(data, node._key);
    _deserialize(data, node._value);
    uint16_t size;
    _deserialize(data, size);
    node._children.reserve(size);
}

#endif // DICTINDEX_H
