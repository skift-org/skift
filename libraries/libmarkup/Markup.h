#pragma once

#include <libsystem/utils/List.h>

typedef struct
{
    char *name;
    char *value;
} MarkupAttribute;

typedef struct
{
    char *type;
    List *childs;
    List *attributes;
} MarkupNode;

MarkupNode *markup_node_create(char *type);

MarkupNode *markup_node_create_adopt(char *type);

void markup_node_destroy(MarkupNode *node);

bool markup_node_is(MarkupNode *node, const char *type);

void markup_node_add_child(MarkupNode *parent, MarkupNode *child);

void markup_node_add_attribute(MarkupNode *node, MarkupAttribute *attribute);

const char *markup_node_get_attribute(MarkupNode *node, const char *name);

const char *markup_node_get_attribute_or_default(MarkupNode *node, const char *name, const char *default_value);

bool markup_node_has_attribute(MarkupNode *node, const char *name);

const char *markup_node_type(MarkupNode *node);

MarkupAttribute *markup_attribute_create(const char *name, const char *value);

MarkupAttribute *markup_attribute_create_adopt(char *name, char *value);

void markup_attribute_destroy(MarkupAttribute *attribute);

/* --- Serialization and Deserialization ------------------------------------ */

char *
markup_stringify(MarkupNode *root);

char *markup_prettify(MarkupNode *root);

MarkupNode *markup_parse(const char *string, size_t size);

MarkupNode *markup_parse_file(const char *path);
