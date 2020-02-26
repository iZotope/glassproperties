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

namespace Glass {
	namespace Meta {
		template <template <typename...> class TL, typename A, typename B> struct Concat {};

		template <template <typename...> class TL, typename... As, typename... Bs>
		struct Concat<TL, TL<As...>, TL<Bs...>> {
			using type = TL<As..., Bs...>;
		};

		static_assert(std::is_same<typename Concat<PropertyList, PropertyList<int, float>,
		                                           PropertyList<float, int>>::type,
		                           PropertyList<int, float, float, int>>::value,
		              "ConcatPropertyLists");

		template <template <typename...> class TL, typename... Ts> struct Join {};

		template <template <typename...> class TL> struct Join<TL> { using type = TL<>; };

		template <template <typename...> class TL, typename... Ts, typename... Us>
		struct Join<TL, TL<Ts...>, Us...> {
			using type = typename Concat<TL, TL<Ts...>, typename Join<TL, Us...>::type>::type;
		};

		static_assert(
		    std::is_same<typename Join<PropertyList, PropertyList<int>, PropertyList<int, float>,
		                               PropertyList<float, int>>::type,
		                 PropertyList<int, int, float, float, int>>::value,
		    "JoinPropertyLists");

		template <template <typename...> class TL, template <typename...> class F, typename U>
		struct GenerateProperties {};

		template <template <typename...> class TL, template <typename...> class F>
		struct GenerateProperties<TL, F, TL<>> {
			using type = PropertyList<>;
		};

		template <template <typename...> class TL, template <typename...> class F, typename... Ts,
		          typename... Rest>
		struct GenerateProperties<TL, F, TL<TL<Ts...>, Rest...>> {
			using type =
			    typename Concat<PropertyList, PropertyList<F<Ts...>>,
			                    typename GenerateProperties<TL, F, TL<Rest...>>::type>::type;
		};

		template <template <typename...> class TL, template <typename...> class F, typename U>
		struct MapEach {};

		template <template <typename...> class TL, template <typename...> class F>
		struct MapEach<TL, F, TL<>> {
			using type = TL<>;
		};

		template <template <typename...> class TL, template <typename...> class F, typename... Ts,
		          typename... Rest>
		struct MapEach<TL, F, TL<TL<Ts...>, Rest...>> {
			using type = typename Concat<TL, TL<typename F<Ts...>::type>,
			                             typename MapEach<TL, F, TL<Rest...>>::type>::type;
		};

		template <template <typename...> class TL, typename T, typename U> struct PrependToAll {
			template <typename... Ts> struct Helper { using type = TL<T, Ts...>; };

			using type = typename MapEach<TL, Helper, U>::type;
		};

		static_assert(
		    std::is_same<
		        typename PrependToAll<
		            PropertyList, int,
		            PropertyList<PropertyList<float>, PropertyList<int, int>>>::type,
		        PropertyList<PropertyList<int, float>, PropertyList<int, int, int>>>::value,
		    "PrependToAll");

		template <template <typename...> class TL, typename... Ts> struct CartesianProduct {};

		template <template <typename...> class TL> struct CartesianProduct<TL> {
			using type = TL<TL<>>;
		};

		template <template <typename...> class TL, typename... Rest>
		struct CartesianProduct<TL, TL<>, Rest...> {
			using type = TL<>;
		};

		template <template <typename...> class TL, typename T, typename... Ts, typename... Rest>
		struct CartesianProduct<TL, TL<T, Ts...>, Rest...> {
			using lower = typename CartesianProduct<TL, Rest...>::type;
			using type =
			    typename Concat<TL, typename PrependToAll<TL, T, lower>::type,
			                    typename CartesianProduct<TL, TL<Ts...>, Rest...>::type>::type;
		};

		static_assert(
		    std::is_same<PropertyList<PropertyList<int, float>, PropertyList<int, int>,
		                              PropertyList<float, float>, PropertyList<float, int>>,
		                 typename CartesianProduct<PropertyList, PropertyList<int, float>,
		                                           PropertyList<float, int>>::type>::value,
		    "Cartesian test");

		template <template <typename> typename F, typename T> struct MapPropertyList {};

		//! Applies a template to all properties in a property list.
		template <template <typename> typename F, typename... Ts>
		struct MapPropertyList<F, Glass::PropertyList<Ts...>> {
			using type = Glass::PropertyList<F<Ts>...>;
		};

		static_assert(
		    std::is_same<typename MapPropertyList<std::optional, PropertyList<int, float>>::type,
		                 PropertyList<std::optional<int>, std::optional<float>>>::value,
		    "MapPropertyList Test");

		//! Applies a value to a list of properties
		template <typename View, typename Value>
		void SetAllProps(View&, const Value&, const Glass::PropertyList<>&) {}

		template <typename View, typename Value, typename Prop, typename... Props>
		void
		SetAllProps(View& view, const Value& value, const Glass::PropertyList<Prop, Props...>&) {
			view.template SetProperty<Prop>(value);
			SetAllProps(view, value, Glass::PropertyList<Props...>{});
		}
	}
}
