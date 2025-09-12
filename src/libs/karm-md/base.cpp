export module Karm.Md:base;

import Karm.Core;

namespace Karm::Md {

export struct Node;

export struct Document {
    Vec<Node> children;

    void repr(Io::Emit& e) const {
        e("(document {})", children);
    }
};

using Paragraph = Vec<Node>;

export struct Heading {
    usize level;
    Paragraph children;

    void repr(Io::Emit& e) const {
        e("(h{} {})", level, children);
    }
};

export struct Code {
    String text;

    void repr(Io::Emit& e) const {
        e("(code {#})", text);
    }
};

export struct Hr {
    void repr(Io::Emit& e) const {
        e("(hr)");
    }
};

using _Node = Union<
    Paragraph,
    Heading,
    Code,
    Hr,
    String>;

export struct Node : _Node {
    using _Node::_Node;

    void repr(Io::Emit& e) const {
        visit(Visitor{
            [&](String const& n) {
                e("{#}", n);
            },
            [&](auto const& n) {
                e("{}", n);
            },
        });
    }
};

} // namespace Karm::Md