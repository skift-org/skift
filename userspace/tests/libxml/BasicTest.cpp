#include <libxml/Parser.h>

#include "tests/Driver.h"

TEST(xml_valid)
{
    String xml_string =
        R"(<Test TestId="0001" TestType="CMD">
  <Name>Convert number to string</Name>
  <CommandLine>Examp1.EXE</CommandLine>
  <Input>1</Input>
  <Output>One</Output>
</Test>)";

    IO::MemoryReader reader(xml_string);
    auto result = Xml::parse(reader);
    Assert::equal(result.result(), Result::SUCCESS);

    Xml::Document doc = result.unwrap();
    Assert::equal(doc.root().name(), "Test");
    Assert::equal(doc.root().children().count(), 4);
}