# smart ptr
1. TUniquePtr
2. TSharedPtr
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

