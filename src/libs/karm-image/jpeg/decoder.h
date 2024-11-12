#pragma once

// JPEG Image decoder
// Based on:
//  - https://github.com/klange/toaruos/blob/6a400b399ca35147f89e6bbec017716d1897ea12/lib/jpeg.c
//  - https://en.wikipedia.org/wiki/JPEG_File_Interchange_Format
//  - https://www.w3.org/Graphics/JPEG/itu-t81.pdf
//  - https://github.com/dannye/jed/blob/master/src/decoder.cpp
//  - https://www.youtube.com/watch?v=CPT4FSkFUgs

#include <karm-base/vec.h>
#include <karm-gfx/buffer.h>
#include <karm-gfx/colors.h>
#include <karm-io/bscan.h>
#include <karm-io/emit.h>
#include <karm-logger/logger.h>

#include "base.h"

namespace Jpeg {

// MARK: Decoder ---------------------------------------------------------------

struct Decoder {
    static bool sniff(Bytes slice);

    static Res<Decoder> init(Bytes slice);

    void skipMarker(Io::BScan &s);

    // MARK: Quantization Tables -----------------------------------------------

    Array<Opt<Quant>, 4> _quant;
    bool _quirkZeroBased = false;

    Res<> defineQuantizationTable(Io::BScan &x);

    // MARK: Start of frame ----------------------------------------------------

    isize _width = 8;
    isize _height = 8;

    isize width() const { return _width; }

    isize height() const { return _height; }

    isize mcuWidth() const { return (_width + 7) / 8; }

    isize mcuHeight() const { return (_height + 7) / 8; }

    struct Component {
        u8 hFactor;
        u8 vFactor;
        u8 quantId;
    };

    Array<Opt<Component>, 4> _components;
    usize _componentCount = 0;

    Res<> startOfFrame(Io::BScan &x);

    // MARK: Restart interval --------------------------------------------------

    usize _restartInterval = 0;

    Res<> defineRestartInterval(Io::BScan &x);

    // MARK: Huffman Tables ----------------------------------------------------

    Array<Opt<Huff>, 4> _dcHuff;
    Array<Opt<Huff>, 4> _acHuff;

    Res<> defineHuffmanTable(Io::BScan &x);

    // MARK: Start of scan -----------------------------------------------------

    struct ScanComponent {
        u8 dcHuffId;
        u8 acHuffId;
    };

    Array<Opt<ScanComponent>, 4> _scanComponents;
    u8 _ss = 0;
    u8 _se = 0;
    u8 _ah = 0;
    u8 _al = 0;

    Res<> startOfScan(Io::BScan &x);

    // MARK: Huffman Data ------------------------------------------------------

    Vec<Mcu> _mcus;

    Res<> decodeHuffman(Io::BScan &s);

    // MARK: Decoding ----------------------------------------------------------

    Res<> decode(Gfx::MutPixels pixels);

    // MARK: Dumping -----------------------------------------------------------

    void repr(Io::Emit &e);
};

} // namespace Jpeg
