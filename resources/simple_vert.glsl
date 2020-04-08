#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 fragNor;
out vec3 WPos;
out vec3 fragLight;
out vec3 H;

void main()
{
	gl_Position = P * V * M * vertPos;
	fragNor = (V * M * vec4(vertNor, 0.0)).xyz;
	WPos = vec3(V * M * vertPos);
	fragLight = vec3(1, 1, 1);
	vec3 eye = -1 * (V * M * vertPos).xyz;
	H = (normalize(eye) + normalize(fragLight)) / 2.0;
}
