# Function
有两个。

一个是相当于可调用对象的引用，并不实际持有所引用的对象。

	TFunctionRef<FuncType>
	// e.g. TfunctionRef<int(float, double)>

另一个是传统的std::function对应物，实际持有可调用对象。

	TFunction<FuncType>
	// e.g. Tfunction<int(float, double)>

TFunctionRef和TFunction对象的尺寸都是固定的。

TFunctionRef只是持有了所引用的可调用对象的指针。

TFunction内部有一个固定大小的存储空间，当持有的可调用对象的尺寸大于内部的存储空间时，会在堆上申请空间。

	template <typename FuncType>
	class TFunction
	{
		FFunctionStorage Storage;
	}

FFunctionStorage是TFunction内部的存储空间，具体下文。

# 实现细节

## 存储空间

内部实际存储的类型FFunctionStorage，其核心是一个inline allocator：
    
	struct FFunctionStorage
	{
		// Let TFunction store up to 32 bytes which are 16-byte aligned before we heap allocate
		TInlineAllocator<2>::ForElementType<TAlignedBytes<16, 16>> Allocator;
	};

TFunction直接持有了一个FFunctionStorage。TFunctionRef只有一个void*。

这个FFunctionStorage类型并没有具体的可调用对象的类型信息，只是一个占位对象。这个位置上具体存储的内容，见下。

## 存储的内容

有了在TFunction中开辟的存储空间，在其中存储的内容是一个接口类型的针对FunctorType特化的子类：

	struct IFunction_OwnedObject
	{
		// Returns the address of the object.
		virtual void* GetAddress() = 0;

		virtual ~IFunction_OwnedObject() = 0;
	};

	template <typename FunctorType>
	struct TFunction_OwnedObject : public IFunction_OwnedObject
	{
		virtual void* GetAddress() override { return &Obj; }
		FunctorType Obj;
	};

这个子类TFunction_OwnedObject就是实际用来存储可调用对象的类型，会被放置在TFunction的Storage对象所占据的空间内，具体构造发生在TFunction的构造函数内：

	template <typename FunctorType>
	TFunction(FunctorType&& InFunc) : Super(NoInit)
	{
		typedef TFunction_OwnedObject<FunctorType> OwnedType;

		OwnedType* NewObj = new (Storage) OwnedType(Forward<FunctorType>(InFunc));
		Super::Set(&NewObj->Obj);
	}

在Storage的空间上，构造了TFunction_OwnedObject<FunctorType>对象。最后一行的Super::Set(&NewObj->Obj)，是用实际持有的那个可调用对象的指针，去调用的基类的Set函数，这个函数是用来构造具体的类型擦除的调用转发器的，见下。

## 可调用对象的类型擦除

核心是TFunction和TFunctionRef的基类TFunctionRefBase，其中定义了operator()，并且类型擦除的具体实现也是在基类中完成：

	template <typename DerivedType, typename FuncType>
	struct TFunctionRefBase;

子类的继承方式是：

	template <typename FuncType>
	class TFunction : TFunctionRefBase<TFunction<FuncType>, FuncType>;

也就是说，其中第一个类型DerivedType是子类的具体类型，如：TFunction<int(float, double)>，第二个类型是函数类型，如：int(float, double)。

再看这个定义，其只有一个偏特化的实现：

	template <typename DerivedType, typename Ret, typename... ParamTypes>
	struct TFunctionRefBase<DerivedType, Ret(ParamTypes...)>
	{
		template <typename FunctorType>
		void Set(FunctorType* Functor);

		Ret operator()(ParamTypes... Params) const
		{
			const DerivedType* Derived = static_cast<const DerivedType*>(this);
			return Callable(Derived->GetPtr(), Params...);
		}

		// A pointer to a function which invokes the call operator on the callable object
		Ret(*Callable)(void*, ParamTypes&...);
	};

operator()中的Derived->GetPtr()，返回的是具体的可调用对象的指针（对于TFunction来说，就是存储在allocator中的实际持有的可调用对象，通过其中存储的的TFunction_OwnedObject<FunctorType>::GetAddress()来返回的那个真正存储的对象。对于TFucntionRef来说，就是持有的那个可调用对象的指针）。

里面的Callable变量存储的是一个具体用来调用调用那个可调用对象的函数代理，具体是在Set函数中被设置的。

	template <typename FunctorType>
	void Set(FunctorType* Functor)
	{
		Callable = &TFunctionRefCaller<FunctorType, Ret(ParamTypes...)>::Call;
	}

其实是一个Caller对象中的静态函数，这个函数有两个定义，主要是为了区别有没有返回值的函数调用：

	template <typename Functor, typename Ret, typename... ParamTypes>
	struct TFunctionRefCaller<Functor, Ret(ParamTypes...)>
	{
		static Ret Call(void* Obj, ParamTypes&... Params)
		{
			return Invoke(*(Functor*)Obj, Forward<ParamTypes>(Params)...);
		}
	};

	template <typename Functor, typename... ParamTypes>
	struct TFunctionRefCaller<Functor, void(ParamTypes...)>
	{
		static void Call(void* Obj, ParamTypes&... Params)
		{
			Invoke(*(Functor*)Obj, Forward<ParamTypes>(Params)...);
		}
	};

这样，使用在类型擦除的Storage中存储了一个TFunction_OwnedObject<FunctorType>，就完成了可调用对象的类型存储时的类型擦除。

在基类中使用一个void*的Callable函数指针，存储了具体涉及到了可调用对象类型Functor类型的调用转发器TFunctionRefCaller<Functor, void(ParamTypes...)>::Call函数，实现了调用转发过程的类型擦除。

并在基类的operator()中通过没有具体类型的Callable函数指针和用没有具体类型的Derived->GetPtr()得到的具体的对象指针，完成了类型擦除的可调用对象的调用过程。

至此function类型中的关键点都已经提到。