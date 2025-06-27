#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_textureCoord;

out vec2 v_textureCoord;

uniform mat4 u_transformMatrix;

void main()
{
  gl_Position = u_transformMatrix * vec4(a_pos, 1.0);
  v_textureCoord = a_textureCoord;
}