# LearnOpenGL

Follow https://learnopengl.com/

Use CMake to make & build projects in each section

Only test & build on windows

```
mkdir build
cd build
cmake ..
cmake --build .
```

## FAQ

1. CL.exe not found??

    Open 'Developer Command Prompt for VS 2019' in Windows finder and Open code there(e.g. type 'code .' in cmd), through the opened vscode editor File-Open Folder... and select your project folder.

    Reference [<https://code.visualstudio.com/docs/cpp/config-msvc>]

2. glslangValidator require??

    Using vscode extensions GLSL-Lint require glslangValidator, download link [<https://github.com/KhronosGroup/glslang/releases/tag/master-tot>].
