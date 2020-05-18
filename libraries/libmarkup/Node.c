#include <libmarkup/Markup.h>
#include <libsystem/cstring.h>

MarkupNode *markup_node_create(char *type)
{
    return markup_node_create_adopt(strdup(type));
}

MarkupNode *markup_node_create_adopt(char *type)
{
    MarkupNode *node = __create(MarkupNode);

    node->type = type;
    node->attributes = list_create();
    node->childs = list_create();

    return node;
}

void markup_node_destroy(MarkupNode *node)
{
    list_destroy_with_callback(node->childs, (ListDestroyElementCallback)markup_node_destroy);
    list_destroy_with_callback(node->attributes, (ListDestroyElementCallback)markup_attribute_destroy);
    free(node->type);
    free(node);
}

void markup_node_add_child(MarkupNode *parent, MarkupNode *child)
{
    list_pushback(parent->childs, child);
}

void markup_node_add_attribute(MarkupNode *node, MarkupAttribute *attribute)
{
    list_pushback(node->attributes, attribute);
}

const char *markup_node_get_attribute(MarkupNode *node, const char *name)
{
    list_foreach(MarkupAttribute, attribute, node->childs)
    {
        if (strcmp(attribute->name, name) == 0)
        {
            return attribute->value;
        }
    }

    return NULL;
}

const char *markup_node_get_attribute_or_default(MarkupNode *node, const char *name, const char *default_value)
{
    list_foreach(MarkupAttribute, attribute, node->attributes)
    {
        if (strcmp(attribute->name, name) == 0)
        {
            if (attribute->value)
            {
                return attribute->value;
            }
            else
            {
                return default_value;
            }
        }
    }

    return default_value;
}

const char *markup_node_type(MarkupNode *node)
{
    return node->type;
}

MarkupAttribute *markup_attribute_create(const char *name, const char *value)
{
    return markup_attribute_create_adopt(strdup(name), value ? strdup(value) : NULL);
}

MarkupAttribute *markup_attribute_create_adopt(char *name, char *value)
{
    MarkupAttribute *attribute = __create(MarkupAttribute);

    attribute->name = name;
    attribute->value = value;

    return attribute;
}

void markup_attribute_destroy(MarkupAttribute *attribute)
{
    free(attribute->name);

    if (attribute->value)
        free(attribute->value);

    free(attribute);
}
