// Copyright 2017-2021 iZotope, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "iZBase/common/common.h"

IZ_PUSH_ALL_WARNINGS
#include "gtest/gtest.h"
IZ_POP_ALL_WARNINGS

#include "Glass/Properties/Types/Builtins.h"

using namespace Glass;

TEST(IntSerialization, IntIn) {
	auto deserialized = IntPropertyType::deserialize("54");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(54, *deserialized);
}

TEST(IntSerialization, HexIntIn) {
	auto deserialized = IntPropertyType::deserialize("0x2a");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(42, *deserialized);
	deserialized = IntPropertyType::deserialize("e1");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(225, *deserialized);
}

TEST(IntSerialization, CapitalHexIntIn) {
	auto deserialized = IntPropertyType::deserialize("0x2A");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(42, *deserialized);
	deserialized = IntPropertyType::deserialize("E1");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(225, *deserialized);
}

TEST(IntSerialization, NegativeIntIn) {
	auto deserialized = IntPropertyType::deserialize("-54");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(-54, *deserialized);
}

TEST(IntSerialization, FloatIn) {
	auto deserialized = IntPropertyType::deserialize("54.3");
	ASSERT_FALSE(deserialized);
}

TEST(IntSerialization, Spaces) {
	auto deserialized = IntPropertyType::deserialize("54 3");
	ASSERT_FALSE(deserialized);
}

TEST(IntSerialization, String) {
	auto deserialized = IntPropertyType::deserialize("4 pizza");
	ASSERT_FALSE(deserialized);
}

TEST(IntSerialization, SerializeDeserialize) {
	int value = 36;
	auto serialized = IntPropertyType::serialize(value);
	ASSERT_EQ(serialized, "36");
	auto deserialized = IntPropertyType::deserialize(serialized);
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(value, *deserialized);
}

TEST(IntSerialization, DeserializeFloat) {
	auto deserialized = IntPropertyType::deserialize("42.0");
	ASSERT_FALSE(deserialized);
	deserialized = IntPropertyType::deserialize("-42.5");
	ASSERT_FALSE(deserialized);
}

TEST(FloatSerialization, FloatIn) {
	auto deserialized = FloatPropertyType::deserialize("54.3");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(54.3f, *deserialized);
	deserialized = FloatPropertyType::deserialize("54.2f");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(54.2f, *deserialized);
}

TEST(FloatSerialization, NegativeFloatIn) {
	auto deserialized = FloatPropertyType::deserialize("-54.3");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(-54.3f, *deserialized);
}

TEST(FloatSerialization, FloatWithSpacesIn) {
	auto deserialized = FloatPropertyType::deserialize(" 54.3 ");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(54.3f, *deserialized);
}

TEST(FloatSerialization, IntIn) {
	auto deserialized = FloatPropertyType::deserialize("54");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(54.f, *deserialized);
}

TEST(FloatSerialization, SerializeDeserialize) {
	float value = 36.4f;
	auto serialized = FloatPropertyType::serialize(value);
	ASSERT_EQ(serialized, "36.4");
	auto deserialized = FloatPropertyType::deserialize(serialized);
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(value, *deserialized);
}

TEST(FloatSerialization, Spaces) {
	auto deserialized = FloatPropertyType::deserialize("54.3 0");
	ASSERT_FALSE(deserialized);
}

TEST(FloatSerialization, String) {
	auto deserialized = FloatPropertyType::deserialize("4 pizza");
	ASSERT_FALSE(deserialized);
}

TEST(BoolSerialization, BoolIn) {
	auto deserialized = BoolPropertyType::deserialize("true");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(true, *deserialized);
}

TEST(BoolSerialization, CapitalizedBoolIn) {
	auto deserialized = BoolPropertyType::deserialize("False");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(false, *deserialized);
}

TEST(BoolSerialization, AllCapsBoolIn) {
	auto deserialized = BoolPropertyType::deserialize("FALSE");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(false, *deserialized);
}

TEST(BoolSerialization, Garbage) {
	auto deserialized = BoolPropertyType::deserialize("true stuff");
	ASSERT_FALSE(deserialized);
}

