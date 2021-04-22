#version 330 core
layout (location = 3) in vec3 bPos;
layout (location = 4) in vec2 tex2;

out vec2 Tex2;
out vec3 realPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
     gl_Position = projection * view * vec4(bPos, 1.0);
     Tex2 = tex2;
     realPos = vec3(model * vec4(bPos,1.0));
     Normal = mat3(transpose(inverse(model))) * vec3(0, 1, 0);
}