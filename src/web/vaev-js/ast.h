#pragma once

#include <karm-base/box.h>
#include <karm-base/union.h>
#include <karm-base/vec.h>

namespace Vaev::Js {

struct Expr;

struct Stmt;

struct Decl;

// MARK: Expressions -----------------------------------------------------------

struct Ident {};

struct Infix {
    enum struct Op {};

    using enum Op;

    Op _op;
    Box<Expr> lhs;
    Box<Expr> rhs;
};

struct Affix {
    enum struct Op {};

    using enum Op;

    Op _op;
    Box<Expr> arg;
};

using _Expr = Union<Ident, Infix, Affix>;

struct Expr : public _Expr {
    using _Expr::_Expr;
};

// MARK: Statements ------------------------------------------------------------

struct Block {
    Vec<Stmt> _stmts;
};

using _Stmt = Union<
    Expr,
    Block>;

struct Stmt : public _Stmt {
    using _Stmt::_Stmt;
};

// MARK: Declaration -----------------------------------------------------------

struct Decl {};

} // namespace Vaev::Js
