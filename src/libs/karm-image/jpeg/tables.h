#pragma once

#include "base.h"

namespace Jpeg {

// MARK: Quantization ----------------------------------------------------------

extern Quant const QUANT_Y50, QUANT_CbCr50;

extern Quant const QUANT_Y75, QUANT_CbCr75;

extern Quant const QUANT_Y100, QUANT_CbCr100;

extern Array<Quant const *, 3> const QUANT50, QUANT75, QUANT100;

// MARK: Huffman Tables --------------------------------------------------------

extern Huff HUFF_DC_Y;

extern Huff HUFF_DC_CbCr;

extern Huff HUFF_AC_Y;

extern Huff HUFF_AC_CbCr;

extern Array<Huff *, 3> const HUFF_DC;
extern Array<Huff *, 3> const HUFF_AC;

} // namespace Jpeg
