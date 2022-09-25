#version 330 core
out vec4 FragColor;

uniform sampler2D renderTexture;
uniform sampler2D addTexture;
uniform bool renderMix;
uniform mat4 projection;
uniform mat4 view;
// uniform mat4 WorldToNDC;


in vec2 TexCoords;
// float near_plane = 0.1f;
// float far_plane = 100.0f;

// // required when using a perspective projection matrix
// float LinearizeDepth(float depth)
// {
//     float z = depth * 2.0 - 1.0; // Back to NDC 
//     return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
// }

// vec3 perspectiveColor(float depthValue)
// {
//     return vec3(LinearizeDepth(depthValue) / far_plane); // perspective
// }


void main()
{
    mat4 WorldToNDC1 = projection * view;
    if (renderMix)
    {
        vec3 renderPosition = vec3(texture(renderTexture, TexCoords));
        if (length(renderPosition) <= 0)
        {
            discard;
        }
        vec4 viewPos = WorldToNDC1 * vec4(renderPosition, 1.0f);
        vec3 pointPosition = vec3(texture(addTexture, TexCoords));
        vec4 pointViewPos = WorldToNDC1 * vec4(pointPosition, 1.0f);
        float pointZ = pointViewPos.z;
        float viewZ = viewPos.z;
        if (length(pointPosition) <= 0)
        {
            FragColor = vec4(vec3(renderPosition), 1.0f);
        }
        else
        {
            if (pointZ <= viewZ)
            {
                FragColor = vec4(vec3(0.0f), 1.0f);
            }
            else
            {
                FragColor = vec4(vec3(renderPosition), 1.0f);
            }
        }
        // FragColor = vec4(vec3(viewPos), 1.0f);
    }
    else
    {
        FragColor = vec4(texture(renderTexture, TexCoords).rgb, 1.0);

    }
}