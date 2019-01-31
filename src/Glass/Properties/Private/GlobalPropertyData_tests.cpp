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

#include "Glass/Properties/Private/GlobalPropertyData.h"

#include "Glass/Properties/Types.h"
#include "iZBase/Util/PropertySerializer.h"

IZ_PUSH_ALL_WARNINGS
#include "gtest/gtest.h"
IZ_POP_ALL_WARNINGS

namespace {
	BETTER_ENUM(TestEnum, int32_t, ThingOne, ThingTwo, Cat);
	using TestEnumType = Glass::PropertyType<TestEnum>;
}

TEST(GlobalPropertyTypeRegistration, IntRegistration) {
	Util::PropertySerializer serializer{};

	Glass::Private::GlobalPropertyData::registerGlobalPropertyTypes(serializer);

	ASSERT_TRUE(serializer.IsTypeRegistered(Glass::IntPropertyType::name));
}

TEST(GlobalPropertyTypeRegistration, EnumPropertyNameRegistration) {
	Util::PropertySerializer serializer{};

	Glass::Private::GlobalPropertyData::registerPropertyType(
	    serializer,
	    Glass::Private::getName<TestEnumType>(),
	    Glass::Private::GlobalPropertyData::GetPropertyTypeSerializationData<TestEnumType>());

	ASSERT_TRUE(serializer.IsTypeRegistered(TestEnumType::name));
	ASSERT_TRUE(serializer.IsTypeNamedEnum(TestEnumType::name));

	auto names = serializer.GetNamedEnumTypeNames(TestEnumType::name);
	ASSERT_EQ(TestEnum::_size_constant, names.size());
	for (size_t i = 0; i < names.size(); ++i) {
		ASSERT_EQ(TestEnum::_names()[i], names[i]);
	}
}

struct TestScratchPropertyType : Glass::PropertyType<int32_t> {
	static constexpr auto name = "TestScratch";
	using scratch_type = std::string;
	static Glass::optional<std::string> serialize(int32_t, const std::string* scratch) {
		return scratch ? Glass::optional<std::string>{*scratch} : Glass::nullopt;
	}
	static Glass::ScratchSpaceAndValue<scratch_type, type> deserialize(const std::string&) {
		return Glass::ScratchSpaceAndValue<scratch_type, type>{std::string{"hello"}, 7};
	}
};

TEST(GlobalPropertyTypeRegistration, SupportsScratchSpace) {
	auto data = Glass::Private::GlobalPropertyData::GetPropertyTypeSerializationData<
	    TestScratchPropertyType>();
	auto deserialized = data.deserialize("test", boost::any{});
	ASSERT_TRUE(deserialized);
	auto reserialized = data.serialize(deserialized->value, deserialized->scratchSpace);
	ASSERT_TRUE(reserialized);
	ASSERT_EQ(std::string{"hello"}, *reserialized);
}

struct TestContextPropertyType : Glass::PropertyType<int32_t> {
	static constexpr auto name = "TestContext";
	using context_type = std::string;
	static Glass::optional<std::string> serialize(int32_t) { return Glass::nullopt; }
	static Glass::optional<int32_t> deserialize(const std::string&, const std::string* context) {
		if (!context) {
			return Glass::nullopt;
		}
		if (*context != "Cool Context") {
			return 17;
		}
		return 43;
	}
};

TEST(GlobalPropertyTypeRegistration, SupportsContext) {
	auto data = Glass::Private::GlobalPropertyData::GetPropertyTypeSerializationData<
	    TestContextPropertyType>();

	auto deserializedBad = data.deserialize("test", 4001);
	//! We expect giving the wrong context gives no result (see above)
	ASSERT_FALSE(deserializedBad);

	//! giving a context that isn't "Cool Context" should return 17 (See above).
	auto deserializedNotCool = data.deserialize("test", std::string{"uncool context"});
	ASSERT_TRUE(deserializedNotCool);
	ASSERT_TRUE(boost::any_cast<int32_t>(&deserializedNotCool->value) != nullptr);
	ASSERT_EQ(17, *boost::any_cast<int32_t>(&deserializedNotCool->value));

	//! giving a context that is "Cool Context" should return 43 (See above).
	auto deserializedCool = data.deserialize("test", std::string{"Cool Context"});
	ASSERT_TRUE(deserializedCool);
	ASSERT_TRUE(boost::any_cast<int32_t>(&deserializedCool->value) != nullptr);
	ASSERT_EQ(43, *boost::any_cast<int32_t>(&deserializedCool->value));
}

struct TestScratchAndContextPropertyType : Glass::PropertyType<int32_t> {
	static constexpr auto name = "TestScratchAndContext";
	using scratch_type = std::string;
	using context_type = std::string;
	static Glass::optional<std::string> serialize(int32_t, const std::string* scratch) {
		return scratch ? Glass::optional<std::string>{*scratch} : Glass::nullopt;
	}
	static Glass::optional<Glass::ScratchSpaceAndValue<scratch_type, type>>
	deserialize(const std::string&, const std::string* context) {
		if (!context) {
			return Glass::nullopt;
		}
		return Glass::ScratchSpaceAndValue<scratch_type, type>{*context, 7};
	}
};

TEST(GlobalPropertyTypeRegistration, SupportsScratchSpaceWithContext) {
	auto data = Glass::Private::GlobalPropertyData::GetPropertyTypeSerializationData<
	    TestScratchAndContextPropertyType>();

	auto deserializedBad = data.deserialize("test", 4001);
	//! We expect giving the wrong context gives no result (see above)
	ASSERT_FALSE(deserializedBad);

	// The test serializer simply copies the context into the scratch space, so test that this
	// pathway works.
	auto deserializedNotCool = data.deserialize("test", std::string{"uncool context"});
	ASSERT_TRUE(deserializedNotCool);
	auto reserializedNotCool = data.serialize(3008, deserializedNotCool->scratchSpace);
	ASSERT_TRUE(reserializedNotCool);
	ASSERT_EQ(std::string{"uncool context"}, *reserializedNotCool);

	auto deserializedCool = data.deserialize("test", std::string{"Cool Context"});
	ASSERT_TRUE(deserializedCool);
	auto reserializedCool = data.serialize(2008, deserializedCool->scratchSpace);
	ASSERT_TRUE(reserializedCool);
	ASSERT_EQ(std::string{"Cool Context"}, *reserializedCool);
}
