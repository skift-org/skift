#pragma once

#include <libutils/Assert.h>
#include <libutils/Iteration.h>
#include <libutils/Optional.h>
#include <libutils/Prelude.h>

namespace Utils
{

template <typename T>
struct List
{
private:
    struct Node
    {
        T value;

        Node *prev = nullptr;
        Node *next = nullptr;

        Node(const T &v) : value{v} {}
    };

    size_t _count = 0;
    Node *_head = nullptr;
    Node *_tail = nullptr;

public:
    bool empty() const
    {
        return _count == 0;
    }

    bool any() const
    {
        return _count > 0;
    }

    size_t count() const
    {
        return _count;
    }

    List() {}

    ~List()
    {
        clear();
    }

    List(const List &other)
    {
        other.foreach([this](const T &el) {
            push_back(el);
        });
    }

    List(List &&other) { *this = std::move(other); }

    List &operator=(const List &other)
    {
        if (this != &other)
        {
            clear();

            other.foreach([this](const T &el) {
                push_back(el);
            });
        }

        return *this;
    }

    List &operator=(List &&other)
    {
        if (this != &other)
        {
            std::swap(_count, other._count);
            std::swap(_head, other._head);
            std::swap(_tail, other._tail);
        }

        return *this;
    }

    void clear()
    {
        Node *current = _head;

        while (current)
        {
            Node *next = current->next;
            delete current;
            current = next;
        }

        _count = 0;
        _head = nullptr;
        _tail = nullptr;
    }

    void remove(const T &value)
    {
        Node *current = _head;

        while (current)
        {
            Node *next = current->next;

            if (current->value == value)
            {
                if (current->prev)
                {
                    current->prev->next = current->next;
                }
                else
                {
                    _head = current->next;
                }

                if (current->next)
                {
                    current->next->prev = current->prev;
                }
                else
                {
                    _tail = current->prev;
                }

                _count--;
                delete current;
                return;
            }

            current = next;
        }
    }

    Optional<T> requeue()
    {
        if (_count == 0)
        {
            return NONE;
        }

        if (_count == 1)
        {
            return _head->value;
        }

        Node *node = _head;
        _head = node->next;
        node->next->prev = nullptr;

        _tail->next = node;
        node->prev = _tail;
        node->next = nullptr;
        _tail = node;

        return node->value;
    }

    void push(const T &value)
    {
        Node *node = new Node{value};

        if (_head == nullptr)
        {
            _tail = node;
        }
        else
        {
            _head->prev = node;
            node->next = _head;
        }

        _count++;
        _head = node;
    }

    void push_back(const T &value)
    {
        Node *node = new Node{value};

        if (_tail == nullptr)
        {
            _head = node;
        }
        else
        {
            _tail->next = node;
            node->prev = _tail;
        }

        _count++;
        _tail = node;
    }

    T pop()
    {
        Assert::truth(any());

        Node *node = _head;

        if (_count == 1)
        {
            _head = nullptr;
            _tail = nullptr;
        }
        else if (_count > 1)
        {
            node->next->prev = nullptr;
            _head = node->next;
        }

        _count--;

        T value = std::move(node->value);
        delete node;
        return value;
    }

    T pop_back()
    {
        Assert::truth(any());

        Node *node = _tail;

        if (_count == 1)
        {
            _head = nullptr;
            _tail = nullptr;
        }
        else if (_count > 1)
        {
            node->prev->next = nullptr;
            _tail = node->prev;
        }

        _count--;

        T value = std::move(node->value);
        delete node;
        return value;
    }

    T &peek()
    {
        Assert::truth(any());
        return _head->value;
    }

    T &peek_back()
    {
        Assert::truth(any());
        return _tail->value;
    }

    bool contains(const T &value)
    {
        bool result = false;

        foreach([&](auto &el) {
            if (el == value)
            {
                result = true;
                return Iteration::STOP;
            }
            else
            {
                return Iteration::CONTINUE;
            }
        });

        return result;
    }

    template <typename TCallback>
    Iteration foreach(TCallback callback) const
    {
        Node *current = _head;

        while (current)
        {
            Node *next = current->next;

            if (callback(current->value) == Iteration::STOP)
            {
                return Iteration::STOP;
            }

            current = next;
        }

        return Iteration::CONTINUE;
    }

    template <typename TCallback>
    Iteration foreach_reversed(TCallback callback) const
    {
        Node *current = _tail;

        while (current)
        {
            Node *next = current->prev;

            if (callback(current->value) == Iteration::STOP)
            {
                return Iteration::STOP;
            }

            current = next;
        }
    }

    struct Iterator
    {
    private:
        Node *_node;

    public:
        Iterator(Node *node) : _node{node}
        {
        }

        Iterator operator++()
        {
            _node = _node->next;
            return *this;
        }

        bool operator!=(const Iterator &other) const
        {
            return _node != other._node;
        }

        const T &operator*() const
        {
            return _node->value;
        }
    };

    Iterator begin() const { return _head; }
    Iterator end() const { return nullptr; }
};

} // namespace Utils
