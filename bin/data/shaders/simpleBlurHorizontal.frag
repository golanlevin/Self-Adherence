#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect src_tex_unit0;
uniform float blurAmnt;


void main( void )
{  
	
	
	//horizontal blur 
	//from http://www.gamerendering.com/2008/10/11/gaussian-blur-filter-shader/
	
	/*
	vec2 st = gl_TexCoord[0].st;
	vec4 color;
	
	color		+=  7.0 * texture2DRect(src_tex_unit0, st + vec2(blurAmnt * -2.0, 0.0));
	color		+= 15.0 * texture2DRect(src_tex_unit0, st + vec2(blurAmnt * -1.0, 0.0));	
	color		+= 20.0 * texture2DRect(src_tex_unit0, st                             );
	color		+= 15.0 * texture2DRect(src_tex_unit0, st + vec2(blurAmnt *  1.0, 0.0));
	color		+=  7.0 * texture2DRect(src_tex_unit0, st + vec2(blurAmnt *  2.0, 0.0));

	color /= 64.0;
	gl_FragColor = color;
	*/
	
	vec2 st = gl_TexCoord[0].st;
	vec4 color;
	color += texture2DRect(src_tex_unit0, st);
	gl_FragColor = color;
}