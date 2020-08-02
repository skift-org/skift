#pragma once

#include <libgraphic/Painter.h>

struct PaintTool;

struct PaintDocument
{
    bool dirty;

    Color primary_color;
    Color secondary_color;

    RefPtr<Bitmap> bitmap;
    Painter painter;

    struct PaintTool *tool;
};

PaintDocument *paint_document_create(int width, int height, Color fill_color);

void paint_document_destroy(PaintDocument *document);

void paint_document_set_tool(PaintDocument *document, struct PaintTool *tool);
