#version 330 core

uniform vec2	u_Resolution;
uniform float	u_Time;

in vec3		ourColor;

void main()
{
	gl_FragColor = vec4(ourColor, 1.0);
//	gl_FragColor = vec4(abs(sin(ourColor * vec3(u_Time))), 1.0);
//	gl_FragColor = vec4(gl_FragCoord.xy / u_Resolution, abs(sin(u_Time)), 1.0);
}
