#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect src_tex_unit0;
uniform float blurRange;
uniform float thresh;
uniform int  bDoThresh;



void main( void )
{  
	
	vec2 st = gl_TexCoord[0].st;
	float sum = 0.0; 
	float blR = blurRange;
	
	
	sum += ((texture2DRect (src_tex_unit0, st + vec2 (-blR, -1.0)  )).r );
	sum += ((texture2DRect (src_tex_unit0, st + vec2 (-blR,  0.0)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 (-blR,  1.0)  )).r ); 
	
	sum += ((texture2DRect (src_tex_unit0, st + vec2 (-1.0, -blR)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 (-1.0, -1.0)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 (-1.0,  0.0)  )).r );
	sum += ((texture2DRect (src_tex_unit0, st + vec2 (-1.0,  1.0)  )).r );
	sum += ((texture2DRect (src_tex_unit0, st + vec2 (-1.0,  blR)  )).r );
	
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 0.0, -3.0)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 0.0, -1.0)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st                      )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 0.0,  1.0)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 0.0,  3.0)  )).r );
	
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 1.0, -blR)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 1.0, -1.0)  )).r );
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 1.0,  0.0)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 1.0,  1.0)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( 1.0,  blR)  )).r );
	
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( blR, -1.0)  )).r ); 
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( blR,  0.0)  )).r );
	sum += ((texture2DRect (src_tex_unit0, st + vec2 ( blR,  1.0)  )).r ); 
	
	if (bDoThresh == 1){
		if (sum > (thresh*21.0)){
			gl_FragColor = vec4 (1,1,1,1);
		} else {
			gl_FragColor = vec4 (0,0,0,1);

			
			
			
			/*
			float gray = sum / 21.0;
			gray = gray * gray * gray * gray;
			gl_FragColor = vec4 (gray, gray, gray, 1.0); 
			*/
		
		}
	} else {
	
		float gray = sum / 21.0;
		gl_FragColor = vec4 (gray, gray, gray, 1.0); 
	}
	
	
}	
	
	
	
	/* -----------------------------
	// True median sorting 
	
	// at top
#define toVec(x) x.rgb

#define s2(a, b)				temp = a; a = min(a, b); b = max(temp, b);
#define mn3(a, b, c)			s2(a, b); s2(a, c);
#define mx3(a, b, c)			s2(b, c); s2(a, c);

#define mnmx3(a, b, c)			mx3(a, b, c); s2(a, b);                                   // 3 exchanges
#define mnmx4(a, b, c, d)		s2(a, b); s2(c, d); s2(a, c); s2(b, d);                   // 4 exchanges
#define mnmx5(a, b, c, d, e)	s2(a, b); s2(c, d); mn3(a, c, e); mx3(b, d, e);           // 6 exchanges
#define mnmx6(a, b, c, d, e, f) s2(a, d); s2(b, e); s2(c, f); mn3(a, b, c); mx3(d, e, f); // 7 exchanges

	// in main
	float temp;

	// Starting with a subset of size 6, remove the min and max each time
	mnmx6(v[0], v[1], v[2], v[3], v[4], v[5]);
	mnmx5(v[1], v[2], v[3], v[4], v[6]);
	mnmx4(v[2], v[3], v[4], v[7]);
	mnmx3(v[3], v[4], v[8]);
	
	gl_FragColor.r = v[4];
	gl_FragColor.g = v[4];
	gl_FragColor.b = v[4];
	gl_FragColor.a = 1.0;
	
	if (bDoThresh == 1){
		if (gl_FragColor.r < thresh){
		    gl_FragColor = vec4 (0,0,0,1); 
		} else {
			gl_FragColor = vec4 (1,1,1,1);
		}
	}
	*/
	
	
	/* ---------------------------------
	// Simple pass thru
	vec2 st = gl_TexCoord[0].st;
	vec4 color;
	color += texture2DRect(src_tex_unit0, st);
	gl_FragColor = color;
	*/
	
	
	
	/* -------------------------
	// Simple gaussian blur
	vec2 st = gl_TexCoord[0].st;
	
	//vertical blur 
	//from http://www.gamerendering.com/2008/10/11/gaussian-blur-filter-shader/
	
	vec4 color;
	
	color		+=  7.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt *  2.0));
	color		+= 15.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt *  1.0));	
	color		+= 20.0 * texture2DRect(src_tex_unit0, st                             );
	color		+= 15.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * -1.0));
	color		+=  7.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * -2.0));
	
	color /= 64.0;
	gl_FragColor = color;
	
	if (bDoThresh == 1){
		if (gl_FragColor.r < thresh){
		    gl_FragColor = vec4 (0,0,0,1); 
		} else {
			gl_FragColor = vec4 (1,1,1,1);
		}
	}
	*/
	



