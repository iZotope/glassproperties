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
#include "iZBase/Util/PropertySerializer.h"

Glass::Private::GlobalPropertyData::PropertySerializationMap&
Glass::Private::GlobalPropertyData::getPropertySerializationMap() {
	static PropertySerializationMap map{20};
	return map;
}

void Glass::Private::GlobalPropertyData::registerGlobalPropertyTypes(
    Util::PropertySerializer& serializer) {
	static const Util::iZUUID uuid{};
	if (serializer.GetDidRegisterTypesForUUID(uuid)) {
		return;
	}
	for (auto& p : getPropertySerializationMap()) {
		registerPropertyType(serializer, p.first, (p.second)());
	}
	serializer.SetDidRegisterTypesForUUID(uuid);
}

template <typename T> static boost::optional<T> toBoostOptional(Glass::optional<T> optT) {
	if (!optT) {
		return boost::none;
	}
	return boost::optional<T>{std::move(*optT)};
}

void Glass::Private::GlobalPropertyData::registerPropertyType(
    Util::PropertySerializer& serializer,
    std::string name,
    Glass::Private::GlobalPropertyData::PropertyTypeSerializationData typeData) {
	serializer.RegisterTypeAdvanced(std::move(name), std::move(typeData));
}
