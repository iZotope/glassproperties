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


#include <memory>
#include <optional>

#include "iZBase/Util/PropertyHolder.h"

namespace Burlap {

	inline namespace Transition {
		class PropertyHolder {
		public:
			PropertyHolder() noexcept;
			explicit PropertyHolder(Util::PropertyHolder*) noexcept;

			bool CreateProperty(const String& name,
			                    const String& typeName,
			                    boost::any value,
			                    boost::any scratchSpace = {});

			template <typename T> std::optional<T> GetProperty(const String& name) const;

			template <typename T> bool SetProperty(const String& name, T&& value);

			Signal<>& GetPropertySignal(const String& name) const;

			void SetStyleSheet(shared_ptr<Util::StyleSheet> styleSheet);

			void SetClassNames(const vector<std::string>& classNames);
			vector<std::string> GetClassNames() const;

		private:
			std::unique_ptr<Util::PropertyHolder> m_storage;
			Util::PropertyHolder* m_propertyHolder;
		};


		template <typename T>
		inline std::optional<T> PropertyHolder::GetProperty(const String& name) const {
			return optional_cast(m_propertyHolder->GetProperty<T>(name));
		}

		template <typename T>
		inline bool PropertyHolder::SetProperty(const String& name, T&& value) {
			return m_propertyHolder->SetProperty<T>(name, std::forward<T>(value));
		}

	}
}
