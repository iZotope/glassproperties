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

using namespace Glass;

TEST(OptionalSerialization, SerializeInt) {
	const auto serialized = OptionalProperty<IntPropertyType>::serialize(42);
	ASSERT_EQ(serialized, "42");
}

TEST(OptionalSerialization, SerializeNullopt) {
	const auto serialized = OptionalProperty<IntPropertyType>::serialize(std::nullopt);
	ASSERT_EQ(serialized, "std::nullopt");
}

TEST(OptionalSerialization, DeserializeInt) {
	const auto deserialized = OptionalProperty<IntPropertyType>::deserialize("42");
	ASSERT_TRUE(deserialized);
	ASSERT_TRUE(*deserialized);
	ASSERT_EQ(**deserialized, 42);
}

TEST(OptionalSerialization, DeserializeNullopt) {
	const auto deserialized = OptionalProperty<IntPropertyType>::deserialize("std::nullopt");
	ASSERT_TRUE(deserialized);
	ASSERT_FALSE(*deserialized);
}

TEST(OptionalSerialization, DeserializeBadInt) {
	const auto deserialized = OptionalProperty<IntPropertyType>::deserialize("pizza");
	ASSERT_FALSE(deserialized);
}
