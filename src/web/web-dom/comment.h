#pragma once

#include "character-data.h"

namespace Web::Dom {

// https://dom.spec.whatwg.org/#interface-comment
struct Comment : public CharacterData {
    using CharacterData::CharacterData;

    static constexpr auto TYPE = NodeType::COMMENT;

    NodeType nodeType() const override {
        return TYPE;
    }
};

} // namespace Web::Dom
