# TLS

## FGenericPlatformTLS
## 平台实现
		FWindowsPlatformTLS：public FGenericPlatformTLS
		typedef FWindowsPlatformTLS FPlatformTLS;

## 实现细节
1. 通过调用Windows API来实现

## API
1. uint32 GetCurrentThreadId(void)
2. uint32 AllocTlsSlot(void)
4. void FreeTlsSlot(uint32 SlotIndex)
5. void SetTlsValue(uint32 SlotIndex,void* Value)
6. void* GetTlsValue(uint32 SlotIndex)
