## Getting Started
Recommended IDE is Visual Studio 2019.

Run **GenerateProjects.bat**. Open solution in the Visual Studio 2019.

To try out demo check the Demo folder.

## Description
This engine currently supports OpenGL API and oriented for Windows platforms only. With further extend to DirectX(for sure) and Linux compatibility(probable). As mentioned in the description for this repository the work is still in progress.

The engine employs a bunch of systems vital for any game engine software such as:
- Renderer system
- Physics system(collision detection only)
- Particle system
- Audio system

For more efficient manipulation with game objects entity component system is made used. For shading programs GLSL is used.

Third party middleware:
- GLFW
- GLM
- ImGui
