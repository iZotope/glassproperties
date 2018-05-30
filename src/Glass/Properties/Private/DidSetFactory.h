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

#include "Glass/Properties/Meta.h"

namespace Glass {
	namespace Private {
		namespace DidSetFactoryHelper {
			template <typename T, typename D,
			          typename = typename std::enable_if<Meta::HasDidSet_v<T, D>, void>::type>
			static void CallDidSet(T* obj, D def = D{}) {
				obj->didSet(std::move(def));
			}
			template <typename W, typename D,
			          typename = typename std::enable_if<!Meta::HasDidSet_v<W, D>, void>::type>
			static void CallDidSet(W*) {}

			template <typename W, typename D,
			          typename = typename std::enable_if<Meta::IsLayoutProperty_v<D>, void>::type>
			static void CallSetNeedsLayout(W* obj, D* = nullptr) {
				static_assert(
				    Meta::HasSetNeedsLayout<W>::value,
				    "W must implement SetNeedsLayout to have a property D of type LayoutProperty.");
				obj->SetNeedsLayout();
			}
			template <typename W, typename D,
			          typename = typename std::enable_if<!Meta::IsLayoutProperty_v<D>, void>::type>
			static void CallSetNeedsLayout(W*) {}

			template <typename W, typename D,
			          typename = typename std::enable_if<Meta::IsDisplayProperty_v<D>, D>::type>
			static void CallSetNeedsDisplay(W* this_, D* = nullptr) {
				static_assert(Meta::HasSetNeedsLayout<W>::value,
				              "W must implement SetNeedsDisplay to have a property D of type "
				              "DisplayProperty.");
				this_->SetNeedsDisplay();
			}
			template <typename W, typename D,
			          typename = typename std::enable_if<!Meta::IsDisplayProperty_v<D>, void>::type>
			static void CallSetNeedsDisplay(W*) {}
		}
		template <typename T, typename D, typename R = DidSetType> struct DidSetFactory {
			static R Create(T*) { return DidSetType{}; }
		};
		template <typename T, typename D>
		struct DidSetFactory<
		    T, D,
		    typename std::enable_if<Meta::HasDidSet_v<T, D> || Meta::IsLayoutProperty_v<D> ||
		                                Meta::IsDisplayProperty_v<D>,
		                            DidSetType>::type> {
			static DidSetType Create(T* hasProperties) {
				return [hasProperties]() {
					DidSetFactoryHelper::CallDidSet<T, D>(hasProperties);
					DidSetFactoryHelper::CallSetNeedsLayout<T, D>(hasProperties);
					DidSetFactoryHelper::CallSetNeedsDisplay<T, D>(hasProperties);
				};
			}
		};
	}
}
