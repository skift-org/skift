#pragma once

#include "kernel/protocol.h"
#include "kernel/shared/syscalls.h"

#define DECL_SYSCALL0(fn) int fn()
#define DECL_SYSCALL1(fn, p1) int fn(p1)
#define DECL_SYSCALL2(fn, p1, p2) int fn(p1, p2)
#define DECL_SYSCALL3(fn, p1, p2, p3) int fn(p1, p2, p3)
#define DECL_SYSCALL4(fn, p1, p2, p3, p4) int fn(p1, p2, p3, p4)
#define DECL_SYSCALL5(fn, p1, p2, p3, p4, p5) int fn(p1, p2, p3, p4, p5)

#define DECL_SYSCALL0(fn) int fn()
#define DECL_SYSCALL1(fn, p1) int fn(p1)
#define DECL_SYSCALL2(fn, p1, p2) int fn(p1, p2)
#define DECL_SYSCALL3(fn, p1, p2, p3) int fn(p1, p2, p3)
#define DECL_SYSCALL4(fn, p1, p2, p3, p4) int fn(p1, p2, p3, p4)
#define DECL_SYSCALL5(fn, p1, p2, p3, p4, p5) int fn(p1, p2, p3, p4, p5)

#define DEFN_SYSCALL0(fn, num)                \
    int fn()                                  \
    {                                         \
        return __syscall(num, 0, 0, 0, 0, 0); \
    }

#define DEFN_SYSCALL1(fn, num, P1)                  \
    int fn(P1 p1)                                   \
    {                                               \
        return __syscall(num, (int)p1, 0, 0, 0, 0); \
    }

#define DEFN_SYSCALL2(fn, num, P1, P2)                    \
    int fn(P1 p1, P2 p2)                                  \
    {                                                     \
        return __syscall(num, (int)p1, (int)p2, 0, 0, 0); \
    }

#define DEFN_SYSCALL3(fn, num, P1, P2, P3)                      \
    int fn(P1 p1, P2 p2, P3 p3)                                 \
    {                                                           \
        return __syscall(num, (int)p1, (int)p2, (int)p3, 0, 0); \
    }

#define DEFN_SYSCALL4(fn, num, P1, P2, P3, P4)                        \
    int fn(P1 p1, P2 p2, P3 p3, P4 p4)                                \
    {                                                                 \
        return __syscall(num, (int)p1, (int)p2, (int)p3, (int)p4, 0); \
    }

#define DEFN_SYSCALL5(fn, num, P1, P2, P3, P4, P5)                          \
    int fn(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)                               \
    {                                                                       \
        return __syscall(num, (int)p1, (int)p2, (int)p3, (int)p4, (int)p5); \
    }
