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

#include "Glass/Properties/PropertyList.h"
#include "iZBase/Util/PropertyHolder.h"

namespace Glass {
	class HasPropertiesBase {
		template <typename T> friend class HasProperties;

	public:
		virtual ~HasPropertiesBase();

	protected:
		HasPropertiesBase();

	private:
		Util::PropertyHolder m_propertyHolder;
	};

	//! Mixin for object that implement glass properties
	//!
	//! typename T: type inheriting from HasProperties
	//!
	//! HasProperties<T> expects that there is a PropertyList T::Properties
	template <typename T> class HasProperties : public virtual HasPropertiesBase, Trackable {
	public:
		template <typename P> typename P::property_type::type GetProperty() const {
			return m_propertyHolder.GetProperty<typename P::property_type::type>(P::name).cast();
		}

		template <typename P> void SetProperty(typename P::property_type::type value) {
			m_propertyHolder.SetProperty(P::name, value);
		}

		template <typename P> typename P::property_type::type GetSerializedValue() const {
			return m_propertyHolder.GetSerializedValue<typename P::property_type::type>(P::name)
			    .cast();
		}

		template <typename P> void SetSerializedValue(typename P::property_type::type value) {
			m_propertyHolder.SetSerializedValue(P::name, value);
		}

	protected:
		HasProperties() {
			for (auto& p : CreatePropertyDefinitionListForPropertyList(typename T::Properties{})) {
				auto property = m_propertyHolder.CreateProperty(p->GetName(), p->GetTypeName(),
				                                                p->GetDefaultValue());
				auto didSet = p->GetDidSetFn(static_cast<T*>(this));
				if (didSet) {
					property.GetSignal()->Connect(this, *didSet);
				}
			}
		}
	};
}
