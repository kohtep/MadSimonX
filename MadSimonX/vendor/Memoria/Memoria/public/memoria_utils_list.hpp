//
// memoria_utils_list.hpp
// 
// An alternative to std::list<T> to eliminate dependency on the CRT.
//

#pragma once

#include <cstddef>
#include <utility>
#include <memory>

#include "memoria_common.hpp"
#include "memoria_core_mempool.hpp"

MEMORIA_BEGIN

template<typename T>
class List
{
private:
    struct Node
    {
        Node *prev;
        Node *next;
        T value;

        Node(const T &val)
            : prev(nullptr), next(nullptr), value(val)
        {
        }

        Node(T &&val)
            : prev(nullptr), next(nullptr), value(std::move(val))
        {
        }

        template<typename... Args>
        Node(Args&&... args)
            : prev(nullptr), next(nullptr), value(std::forward<Args>(args)...)
        {
        }
    };

    Node *_head = nullptr;
    Node *_tail = nullptr;
    size_t _size = 0;

    Node *create_node()
    {
        return static_cast<Node *>(::operator new(sizeof(Node)));
    }

    void destroy_node(Node *node)
    {
        std::destroy_at(node);
        ::operator delete(node);
    }

public:
    class iterator
    {
    public:
        iterator(Node *ptr) : _ptr(ptr) {}

        T &operator*() { return _ptr->value; }
        T *operator->() { return &_ptr->value; }

        iterator &operator++()
        {
            _ptr = _ptr->next;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator &operator--()
        {
            _ptr = _ptr->prev;
            return *this;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const iterator &other) const { return _ptr == other._ptr; }
        bool operator!=(const iterator &other) const { return _ptr != other._ptr; }

        Node *node() const { return _ptr; }

    private:
        Node *_ptr;
    };

    List() = default;

    ~List()
    {
        clear();
    }

    void push_back(const T &value)
    {
        emplace_back(value);
    }

    void push_back(T &&value)
    {
        emplace_back(std::move(value));
    }

    void push_front(const T &value)
    {
        emplace_front(value);
    }

    void push_front(T &&value)
    {
        emplace_front(std::move(value));
    }

    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        Node *node = create_node();
        std::construct_at(node, std::forward<Args>(args)...);
        node->prev = _tail;
        node->next = nullptr;

        if (_tail)
            _tail->next = node;
        else
            _head = node;

        _tail = node;
        ++_size;
    }

    template<typename... Args>
    void emplace_front(Args&&... args)
    {
        Node *node = create_node();
        std::construct_at(node, std::forward<Args>(args)...);
        node->prev = nullptr;
        node->next = _head;

        if (_head)
            _head->prev = node;
        else
            _tail = node;

        _head = node;
        ++_size;
    }

    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args)
    {
        if (pos.node() == _head)
        {
            emplace_front(std::forward<Args>(args)...);
            return iterator(_head);
        }

        if (pos.node() == nullptr)
        {
            emplace_back(std::forward<Args>(args)...);
            return iterator(_tail);
        }

        Node *current = pos.node();
        Node *node = create_node();
        std::construct_at(node, std::forward<Args>(args)...);

        node->prev = current->prev;
        node->next = current;

        current->prev->next = node;
        current->prev = node;

        ++_size;
        return iterator(node);
    }

    void pop_back()
    {
        if (!_tail)
            return;

        Node *node = _tail;
        _tail = _tail->prev;

        if (_tail)
            _tail->next = nullptr;
        else
            _head = nullptr;

        destroy_node(node);
        --_size;
    }

    void pop_front()
    {
        if (!_head)
            return;

        Node *node = _head;
        _head = _head->next;

        if (_head)
            _head->prev = nullptr;
        else
            _tail = nullptr;

        destroy_node(node);
        --_size;
    }

    iterator insert(iterator pos, const T &value)
    {
        return emplace(pos, value);
    }

    iterator insert(iterator pos, T &&value)
    {
        return emplace(pos, std::move(value));
    }

    iterator erase(iterator pos)
    {
        Node *node = pos.node();
        if (!node)
            return end();

        Node *next = node->next;

        if (node->prev)
            node->prev->next = node->next;
        else
            _head = node->next;

        if (node->next)
            node->next->prev = node->prev;
        else
            _tail = node->prev;

        destroy_node(node);
        --_size;

        return iterator(next);
    }

    void resize(size_t count)
    {
        while (_size > count)
            pop_back();

        while (_size < count)
            emplace_back();
    }

    void clear()
    {
        Node *current = _head;
        while (current)
        {
            Node *next = current->next;
            destroy_node(current);
            current = next;
        }

        _head = nullptr;
        _tail = nullptr;
        _size = 0;
    }

    bool empty() const
    {
        return _size == 0;
    }

    size_t size() const
    {
        return _size;
    }

    T &front()
    {
        return _head->value;
    }

    const T &front() const
    {
        return _head->value;
    }

    T &back()
    {
        return _tail->value;
    }

    const T &back() const
    {
        return _tail->value;
    }

    iterator begin()
    {
        return iterator(_head);
    }

    iterator end()
    {
        return iterator(nullptr);
    }
};

MEMORIA_END