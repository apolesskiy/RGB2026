
#include <gtest/gtest.h>
#include "rgb2026.h"

TEST(ConvertFlashPeriod, ConvertFlashPeriod)
{/*
  ASSERT_EQ(0, rgb2026::convertFlashPeriod(0));
  ASSERT_EQ(0, rgb2026::convertFlashPeriod(128));
  ASSERT_EQ(0, rgb2026::convertFlashPeriod(255));
  ASSERT_EQ(1, rgb2026::convertFlashPeriod(256));
  ASSERT_EQ(1, rgb2026::convertFlashPeriod(384));
  ASSERT_EQ(2, rgb2026::convertFlashPeriod(512));
  ASSERT_EQ(3, rgb2026::convertFlashPeriod(620));
  ASSERT_EQ(3, rgb2026::convertFlashPeriod(640));
  ASSERT_EQ(3, rgb2026::convertFlashPeriod(680));
  ASSERT_EQ(4, rgb2026::convertFlashPeriod(768));*/
  ASSERT_EQ(111, rgb2026::convertFlashPeriod(14500));
  //ASSERT_EQ(126, rgb2026::convertFlashPeriod(16379));
  //ASSERT_EQ(126, rgb2026::convertFlashPeriod(65534));
}

TEST(ConvertColor, ConvertColor)
{
  ASSERT_EQ(0, rgb2026::convertColor(0));
  ASSERT_EQ(96, rgb2026::convertColor(128));
  ASSERT_EQ(144, rgb2026::convertColor(192));
  ASSERT_EQ(192, rgb2026::convertColor(255));
}

TEST(ConvertRampTime, Scale8xFast_WholeNums)
{
  // Trise low, Tfall high.
  ASSERT_EQ(0x0311, rgb2026::convertRampTime(16, 16));
  ASSERT_EQ(0x03F1, rgb2026::convertRampTime(16, 240));
  ASSERT_EQ(0x0377, rgb2026::convertRampTime(112, 112));
}

TEST(ConvertRampTime, Scale8xFast_Rounding)
{
  ASSERT_EQ(0x0311, rgb2026::convertRampTime(1, 23));
  ASSERT_EQ(0x0321, rgb2026::convertRampTime(1, 24));
  ASSERT_EQ(0x0336, rgb2026::convertRampTime(100, 50));
  ASSERT_EQ(0x03FF, rgb2026::convertRampTime(247, 247));
}

TEST(ConvertRampTime, Scale1x_WholeNums)
{
  ASSERT_EQ(0x0022, rgb2026::convertRampTime(256, 256));
  ASSERT_EQ(0x00F1, rgb2026::convertRampTime(128, 1920));
  ASSERT_EQ(0x0077, rgb2026::convertRampTime(896, 896));
}

TEST(ConvertRampTime, Scale1x_Rounding)
{
  ASSERT_EQ(0x0041, rgb2026::convertRampTime(1, 575));
  ASSERT_EQ(0x0051, rgb2026::convertRampTime(1, 576));
  ASSERT_EQ(0x0036, rgb2026::convertRampTime(720, 400));
  ASSERT_EQ(0x00FF, rgb2026::convertRampTime(1870, 1920));
}

TEST(ConvertRampTime, Scale2x_WholeNums)
{
  ASSERT_EQ(0x01F1, rgb2026::convertRampTime(128, 3840));
  ASSERT_EQ(0x0199, rgb2026::convertRampTime(2304, 2304));
}

TEST(ConvertRampTime, Scale2x_Rounding)
{
  ASSERT_EQ(0x0191, rgb2026::convertRampTime(1, 2431));
  ASSERT_EQ(0x01A1, rgb2026::convertRampTime(1, 2432));
  ASSERT_EQ(0x01FF, rgb2026::convertRampTime(3740, 3840));
}

TEST(ConvertRampTime, Scale4x_WholeNums)
{
  ASSERT_EQ(0x02F1, rgb2026::convertRampTime(256, 7680));
  ASSERT_EQ(0x0299, rgb2026::convertRampTime(4608, 4608));
}

TEST(ConvertRampTime, Scale4x_Rounding)
{
  ASSERT_EQ(0x0291, rgb2026::convertRampTime(1, 4863));
  ASSERT_EQ(0x02A1, rgb2026::convertRampTime(1, 4864));
  ASSERT_EQ(0x02FF, rgb2026::convertRampTime(7480, 7680));
}