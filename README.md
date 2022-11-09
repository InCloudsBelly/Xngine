# Xngine
### Xngine: A Personal Rendering Engine

<div align=center>
<img src="pic/1.png"> 

图1 算法步骤
</div>


## Build
```shell
git clone --recursive https://github.com/MustNotBeGreasy/Xngine.git
cd Xngine
run Win-GenProjects.bat
```
### Platform: &nbsp;Only Windows yet.
### Rendering API: &nbsp; OpenGL 4.5&nbsp;&nbsp;&nbsp;TODO:&nbsp;&nbsp;Vulkan,D3D12
<br>

## Features:
- [x] **Renderer System**
- [x] **Resource Manager**
- [x] **Primative ECS System**
- [x] **UI by ImGui**
- [x] **MSAA & Some PostProcessing**

TODO: **Graphic Alogorithm**
- [ ] **G buffer**
- [ ] **FXAA Fast Approximate Antialiasing**
- [ ] **RSM Reflective Shadow Map**
- [ ] **Stochastic Light Culling**
- [ ] **SSAO Screen Space Ambient Occlusion**
- [ ] **Image Space Horizon Based Ambient Occlusion**
- [ ] **SSR Screen Space Reflections**
- [ ] **SH Spherical Harmonic Lighting**
- [ ] **Tile Shading**
- [ ] **UI by ImGui**
- [ ] **MSAA & Some PostProcessing**
- [ ] **...**

### 代码规范
Xngine 代码规范主要参考Unreal和HEngine(github)，具体可参考：
https://docs.unrealengine.com/4.27/zh-CN/ProductionPipelines/DevelopmentSetup/CodingStandard/

原则：（后面的规则优先级高于前面的规则）
1. 尽量不使用下划线
1. 变量一律小写字母开头
1. 非类内静态变量（全局变量、局部变量），在前面加小写字母s
1. 如果是类内部成员变量，在前面加小写字母m
1. 如果是bool类型变量，在前面加小写字母b
1. 类内成员统一放在类的最末尾（方法置于前）
1. 默认情况下 std::string 为相对路径，而 std::filesystem::path 为绝对路径，相对路径到绝对路径需要由 AssetManager::GetFullPath() 去获取

## Credits
* Cherno [Hazel](https://github.com/TheCherno/Hazel)
* BoomingTech [Pilot](https://github.com/BoomingTech/Pilot)
* [LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL)
* [HEngine](https://github.com/hebohang/HEngine)
* [MoravaEngine](https://github.com/dtrajko/MoravaEngine)
