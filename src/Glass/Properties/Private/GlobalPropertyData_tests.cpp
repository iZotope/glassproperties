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

#include "iZBase/Util/VariantUtils.h"
#include "Glass/Properties/Types/Builtins.h"
#include "Glass/Properties/Private/GlobalPropertyData.h"
#include "Glass/Properties/Private/getName.h"
#include "Glass/Properties/Types/Builtins.h"
#include "iZBase/Util/PropertySerializer.h"

IZ_PUSH_ALL_WARNINGS
#include "gtest/gtest.h"
IZ_POP_ALL_WARNINGS

using Controller::KVCAny;

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
	const auto designAidInfo =
	    serializer.GetDesignAidInfoForType(Glass::Private::getName<TestEnumType>());
	ASSERT_TRUE(serializer.IsTypeRegistered(Glass::Private::getName<TestEnumType>()));
	ASSERT_TRUE(designAidInfo.type() == typeid(Util::PropertySerializer::DesignAidEnumInfo));

	const auto& names =
	    boost::get<Util::PropertySerializer::DesignAidEnumInfo>(designAidInfo).names;
	ASSERT_EQ(TestEnum::_size_constant, names.size());
	for (size_t i = 0; i < names.size(); ++i) {
		ASSERT_EQ(TestEnum::_names()[i], names[i]);
	}
}

struct TestScratchPropertyType : Glass::PropertyType<int32_t> {
	static constexpr auto name = "TestScratch";
	using scratch_type = std::string;
	static std::optional<std::string> serialize(int32_t, const std::string* scratch) {
		return scratch ? std::optional<std::string>{*scratch} : std::nullopt;
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
	static std::optional<std::string> serialize(int32_t) { return std::nullopt; }
	static std::optional<int32_t> deserialize(const std::string&, const std::string* context) {
		if (!context) {
			return std::nullopt;
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
	static std::optional<std::string> serialize(int32_t, const std::string* scratch) {
		return scratch ? std::optional<std::string>{*scratch} : std::nullopt;
	}
	static std::optional<Glass::ScratchSpaceAndValue<scratch_type, type>>
	deserialize(const std::string&, const std::string* context) {
		if (!context) {
			return std::nullopt;
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

struct TestKeypathPropertyType : Glass::PropertyType<int32_t*> {
	static constexpr auto name = "TestContext";
	static std::optional<std::string> serialize(int32_t*) { return std::nullopt; }
	static std::optional<int32_t*> deserialize(const std::string&) { return std::nullopt; }
	static const bool is_keypath = true;
};


TEST(GlobalPropertyTypeRegistration, SupportsIsKeypath) {
	Util::PropertySerializer serializer{};

	Glass::Private::GlobalPropertyData::registerPropertyType(
	    serializer,
	    Glass::Private::getName<TestKeypathPropertyType>(),
	    Glass::Private::GlobalPropertyData::GetPropertyTypeSerializationData<
	        TestKeypathPropertyType>());

	auto designAidInfo = Util::variant_cast<Util::PropertySerializer::DesignAidKVCPathInfo>(
	    serializer.GetDesignAidInfoForType(Glass::Private::getName<TestKeypathPropertyType>()));
	int32_t test = 32;
	ASSERT_TRUE(designAidInfo);
	ASSERT_TRUE(designAidInfo->canSetKVCValue(KVCAny{&test}));
	ASSERT_FALSE(designAidInfo->canSetKVCValue(KVCAny{"This is not a pipe"}));
}

struct TestMultipleKeypathsPropertyType : Glass::PropertyType<int64_t> {
	static constexpr auto name = "TestContext";
	static std::optional<std::string> serialize(int64_t) { return std::nullopt; }
	static std::optional<int64_t> deserialize(const std::string&) { return std::nullopt; }
	using allowed_keypath_types = std::tuple<int32_t*, float*>;
};


TEST(GlobalPropertyTypeRegistration, SupportsMultipleKeypathTypes) {
	Util::PropertySerializer serializer{};

	Glass::Private::GlobalPropertyData::registerPropertyType(
	    serializer,
	    Glass::Private::getName<TestMultipleKeypathsPropertyType>(),
	    Glass::Private::GlobalPropertyData::GetPropertyTypeSerializationData<
	        TestMultipleKeypathsPropertyType>());

	auto designAidInfo = Util::variant_cast<Util::PropertySerializer::DesignAidKVCPathInfo>(
	    serializer.GetDesignAidInfoForType(
	        Glass::Private::getName<TestMultipleKeypathsPropertyType>()));
	ASSERT_TRUE(designAidInfo);
	int32_t test1;
	ASSERT_TRUE(designAidInfo->canSetKVCValue(KVCAny{&test1}));
	float test2;
	ASSERT_TRUE(designAidInfo->canSetKVCValue(KVCAny{&test2}));
	ASSERT_FALSE(designAidInfo->canSetKVCValue(KVCAny{"I am not a number!"}));
}
