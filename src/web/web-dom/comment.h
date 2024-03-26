#pragma once

#include "character-data.h"

namespace Web::Dom {

struct Comment : public CharacterData {
    using CharacterData::CharacterData;

    NodeType nodeType() override {
        return NodeType::COMMENT;
    }
};

} // namespace Web::Dom
