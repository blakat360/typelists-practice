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

// return the larger of two types
template <typename T, typename U>
struct LargerTypeT {
    using Type = typename conditional<sizeof(T) >= sizeof(U), T, U>::type;
};

template <typename T, typename U>
using LargerType = typename LargerTypeT<T, U>::Type;

// reduce
template <typename List, template<typename T, typename U> class F, typename I, bool Empty = IsEmpty<List>::value>
struct ReduceT;

template <typename List, template<typename T, typename U> class F, typename I>
struct ReduceT<List, F, I, true> {
    using Type = I;
};

template <typename List, template<typename T, typename U> class F, typename I>
struct ReduceT<List, F, I, false> :
        public ReduceT<
            Tail<List>,
            F,
            F<I, Head<List>>
        >{};

template <typename List, template<typename T, typename U> class F, typename I>
using Reduce = typename ReduceT<List, F, I>::Type;

// get the largest type using an accumulator

template <typename List>
using LargestTypeAcc = Reduce<List, LargerType, char>;

// reverse using an accumulator
template <typename List>
using ReverseAcc = Reduce<List, PushFront, Typelist<>>;

// insertion sort using meta comparison function

template <typename List, template<typename T, typename U> class F, typename V, bool Empty = IsEmpty<List>::value>
struct InsertSortedT;

template <typename List, template<typename T, typename U> class F, typename V>
struct InsertSortedT<List, F, V, true> {
    using Type = PushFront<List, V>;
};

template <typename List, template<typename T, typename U> class F, typename V>
struct InsertSortedT<List, F, V, false> {
    using Type = typename conditional<
            F<V, Head<List>>::value,
            PushFront<List, V>,
            PushFront<
                    typename InsertSortedT<Tail<List>, F, V>::Type,
                    Head<List>
            >
    >::type;
};

template <typename List, template<typename T, typename U> class F, bool Empty = IsEmpty<List>::value>
struct InsertionSortT;

template <typename List, template<typename T, typename U> class F>
struct InsertionSortT<List, F, true> {
    using Type = List;
};

template <typename List, template<typename T, typename U> class F>
struct InsertionSortT<List, F, false> {
    using Type = typename InsertSortedT<
            typename InsertionSortT<Tail<List>, F>::Type,
            F,
            Head<List>
        >::Type;
};


template <typename List, template<typename T, typename U> class F>
using InsertionSort = typename InsertionSortT<List, F>::Type;

// sort a list of types into order of increasing size
template <typename T, typename U>
struct LargerTypeBoolT {
    static constexpr bool value = is_same<LargerType<T, U>, U>::value;
};

template <typename List>
using SortBySize = InsertionSort<List, LargerTypeBoolT>;

// get a list where size of all neighbours are distinct - works like unique program in cli

template <typename List, typename V, bool Empty = IsEmpty<List>::value>
struct PushIfUniqueT;

template <typename List, typename V>
class PushIfUniqueT<List, V, false>  {
    static constexpr bool sizes_differ = sizeof(Head<Reverse<List>>) != sizeof(V);
public:
    using Type = typename conditional<sizes_differ, PushBack<List, V>, List>::type;
};

template <typename List, typename V>
struct PushIfUniqueT<List, V, true>  {
    using Type = Typelist<V>;
};

template <typename List, typename V>
using PushIfUnique = typename PushIfUniqueT<List, V>::Type;

template <typename List>
using Unique = Reduce<List, PushIfUnique, Typelist<>>;


int main() {
    using DistinctSignedIntegralTypes = Unique<SignedIntegralTypes>; // long long == long on my machine :(

    static_assert(is_same<Head<DistinctSignedIntegralTypes>, signed char>::value, "");
    static_assert(is_same<Tail<SignedIntegralTypes>, Typelist<short, int, long, long long>>::value, "");
    static_assert(is_same<Tail<Typelist<int, char>>, Typelist<char>>::value, "");
    static_assert(is_same<PushFront<Typelist<int, char>, bool>, Typelist<bool, int, char>>::value, "");
    static_assert(is_same<NthElement<DistinctSignedIntegralTypes, 0>, signed char>::value, "");
    static_assert(is_same<NthElement<DistinctSignedIntegralTypes, 1>, short>::value, "");
    static_assert(is_same<NthElement<DistinctSignedIntegralTypes, 2>, int>::value, "");
    static_assert(is_same<LargestType<DistinctSignedIntegralTypes>, long>::value, "");
    static_assert(is_same<PushBack<Typelist<int, char>, bool>, Typelist<int, char, bool>>::value, "");
    static_assert(is_same<PushBack<Typelist<>, bool>, Typelist<bool>>::value, "");
    static_assert(is_same<Reverse<Typelist<bool, int, char>>, Typelist<char, int, bool>>::value, "");
    static_assert(is_same<PopBack<Typelist<int, char>>, Typelist<int>>::value, "");
    static_assert(is_same<AddConst<char>, char const>::value, "");
    static_assert(is_same<AddConst<char>, const char>::value, "");
    static_assert(is_same<AddConst<char*>, char* const>::value, "");
    static_assert(is_same<Map<DistinctSignedIntegralTypes, AddConst>, Typelist<signed char const, short const, int const, long const>>::value, "");
    static_assert(is_same<Map<Typelist<char, char, char*, char const * >, AddConst>, Typelist<char const, const char, char * const, const char * const>>::value, "");
    static_assert(is_same<LargestTypeAcc<DistinctSignedIntegralTypes>, long>::value, "");
    static_assert(is_same<LargestTypeAcc<Typelist<>>, char>::value, "");
    static_assert(is_same<ReverseAcc<Typelist<bool, int, char>>, Typelist<char, int, bool>>::value, "");
    static_assert(is_same<SortBySize<Reverse<DistinctSignedIntegralTypes>>, DistinctSignedIntegralTypes>::value, "");
}
