# LogThreadSafe
[LogThreadSafe](https://answers.unrealengine.com/questions/347305/is-ue-log-thread-safe.html)

>The answer is "it depends." Each FOutputDevice can overload CanBeUsedOnAnyThread() to indicate whether it's threadsafe or not. Typically if it's expected to be called from a particular thread it will assert with check(AllowedThreadId == GetThreadId());