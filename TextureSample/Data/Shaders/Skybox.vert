#version 450

layout( location = 0 ) in vec4 appPosition;

layout( set = 0, binding = 0) uniform uniformBuffer
{
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};

layout( location = 0) out vec3 vertTexcoord;

void main()
{
	vec3 position = mat3(modelViewMatrix) * appPosition.xyz;
	gl_Position = (projectionMatrix * vec4(position.xyz, 1.0f)).xyzz;
	vertTexcoord = appPosition.xyz;
}

