#include "gtest/gtest.h"

#include "inip/inip.hpp"

TEST(TestOddFormat, OddFormatsTest)
{
  const std::string inictx = R"(
[Section]
; You can use \ to escape
Key\=With\;Equal \== Value [With] Any Th\#\;ing \\
Key = This is a \\\
   very long \
String
Key2 \==\=
  )";

  inip::err::ErrList errlist;
  inip::ini ini("");
  auto result = ini.load_str(inictx, errlist, true);

  EXPECT_TRUE(result.valid());

  EXPECT_EQ("Value [With] Any Th#;ing \\\\", ini.get("Section", "Key=With;Equal ="));
  EXPECT_EQ("This is a \\\\   very long String", ini.get("Section", "Key"));
  EXPECT_EQ("=", ini.get("Section", "Key2 ="));
}
