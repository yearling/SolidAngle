# ProjectionMatrix 
实现原型`D3DXMatrixPerspectiveFovLH`

## 实现原理
将向量（x,y,z,1)变换到clip空间，Dx下z(0,1),x,y(-1,1)
[x,y,z,1]*Project = (-1,1),(-1,1),(0,1),1

## x,y 变换
1. 这里的FOV指的是Y轴方向上的FOV，tan(fov/2)= (近平面Width/2) / Znear; 
2. 近平面Width/2 = Znear* tan(fov/2)*aspert; 
3. 计算投影到近平面上的(x投）= Znear/Z*X;
4. 归一到[0~1], (x投/（近平面Width/2)) =  x/((tan(fov/2) * aspert*Z)，__记为ScaleX__
5. 同理，(y投/(近平面Height/2)) = y/((tan(fov/2) * Z);__记为ScaleY__
6. 这样，投影矩阵只管X,Y值可以表示为
	
	ScaleX   0         未知   0
	 0      ScaleY     未知   0
     0       0         未知   1
     0       0         未知   0

__可见：一个向量乘完投影矩阵后，四个向量中，第一位x投影后的位置，第二位 y投影后的位置，第四位z的原值用来z test. 只剩下第三位了
## z变换
1. __目的__：为了透视投影矫正计算方便，存了归一化的 1/z
2. 设投影矩阵第三列为[0,0,a,b],与向量相乘，得[0,0,az,b],除z归一化到（0，1）,得到a+b/z， 故在z= Znear时，a+b/Znear = 0; a+b/Zfar = 1，解方程得： a = Zfar/(Zfar-Znear) ,b = - Znear*Zfar / (Zfar- Znear)


## UE中计算投影矩阵的类
1. FPerspectiveMatrix
2. FReversedZPerspectiveMatrix
   1. 近平面z映射到1，远平面映射到0，代入到a+ b/Znear = 1; a+b/Zfar = 0,解方程得到 a = Znear/(Znear-Zfar), b= - Znear*Zfar/(Znear-Zfar).

__注意__ :第一个参数(halfFOV)是指X方向上的




##opengl相关矩阵
glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.3f, 1000.0f);

◆ perspective()
`GLM_FUNC_DECL mat<4, 4, T, defaultp> glm::perspective	(	T 	fovy,
T 	aspect,
T 	near,
T 	far 
)`		
Creates a matrix for a symetric perspective-view frustum based on the default handedness and default near and far clip planes definition.

To change default handedness use GLM_FORCE_LEFT_HANDED. To change default near and far clip planes definition use GLM_FORCE_DEPTH_ZERO_TO_ONE.

Parameters
1.	fovy	Specifies the field of view angle in the y direction. Expressed in radians.
2.	aspect	Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
3.	near	Specifies the distance from the viewer to the near clipping plane (always positive).
4.	far	Specifies the distance from the viewer to the far clipping plane (always positive).
Template Parameters
5.	T	A floating-point scalar type


推导过程
__注意__:opengl从camera space到NDC space的时候，简单的进行z值取反，来实现从右手坐标到左手坐标系的变换。就是简单的z值取反，不涉及x，y轴方向的反转

