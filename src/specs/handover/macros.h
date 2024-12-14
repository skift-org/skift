#pragma once

namespace Handover {

// clang-format off

#define HandoverSection$() \
    [[gnu::used, gnu::section(".handover")]]

#define HandoverRequests$(...)                          \
    HandoverSection$()                                  \
    static ::Handover::Request const __handover__[] = { \
        {::Handover::Tag::MAGIC, 0, 0},                 \
        __VA_ARGS__ __VA_OPT__(, )                      \
        {::Handover::Tag::END, 0, 0},                   \
    };

// clang-format on

} // namespace Handover
