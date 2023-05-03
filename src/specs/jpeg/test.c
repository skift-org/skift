
/* Build IDCT matrix */
static struct idct *build_matrix(struct idct *i, struct stream *st, int idx, uint8_t *quant, int oldcoeff, int *outcoeff) {
    memset(i, 0, sizeof(struct idct));

    int code = get_code(&huffman_tables[idx], st);
    int bits = get_bitn(st, code);
    int dccoeff = decode(code, bits) + oldcoeff;

    add_zigzag(i, 0, dccoeff * quant[0]);
    int l = 1;

    while (l < 64) {
        code = get_code(&huffman_tables[16 + idx], st);
        if (code == 0)
            break;
        if (code > 15) {
            l += (code >> 4);
            code = code & 0xF;
        }
        bits = get_bitn(st, code);
        int coeff = decode(code, bits);
        add_zigzag(i, l, coeff * quant[l]);
        l += 1;
    }

    *outcoeff = dccoeff;
    return i;
}

/* Concvert YCbCr values to RGB pixels */
static void draw_matrix(int x, int y, struct idct *L, struct idct *cb, struct idct *cr) {
    for (int yy = 0; yy < 8; ++yy) {
        for (int xx = 0; xx < 8; ++xx) {
            int o = xy_to_lin(xx, yy);
            int r, g, b;
            color_conversion(L->base[o], cb->base[o], cr->base[o], &r, &g, &b);
            uint32_t c = 0xFF000000 | (r << 16) | (g << 8) | b;
            set_pixel((x * 8 + xx), (y * 8 + yy), c);
        }
    }
}

static void start_of_scan(FILE *f, int len) {

    TRACE("Reading image data");

    /* Skip header */
    fseek(f, len, SEEK_CUR);

    /* Initialize bit stream */
    struct stream _st = {0};
    _st.file = f;
    struct stream *st = &_st;

    int old_lum = 0;
    int old_crd = 0;
    int old_cbd = 0;
    for (int y = 0; y < sprite->height / 8 + !!(sprite->height & 0x7); ++y) {
        TRACE("Star row %d", y);
        for (int x = 0; x < sprite->width / 8 + !!(sprite->width & 0x7); ++x) {
            if (y >= 134) {
                TRACE("Start col %d", x);
            }

            /* Build matrices */
            struct idct matL, matCr, matCb;
            build_matrix(&matL, st, 0, quant[quant_mapping[0]], old_lum, &old_lum);
            build_matrix(&matCb, st, 1, quant[quant_mapping[1]], old_cbd, &old_cbd);
            build_matrix(&matCr, st, 1, quant[quant_mapping[2]], old_crd, &old_crd);

            if (y >= 134) {
                TRACE("Draw col %d", x);
            }
            draw_matrix(x, y, &matL, &matCb, &matCr);
        }
    }

    TRACE("Done.");
}
