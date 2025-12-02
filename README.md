# Skyscape - 现代 OpenGL 飞行模拟系统

Skyscape 是一个基于 C++17 和 OpenGL 3.3+ 开发的高性能飞行模拟引擎。本项目展示了现代图形编程的核心技术，包括程序化无限地形生成、自定义模型加载、天空盒渲染以及基于物理的摄像机控制。

![OpenGL](https://img.shields.io/badge/OpenGL-3.3+-blue.svg)
![C++](https://img.shields.io/badge/C++-17-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)

## ✨ 核心功能

### 1. 🏔️ 无限地形系统 (Infinite Terrain)
- **核心技术**：基于 Perlin Noise（柏林噪声）的程序化生成算法。
- **动态加载**：采用分块（Chunk）管理机制，根据摄像机位置动态生成前方地形并卸载后方地形，实现理论上的无限飞行。
- **高度着色**：
  - **雪山** (> 60m)：白色，高光反射强。
  - **岩石** (40-60m)：灰棕色，粗糙质感。
  - **森林** (15-40m)：深绿色。
  - **草地** (0-15m)：嫩绿色。
  - **沙滩** (-5 到 0m)：黄色。
  - **水域** (< -5m)：深蓝色。
- **视觉增强**：实现了基于深度的距离雾（Distance Fog），使远处地形平滑融入天空。

### 2. ✈️ 战斗机模拟 (F-22 Raptor)
- **模型加载**：集成了自定义 OBJ 解析器，能够加载复杂的 F-22 战斗机模型。
- **渲染效果**：使用 Phong 光照模型，模拟金属机身的漫反射和高光。
- **飞行控制**：实现了平滑的跟随摄像机，模拟真实飞行的惯性。

### 3. 🌅 沉浸式环境
- **天空盒**：使用 6 面立方体贴图（Cubemap）渲染无缝天空。
- **光照系统**：全局方向光模拟太阳光照。

## 🎮 操作指南

| 按键 | 功能 | 说明 |
|------|------|------|
| **W** | 前进 | 飞机向前加速飞行 |
| **S** | 后退 | 减速 |
| **A** | 左转 | 向左偏航 |
| **D** | 右转 | 向右偏航 |
| **Shift** | 加速 | 开启加力模式 (800 单位/秒) |
| **鼠标** | 视角 | 控制飞机的俯仰和翻滚 |
| **Esc** | 退出 | 关闭程序 |

## 📁 项目结构详解

本项目采用模块化架构，代码结构清晰：

### `src/` - 源代码目录

#### `src/core/` - 核心基础设施
- **`Window.cpp/h`**: 封装了 GLFW 窗口创建、OpenGL 上下文初始化、视口调整回调。它是程序的"骨架"。
- **`Input.h`**: (如有) 处理键盘和鼠标输入状态。

#### `src/graphics/` - 图形渲染引擎
- **`Shader.cpp/h`**: 核心着色器类。负责读取 GLSL 文件、编译顶点/片段着色器、链接程序，并提供设置 Uniform 变量（`setBool`, `setInt`, `setMat4` 等）的接口。
- **`Camera.h`**: 摄像机类。处理视图矩阵（View Matrix）和投影矩阵（Projection Matrix）的计算，实现 FPS 风格或跟随风格的视角控制。
- **`Mesh.h`**: (如有) 定义顶点数据结构和 VAO/VBO 的绑定逻辑。

#### `src/world/` - 游戏世界逻辑
- **`InfiniteTerrain.cpp/h`**: **核心文件**。
  - 维护一个 `std::map` 存储当前活动的 terrain chunks。
  - `update()` 函数每帧计算玩家所在的网格坐标，动态添加新块、移除旧块。
  - `getHeight()` 函数使用噪声算法计算地形高度。
- **`Plane.cpp/h`**: 
  - 负责加载 `assets/models/airplane.obj`。
  - 解析 OBJ 文件格式（顶点 v、法线 vn、面 f）。
  - 处理飞机的渲染和变换矩阵。
- **`Skybox.cpp/h`**: 加载立方体贴图纹理并渲染天空盒。

#### `src/main.cpp` - 程序入口
- 初始化窗口和 OpenGL。
- 配置全局状态（深度测试 `glEnable(GL_DEPTH_TEST)`）。
- 主渲染循环（Render Loop）：处理输入 -> 更新逻辑 -> 清除屏幕 -> 渲染场景 -> 交换缓冲区。

### `assets/` - 资源目录

- **`shaders/`**: GLSL 着色器代码。
  - `infinite_terrain.vert/frag`: 地形的高度着色和雾效逻辑。
  - `plane.vert/frag`: 飞机的光照渲染。
  - `skybox.vert/frag`: 天空盒渲染。
- **`models/`**: 3D 模型文件（.obj）。
- **`textures/`**: 纹理图片（.jpg, .png）。

## 🛠️ 构建与运行

### 环境要求
- Windows 10/11
- Visual Studio 2019/2022 (MSVC)
- CMake 3.15+
- 显卡支持 OpenGL 3.3

### 依赖库 (Vendor)
- **GLFW**: 窗口管理 (自动下载)
- **GLM**: 数学库 (自动下载)
- **STB**: 图像加载 (自动下载)
- **GLAD**: OpenGL 加载器 (**需手动配置**)

### 构建步骤

1. **配置 GLAD** (如果 `vendor/glad` 为空):
   - 访问 [glad.dav1d.de](https://glad.dav1d.de/)
   - Profile: **Core**, Version: **3.3**
   - Generate -> Download -> 解压到 `vendor/glad`

2. **编译项目**:
   ```powershell
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Debug
   ```

3. **运行**:
   ```powershell
   .\bin\Debug\Skyscape.exe
   ```

## 🎨 进阶自定义

想要更换更酷的飞机模型？想要给地形贴上真实的卫星地图？
请查看我们详细的自定义指南：

👉 **[CUSTOMIZATION.md](./CUSTOMIZATION.md)**

---
**Skyscape Team** | 2025
