#include <libio/File.h>
#include <libxml/Parser.h>

#include "tests/Driver.h"

TEST(xml_basic_test)
{
    IO::File file("/Files/Tests/xml/basic.xml", OPEN_READ);
    Assert::is_true(file.exist());
    auto result = Xml::parse(file);
    Assert::equal(result.result(), Result::SUCCESS);

    Xml::Document doc = result.unwrap();

    // Check the root
    Assert::equal(doc.root().name(), "Root");
    Assert::equal(doc.root().attributes().count(), 2);
    Assert::equal(doc.root().children().count(), 4);

    // Check the testid attribute
    Assert::is_true(doc.root().attributes().has_key("TestAttr"));
    Assert::equal(doc.root().attributes()["TestAttr"], "0001");

    // Check the name child
    auto &child = doc.root().children()[0];
    Assert::equal(child.name(), "Name");
    Assert::equal(child.content(), "Convert number to string");
}

TEST(xml_empty_tags_test)
{
    IO::File file("/Files/Tests/xml/empty_tags.xml", OPEN_READ);
    Assert::is_true(file.exist());
    auto result = Xml::parse(file);
    Assert::equal(result.result(), Result::SUCCESS);

    Xml::Document doc = result.unwrap();

    // Check the root
    Assert::equal(doc.root().name(), "Root");
    Assert::equal(doc.root().attributes().count(), 0);
    Assert::equal(doc.root().children().count(), 1);

    // Check the empty tag child
    auto &child = doc.root().children()[0];
    Assert::equal(child.name(), "EmptyTag");
    Assert::is_true(child.content().empty());

    // Check the testid attribute
    Assert::is_true(child.attributes().has_key("TestAttr"));
    Assert::equal(child.attributes()["TestAttr"], "0001");
}