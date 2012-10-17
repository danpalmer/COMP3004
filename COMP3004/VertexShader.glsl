#version 150
precision highp float;

in vec3 in_Position;
in vec3 in_Color;

uniform mat4 mvpmatrix;

out vec3 ex_Color;

void main(void) {
	gl_Position = mvpmatrix * vec4(in_Position, 1.0);
	ex_Color = in_Color;
}
