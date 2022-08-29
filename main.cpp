#include <bits/stdc++.h>

using namespace std;

// typelist definition

template <typename... Elements>
class Typelist;

using SignedIntegralTypes =
        Typelist<signed char, short, int, long, long long>;

// check if a typelist is empty

template <typename List>
struct IsEmpty {
    static constexpr bool value = false;
};

template <>
struct IsEmpty<Typelist<>> {
    static constexpr bool value = true;
};

// get head of typelist

template <typename List>
class HeadT;

template <typename Head, typename... Tail>
struct HeadT<Typelist<Head, Tail...>> {
    using Type = Head;
};

template <typename List>
using Head = typename HeadT<List>::Type;

// get tail of typelist

template <typename List>
class TailT;

template <typename Head, typename... Tail>
struct TailT<Typelist<Head, Tail...>> {
    using Type = Typelist<Tail...>;
};

template <typename List>
using Tail = typename TailT<List>::Type;

// push to front of typelist

template <typename List, typename Elem>
class PushFrontT;

template <typename... List, typename Elem>
struct PushFrontT<Typelist<List...>, Elem> {
    using Type = Typelist<Elem, List...>;
};

template <typename List, typename Elem>
using PushFront = typename PushFrontT<List, Elem>::Type;

// get nth element of typelist

template <typename List, unsigned N>
struct NthElementT: NthElementT<Tail<List>, N-1> {};

template <typename List>
struct NthElementT<List, 0>: HeadT<List> {};

template <typename List, unsigned N>
using NthElement = typename NthElementT<List, N>::Type;

// get first largest element in list

template <typename List, bool Empty = IsEmpty<List>::value>
class LargestTypeT;

template <typename List>
class LargestTypeT<List, false> {
    using Head =  Head<List>;
    using Tail = typename LargestTypeT<Tail<List>>::Type;
public:
    using Type = typename conditional<(sizeof(Head) >= sizeof(Tail)), Head, Tail>::type;
};

template <typename List>
class LargestTypeT<List, true> {
public:
    using Type = char;
};

template <typename List>
using LargestType = typename LargestTypeT<List>::Type;

// push to back of typelist

template <typename List, typename Elem>
class PushBackT;

template <typename... List, typename Elem>
struct PushBackT<Typelist<List...>, Elem> {
    using Type = Typelist<List..., Elem>;
};

template <typename List, typename Elem>
using PushBack = typename PushBackT<List, Elem>::Type;

// reverse typelist

template <typename List, bool Empty = IsEmpty<List>::value>
struct ReverseT;

template <typename List>
using Reverse = typename ReverseT<List>::Type;

template <typename List>
struct ReverseT<List, false> : PushBackT<Reverse<Tail<List>>, Head<List>> {};

template <typename List>
struct ReverseT<List, true> {
    using Type = List;
};

// pop back of typelist

template <typename List>
struct PopBackT : ReverseT<Tail<Reverse<List>>> {};

template <typename List>
using PopBack = typename PopBackT<List>::Type;

// make a type a constant

template <typename T>
struct AddConstT {
    using Type = T const;
};

template <typename T>
using AddConst = typename AddConstT<T>::Type;

// map template function over typelist

template <typename List, template<typename T> class MetaFun, bool Empty = IsEmpty<List>::value>
class MapT;

template <typename List, template<typename T> class MetaFun>
struct MapT<List, MetaFun, true> {
    using Type = List;
};

template <typename List, template<typename T> class MetaFun>
class MapT<List, MetaFun, false> :
public PushFrontT<typename MapT<Tail<List>, MetaFun>::Type, MetaFun<Head<List>>> {};

template <typename List, template<typename T> class MetaFun>
using Map = typename MapT<List, MetaFun>::Type;

int main() {
    static_assert(is_same<Head<SignedIntegralTypes>, signed char>::value, "");
    static_assert(is_same<Tail<SignedIntegralTypes>, Typelist<short, int, long, long long>>::value, "");
    static_assert(is_same<Tail<Typelist<int, char>>, Typelist<char>>::value, "");
    static_assert(is_same<PushFront<Typelist<int, char>, bool>, Typelist<bool, int, char>>::value, "");
    static_assert(is_same<NthElement<SignedIntegralTypes, 0>, signed char>::value, "");
    static_assert(is_same<NthElement<SignedIntegralTypes, 1>, short>::value, "");
    static_assert(is_same<NthElement<SignedIntegralTypes, 2>, int>::value, "");
    static_assert(is_same<LargestType<SignedIntegralTypes>, long>::value, ""); // long is long long on this platform, would be better to verify that the return value is the same size as long long but idk how to do this yet
    static_assert(is_same<PushBack<Typelist<int, char>, bool>, Typelist<int, char, bool>>::value, "");
    static_assert(is_same<PushBack<Typelist<>, bool>, Typelist<bool>>::value, "");
    static_assert(is_same<Reverse<Typelist<bool, int, char>>, Typelist<char, int, bool>>::value, "");
    static_assert(is_same<PopBack<Typelist<int, char>>, Typelist<int>>::value, "");
    static_assert(is_same<AddConst<char>, char const>::value, "");
    static_assert(is_same<AddConst<char>, const char>::value, "");
    static_assert(is_same<AddConst<char*>, char* const>::value, "");
    static_assert(is_same<Map<SignedIntegralTypes, AddConst>, Typelist<signed char const, short const, int const, long const, long long const>>::value, "");
    static_assert(is_same<Map<Typelist<char, char, char*, char const * >, AddConst>, Typelist<char const, const char, char * const, const char * const>>::value, "");
}
