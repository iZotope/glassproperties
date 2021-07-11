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

#include "Glass/Properties/Types/Builtins.h"

#include "iZBase/Util/VariantUtils.h"
#include "Glass/Properties/RegisterPropertyType.h"
#include "Glass/Properties/Types/Private/ToStringStripZeros.h"
#include "Glass/Properties/Types/Private/separateCommas.h"
#include "Glass/Properties/Types/Private/separateSingleSpacedParams.h"

using namespace Glass;
using namespace Glass::Private;

std::string BoolPropertyType::serialize(bool value) {
	return value ? "true" : "false";
}
std::optional<bool>
BoolPropertyType::deserialize(const std::string &serializedValue) {
  auto value = String{serializedValue}.ToBool();
  if (!value.IsValid()) {
    return std::nullopt;
  }
  return value.cast();
}
GLASS_REGISTER_PROPERTY_TYPE(BoolPropertyType)

std::string IntPropertyType::serialize(int32_t value) {
	return std::string{String("%1").Arg(value).UTF8()};
}
std::optional<int32_t>
IntPropertyType::deserialize(const std::string &serializedValue) {
  auto value = String{serializedValue}.ToInt();
  if (!value.IsValid()) {
    if (std::all_of(serializedValue.begin(), serializedValue.end(),
                    ::isxdigit) ||
        String{serializedValue}.Contains("0x")) {
      // Also convert valid hex strings to int
      value = strtoul(serializedValue.c_str(), NULL, 16);
    }
    if (value.IsValid()) {
      return value.cast();
    }
    return std::nullopt;
  }
  return value.cast();
}
GLASS_REGISTER_PROPERTY_TYPE(IntPropertyType)

std::string FloatPropertyType::serialize(float value) {
	auto result = ToStringStripZeros(value, 5);
	return std::string{String("%1").Arg(result).UTF8()};
}
std::optional<float>
FloatPropertyType::deserialize(const std::string &serializedValue) {
  auto reference = serializedValue;
  if (reference.find('f') != std::string::npos) {
    reference = reference.erase(reference.find('f'));
  }
  auto value = String{reference}.ToFloat();
  if (!value.IsValid()) {
    return std::nullopt;
  }
  return value.cast();
}
GLASS_REGISTER_PROPERTY_TYPE(FloatPropertyType)

std::string Float4DimPropertyType::serialize(const type& value) {
	return boost::apply_visitor(
	    ::Util::overload<std::string>(
	        [](float rad) { return fmt::format("{:g}", rad); },
	        [](std::array<float, 4> rad) {
		        return fmt::format("{:g} {:g} {:g} {:g}", rad[0], rad[1], rad[2], rad[3]);
	        }),
	    value);
}
std::optional<Float4Dim>
Float4DimPropertyType::deserialize(const std::string &serializedValue) {
  auto results = separateCommas(serializedValue);
  auto length = separateSingleSpacedParams(serializedValue);
  if (results.size() < 4 && results.size() > 1) {
    return std::nullopt;
  }

  if (results.size() == 1 && length.size() == 1) {
    return Float4Dim{String{results[0]}.ToFloat()};
  } else if (results.size() == 4 || length.size() == 4) {
    auto float4DimResults = results.size() == 4 ? results : length;
    std::array<float, 4> arrayValues;
    int index = 0;
    for (auto result : float4DimResults) {
      arrayValues[index] = String{result}.ToFloat().cast();
      index++;
    }
    return Float4Dim{arrayValues};
  }
  return std::nullopt;
}
GLASS_REGISTER_PROPERTY_TYPE(Float4DimPropertyType)


std::string StringPropertyType::serialize(std::string value) {
	return value;
}
std::optional<std::string>
StringPropertyType::deserialize(const std::string &value) {
  return value;
}
GLASS_REGISTER_PROPERTY_TYPE(StringPropertyType)
