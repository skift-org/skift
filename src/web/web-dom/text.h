#pragma once

#include "character-data.h"

namespace Web::Dom {

// https://dom.spec.whatwg.org/#text
struct Text : public CharacterData {
    using CharacterData::CharacterData;

    NodeType nodeType() const override {
        return NodeType::TEXT;
    }
};

} // namespace Web::Dom
