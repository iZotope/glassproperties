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

#include "Glass/Properties/HasPropertiesBase.h"
#include "Glass/Properties/PropertyList.h"

namespace Glass {
	namespace internal {}
	//! Mixin for object that implement glass properties
	//!
	//! typename T: type inheriting from HasProperties, must be a subclass of HasPropertiesBase
	//!
	//! HasProperties<T> expects that there is a PropertyList T::Properties
	template <typename T, typename U = T> class HasProperties {
	public:
		template <typename P>
		typename std::enable_if<PropertyListHasType<Halp, typename P::property_type::type>::value,
		                        typename P::property_type::type>::type
		GetProperty() const {
			return getPropertyHolder()
			    .template GetProperty<typename P::property_type::type>(P::name)
			    .cast();
		}

		template <typename P> void SetProperty(typename P::property_type::type value) {
			getPropertyHolder().template SetProperty<typename P::property_type::type>(P::name,
			                                                                          value);
		}

		template <typename P> typename P::property_type::type GetSerializedValue() const {
			return getPropertyHolder()
			    .template GetSerializedValue<typename P::property_type::type>(P::name)
			    .cast();
		}

		template <typename P> void SetSerializedValue(typename P::property_type::type value) {
			getPropertyHolder().template SetSerializedValue<typename P::property_type::type>(
			    P::name, value);
		}

	protected:
		HasProperties() {
			for (auto& p : CreatePropertyDefinitionListForPropertyList(typename T::Properties{})) {
				auto property = getPropertyHolder().CreateProperty(p->GetName(), p->GetTypeName(),
				                                                   p->GetDefaultValue());
				auto didSet = p->GetDidSetFn(static_cast<T*>(this));
				if (didSet) {
					property.GetSignal()->Connect(&getTrackable(), *didSet);
				}
			}
		}

	private:
		const Util::PropertyHolder& getPropertyHolder() const {
			return static_cast<const U*>(this)->m_propertyHolder;
		}

		Util::PropertyHolder& getPropertyHolder() {
			return static_cast<U*>(this)->m_propertyHolder;
		}

		Trackable& getTrackable() { return static_cast<U*>(this)->m_trackable; }
	};
}
