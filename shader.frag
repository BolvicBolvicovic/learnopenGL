#version 330 core

#define PI 3.14159265359

uniform vec2		u_Resolution;
uniform float		u_Time;
uniform	sampler2D	u_Texture1;
uniform	sampler2D	u_Texture2;

in vec3		ourColor;
in vec2		texCoord;

float
plot(vec2 st, float pct)
{
	return	smoothstep(pct - 0.02, pct, st.y) -
			smoothstep(pct, pct + 0.02, st.y);
}

void
main()
{
	vec2	st		= gl_FragCoord.xy / u_Resolution;

	float	y		= pow(st.x, 5.0);

	vec3	color	= vec3(y);
	float	pct		= plot(st, y);

	color 			= ourColor + pct * vec3(1.0);
	gl_FragColor	= vec4(color, 1.0);
	gl_FragColor	*= mix(texture(u_Texture1, texCoord), texture(u_Texture2, texCoord), 0.4);
//	gl_FragColor = vec4(ourColor, 1.0);
//	gl_FragColor = vec4(abs(sin(ourColor * vec3(u_Time))), 1.0);
//	gl_FragColor = vec4(gl_FragCoord.xy / u_Resolution, abs(sin(u_Time)), 1.0);
}
