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

#include "Burlap/Properties/PropertyHolder.h"

Burlap::PropertyHolder::PropertyHolder() noexcept
    : m_storage{std::make_unique<Util::PropertyHolder>()}
    , m_propertyHolder{m_storage.get()} {}

Burlap::PropertyHolder::PropertyHolder(Util::PropertyHolder* propertyHolder) noexcept
    : m_storage{}
    , m_propertyHolder{propertyHolder} {}

bool Burlap::PropertyHolder::CreateProperty(const String& name,
                                            const String& typeName,
                                            boost::any value,
                                            boost::any scratchSpace) {
	auto property =
	    m_propertyHolder->CreateProperty(name, typeName, std::move(value), std::move(scratchSpace));
	return static_cast<bool>(property.GetSignal());
}

Signal<>& Burlap::PropertyHolder::GetPropertySignal(const String& name) const {
	return m_propertyHolder->GetPropertySignal(name);
}

void Burlap::PropertyHolder::SetStyleSheet(shared_ptr<Util::StyleSheet> styleSheet) {
	m_propertyHolder->SetStyleSheet(styleSheet);
}

void Burlap::PropertyHolder::SetClassNames(const vector<std::string>& classNames) {
	vector<String> names = classNames | ranges::v3::to_<vector<String>>();
	m_propertyHolder->SetProperty(Util::PropertyHolder::ClassList::PROPERTY,
	                              Util::PropertyHolder::ClassList{names});
}

vector<std::string> Burlap::PropertyHolder::GetClassNames() const {
	auto classList = m_propertyHolder->GetProperty<Util::PropertyHolder::ClassList>(
	    Util::PropertyHolder::ClassList::PROPERTY);

	if (classList.IsValid()) {
		auto stringList = static_cast<vector<String>>(classList.get());
		return stringList |
		       ranges::view::transform([](const String& s) { return s.ToStandardString(); });
	} else {
		return vector<std::string>();
	}
}
