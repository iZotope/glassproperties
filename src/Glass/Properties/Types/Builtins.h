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


#pragma once

#include "Glass/Float4Dim.h"
#include "Glass/Properties/Types/PropertyType.h"
#include "Glass/Properties/Private/RegisterPropertyType.h"

namespace Glass {
	struct IntPropertyType : PropertyType<IntPropertyType> {
		using type = int32_t;
		static constexpr auto name = "Int";
		static std::string serialize(int32_t value);
		static std::optional<int32_t> deserialize(const std::string& serializedValue);
	};

	struct FloatPropertyType : PropertyType<FloatPropertyType> {
		using type = float;
		static constexpr auto name = "Float";
		static std::string serialize(float value);
		static std::optional<float> deserialize(const std::string& serializedValue);
	};

        using Float4Dim = boost::variant<float, std::array<float, 4>>;
	struct Float4DimPropertyType : PropertyType<Float4DimPropertyType> {
		using type = Float4Dim;
		static constexpr auto name = "Float4Dim";
		static std::string serialize(const type& value);
		static std::optional<Float4Dim> deserialize(const std::string& serializedValue);
	};

	struct BoolPropertyType : PropertyType<BoolPropertyType> {
		using type = bool;
		static constexpr auto name = "Bool";
		static std::string serialize(bool value);
		static std::optional<bool> deserialize(const std::string& serializedValue);
	};

	struct StringPropertyType : PropertyType<StringPropertyType> {
		using type = std::string;
		static constexpr auto name = "std::string";
		static std::string serialize(std::string value);
		static std::optional<std::string> deserialize(const std::string& serializedValue);
	};
}
