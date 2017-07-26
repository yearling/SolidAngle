# smart ptr
1. TUniquePtr
2. TSharedPtr
3. TSharedRef
4. TWeakPtr
3. TScopedPointer

## TUniquePtr
与std::unique_ptr行为一致。  
使用方法：

1. TUnique<int> TestIntPtr(new int(5)); 
2. TUnique<int[]> TestIntPtrArray(new int[5]); 
3. TUnique<int> MovePtr= MoveTmp(TestIntPtr);  
4. ERROR: TUnique<int> Copy(TestIntPtr); 
5. int *pInterPtr = TestIntPtr.Get();
6. 设置Deleter,

		class DeleteOps
		{
			public: void operator()(int* p) { delete p;}
		}
		TUnique<int,DeleteOps> PtrWithDeleter(new int(5),DeletePos()); 



## TScopedPointer
Deprecated in UE4.15

## TAutoPtr
Deprecated in UE4.15

# TSharedPtr
类似std::shared_ptr的实现，好处有 

1. Clean syntax 简介的语法，使用方式跟普通的裸指针类似；
2. Prevents memory leak 防止内存泄露；
3. Weak referencing 弱引用；
4. Thread Safety 线程安全的（标准不是线程安全的，ue有线程安全的）
5. Ubiquitous :可以创建指向__Any__类型的指针
6. Runtime safety: Shared Reference 永远不可能为空并且总能使被Deferenced. 
7. No reference cycles. 没有循环引用
8. Confers intent. You can easily tell an object *owner* from an *observer*.？？
9. Performance. 微小的overhead.
10. Robust features. 支持const, 前置声明，
11. Memory 仅仅是2个c++的大小in 64bit

##　TShreadPtr一共有三种
1. __TSharedRef__ : Non-nullable, reference counted non-instrusive authoritative smart pointer. 
2. __TSharedPtr__ : Reference counted non-instrusive authoritative smart pointer. 
3. __TWeakPtr__: Reference counted non-intrusive weak pointer reference 

### TSharedPtr 辅助函数
1. MakeShareable() - Used to initialize shared pointers from C++ pointers (enables implicit conversion)
2. TSharedFromThis - You can derive your own class from this to acquire a TSharedRef from "this"
3. StaticCastSharedRef() - Static cast utility function, typically used to downcast to a derived type.
4. ConstCastSharedRef() - Converts a 'const' reference to 'mutable' smart reference
5. StaticCastSharedPtr() - Dynamic cast utility function, typically used to downcast to a derived type.
6. ConstCastSharedPtr() - Converts a 'const' smart pointer to 'mutable' smart pointer

### TSharedPtr 注意事项
1. 多使用TShreadRef来替代TShreadPtr，因为TShreadRef不可能为nullptr
2. TSharedPtr::Reset(): 减少一次引用（引用为0为释放内存）
3. MakeShareable() 隐式的转化为TSharedRef或者TSharedPtr
4. 不能给TSharedRef赋nullptr
5. 将TSharedRef或者TSharedPtr作为函数参数，而不是TWeakPtr
5. 线程安全的TSharedPtr有一点慢 
6. 可以前置声明TSharedPtr,使用incomplete type,而TUniquePtr不行
7. 从性能角度出发，最少好调用 TWeakPtr::Pin (or conversions to TSharedRef/TSharedPtr)
8. 使用帮助函数来实现const/dynamic cast

### TSharedPtr的限制
1. 与SObject不兼容；
2. 没有自定义的Deleters
3. pointer Array暂时不支持 
4. 不支持隐式转为bool

## 与std::shared_ptr的不同
1. 使用Pin()从weakpointer到sharedpointer
2. TSharedPtr有可选线程安全版本
3. TSharedFromThis返回的是一个TSharedRef而不是一个TSharedPtr
4. 没有use_count(),unique()等函数；
5. 没有异常处理；
6. Custom allocators and custom deleters 不支持；

## 为什么有UE版的智能指针
1. std::shared_ptr 全平台没有完全实现 
2. 为了所有平台实现一致
3. 可以与UE的容器与类型一致
4. 添加MakeShareable,assign to nullptr等功能
5. 特定平台优化：Better control over platform specifics, including threading and optimizations
6. 不需要异常处理
7. 在对性能要求高的地方优化：We wanted more control over performance (inlining, memory, use of virtuals, etc.
8. 更好debug:(liberal code comments, etc.)
9. 最好不引用第三方库

# 注意
UE的默认不是线程安全版的就是一个特化版的，单线程下使用；
UE的线程安全版的，跟std::shared_ptr的线程安全程度一样，是多线程不安全的！！！！多线程不安全！！！