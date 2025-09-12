module;

#include <karm-logger/logger.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

export module Karm.Ml:gguf;

import Karm.Core;
import :vocab;

namespace Karm::Ml::Gguf {

enum struct Type : u32 {
    F32 = 0,
    F16 = 1,
    Q4_0 = 2,
    Q4_1 = 3,
    Q4_2 = 4, // deprecated
    Q4_3 = 5, // deprecated
    Q5_0 = 6,
    Q5_1 = 7,
    Q8_0 = 8,
    Q8_1 = 9,
    Q2_K = 10,
    Q3_K = 11,
    Q4_K = 12,
    Q5_K = 13,
    Q6_K = 14,
    Q8_K = 15,
    IQ2_XXS = 16,
    IQ2_XS = 17,
    IQ3_XXS = 18,
    IQ1_S = 19,
    IQ4_NL = 20,
    IQ3_S = 21,
    IQ2_S = 22,
    IQ4_XS = 23,
    I8 = 24,
    I16 = 25,
    I32 = 26,
    I64 = 27,
    F64 = 28,
    IQ1_M = 29,

    _LEN,
};

enum struct ValueType : u32 {
    UINT8 = 0,
    INT8 = 1,
    UINT16 = 2,
    INT16 = 3,
    UINT32 = 4,
    INT32 = 5,
    FLOAT32 = 6,
    BOOL = 7,
    STRING = 8,
    ARRAY = 9,
    UINT64 = 10,
    INT64 = 11,
    FLOAT64 = 12,

    _LEN,
};

struct [[gnu::packed]] Header {
    static constexpr u32 MAGIC = 0x46554747;
    u32le magic; // "GGUF"
    u32le version;
    u64le tensorCount;
    u64le metadataCount;
};

// MARK: Metadata --------------------------------------------------------------

Serde::Value _loadMetadataValue(Io::BScan& s, ValueType type);

Str _loadMetadataString(Io::BScan& s) {
    auto len = s.nextU64le();
    return s.nextStr(len);
}

Serde::Value _loadMetaDataArray(Io::BScan& s) {
    Serde::Array res;
    auto type = static_cast<ValueType>(s.nextU32le());
    auto len = s.nextU64le();
    for (usize _ : range(len)) {
        res.pushBack(_loadMetadataValue(s, type));
    }
    return res;
}

Serde::Value _loadMetadataValue(Io::BScan& s, ValueType type) {
    switch (type) {
    case ValueType::BOOL:
        return static_cast<bool>(s.nextU8le());

    case ValueType::UINT8:
        return static_cast<Serde::Integer>(s.nextU8le());
    case ValueType::INT8:
        return static_cast<Serde::Integer>(s.nextI8le());

    case ValueType::UINT16:
        return static_cast<Serde::Integer>(s.nextU16le());
    case ValueType::INT16:
        return static_cast<Serde::Integer>(s.nextI16le());

    case ValueType::UINT32:
        return static_cast<Serde::Integer>(s.nextU32le());
    case ValueType::INT32:
        return static_cast<Serde::Integer>(s.nextI32le());

    case ValueType::UINT64:
        return static_cast<Serde::Integer>(s.nextU64le());

    case ValueType::INT64:
        return static_cast<Serde::Integer>(s.nextI64le());

    case ValueType::FLOAT32:
        return s.next<f32>();

    case ValueType::FLOAT64:
        return s.next<f32>();

    case ValueType::STRING:
        return String{_loadMetadataString(s)};

    case ValueType::ARRAY:
        return _loadMetaDataArray(s);

    default:
        panic("unknow value type");
    }
}

Res<Tuple<Str, Serde::Value>> _loadMetadataRecord(Io::BScan& s) {
    auto key = _loadMetadataString(s);
    auto type = static_cast<ValueType>(s.nextU32le());
    return Ok(Tuple<Str, Serde::Value>{key, _loadMetadataValue(s, type)});
}

Res<Serde::Value> _loadMetadata(Io::BScan& s, usize len) {
    Serde::Object object;
    for (usize _ : range(len)) {
        auto [key, value] = try$(_loadMetadataRecord(s));
        object.put(key, std::move(value));
    }
    return Ok(std::move(object));
}

// MARK: Vocab -----------------------------------------------------------------

Res<BpeVocab> _loadVocab(Serde::Value& metadata) {
    if (metadata.get("tokenizer.ggml.model").asStr() != "gpt2"s)
        return Error::invalidData("unsupported tokenizer model");

    auto tokens = metadata.get("tokenizer.ggml.tokens");
    auto types = metadata.get("tokenizer.ggml.token_type");
    auto merges = metadata.get("tokenizer.ggml.merges");

    BpeVocab vocab;

    for (auto i : range(tokens.len())) {
        vocab._infos.pushBack({
            .type = static_cast<BpeVocab::TokenInfos::Type>(types.get(i).asInt()),
            .text = tokens.get(i).asStr(),
            .token = static_cast<BpeVocab::Token>(i),
        });
    }

    return Ok(std::move(vocab));
}

// MARK: Tensors ---------------------------------------------------------------

struct TensorInfos {
    String name;
    Vec<u64> dims;
    Type type;
    u64 offset;
};

TensorInfos _loadTensor(Io::BScan& s) {
    TensorInfos infos;
    infos.name = _loadMetadataString(s);
    auto dimsLen = s.nextU32le();
    Vec<u64> dims;
    for (usize _ : range(dimsLen)) {
        infos.dims.pushBack(s.nextU64le());
    }
    infos.type = static_cast<Type>(s.nextU32le());
    infos.offset = s.nextU64le();

    return infos;
}

Res<Vec<TensorInfos>> _loadTensors(Io::BScan& s, usize len) {
    Vec<TensorInfos> res;
    for (usize _ : range(len))
        res.pushBack(_loadTensor(s));
    return Ok(std::move(res));
}

export Res<> loadGguf(Ref::Url url) {
    yap("loading gguf file from {}", url);
    auto file = try$(Sys::File::open(url));
    auto mmap = try$(Sys::mmap().map(file));

    yap("mmap'ed model file of {}mib", mmap.bytes().len() / mib(1));
    Io::BScan s{mmap.bytes()};
    auto header = s.next<Header>();

    if (header.magic != Header::MAGIC)
        return Error::invalidData("not a gguf file");

    yap("header:");
    yap(" - magic:{:x}", header.magic);
    yap(" - version:{}", header.version);
    yap(" - tensorCount:{}", header.tensorCount);
    yap(" - metadataCount:{}", header.metadataCount);

    auto metadata = try$(_loadMetadata(s, header.metadataCount));

    yap("metadata:");
    yap(" - general.architecture: {}", metadata.get("general.architecture"));
    yap(" - general.type: {}", metadata.get("general.type"));
    yap(" - general.license: {}", metadata.get("general.license"));
    yap(" - tokenizer.ggml.model: {}", metadata.get("tokenizer.ggml.model"));

    auto vocab = try$(_loadVocab(metadata));
    yap("loaded {} tokens", vocab._infos.len());

    auto tensors = try$(_loadTensors(s, header.tensorCount));
    yap("loaded {} tensors", tensors.len());

    for (u8 r : range(255)) {
        yap("{} -> {}", r, BpeVocab::_u8ToUnicode(r));
    }

    auto input = "Hello, world!"s;
    auto tokens = vocab.encode(input);
    auto string = vocab.decode(tokens);

    yap("input: {#} tokens: {#} string: {#}", input, tokens, string);

    return Ok();
}

} // namespace Karm::Ml::Gguf
