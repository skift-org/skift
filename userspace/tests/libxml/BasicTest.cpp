#include <libio/File.h>
#include <libxml/Parser.h>

#include "tests/Driver.h"

TEST(xml_valid)
{
    IO::File file("/Files/Tests/xml/basic.xml", OPEN_READ);
    Assert::equal(file.exist(), true);
    auto result = Xml::parse(file);
    Assert::equal(result.result(), Result::SUCCESS);

    Xml::Document doc = result.unwrap();

    // Check the root
    Assert::equal(doc.root().name(), "Test");
    Assert::equal(doc.root().attributes().count(), 2);
    Assert::equal(doc.root().children().count(), 4);

    // Check the testid attribute
    Assert::is_true(doc.root().attributes().has_key("TestId"));
    Assert::equal(doc.root().attributes()["TestId"], "0001");

    // Check the name child
    auto &child = doc.root().children()[0];
    Assert::equal(child.name(), "Name");
    Assert::equal(child.content(), "Convert number to string");
}