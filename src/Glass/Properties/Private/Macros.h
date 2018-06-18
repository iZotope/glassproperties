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

#if BOOST_PP_VARIADICS_MSVC
#include <boost/preprocessor/facilities/empty.hpp>
#endif
#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_list.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#define GLASS_P(Suffix, DefType, DefName, PropType, DefValue)                                      \
	GLASS_P_##Suffix(DefType, DefName, PropType, DefValue)

#define GLASS_P_Property(DefType, DefName, PropType, DefValue)                                     \
	struct DefType : ::Glass::PropertyDefinition<DefType, PropType> {                              \
		static constexpr const char* name = DefName;                                               \
		static PropType::type defaultValue() { return DefValue; }                                  \
	};

#define GLASS_P_OptionalProperty(DefType, DefName, PropType, DefValue)                             \
	struct DefType : ::Glass::PropertyDefinition<DefType, ::Glass::OptionalProperty<PropType>> {   \
		static constexpr const char* name = DefName;                                               \
		static ::Glass::OptionalProperty<PropType>::type defaultValue() { return DefValue; }       \
	};

#define GLASS_P_LayoutProperty(DefType, DefName, PropType, DefValue)                               \
	struct DefType : ::Glass::PropertyDefinition<DefType, PropType>, ::Glass::LayoutProperty {     \
		static constexpr const char* name = DefName;                                               \
		static PropType::type defaultValue() { return DefValue; }                                  \
	};

#define GLASS_P_OptionalLayoutProperty(DefType, DefName, PropType, DefValue)                       \
	struct DefType : ::Glass::PropertyDefinition<DefType, ::Glass::OptionalProperty<PropType>>,    \
	                 ::Glass::LayoutProperty {                                                     \
		static constexpr const char* name = DefName;                                               \
		static ::Glass::OptionalProperty<PropType>::type defaultValue() { return DefValue; }       \
	};

#define GLASS_P_DisplayProperty(DefType, DefName, PropType, DefValue)                              \
	struct DefType : ::Glass::PropertyDefinition<DefType, PropType>, ::Glass::DisplayProperty {    \
		static constexpr const char* name = DefName;                                               \
		static PropType::type defaultValue() { return DefValue; }                                  \
	};

#define GLASS_P_OptionalDisplayProperty(DefType, DefName, PropType, DefValue)                      \
	struct DefType : ::Glass::PropertyDefinition<DefType, ::Glass::OptionalProperty<PropType>>,    \
	                 ::Glass::DisplayProperty {                                                    \
		static constexpr const char* name = DefName;                                               \
		static ::Glass::OptionalProperty<PropType>::type defaultValue() { return DefValue; }       \
	};

#define GLASS_P_UIProperty(DefType, DefName, PropType, DefValue)                                   \
	struct DefType : ::Glass::PropertyDefinition<DefType, PropType>, ::Glass::UIProperty {         \
		static constexpr const char* name = DefName;                                               \
		static PropType::type defaultValue() { return DefValue; }                                  \
	};

#define GLASS_P_OptionalUIProperty(DefType, DefName, PropType, DefValue)                           \
	struct DefType                                                                                 \
	    : ::Glass::OptionalPropertyDefinition<DefType, ::Glass::OptionalProperty<PropType>>,       \
	      ::Glass::UIProperty {                                                                    \
		static constexpr const char* name = DefName;                                               \
		static ::Glass::OptionalProperty<PropType>::type defaultValue() { return DefValue; }       \
	};

#define GLASS_P_5(BaseType, DefType, DefName, PropType, DefValue)                                  \
	GLASS_P(BaseType, DefType, DefName, PropType, DefValue);

#define GLASS_P_4(BaseType, DefType, PropType, DefValue)                                           \
	GLASS_P_5(BaseType, DefType, #DefType, PropType, DefValue)

#if !BOOST_PP_VARIADICS_MSVC
#define GLASS_P_X(...) BOOST_PP_OVERLOAD(GLASS_P_, __VA_ARGS__)(__VA_ARGS__)
#else
#define GLASS_P_X(...)                                                                             \
	BOOST_PP_CAT(BOOST_PP_OVERLOAD(GLASS_P_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

#define CALL_GLASS_P_X(R, Data, Elem) GLASS_P_X Elem

#define GLASS_PS(Args) BOOST_PP_LIST_FOR_EACH(CALL_GLASS_P_X, _, Args)

#define GLASS_PL_OP(s, data, elem) BOOST_PP_TUPLE_ELEM(1, elem)

#define GLASS_PL(...)                                                                              \
	::Glass::PropertyList<BOOST_PP_SEQ_ENUM(                                                       \
	    BOOST_PP_SEQ_TRANSFORM(GLASS_PL_OP, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))>
