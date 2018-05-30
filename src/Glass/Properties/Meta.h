
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

namespace Glass {
	namespace Meta {
		template <typename T, typename D, typename = std::void_t<>>
		struct HasDidSet : std::false_type {};

		template <typename T, typename D>
		struct HasDidSet<T, D, std::void_t<decltype(std::declval<T>().didSet(D{}))>>
		    : std::true_type {};

		template <typename T, typename D> constexpr bool HasDidSet_v = HasDidSet<T, D>::value;

		template <typename T, typename = std::void_t<>>
		struct HasSetNeedsLayout : std::false_type {};

		template <typename T>
		struct HasSetNeedsLayout<T, std::void_t<decltype(std::declval<T>().SetNeedsLayout())>>
		    : std::true_type {};

		template <typename T> constexpr bool HasSetNeedsLayout_v = HasSetNeedsLayout<T>::value;

		template <typename T, typename = std::void_t<>>
		struct HasSetNeedsDisplay : std::false_type {};

		template <typename T>
		struct HasSetNeedsDisplay<T, std::void_t<decltype(std::declval<T>().SetNeedsDisplay())>>
		    : std::true_type {};

		template <typename T> constexpr bool HasSetNeedsDisplay_v = HasSetNeedsDisplay<T>::value;

		template <typename D> struct IsLayoutProperty {
			static constexpr bool value = std::is_base_of<LayoutProperty, D>::value;
		};

		template <typename D> constexpr bool IsLayoutProperty_v = IsLayoutProperty<D>::value;

		template <typename D> struct IsDisplayProperty {
			static constexpr bool value = std::is_base_of<DisplayProperty, D>::value;
		};

		template <typename D> constexpr bool IsDisplayProperty_v = IsDisplayProperty<D>::value;

	}
}