TEST(BoolSerialization, Double) {
	auto deserialized = BoolPropertyType::deserialize("true false");
	ASSERT_FALSE(deserialized);
}

TEST(BoolSerialization, String) {
	auto deserialized = BoolPropertyType::deserialize("pizza");
	ASSERT_FALSE(deserialized);
}

TEST(BoolSerialization, Zero) {
	auto deserialized = BoolPropertyType::deserialize("0");
	ASSERT_FALSE(*deserialized);
}

TEST(BoolSerialization, SerializeDeserialize) {
	bool fValue = true;
	std::string serialized = BoolPropertyType::serialize(fValue);
	ASSERT_EQ(serialized, "true");
	auto deserialized = BoolPropertyType::deserialize(serialized);
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(fValue, *deserialized);
	fValue = false;
	serialized = BoolPropertyType::serialize(fValue);
	ASSERT_EQ(serialized, "false");
	deserialized = BoolPropertyType::deserialize(serialized);
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(fValue, *deserialized);
}

TEST(Float4DimComparison, ScalarEqual) {
	ASSERT_TRUE((Float4Dim{4.f} == Float4Dim{std::array<float, 4>{{4.f, 4.f, 4.f, 4.f}}}));
}

TEST(Float4DimComparison, ScalarNotEqual) {
	ASSERT_TRUE((Float4Dim{4.f} != Float4Dim{std::array<float, 4>{{4.f, 4.2f, 4.f, 4.f}}}));
}

TEST(Float4DimPropertyTypeSerialization, FloatIn) {
	auto deserialized = Float4DimPropertyType::deserialize("4.3");
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_TRUE(*deserialized == Float4Dim{4.3f});
}

TEST(Float4DimPropertyTypeSerialization, SerializeDeserializeFloat) {
	auto testValue = 36.4f;
	auto serialized = Float4DimPropertyType::serialize(testValue);
	ASSERT_EQ(serialized, "36.4");
	auto deserialized = Float4DimPropertyType::deserialize(serialized);
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_TRUE(*deserialized == Float4Dim{testValue});
}

TEST(Float4DimPropertyTypeSerialization, SerializeDeserializeArray) {
	std::array<float, 4> testArray = {{4.f, 0.f, 0.f, 4.f}};
	auto serialized = Float4DimPropertyType::serialize(testArray);
	ASSERT_EQ(serialized, "4 0 0 4");
	auto deserialized = Float4DimPropertyType::deserialize(serialized);
	ASSERT_TRUE(deserialized);
	ASSERT_TRUE(*deserialized == Float4Dim{testArray});
}

TEST(Float4DimPropertyTypeSerialization, DeserializeArrayWithCommas) {
	const auto deserialized = Float4DimPropertyType::deserialize("4.0, 0.0, 0.0, 4.0");
	ASSERT_TRUE(deserialized);
	ASSERT_TRUE(*deserialized == (Float4Dim{std::array<float, 4>{{4.f, 0.f, 0.f, 4.f}}}));
}

TEST(Float4DimPropertyTypeSerialization, TooSmall) {
	const auto deserialized = Float4DimPropertyType::deserialize("4.0 0.0");
	ASSERT_FALSE(deserialized);
}

TEST(Float4DimPropertyTypeSerialization, TooBig) {
	const auto deserialized = Float4DimPropertyType::deserialize("4.0 0.0 0.0 0.0 0.0");
	ASSERT_FALSE(deserialized);
}

TEST(StringSerialization, StringIn) {
	const auto testString = "What # about $ a string \\ with \"SPACES\"?";
	auto deserialized = StringPropertyType::deserialize(testString);
	ASSERT_TRUE(std::nullopt != deserialized);
	ASSERT_EQ(*deserialized, testString);
}

TEST(StringSerialization, SerializeDeserialize) {
	const auto testString = "What # about $ a string \\ with \"SPACES\"?";
	auto serialized = StringPropertyType::serialize(testString);
	ASSERT_EQ(serialized, testString);
	auto deserialized = StringPropertyType::deserialize(serialized);
	ASSERT_TRUE(deserialized);
	ASSERT_EQ(*deserialized, testString);
}
