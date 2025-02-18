#pragma once

#include "character-data.h"

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#text
struct Text : public CharacterData {
    static constexpr auto TYPE = NodeType::TEXT;

    using CharacterData::CharacterData;

    NodeType nodeType() const override {
        return TYPE;
    }
};

} // namespace Vaev::Dom
