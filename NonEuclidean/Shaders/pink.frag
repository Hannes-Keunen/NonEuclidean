#version 150
precision highp float;

//Inputs
uniform sampler2D tex;

//Outputs
out vec4 color;

void main(void) {
	color = vec4(1.0, 0.0, 1.0, 1.0);
}
