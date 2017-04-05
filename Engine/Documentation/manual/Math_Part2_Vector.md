# 数学库
## 向量运算
### 定义
	typedef __m128	VectorRegister  //Windows
	typedef float32x4_t __attribute((aligned(16))) VectorRegister;    // Arm 

**注意** UE很多SSE的操作都是用宏来实现的，不清楚为什么不用inline()，有可能是因为inline的时候，传参会有问题~~fastcall,vector call什么的  
**注意** 黑体是基本宏函数
### Mask 掩码
**SHUFFLEMASK**：用来选择相应的Scalar  

### Set
**MakeVectorRegister**  
**MakeVectorRegisterInt**  
**VectorZero**  
**VectorOne**
**VectorSetFloat3**  
**VectorSetFloat1**  
**VectorSet**  
**VectorSet_W0**  
**VectorSet_W1** 

### Get
**VectorGetComponent**  

### Load(Memory To Register)
**注意： 没有表明是Align的都是不对齐的**  
**VectorLoadFloat3**  
**VectorLoadFloat3_W0**  
**VectorLoadFloat3_W1**  
**VectorLoadAligned**  
**VectorLoadFloat1**  
**VectorLoadByte4**  
**VectorLoadByte4Reverse**  
**VectorStoreURGB10A2N**  
**VectorLoadURGBA16N**  

### Store(Register to Memory)  
**注意： 没有表明是Align的都是不对齐的**  
**VectorStoreAligned**  
**VectorStoreAlignedStreamed**  
**VectorStore**  
**VectorStoreFloat3**  
**VectorStoreFloat1**  
**VectorStoreByte4**  
**VectorLoadURGB10A2N**  
**VectorStoreURGBA16N**  

### Replicate
**VectorReplicate**

### Sign
**VectorAbs**  
**VectorNegate** 
**VectorSign**   

### Add Subtract Mutiply Dot Cross
**VectorAdd**  
**VectorSubtract**  
**VectorMultiply**  
**VectorMultiplyAdd**  
**VectorDivide**   
**VectorDot3**  
**VectorDot4**  
**VectorCross**  
### Compare
**VectorCompareEQ**  
**VectorCompareNE**  
**VectorCompareGT**  
**VectorCompareGE**  
**VectorMask_LT**
**VectorAnyGreaterThan**  

### Select
**VectorSelect**
**VectorSwizzle** 
**VectorShuffle**  
**VectorMask_LE**  
**VectorMask_GT**  
**VectorMask_GE**  
**VectorMask_EQ**  
**VectorMask_NE** 
**VectorMaskBits**
**VectorMergeVecXYZ_VecW**


### Bit Operation 位操作
**VectorBitwiseOr**  
**VectorBitwiseAnd**  
**VectorBitwiseXor**  

### Pow 指/对数操作
**VectorPow**  
**VectorReciprocalSqrt**  
**VectorReciprocal**  
**VectorReciprocalLen** 
**VectorReciprocalSqrtAccurate** 
**VectorReciprocalAccurate**  
**VectorExp** 
**VectorExp2**  
**VectorLog**  
**VectorLog2** 
 
### Normalize 归一化
**VectorNormalize**  

### Matrix
**VectorMatrixMultiply**  
**VectorMatrixInverse**  
**VectorTransformVector**  

### Quaternion
**VectorQuaternionMultiply2**  
**VectorQuaternionMultiply**  

### Min Max
**VectorMin**  
**VectorMax**  

### Control Register
**VectorGetControlRegister**  
**VectorSetControlRegister**  
**VECTOR_ROUND_TOWARD_ZERO**

### NaN Infinit
**VectorContainsNaNOrInfinite**  

### Trunc Ceil Floor
**VectorTruncate**  
**VectorFractional**  
**VectorCeil**  
**VectorFloor**  
**VectorMod**  
**VectorStep**  

### 三角函数
**VectorSin**
**VectorCos**  
**VectorSinCos**
**VectorTan**  
**VectorASin**  
**VectorACos**  
**VectorATan**  
**VectorATan2**  

### Constant 常数