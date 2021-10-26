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

#include "Glass/Properties/SimplePropertyHolder.h"

bool Glass::SimplePropertyHolder::CreateProperty(std::string_view name,
                                                 std::string_view typeName,
                                                 boost::any value,
                                                 boost::any scratchSpace) {
	UNREF_PARAM(typeName);
	UNREF_PARAM(scratchSpace);

	auto it = std::find_if(m_propertyValues.cbegin(), m_propertyValues.cend(), [&](const auto& e) {
		return std::string_view{e.first} == name;
	});
	if (it != m_propertyValues.end()) {
		return false;
	}

	m_propertyValues.emplace(std::string{name}, PropertyValue{std::move(value)});
	return true;
}

Signal<>& Glass::SimplePropertyHolder::GetPropertySignal(std::string_view name) {
	return m_propertyValues.at(std::string{name}).signal;
}
