# Advanced GLSL

## GLSL's built-in variables

### Vertex shader variables

- gl_Position

    gl_Position is the clip-space output position vector of the vertex shader.

    Setting gl_Position in the vertex shader is a strict requirement if you want to render anything on the screen.

- gl_PointSize

    gl_PointSize is a float variable where you can set the point's width and height in pixels.

    By setting the point's size in the vertex shader we get per-vertex control over this point's dimensions.

    Influencing the point sizes in the vertex shader is disabled by default, but if you want to enable this you'll have to enable OpenGL's GL_PROGRAM_POINT_SIZE.

- gl_VertexID

    The integer variable gl_VertexID holds the current ID of the vertex we're drawing.

    When doing indexed rendering (with glDrawElements) this variable holds the current index of the vertex we're drawing.

    When drawing without indices (via glDrawArrays) this variable holds the number of the currently processed vertex since the start of the render call.

### Fragment shader variables

- gl_FragCoord

    the z component of the gl_FragCoord vector is equal to the depth value of that particular fragment.

    The gl_FragCoord's x and y component are the window- or screen-space coordinates of the fragment, originating from the bottom-left of the window.

    We specified a render window of 800x600 with glViewport so the screen-space coordinates of the fragment will have x values between 0 and 800, and y values between 0 and 600.

- gl_FrontFacing

    The gl_FrontFacing variable is a bool that is true if the fragment is part of a front face and false otherwise.

- gl_FragDepth

    GLSL gives us an output variable called gl_FragDepth that we can use to manually set the depth value of the fragment within the shader.

    From OpenGL 4.2 however, we can still sort of mediate between both sides by redeclaring the gl_FragDepth variable at the top of the fragment shader with a depth condition: ``layout (depth_<condition>) out float gl_FragDepth;``

    |Condition|Description|
    |---|---|
    |any|     The default value. Early depth testing is disabled.|
    |greater|You can only make the depth value larger compared to gl_FragCoord.z.|
    |less|  You can only make the depth value smaller compared to gl_FragCoord.z.|
    |unchanged|If you write to gl_FragDepth, you will write exactly gl_FragCoord.z.|

## Interface blocks

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT
{
    vec2 TexCoords;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.TexCoords = aTexCoords;
}  
```

```glsl
#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture;

void main()
{
    FragColor = texture(texture, fs_in.TexCoords);
}
```

## Uniform buffer objects

OpenGL gives us a tool called uniform buffer objects that allow us to declare a set of global uniform variables that remain the same over any number of shader programs. When using uniform buffer objects we set the relevant uniforms only once in fixed GPU memory.

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}  
```

### Uniform block layout

|Type|Layout rule|
|---|---|
|Scalar e.g. int or bool|Each scalar has a base alignment of N.|
|Vector|          Either 2N or 4N. This means that a vec3 has a base alignment of 4N.|
|Array of scalars or vectors|Each element has a base alignment equal to that of a vec4.|
|Matrices|    Stored as a large array of column vectors, where each of those vectors has a base alignment of vec4.|
|Struct|     Equal to the computed size of its elements according to the previous rules, but padded to a multiple of the size of a vec4.|

```glsl
layout (std140) uniform ExampleBlock
{
                     // base alignment  // aligned offset
    float value;     // 4               // 0
    vec3 vector;     // 16              // 16  (offset must be multiple of 16 so 4->16)
    mat4 matrix;     // 16              // 32  (column 0)
                     // 16              // 48  (column 1)
                     // 16              // 64  (column 2)
                     // 16              // 80  (column 3)
    float values[3]; // 16              // 96  (values[0])
                     // 16              // 112 (values[1])
                     // 16              // 128 (values[2])
    bool boolean;    // 4               // 144
    int integer;     // 4               // 148
};
```

### Using uniform buffers

```c++
unsigned int uboExampleBlock;
glGenBuffers(1, &uboExampleBlock);
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

To set a shader uniform block to a specific binding point we call glUniformBlockBinding that takes a program object, a uniform block index, and the binding point to link to. The uniform block index is a location index of the defined uniform block in the shader. This can be retrieved via a call to glGetUniformBlockIndex that accepts a program object and the name of the uniform block.

```c++
unsigned int lights_index = glGetUniformBlockIndex(shaderA.ID, "Lights");
glUniformBlockBinding(shaderA.ID, lights_index, 2);
```

The function glBindbufferBase expects a target, a binding point index and a uniform buffer object. This function links uboExampleBlock to binding point 2; from this point on, both sides of the binding point are linked. You can also use glBindBufferRange that expects an extra offset and size parameter - this way you can bind only a specific range of the uniform buffer to a binding point. Using glBindBufferRange you could have multiple different uniform blocks linked to a single uniform buffer object.

```c++
glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboExampleBlock);
// or
glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboExampleBlock, 0, 152);
```

```c++
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
int b = true; // bools in GLSL are represented as 4 bytes, so we store it in an integer
glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &b);
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```
