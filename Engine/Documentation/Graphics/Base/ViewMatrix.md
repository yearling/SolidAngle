# ViewMatrix
ViewMatrix用来计算从世界空间到相机空间的变换矩阵。

实现方式与D3DXMatrixLookAtLH一样

## 参数
1. EyePosition
2. LookAtPosition
3. UpVector

## 原理
计算从世界坐标到相机坐标的变换
1. 世界坐标原点translate到相机原点（ - EyePosition)
2. 世界坐标rotate到相机坐标
	1. 相机坐标的计算
		1. 左手DX下，默认前向是Z轴正方向，故(LookAtPosition - EyePosition)就是Z轴正方向；
		2. Cross(Y,Z)得到X轴的正方向；（默认x轴与Up轴是垂直的，如果Roll的话就不垂直了）
		3. Cross(Z,X)得到Y轴正方向;(三轴正交）
		
	2.  以上三步可以拼出相机的坐标，因为要求从世界坐标到相机坐标的变换矩阵，对相机坐标取逆（因为是正交矩阵，求个转置就行）
	
3. 世界坐标到相机坐标= translat* (相机坐标）的逆
