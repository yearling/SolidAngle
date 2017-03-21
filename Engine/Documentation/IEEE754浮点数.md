# IEEE754标准浮点

单精度
<table style="border-collapse:collapse;"><thead><tr><th>Sign（高位）</th><th>Exponent</th><th>Mantissa（低位）</th></tr></thead><tbody><tr><td>1</td><td>8</td><td>23</td></tr></tbody></table>

双精度

|-----|------|
|Sign（高位）|Exponent|Mantissa（低位）|
|1|11|52|


###特殊值

**NaN**： 指数域全为1，且尾数域不等于零的浮点数.``(*((uint32*)&F)) & 0x7FFFFFFF) > 0x7F800000;``

**Infinity**:指数域全为1，且尾数域全为0

**Positive zero** : 0

**Negative zero** : 0x80000000

**Q_NAN** : 07FC0000

**+INF** :  0x7F800000

**-INF** : 0xFF800000

##Debug
###DEN后缀
[Denormal number](https://en.wikipedia.org/wiki/Denormal_number)
就是特别小的小数，比下限还小，接近0  
