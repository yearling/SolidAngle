#IEEE754标准浮点

单精度

|-----|------|
|Sign（高位）|Exponent|Mantissa（低位）|
|1|8|23|


双精度

|-----|------|
|Sign（高位）|Exponent|Mantissa（低位）|
|1|11|52|


###特殊值

**NaN**： 指数域全为1，且尾数域不等于零的浮点数.``(*((uint32*)&F)) & 0x7FFFFFFF) > 0x7F800000;``

**Infinity**:指数域全为1，且尾数域全为0