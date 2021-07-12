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
#include "Glass/Properties/Types/OptionalProperty.h"
#include "Glass/Properties/Types/VectorProperty.h"

using namespace Glass;

TEST(VectorSerialization, SerializeFloatVector) {
	const auto testVector = vector<float>{-17.3f, -0.00201f, 0.003301f, 42.5f};
	const auto serialized = VectorProperty<FloatPropertyType>::serialize(testVector);
	ASSERT_EQ(serialized, "vector(-17.3, -0.00201, 0.0033, 42.5)");
}

TEST(VectorSerialization, DeserializeFloatVector) {
	const auto testString = "vector(  -17.3 , -2.01e-3, .00330,    42.5f ) ";
	const auto deserialized = VectorProperty<FloatPropertyType>::deserialize(testString);
	ASSERT_TRUE(deserialized);
	ASSERT_EQ(*deserialized, (vector<float>{-17.3f, -0.00201f, 0.0033f, 42.5f}));
}

TEST(VectorSerialization, DeserializeFloatVectorFailure) {
	const auto testString = "vector(  -17.3 , -2.01e-3, .003301,    42.5f, pizza ) ";
	const auto deserialized = VectorProperty<FloatPropertyType>::deserialize(testString);
	ASSERT_FALSE(deserialized);
}

TEST(VectorSerialization, SerializeStringVector) {
	const auto testVector = vector<std::string>{"test string", "hello, world"};
	const auto serialized = VectorProperty<StringPropertyType>::serialize(testVector);
	ASSERT_EQ(serialized, "vector(test string, hello\\, world)");
}

TEST(VectorSerialization, DeserializeStringVector) {
	const auto testString = "vector(test string, hello\\, world)";
	const auto deserialized = VectorProperty<StringPropertyType>::deserialize(testString);
	ASSERT_TRUE(deserialized);
	ASSERT_EQ(deserialized, (vector<std::string>{"test string", "hello, world"}));
}

TEST(VectorSerialization, SerializeOptionalIntVector) {
	const auto testVector = vector<std::optional<int>>{1, 2, std::nullopt, 4};
	const auto serialized =
	    VectorProperty<OptionalProperty<IntPropertyType>>::serialize(testVector);
	ASSERT_EQ(serialized, "vector(1, 2, std::nullopt, 4)");
}

TEST(VectorSerialization, DeserializeOptionalIntVector) {
	const auto testString = "vector(1, 2, std::nullopt, 4)";
	const auto deserialized =
	    VectorProperty<OptionalProperty<IntPropertyType>>::deserialize(testString);
	ASSERT_TRUE(deserialized);
	ASSERT_EQ(*deserialized, (vector<std::optional<int>>{1, 2, std::nullopt, 4}));
}
