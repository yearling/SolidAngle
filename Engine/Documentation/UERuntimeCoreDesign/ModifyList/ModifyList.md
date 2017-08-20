# Modify List
Modify UE source to accomodate self using

## Explose MallocLeakDetection
Move the Runtime/Core/Private/HAL/MallocLeakDetection.h to Runtime/Core/Public/HAL/MallocLeakDetection.h 
So we can used the memory leak detection in the source code without using console

## Add Deleter to TUniquePtr's construction.