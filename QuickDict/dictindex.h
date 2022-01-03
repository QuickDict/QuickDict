#ifndef DICTINDEX_H
#define DICTINDEX_H

/*
 * Reference: [Compressing dictionaries with a DAWG](http://stevehanov.ca/blog/index.php?id=115)
 */

#include <algorithm>
#include <stack>
#include <stdio.h>
#include <unordered_map>
#include <utility>
#include <vector>

using Size = unsigned int;

template<typename Key, typename Value>
struct DictIndexNode
{
    using KeyIter = typename std::iterator_traits<typename Key::iterator>::value_type;

    DictIndexNode(const KeyIter &key = KeyIter(), const Value &value = Value())
        : _key(key)
        , _value(value)
    {}

    void addChild(DictIndexNode *child)
    {
        _children.push_back(child); // `_children` is in order because `addEntry` is called with ordered data.
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
bool operator==(const DictIndexNode<Key, Value> &lhs, const DictIndexNode<Key, Value> &rhs)
{
    return lhs._key == rhs._key && lhs._children == rhs._children;
}

template<typename Key, typename Value>
struct std::hash<DictIndexNode<Key, Value>>
{
    using KeyIter = typename std::iterator_traits<typename Key::iterator>::value_type;
    using DictIndexNode = DictIndexNode<Key, Value>;
    std::size_t operator()(const DictIndexNode &n) const noexcept
    {
        return std::hash<KeyIter>{}(n._key) ^ std::hash<std::vector<DictIndexNode *>>{}(n._children);
    }
};

template<typename Key, typename Value>
struct std::hash<std::vector<DictIndexNode<Key, Value> *>>
{
    using DictIndexNodePtr = DictIndexNode<Key, Value> *;
    std::size_t operator()(const std::vector<DictIndexNodePtr> &vec) const noexcept
    {
        size_t hash_value = 0;
        for (const auto &ptr : vec)
            hash_value ^= std::hash<void *>{}(static_cast<void *>(ptr));
        return hash_value;
    }
};

template<typename Key, typename Value>
class DictIndex
{
    using KeyIter = typename std::iterator_traits<typename Key::iterator>::value_type;
    using DictIndexNode = DictIndexNode<Key, std::vector<Value>>;

public:
    DictIndex() { uncheckedNodes.push_back(&rootNode); };
    DictIndexNode *addEntry(const Key &key, const Value &value)
    {
        DictIndexNode *previousNode = &rootNode;
        int n = key.size();
        int index = 0;
        for (; index < n; ++index) {
            DictIndexNode *child = previousNode->findChild(key[index]);
            if (!child)
                break;
            previousNode = child;
        }
        // minimize(index);
        DictIndexNode *ret = previousNode;
        for (; index < n; ++index) {
            DictIndexNode *child = new DictIndexNode(key[index]);
            previousNode->addChild(child);
            previousNode = child;
            // uncheckedNodes.push_back(child);
        }
        previousNode->_value.push_back(value);
        return ret;
    }
    DictIndexNode *findEntry(const Key &key) const
    {
        DictIndexNode const *previousNode = &rootNode;
        int n = key.length();
        int index = 0;
        for (; index < n; ++index) {
            DictIndexNode *child = previousNode->findChild(key[index]);
            if (!child)
                break;
            previousNode = child;
        }
        if (index < n || previousNode == &rootNode)
            return nullptr;
        return const_cast<DictIndexNode *>(previousNode);
    }
    std::vector<std::pair<Key, DictIndexNode *>> allEntries() const
    {
        std::vector<std::pair<Key, DictIndexNode *>> entries;
        DictIndexNode *levelMarker = nullptr;
        std::vector<KeyIter> keySequence;
        std::stack<const DictIndexNode *> s;
        s.push(&rootNode);
        while (!s.empty()) {
            DictIndexNode *node = const_cast<DictIndexNode *>(s.top());
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
    void clear(DictIndexNode *node = nullptr)
    {
        if (!node)
            node = &rootNode;
        for (const auto &child : node->_children)
            clear(child);
        if (node != &rootNode)
            delete node;
    }
    void minimize(int upTo)
    {
        for (auto it = uncheckedNodes.end() - 1; it > uncheckedNodes.begin() + upTo; --it) {
            auto node = checkedNodes.find(**it);
            if (node == checkedNodes.end())
                checkedNodes[**it] = *it;
            else
                (*node).second->_value.insert((*node).second->_value.end(), (*it)->_value.begin(), (*it)->_value.end());
            DictIndexNode *parent = *(it - 1);
            auto pos = std::lower_bound(parent->_children.begin(),
                                        parent->_children.end(),
                                        *it,
                                        [](const auto &lhs, const auto &rhs) { return lhs->_key < rhs->_key; });
            *pos = checkedNodes[**it];
            if (node != checkedNodes.end())
                delete *it;
            uncheckedNodes.pop_back();
        }
    };
    void finish() { minimize(0); }
    size_t serialize(FILE *fp, DictIndexNode *node = nullptr)
    {
        if (!node)
            node = &rootNode;

        Size begin = ftell(fp);

        _serialize(fp, node->_key);
        _serialize(fp, node->_value);
        ushort num = node->_children.size();
        fwrite(&num, sizeof(num), 1, fp);
        Size children_offset = ftell(fp);
        for (int i = 0; i < num; ++i)
            fwrite(&begin, sizeof(begin), 1, fp); // placeholder
        Size *children_offset_arr = (Size *) malloc(sizeof(Size) * num);
        for (int i = 0; i < num; ++i) {
            children_offset_arr[i] = ftell(fp);
            serialize(fp, node->_children[i]);
        }
        fseek(fp, children_offset, SEEK_SET);
        for (int i = 0; i < num; ++i) {
            fwrite(&children_offset_arr[i], sizeof(Size), 1, fp);
        }
        free(children_offset_arr);

        fseek(fp, 0, SEEK_END);
        Size end = ftell(fp);
        return end - begin;
    }
    size_t deserialize(FILE *fp, DictIndexNode *node = nullptr)
    {
        if (!node)
            node = &rootNode;

        Size begin = ftell(fp);

        _deserialize(fp, node->_key);
        _deserialize(fp, node->_value);
        ushort num = node->_children.size();
        fread(&num, sizeof(num), 1, fp);
        node->_children.resize(num);
        Size *children_offset_arr = (Size *) malloc(sizeof(Size) * num);
        fread(children_offset_arr, sizeof(Size), num, fp);
        for (int i = 0; i < num; ++i) {
            fseek(fp, children_offset_arr[i], SEEK_SET);
            DictIndexNode *child = new DictIndexNode;
            deserialize(fp, child);
            node->_children[i] = child;
        }
        free(children_offset_arr);

        fseek(fp, 0, SEEK_END);
        Size end = ftell(fp);
        return end - begin;
    }

private:
    std::vector<DictIndexNode *> uncheckedNodes;
    std::unordered_map<DictIndexNode, DictIndexNode *> checkedNodes;
    DictIndexNode rootNode;
};

template<typename T>
size_t _serialize(FILE *fp, const T &data)
{
    return fwrite(&data, sizeof(T), 1, fp);
}

template<typename T>
size_t _deserialize(FILE *fp, T &data)
{
    return fread(&data, sizeof(T), 1, fp);
}

template<typename T>
size_t _serialize(FILE *fp, const std::vector<T> &vec)
{
    size_t bytes = 0;
    ushort len = vec.size();
    bytes += fwrite(&len, sizeof(len), 1, fp);
    for (const auto &e : vec)
        bytes += _serialize(fp, e);
    return bytes;
}

template<typename T>
size_t _deserialize(FILE *fp, std::vector<T> &vec)
{
    size_t bytes = 0;
    ushort len;
    bytes += fread(&len, sizeof(len), 1, fp);
    vec.clear();
    vec.reserve(len);
    for (int i = 0; i < len; ++i) {
        T v;
        bytes += _deserialize(fp, v);
        vec.push_back(v);
    }
    return bytes;
}

template<typename T1, typename T2>
size_t _serialize(FILE *fp, const std::pair<T1, T2> &pair)
{
    return fwrite(&pair.first, sizeof(T1), 1, fp) + fwrite(&pair.second, sizeof(T2), 1, fp);
}

template<typename T1, typename T2>
size_t _deserialize(FILE *fp, std::pair<T1, T2> &pair)
{
    return fread(&pair.first, sizeof(T1), 1, fp) + fread(&pair.second, sizeof(T2), 1, fp);
}

#endif // DICTINDEX_H
