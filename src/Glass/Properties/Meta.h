
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
		template <typename T, typename D, typename = void> constexpr inline bool HasDidSet = false;

		template <typename T, typename D>
		constexpr inline bool
		    HasDidSet<T, D, std::void_t<decltype(std::declval<T>().didSet(D{}))>> = true;

		template <typename T, typename = void> constexpr inline bool HasSetNeedsLayout = false;

		template <typename T>
		constexpr inline bool
		    HasSetNeedsLayout<T, std::void_t<decltype(std::declval<T>().SetNeedsLayout())>> = true;

		template <typename T, typename = void> constexpr inline bool HasSetNeedsDisplay = false;

		template <typename T>
		constexpr inline bool
		    HasSetNeedsDisplay<T, std::void_t<decltype(std::declval<T>().SetNeedsDisplay())>> =
		        true;

		template <typename D>
		constexpr inline bool IsLayoutProperty = std::is_base_of<LayoutProperty, D>::value;

		template <typename D>
		constexpr inline bool IsDisplayProperty = std::is_base_of<DisplayProperty, D>::value;
	}
}
