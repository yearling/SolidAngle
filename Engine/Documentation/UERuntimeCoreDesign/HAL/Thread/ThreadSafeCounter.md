# ThreadSafeCounter

## FThreadSafeCounter 32位
class FThreadSafeCounter
{
public:
	typedef int32 IntegerType;
	volatile int32 Counter;
}
通过原子操作来实现计数器Counter的线程安全

## FThreadSafeCounter 64位
class FThreadSafeCounter64
{
}

## FThreadSafeBool 

class FThreadSafeBool :private FThreadSafeCounter