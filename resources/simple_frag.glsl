#version 330 core 
in vec3 fragNor;
in vec3 WPos;
in vec3 fragLight;
in vec3 H;
//to send the color to a frame buffer
layout(location = 0) out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;

/* Very simple Diffuse shader with a directional light*/
void main()
{
	//vec3 Dcolor, Scolor;
    //vec3 Dlight = vec3(1, 1, 1);
	vec3 normal = normalize(fragNor);
	//Dcolor = MatDif*max(dot(normalize(Dlight), normal), 0)+MatAmb;
	//color = vec4(Dcolor, 1.0);
	
    vec3 lightColor = vec3(1, 1, 1);
	vec3 light = normalize(fragLight);
	vec3 h = normalize(H);
	
	//color = vec4(pow(max(0, dot(H, normal)), shine) * MatSpec, 1.0);
	color = vec4(MatDif * max(0, dot(normal, light)) * lightColor
		+ MatSpec * pow(max(0, dot(h, normal)), shine) * lightColor
		+ MatAmb * lightColor, 1.0);
}
