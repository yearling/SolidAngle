# 数学库
分2部分：  
1.	标量运算,黑体是平台相关优化过的  
2.	向量运算
## 类型转换 
### Trunc
对负数来说是ceil,对正数来说是floor,也就是说往数轴的0轴取值  
**TruncToInt**  
**TruncToFloat**
### Floor  
往数轴的左端取值  
**FloorToInt**
**FloorToFloat**  
**FloorToDouble**  
### Round 
其实就是 floor(+0.5)  
**RoundToInt**  
**RoundToFloat**  
**RoundToDouble**  
RoundHalfToEven  
RoundHalfFromZero  
RoundHalfToZero  
RoundFromZero  
RoundToZero  
RoundToNegativeInfinity  
RoundToPositiveInfinity 
### Ceil
往数轴的右端取值  
**CeilToInt**  
**CeilToFloat**  
**CeilToDouble**
### Frac
**Fractional**  ：返回的是带符号的  
**Frac**        ：返回的都是正的，负数会MOD
### MOD
**Modf**        ：拆成整数和小数部分
### Exponent
**Exp**  
**Exp2**  
**Loge**  
**LogX**  
**Log2**  
**Sqrt**  
**Pow**  
**InvSqrt**  
**InvSqrtEst**  
Square
### FMOD
**Fmod**       ：浮点%
### 三角函数
**Sin**  
**Asin**  
**Cos**  
**Acos**  
**Tan**  
**Atan**  
**Atan2**  
SinCos  
FastAsin  

### 浮点操作
**IsNaN**   
**IsFinite**    
**IsNegativeFloat**  
**IsNegativeDouble**

### 随机数
**Rand**  
**RandInit**  
**FRand**  
**SRandInit**  
**GetRandSeed**  
**SRand**  
RandHelper 	：Return a random number in [0,A)
RandRange
RandBool
VRand
VRandCone
RandPointInBox
### 位操作
**FloorLog2**  
**CountLeadingZeros**    
**CountTrailingZeros**  
**CeilLogTwo**   
**RoundUpToPowerOfTwo**  
IsPowerOfTwo  
ExtractBoolFromBitfield  
Quantize8UnsignedByte  
Quantize8SignedByte  
### Morton
**MortonCode2**  
**ReverseMortonCode2**  
**MortonCode3**  
**ReverseMortonCode3**  

### Select
**FloatSelect**

### 符号
**Abs**  
**Sign**

### Max,Min
**Max**  
**Min**  
Max3 
Min3
### 反射
GetReflectionVector

### 集合
IsWithin  
IsWithinInclusive  
IsNearlyEqual  
IsNearlyZero  

### Clamp
Clamp  
GridSnap  
DivideAndRoundUp  
DivideAndRoundDown  
GetMappedRangeValueClamped  
GetRangePct  
GetRangeValue  

### Degree,Radian,角度弧度 
RadiansToDegrees  
DegreesToRadians  
ClampAngle  
FindDeltaAngleDegrees  
FindDeltaAngle  
UnwindRadians  
UnwindDegrees  
WindRelativeAnglesDegrees  
FixedTurn

### 笛卡尔坐标 极坐标 
CartesianToPolar  
PolarToCartesian
GetDotDistance  
GetAzimuthAndElevation

### Lerp 插值
Lerp  
LerpStable  
BiLerp  
CubicInterp  
CubicInterpDerivative  
CubicInterpSecondDerivative  
InterpEaseIn  
InterpEaseOut  
InterpEaseInOut  
InterpStep  
InterpSinIn  
InterpSinOut  
InterpSinInOut  
InterpExpoIn  
InterpExpoOut  
InterpExpoInOut  
InterpCircularIn  
InterpCircularOut  
InterpCircularInOut  
CubicCRSplineInterp  
CubicCRSplineInterpSafe  
SmoothStep  
ApplyScaleToFloat  

### 向量插值 Lerp
VInterpNormalRotationTo  
VInterpConstantTo  
VInterpTo  
Vector2DInterpConstantTo  
Vector2DInterpTo  
RInterpConstantTo  
RInterpTo
FInterpConstantTo  
FInterpTo  
CInterpTo  

### Plust
MakePulsatingValue

### 几何求交及碰撞
LinePlaneIntersection  
ComputeProjectedSphereScissorRect  
PlaneAABBIntersection  
SphereAABBIntersection  
PointBoxIntersection  
LineBoxIntersection  
LineExtentBoxIntersection  
LineSphereIntersection  
SphereConeIntersection  
ClosestPointOnLine  
ClosestPointOnInfiniteLine  
IntersectPlanes3  
IntersectPlanes2  
PointDistToLine  
ClosestPointOnSegment  
ClosestPointOnSegment2D  
PointDistToSegment  
PointDistToSegmentSquared  
SegmentDistToSegment  
SegmentDistToSegmentSafe  
GetTForSegmentPlaneIntersect  
SegmentPlaneIntersection  
SegmentIntersection2D  
ClosestPointOnTriangleToPoint  
ClosestPointOnTetrahedronToPoint  
SphereDistToLine  
GetDistanceWithinConeSegment  
PointsAreCoplanar


### 格式化Format
FormatIntToHumanReadable

### MemoryTest
MemoryTest

### 公式求值
Eval

### 质心公式
GetBaryCentric2D  
ComputeBaryCentric2D  
ComputeBaryCentric3D  
