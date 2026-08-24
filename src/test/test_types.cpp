#include <gtest/gtest.h>

#include <climits>

#include "inip/Exception.hpp"
#include "inip/Types.hpp"

TEST(TypesTest, Bool2StrTest)
{
  EXPECT_EQ(::inip::Types::bool2str(true), "true");
  EXPECT_EQ(::inip::Types::bool2str(false), "false");
}

TEST(TypesTest, Str2BoolTest)
{
  EXPECT_TRUE(::inip::Types::str2bool("true"));
  EXPECT_TRUE(::inip::Types::str2bool("True"));
  EXPECT_TRUE(::inip::Types::str2bool("TRUE"));
  EXPECT_TRUE(::inip::Types::str2bool("1"));
  EXPECT_TRUE(::inip::Types::str2bool("yes"));
  EXPECT_TRUE(::inip::Types::str2bool("Yes"));
  EXPECT_TRUE(::inip::Types::str2bool("YES"));
  EXPECT_TRUE(::inip::Types::str2bool("on"));
  EXPECT_TRUE(::inip::Types::str2bool("On"));
  EXPECT_TRUE(::inip::Types::str2bool("ON"));

  EXPECT_FALSE(::inip::Types::str2bool("false"));
  EXPECT_FALSE(::inip::Types::str2bool("False"));
  EXPECT_FALSE(::inip::Types::str2bool("FALSE"));
  EXPECT_FALSE(::inip::Types::str2bool("0"));
  EXPECT_FALSE(::inip::Types::str2bool("no"));
  EXPECT_FALSE(::inip::Types::str2bool("No"));
  EXPECT_FALSE(::inip::Types::str2bool("NO"));
  EXPECT_FALSE(::inip::Types::str2bool("off"));
  EXPECT_FALSE(::inip::Types::str2bool("Off"));
  EXPECT_FALSE(::inip::Types::str2bool("OFF"));

  EXPECT_THROW(::inip::Types::str2bool("invalid"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2bool(""), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2bool("maybe"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2bool("2"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2bool("-1"), ::inip::err::Errors);
}

TEST(TypesTest, Str2IntTest)
{
  EXPECT_EQ(::inip::Types::str2int("0"), 0);
  EXPECT_EQ(::inip::Types::str2int("42"), 42);
  EXPECT_EQ(::inip::Types::str2int("-42"), -42);
  EXPECT_EQ(::inip::Types::str2int("+100"), 100);
  EXPECT_EQ(::inip::Types::str2int("2147483647"), 2147483647);
  EXPECT_EQ(::inip::Types::str2int("-2147483648"), -2147483648);
  EXPECT_EQ(::inip::Types::str2int("123abc"), 123);

  EXPECT_THROW(::inip::Types::str2int(""), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2int("abc"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2int("abc123"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2int("not_a_number"), ::inip::err::Errors);
}

TEST(TypesTest, Str2UIntTest)
{
  EXPECT_EQ(::inip::Types::str2uint("0"), 0);
  EXPECT_EQ(::inip::Types::str2uint("42"), 42);

  // -1 throw exception INVALID_VALUE
  // uint required stoul, -1 will return std::numeric_limits<unsigned long>::max()
  // if large than std::numeric_limits<unsigned int>::max(), ,  throw exception
  EXPECT_THROW(::inip::Types::str2uint("-1"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2uint("114.514"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2uint("invalid"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2uint(""), ::inip::err::Errors);
}

TEST(TypesTest, Str2LongTest)
{
  EXPECT_EQ(::inip::Types::str2long("0"), 0L);
  EXPECT_EQ(::inip::Types::str2long("42"), 42L);
  EXPECT_EQ(::inip::Types::str2long("-32"), -32L);
#if defined(__linux__) || defined(__unix__)
  std::cout << "Platform is linux/unix, use 64-bit long\n";
  EXPECT_EQ(::inip::Types::str2long("9223372036854775807"), 9223372036854775807L);
#elif defined(_WIN32) || defined(_WIN64)
  std::cout << "Platform is windows, use 32-bit long\n";
  EXPECT_THROW(::inip::Types::str2long("9223372036854775807"), ::inip::err::Errors);
#else
  std::cout << "Platform is unkonow, skip test long max\n";
#endif

  EXPECT_THROW(::inip::Types::str2long("invalid"), ::inip::err::Errors);
}

TEST(TypesTest, Str2ULongTest)
{
  EXPECT_EQ(::inip::Types::str2ulong("0"), 0UL);
  EXPECT_EQ(::inip::Types::str2ulong("42"), 42UL);

#if defined(__linux__) || defined(__unix__)
  std::cout << "Platform is linux/unix, use 64-bit long\n";
  EXPECT_EQ(::inip::Types::str2ulong("18446744073709551615"), 18446744073709551615UL);
#elif defined(_WIN32) || defined(_WIN64)
  std::cout << "Platform is windows, use 32-bit long\n";
  EXPECT_THROW(::inip::Types::str2long("18446744073709551615"), ::inip::err::Errors);
#else
  std::cout << "Platform is unkonow, skip test ulong max\n";
#endif

  EXPECT_THROW(::inip::Types::str2ulong("invalid"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2ulong(""), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2ulong("-1"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2ulong("12.34"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2ulong("114.514"), ::inip::err::Errors);
}

TEST(TypesTest, Str2LongLongTest)
{
  EXPECT_EQ(::inip::Types::str2longlong("0"), 0LL);
  EXPECT_EQ(::inip::Types::str2longlong("42"), 42LL);
  EXPECT_EQ(::inip::Types::str2longlong("-42"), -42LL);
  EXPECT_EQ(::inip::Types::str2longlong("9223372036854775807"), 9223372036854775807LL);

  EXPECT_THROW(::inip::Types::str2longlong("invalid"), ::inip::err::Errors);
}

TEST(TypesTest, Str2ULongLongTest)
{
  EXPECT_EQ(::inip::Types::str2ulonglong("0"), 0ULL);
  EXPECT_EQ(::inip::Types::str2ulonglong("42"), 42ULL);
  EXPECT_EQ(::inip::Types::str2ulonglong("18446744073709551615"), 18446744073709551615ULL);
  EXPECT_EQ(::inip::Types::str2ulonglong("-1"), std::numeric_limits<unsigned long long>::max());
  EXPECT_THROW(::inip::Types::str2ulonglong("invalid"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2ulonglong("invalid"), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2ulonglong(""), ::inip::err::Errors);
}

TEST(TypesTest, Str2FloatTest)
{
  EXPECT_FLOAT_EQ(::inip::Types::str2float("0.0"), 0.0f);
  EXPECT_FLOAT_EQ(::inip::Types::str2float("3.14"), 3.14f);
  EXPECT_FLOAT_EQ(::inip::Types::str2float("-3.14"), -3.14f);
  EXPECT_FLOAT_EQ(::inip::Types::str2float("1e-5"), 1e-5f);
  EXPECT_FLOAT_EQ(::inip::Types::str2float("1e+5"), 1e+5f);
  EXPECT_FLOAT_EQ(::inip::Types::str2float("inf"), std::numeric_limits<float>::infinity());
  EXPECT_FLOAT_EQ(::inip::Types::str2float("-inf"), -std::numeric_limits<float>::infinity());

  EXPECT_THROW(::inip::Types::str2float(""), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2float("abc"), ::inip::err::Errors);
}

TEST(TypesTest, Str2DoubleTest)
{
  EXPECT_DOUBLE_EQ(::inip::Types::str2double("0.0"), 0.0);
  EXPECT_DOUBLE_EQ(::inip::Types::str2double("3.141592653589793"), 3.141592653589793);
  EXPECT_DOUBLE_EQ(::inip::Types::str2double("-3.141592653589793"), -3.141592653589793);
  EXPECT_DOUBLE_EQ(::inip::Types::str2double("1e-10"), 1e-10);
  EXPECT_DOUBLE_EQ(::inip::Types::str2double("1e+10"), 1e+10);
  EXPECT_DOUBLE_EQ(::inip::Types::str2double("inf"), std::numeric_limits<double>::infinity());
  EXPECT_DOUBLE_EQ(::inip::Types::str2double("-inf"), -std::numeric_limits<double>::infinity());

  EXPECT_THROW(::inip::Types::str2double(""), ::inip::err::Errors);
  EXPECT_THROW(::inip::Types::str2double("abc"), ::inip::err::Errors);
}

TEST(TypesTest, Str2IntOverflowTest)
{
  // near the cedge but not overflow
  EXPECT_EQ(::inip::Types::str2int("2147483647"), 2147483647);

  // NOTE: overflow are UBs, different   platform/compilers behave differently
  // So this maybe faile

  try {
    ::inip::Types::str2int("999999999999999999999");
    std::cout << "Note: std::stoi didn't throw on large value, returned: "
              << ::inip::Types::str2int("999999999999") << "\n";
  }
  catch (const std::out_of_range &) {
    SUCCEED();
  }
  catch (const ::inip::err::Errors &) {
    SUCCEED();
  }
  catch (...) {
    FAIL() << "Unexpected exception type";
  }
}
