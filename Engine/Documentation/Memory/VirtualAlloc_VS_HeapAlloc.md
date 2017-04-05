#VirtualAlloc vs HeapAlloc
**VirtualAlloc** is a specialized allocation of the OS VM system. Allocations in the VM system must be made at an allocation granularity which (the allocation granularity) is architecture dependent. Allocation in the VM system is one of the most basic forms of memory allocation. VM allocations can take several forms, memory is not necessarily dedicated or physically backed in RAM (though it can be). VM allocation is typically a special purpose type of allocation, either because of the allocation has to  
1.	be very large,  
2.	needs to be shared,  
3.	must be aligned on a particular value (performance reasons) or
    the caller need not use all of this memory at once...
    etc...  

**HeapAlloc** is essentially what malloc and new both eventually call. It is designed to be very fast and usable under many different types of scenarios of a general purpose allocation. It is the "Heap" in a classic sense. Heaps are actually setup by a VirtualAlloc, which is what is used to initially reserve allocation space from the OS. After the space is initialized by VirtualAlloc, various tables, lists and other data structures are configured to maintain and control the operation of the HEAP. Some of that operation is in the form of dynamically sizing (growing and shrinking) the heap, adapting the heap to particular usages (frequent allocations of some size), etc..

new and malloc are somewhat the same, malloc is essentially an exact call into HeapAlloc( heap-id-default ); new however, can [additionally] configure the allocated memory for C++ objects. For a given object, C++ will store vtables on the heap for each caller. These vtables are redirects for execution and form part of what gives C++ it's OO characteristics like inheritance, function overloading, etc...

Some other common allocation methods like _alloca() and _malloca() are stack based; FileMappings are really allocated with VirtualAlloc and set with particular bit flags which designate those mappings to be of type FILE.

Most of the time, you should allocate memory in a way which is consistent with the use of that memory ;).  new in C++, malloc for C, VirtualAlloc for massive or IPC cases.

*** Note, large memory allocations done by HeapAlloc are actually shipped off to VirtualAlloc after some size (couple hundred k or 16 MB or something I forget, but fairly big :) ).

*** EDIT I briefly remarked about IPC and VirtualAlloc, there is also something very neat about a related VirtualAlloc which none of the responders to this question have discussed.

VirtualAllocEx is what one process can use to allocate memory in an address space of a different process. Most typically, this is used in combination to get remote execution in the context of another process via CreateRemoteThread (similar to CreateThread, the thread is just run in the other process).