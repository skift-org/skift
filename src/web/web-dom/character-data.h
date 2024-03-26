#pragma once

#include "node.h"

namespace Web::Dom {

struct CharacterData : public Dom::Node {
    String data;

    CharacterData(String data)
        : data(data) {
    }

    void appendData(String const &data) {
        // HACK: This is not efficient and pretty slow,
        //       but it's good enough for now.
        StringBuilder sb;
        sb.append(this->data);
        sb.append(data);
        this->data = sb.take();
    }

    void appendData(Rune rune) {
        // HACK: This is not efficient and pretty slow,
        //       but it's good enough for now.
        StringBuilder sb;
        sb.append(this->data);
        sb.append(rune);
        this->data = sb.take();
    }

    void _dump(Io::Emit &e) override {
        e(" data={#}", this->data);
    }
};

} // namespace Web::Dom
