# Double to float 精度丢失bug预防
## 场景
时间函数里，返回高精度时间时，是double变量，但游戏逻辑中很多都是float变量，本来一帧之间的时间就很小，如果用秒表示就更小了，很容易造成从double--> float时精度丢失，然后，每帧时间间隔可能为负！！

## 解决方案
见WindowsPlatformTime.h 中FWindowsPlatformTime：：Seconds 

		static FORCEINLINE double Seconds()
		{
			Windows::LARGE_INTEGER Cycles;
			Windows::QueryPerformanceCounter(&Cycles);
	
			// add big number to make bugs apparent where return value is being passed to float
			return Cycles.QuadPart * GetSecondsPerCycle() + 16777216.0;
		}

1. 如果加上魔数16777216.0,帧间隔中负值出现的概率很大，
2. 如果去掉魔数16777216.0，帧间隔中负值出现的概率很小，但有，不容易观测到，不好查

## WHY?!!
[原因地址](https://stackoverflow.com/questions/12596695/why-does-a-float-variable-stop-incrementing-at-16777216-in-c)



Short roundup of IEEE-754 floating point numbers (32-bit) off the top of my head:

1. 1 bit sign (0 means positive number, 1 means negative number)
2. 8 bit exponent (with -127 bias, not important here)
3. 23 bits "mantissa"
With exceptions for the exponent values 0 and 255, you can calculate the value as: (sign ? -1 : +1) * 2^exponent * (1.0 + mantissa)
The mantissa bits represent binary digits after the decimal separator, e.g. 1001 0000 0000 0000 0000 000 = 2^-1 + 2^-4 = .5 + .0625 = .5625 and the value in front of the decimal separator is not stored but implicitly assumed as 1 (if exponent is 255, 0 is assumed but that's not important here), so for an exponent of 30, for instance, this mantissa example represents the value 1.5625
Now to your example:

16777216 is exactly 224, and would be represented as 32-bit float like so:

sign = 0 (positive number)
exponent = 24 (stored as 24+127=151=10010111)
mantissa = .0
As 32 bits floating-point representation: 0 10010111 00000000000000000000000
Therefore: Value = (+1) * 2^24 * (1.0 + .0) = 2^24 = 16777216
Now let's look at the number 16777217, or exactly 224+1:

sign and exponent are the same
mantissa would have to be exactly 2-24 so that (+1) * 2^24 * (1.0 + 2^-24) = 2^24 + 1 = 16777217
And here's the problem. The mantissa cannot have the value 2-24 because it only has 23 bits, so the number 16777217 just cannot be represented with the accuracy of 32-bit floating points numbers!



		true number           stored if float
        ----------------------------------------
        16,777,216            16,777,216
        16,777,217            16,777,216
        16,777,218            16,777,218
        16,777,219            16,777,220    [sic]
        16,777,220            16,777,220
        16,777,221            16,777,220
        16,777,222            16,777,222
        16,777,223            16,777,224    [sic]
        16,777,224            16,777,224
        16,777,225            16,777,224
        ----------------------------------------

说白了就是float 在0~16777215 的时候，任何一个比较整的小数f, +1.0之后，肯定是f+1.0,但是，（float)16777216 == (float) 16777217。
当一个<=1的flaot加上16777216后，可能和之前一样，这样通过float在算帧之前的时间差的时候，误差会大大的加大，导致错误容易被发现。提高debug的效率。