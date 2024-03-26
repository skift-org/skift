#pragma once

#include "character-data.h"

namespace Web::Dom {

// https://dom.spec.whatwg.org/#interface-comment
struct Comment : public CharacterData {
    using CharacterData::CharacterData;

    NodeType nodeType() const override {
        return NodeType::COMMENT;
    }
};

} // namespace Web::Dom
