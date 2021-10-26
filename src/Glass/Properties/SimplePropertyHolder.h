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


#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Glass {
	class SimplePropertyHolder {
	public:
		bool CreateProperty(std::string_view name,
		                    std::string_view typeName,
		                    boost::any value,
		                    boost::any scratchSpace = {});

		template <typename T> std::optional<T> GetProperty(const std::string_view name) const;

		template <typename T> bool SetProperty(const std::string_view name, T&& value);

		Signal<>& GetPropertySignal(std::string_view name);

	private:
		struct PropertyValue {
			boost::any value;
			Signal<> signal{};
		};
		std::unordered_map<std::string, PropertyValue> m_propertyValues;
	};


	template <typename T>
	inline std::optional<T> SimplePropertyHolder::GetProperty(std::string_view name) const {
		const auto& it =
		    std::find_if(m_propertyValues.cbegin(), m_propertyValues.cend(), [&](const auto& e) {
			    return std::string_view{e.first} == name;
		    });

		if (it == m_propertyValues.cend()) {
			return {};
		}

		auto* value = boost::any_cast<T>(&it->second.value);
		if (!value) {
			return {};
		}

		return *value;
	}

	template <typename T>
	inline bool SimplePropertyHolder::SetProperty(std::string_view name, T&& value) {
		auto it = std::find_if(m_propertyValues.begin(),
		                       m_propertyValues.end(),
		                       [&](const auto& e) { return std::string_view{e.first} == name; });

		if (it == m_propertyValues.end()) {
			return {};
		}

		if (!boost::any_cast<std::remove_cvref_t<T>>(&(it->second.value))) {
			return {};
		}

		it->second.value = std::forward<T>(value);
		it->second.signal();

		return true;
	}

}
