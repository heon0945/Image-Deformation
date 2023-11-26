#version 150 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D myTexture;
uniform vec2 screen;

uniform float pointSize;
//uniform float movingRange;
uniform float changeMovingRange;
uniform float divInput;
uniform int pointnum;

uniform vec2 controls[50];
uniform vec2 ranges[50];
uniform vec2 deformDir[50];

uniform int deleteIndex;

void main(void)
{	

	vec2 calTexCoord = TexCoord;

	//if it is control point area
	bool areaC = false;
	bool areaM = false;

	float dist[50];
	//vec3 newcol;

	for(int i = 0; i < pointnum; i++){
		dist[i] = length(gl_FragCoord.xy-controls[i]);
		
		float deformRange = 60;

		if(dist[i] < pointSize) 
			areaC = true;
		else if (length(gl_FragCoord.xy-ranges[i]) < changeMovingRange) 
			areaM = true;

		// recalculate Texture Coordinate considering deformation
		if (dist[i] < deformRange) {

			//gaussian calculation -> weight
			float weight;
			float sigma = deformRange / 3;

			weight = (1 / (sigma * sqrt(2 * 3.141592f))) * exp(( -1 / 2 ) * pow( (sigma/dist[i]), 2));	

			float effectDist = (deformRange - dist[i]) / divInput;
		
			calTexCoord = vec2((gl_FragCoord.x - deformDir[i].x * weight * effectDist) / screen.x,(gl_FragCoord.y - deformDir[i].y * weight * effectDist) / screen.y);

		}
	}
	
	if (deleteIndex != -1){
		for (int i = deleteIndex; i<pointnum - 1; i++){
			dist[i] = dist[i+1];
		}
	}

	//make control point
	if(areaC)
		FragColor = vec4(mix(vec4(texture(myTexture, calTexCoord).rgb, 1).rgb, vec3(1, 1, 1), 0.5), 1);
	//make control range
	//else if(areaM)
		//FragColor = vec4(mix(vec4(texture(myTexture, calTexCoord).rgb, 1).rgb, vec3(1, 1, 1), 0.2), 1);
	//rest of the image
	else	
		FragColor = vec4(texture(myTexture, calTexCoord).rgb, 1);


}
