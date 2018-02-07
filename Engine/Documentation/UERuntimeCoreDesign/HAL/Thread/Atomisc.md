# Atomic

## 原子变量
抽象接口 	FGenericPlatformAtomics 
实现   FWindowsPlatformAtomics
封装  typedef FWindowsPlatformAtomics FPlatformAtomics;
## 函数

### 加/减一
int32 InterlockedIncrement( volatile int32* Value )
int32 InterlockedDecrement( volatile int32* Value )
int64 InterlockedIncrement( volatile int64* Value )
int64 InterlockedDecrement( volatile int64* Value )

### 加/减某数
int32 InterlockedAdd( volatile int32* Value, int32 Amount )
int64 InterlockedAdd( volatile int64* Value, int64 Amount )

### 替换 
int32 InterlockedExchange( volatile int32* Value, int32 Exchange )
void* InterlockedExchangePtr( void** Dest, void* Exchange )

int32 InterlockedCompareExchange
void* InterlockedCompareExchangePointer( void** Dest, void* Exchange, void* Comparand )