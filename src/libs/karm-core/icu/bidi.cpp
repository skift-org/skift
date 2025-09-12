export module Karm.Core:icu.bidi;

import :base.base;
import :base.string;

namespace Karm::Icu {

// Implementation of unicode bidi algorithm
// https://unicode.org/reports/tr9/
// https://www.unicode.org/Public/PROGRAMS/BidiReferenceJava/BidiReference.java

// 3.2 MARK: Bidirectional Character Types -------------------------------------
// https://unicode.org/reports/tr9/#Bidirectional_Character_Types
export enum struct BidiType : u8 {
    L = 0,    //< Left-to-right
    LRE = 1,  //< Left-to-Right Embedding
    LRO = 2,  //< Left-to-Right Override
    R = 3,    //< Right-to-Left
    AL = 4,   //< Right-to-Left Arabic
    RLE = 5,  //< Right-to-Left Embedding
    RLO = 6,  //< Right-to-Left Override
    PDF = 7,  //< Pop Directional Format
    EN = 8,   //< European Number
    ES = 9,   //< European Number Separator
    ET = 10,  //< European Number Terminator
    AN = 11,  //< Arabic Number
    CS = 12,  //< Common Number Separator
    NSM = 13, //< Non-Spacing Mark
    BN = 14,  //< Boundary Neutral
    B = 15,   //< Paragraph Separator
    S = 16,   //< Segment Separator
    WS = 17,  //< Whitespace
    ON = 18,  //< Other Neutrals
    LRI = 19, //< Left-to-Right Isolate
    RLI = 20, //< Right-to-Left Isolate
    FSI = 21, //< First-Strong Isolate
    PDI = 22, //< Pop Directional Isolate

    _LEN
};

export struct Bidi {
    // MARK: Resolving Embedding Levels
    // https://unicode.org/reports/tr9/#Resolving_Embedding_Levels
    void run() {
        // Applying rule P1 to split the text into paragraphs, and for each of these:

        //   Applying rules P2 and P3 to determine the paragraph level.

        //   Applying rule X1 (which employs rules X2–X8) to determine explicit embedding levels and directions.

        //   Applying rule X9 to remove many control characters from further consideration.

        //   Applying rule X10 to split the paragraph into isolating run sequences and for each of these:

        //       Applying rules W1–W7 to resolve weak types.

        //       Applying rules N0–N2 to resolve neutral types.

        //       Applying rules I1–I2 to resolve implicit embedding levels.
    }
};

} // namespace Karm::Icu
