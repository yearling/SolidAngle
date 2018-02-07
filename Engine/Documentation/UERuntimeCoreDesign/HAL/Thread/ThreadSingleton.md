# ThreadSingleton

## 线程单例
1. 每个线程只有一个实例，使用Get()方法来得到该实例；
2. 每个线程的实例共享同一个TlsSlot值，TlsSlot值对应的slot中存着当前单例对象的指针，该对象分配在堆上。

## 继承自FTlsAutoCleanUP
生命周期由FRunnerThread来控制`FRunableThread::FreeTls()`

## 创建
1. 第一次使用Get()时
	1. 第一个参数是创建函数（new T)，第二个参数是当前ThreadSingleton存存的TLs的索引；
	2. 检查TLsSlot是不是0xFFFFFF的非法值，__TLSSlot是每个线程的ThreadSingleton共享的！！__，所以接下来分配TLS并且更新TLS时需要加锁！！
	3. 通过TLSSlot来获取当前slot的值，如果值为0，说明未初始化，则需要创建T，并且注册到FRunnerThread,然后把T的地址写回TLS。

## 使用方法
class EntityID : public TThreadSingleton<EntityID>
	