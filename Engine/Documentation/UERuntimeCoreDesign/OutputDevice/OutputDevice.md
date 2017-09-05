# OutputDevice
## 文件结构

	HAL/PlatformOutputDevices.h
		|- GenericPlatformOutputDevices.h 
			|- Misc/OutputDevice.h
				|- FOutputDevice
			|- Misc/OutputDeviceConsole.h 
				|- FOutputDeviceConsole
			|- Misc/OutputDeviceError.h
				|- FOutputDeviceError
			|- Misc/FFeedbackContext.h 
				|- FFeedbackContext
			|- 定义
				|- SetupOutputDevices()//初始化OutputDevice
			|- 接口
				|- static FOutputDevice*			GetEventLog(); //开启WANTS_WINDOWS_EVENT_LOGGING宏，把Log写到WindowsEvent中,默认返回空
				|- static FOutputDeviceConsole*	GetLogConsole(); //弹出标准控制台
				|- static FOutputDeviceError*		GetError();
				|- static FFeedbackContext*		GetWarn();
				|- static FOutputDevice* 		GetLog();
		|- Windows/WindowsPlatformOutputDevices.h
			实现接口	
		|- typedef FWindowsPlatformOutputDevices FPlatformOutputDevices;

# GLog
1. GLog类型为FOutputDeviceRedirector,就是一个分发器，把Log中的内存发到console,log文件，debug的输出，window event中。 
2. GLog是个Singleton
3. 初始化时会记录当前ThreadID,
4. 结束时调用TearDown();
5. Add/RemoveOutputDevice()用来添加或者删除用来转发的Output