#version 450

layout( location = 0 ) in vec3 vertTexcoord;

layout( set = 0, binding = 1 ) uniform samplerCube Cubemap;

layout( location = 0 ) out vec4 fragColor;

void main()
{
	fragColor = texture(Cubemap, vertTexcoord);
}