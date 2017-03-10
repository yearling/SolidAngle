#pragma once
class YDefaultAllocator;
class YDefaultSetAllocator;

class YString;

template<typename KeyType, typename ValueType> class YPair;
template<typename T, typename Allocator = FDefaultAllocator> class YArray;
template<typename T> class YTransArray;
template<typename KeyType, typename ValueType, bool bInAllowDuplicateKeys> struct YDefaultMapKeyFuncs;
template<typename KeyType, typename ValueType, typename SetAllocator = YDefaultSetAllocator, typename KeyFuncs = YDefaultMapKeyFuncs<KeyType, ValueType, false> > class YMap;
template<typename KeyType, typename ValueType, typename SetAllocator = YDefaultSetAllocator, typename KeyFuncs = YDefaultMapKeyFuncs<KeyType, ValueType, true > > class YMultiMap;
