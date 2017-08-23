# FRotator
## UE坐标系
1. 左手系，X轴前，Y轴右，Z轴上
2. 欧拉角用FVector来表示（x,y,z),表示绕x轴转（Roll),绕Y轴转（Pitch),绕Z轴转（Yaw); 
3. FRotator中
	1. 绕X轴为Roll,0头顶朝上，正顺时针，负逆时针
	2. 绕Y轴为Pitch,0直视，正向上看，负向下看
	3. 绕Z轴为Yaw,0朝东，正朝北，负朝南
4. UE中欧拉角Euler与FRotator的转化

		Pitch -->　Y Axis
		Yaw   -->  Z Axis
		Roll  -->  X Axis
		FVector FRotator::Euler() const
		{
			return FVector( Roll, Pitch, Yaw );
		}

		FRotator FRotator::MakeFromEuler(const FVector& Euler)
		{
			return FRotator(Euler.Y, Euler.Z, Euler.X);
		}

## FRotationTranslationMatrix（FRotator ==> FMatrix) 
已经修正为Directx下的，ZYX旋转（内旋）顺序；

## FRotator FMatrix::Rotator() const （FMatrix == >FRoator)

## FQuat::Rotator() (FQuat ==> FRotator)
已经修正为Directx下的，ZYX旋转（内旋）顺序；

## FQuat FRotator::Quaternion() const(FRotator ==> FQuat)
已经修正为Directx下的,SSE代码可能有问题

## FQuatRotationTranslationMatrix
不需要

## FScaleRotationTranslationMatrix(FRotator ==>FMatrix)
已经修正， `Scale*Rotate*Transpose`


