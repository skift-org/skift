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
    Xml::Document doc;
    Assert::equal(Xml::parse(reader).result(), Result::SUCCESS);
}