#pragma once

#include "cons.h"
#include "iter.h"
#include "opt.h"
#include "rc.h"
#include "std.h"

namespace Karm {

template <typename T>
concept LlNode = requires(T t) {
    { t.next } -> Meta::Convertible<T *>;
    { t.prev } -> Meta::Convertible<T *>;
};

// Base linked list implementation that allows low-level manipulation of nodes.
// aka intrusive linked list
template <LlNode Node>
struct Ll {
    Node *head = nullptr;
    Node *tail = nullptr;
    usize len = 0;

    Ll() = default;

    Ll(Ll const &other) {
        if (other.len == 0)
            return;

        head = new Node(*other.head);
        tail = head;

        auto *node = other.head->next;
        while (node) {
            append(new Node(*node), tail);
            node = node->next;
        }
    }

    Ll(Ll &&other)
        : head(std::exchange(other.head, nullptr)),
          tail(std::exchange(other.tail, nullptr)),
          len(std::exchange(other.len, 0)) {}

    ~Ll() {
        clear();
    }

    Ll &operator=(Ll const &other) {
        *this = Ll(other);
        return *this;
    }

    Ll &operator=(Ll &&other) {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(len, other.len);
        return *this;
    }

    Node *detach(Node *node) {
        if (node->prev)
            node->prev->next = node->next;

        if (node->next)
            node->next->prev = node->prev;

        if (node == head)
            head = node->next;

        if (node == tail)
            tail = node->prev;

        node->prev = nullptr;
        node->next = nullptr;
        len--;
        return node;
    }

    Node *append(Node *node, Node *after) {
        if (node->next or node->prev)
            panic("node already in list");

        if (after) {
            node->next = after->next;
            node->prev = after;
            after->next = node;
            if (node->next)
                node->next->prev = node;
            if (after == tail)
                tail = node;
        } else {
            if (tail) {
                tail->next = node;
                node->prev = tail;
                tail = node;
            } else {
                head = node;
                tail = node;
            }
        }
        len++;
        return node;
    }

    Node *prepend(Node *node, Node *before) {
        if (node->next or node->prev)
            panic("node already in list");

        if (before) {
            node->next = before;
            node->prev = before->prev;
            before->prev = node;
            if (node->prev)
                node->prev->next = node;
            if (before == head)
                head = node;
        } else {
            if (head) {
                head->prev = node;
                node->next = head;
                head = node;
            } else {
                head = node;
                tail = node;
            }
        }
        len++;
        return node;
    }

    void trunc(usize len) {
        if (len >= this->len) {
            return;
        }

        if (len == 0) {
            clear();
            return;
        }

        while (this->len > len) {
            auto node = tail;
            detach(tail);
            delete node;
        }
    }

    void clear() {
        auto *node = head;
        while (node) {
            auto *next = node->next;
            delete node;
            node = next;
        }
        head = nullptr;
        tail = nullptr;
        len = 0;
    }

    Node *peek(usize i) {
        if (i >= len)
            panic("index out of range");

        if (i < len / 2) {
            auto *node = head;
            for (usize j = 0; j < i; j++)
                node = node->next;
            return node;
        }

        auto *node = tail;
        for (usize j = len - 1; j > i; j--)
            node = node->prev;
        return node;
    }

    Node const *peek(usize i) const {
        if (i >= len)
            panic("index out of range");

        if (i < len / 2) {
            auto *node = head;
            for (usize j = 0; j < i; j++)
                node = node->next;
            return node;
        }

        auto *node = tail;
        for (usize j = len - 1; j > i; j--)
            node = node->prev;
        return node;
    }
};

template <typename T>
struct List {
    struct Node {
        T value;
        Node *next = nullptr;
        Node *prev = nullptr;
    };

    Ll<Node> _ll;

    constexpr List() = default;

    ~List() {
        clear();
    }

    /* --- Indexing --- */

    void insert(usize i, T el) {
        emplace(i, std::move(el));
    }

    template <typename... Args>
    void emplace(usize i, Args &&...args) {
        if (i > _ll.len)
            panic("index out of range");
        if (i == _ll.len) {
            _ll.append(new Node{std::forward<Args>(args)...}, _ll.tail);
        } else {
            _ll.prepend(new Node{std::forward<Args>(args)...}, _ll.peek(i));
        }
    }

    void removeAt(usize i) {
        if (i >= _ll.len)
            panic("index out of range");
        delete _ll.detach(_ll.peek(i));
    }

    void remove(T const &elem) {
        auto *node = _ll.head;
        while (node) {
            if (node->value == elem) {
                delete _ll.detach(node);
                return;
            }
            node = node->next;
        }
    }

    void trunc(usize len) {
        _ll.trunc(len);
    }

    void clear() {
        _ll.clear();
    }

    /* --- Random Access --- */

    T &peek(usize i) {
        return _ll.peek(i)->value;
    }

    T const &peek(usize i) const {
        return _ll.peek(i)->value;
    }

    T &operator[](usize i) {
        return peek(i);
    }

    T const &operator[](usize i) const {
        return peek(i);
    }

    /* -- Front Access --- */

    void pushFront(T const &value) {
        pushFront(T{value});
    }

    void pushFront(T &&value) {
        emplaceFront(std::move(value));
    }

    template <typename... Args>
    void emplaceFront(Args &&...args) {
        emplace(0, std::forward<Args>(args)...);
    }

    T popFront() {
        T buf = std::move(_ll.head->buf);
        delete _ll.detach(_ll.head);
        return buf;
    }

    /* --- Back Access --- */

    void pushBack(T const &value) {
        pushBack(T{value});
    }

    void pushBack(T &&value) {
        emplaceBack(std::move(value));
    }

    template <typename... Args>
    void emplaceBack(Args &&...args) {
        emplace(_ll.len, std::forward<Args>(args)...);
    }

    T popBack() {
        auto value = std::move(_ll.tail->value);
        delete _ll.detach(_ll.tail);
        return value;
    }

    /* --- Queue --- */

    bool requeue() {
        if (_ll.len < 2)
            return false;
        auto *node = _ll.detach(_ll.head);
        _ll.append(node, _ll.tail);
        return true;
    }

    /* --- Iteration --- */

    template <typename Self>
    static auto _iter(Self *self) {
        return Iter([curr = self->_ll.head]() mutable -> T * {
            if (curr) {
                auto &ret = curr->value;
                curr = curr->next;
                return &ret;
            }
            return nullptr;
        });
    }

    template <typename Self>
    static auto _iterRev(Self *self) {
        return Iter([curr = self->_ll.tail]() mutable -> T * {
            if (curr) {
                auto &ret = curr->value;
                curr = curr->prev;
                return &ret;
            }
            return nullptr;
        });
    }

    constexpr auto iter() {
        return _iter(this);
    }

    constexpr auto iter() const {
        return _iter(this);
    }

    constexpr auto iterRev() {
        return _iterRev(this);
    }

    constexpr auto iterRev() const {
        return _iterRev(this);
    }

    /* --- Len & Buf --- */

    usize len() const {
        return _ll.len;
    }
};

} // namespace Karm
