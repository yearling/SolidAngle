# UE操作手册

###  从命令行运行编辑器
UE4Editor.exe "[ProjectPath][ProjectName].uproject"

### 从可执行程序运行编辑器
[LauncherInstall][VersionNumber]\Engine\Binaries\Win64\UE4Editor.exe "[ProjectPath][ProjectName].uproject"

### 从命令行运行未烘焙的游戏
UE4Editor.exe "[ProjectPath][ProjectName].uproject" -game

### 从可执行程序运行未烘焙的游戏
[LauncherInstall][VersionNumber]\Engine\Binaries\Win64\UE4Editor.exe "[ProjectPath][ProjectName].uproject" -game

### 编译位置

| 配置	        |EXE 名称         | EXE 位置  |
| ------------- |:---------------------:|:-----|
| Development（开发）      | [ProjectName].exe | [PackageDirectory]\WindowsNoEditor\[ProjectName]\Binaries\Win64 |
| Shipping（发行）      | [ProjectName]-Win32-Shipping.exe.exe      |  	[PackageDirectory]\WindowsNoEditor\[ProjectName]\Binaries\Win32 |

### 控制台命令

| 命令	        |参数        
| ------------- |:---------------------:|
| 退出游戏 | EXIT/QUIT |
| 退出游戏；返回到主菜单 | DISCONNECT |
| 加载一张新地图 | OPEN [MapURL] |
| 加载一张新地图并重置所有设置| TRAVEL [MapURL] |
| 指定渲染模式| VIEWMODE [Mode]|
| 线框 | WIREFRAME |
| 无光照 | UNLIT |
| 带光照 | LIT |
| 细节光照 | DETAILLIGHTING |
| 仅光照 | LIGHTINGONLY |
| 光源复杂度 | Light Complexity |
| 着色器复杂度 | SHADERCOMPLEXITY |

### 在启动时载入地图
该地图在您游戏项目的Config目录下的DefaultEngine.ini配置文件中被定义

	[URL]
	...
	Map=/Game/Maps/VehicleMenu
	...

### 在编辑器中运行（PIE，play in editor)
- 点击play
- Shift+F1 弹出鼠标   
- 右键单击 关卡编辑器视口中的文本，然后在弹出菜单中选择 保存模拟更改（Keep Simulation Changes）。默认情况下，您在运行会话期间对关卡中的 Actor 所做的任何值更改都会在该会话结束时还原（从而防止意外覆盖原来的值）。

### 在编辑器中模拟（SIE,simulate in editor)

### 主界面快捷键
| 快捷键	        |操作        
| ------------- |:---------------------:|
| MMB + Drag (on Pivot) | Moves the pivot of the selection temporarily to offset transformations. |
| Ctrl + W (on an Actor) | Duplicates the selected Actor. |
| H (on an Actor) | Hides the currently selected Actor.|
| Ctrl + H | Un-hides all Actors.|
| Shift + E (on an Actor)| Selects all matching Actors in the level of the same type as the selected Actor.|

### 将光源锁定到视图
在‘视口选项’-->lock viewport to Ator 在12.0的版本中找不到这个功能了，只能在actor的右键菜单中看到sanp view to object 或者 snap object to view

### 调整视口
 Alt 和 G 、H 、 J 或 K 键来循环调换视口类型。这些操作将会把视口分别设置为 透视口、前视口、顶视口或侧视口