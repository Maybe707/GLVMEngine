#version 450

layout(location = 0) out vec4 outColor;

void main()
{             
  //	gl_FragDepth = gl_FragCoord.x;
	outColor = vec4(0.0, 0.35, 0.0, 0.0);
}

