#version 150 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D myTexture;
uniform vec2 screen;

uniform float pointSize;
uniform float changeMovingRange;
uniform float divInput;
uniform int pointnum;

uniform vec2 controls[50];
uniform vec2 ranges[50];
uniform vec2 deformDir[50];

uniform int deleteIndex;
uniform int isclicked;
uniform int movingControl;
uniform int onRange;
uniform int onPoint;

void main(void)
{	

	vec2 calTexCoord = TexCoord;

	//if it is control point area
	bool areaC = false;
	bool areaM = false;

	float dist[50];

	int index = -1;

	for(int i = 0; i < pointnum; i++){
		dist[i] = length(gl_FragCoord.xy-controls[i]);
		
		float deformRange = 60;

		if(dist[i] < pointSize) {
			index = i;
			areaC = true;
		}
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
		for (int j = deleteIndex; j<pointnum - 1; j++){
			dist[j] = dist[j+1];
		}
	}

	//make control point
	if(areaC){
		if (onPoint == 0){
			FragColor =vec4(vec4(texture(myTexture, calTexCoord).rgb, 1).rgb, 1);
		} else {
			if(isclicked == 0 && index == movingControl)
				FragColor =vec4(mix(vec4(texture(myTexture, calTexCoord).rgb, 1).rgb, vec3(1, 0, 0), 0.5), 1);
			else 
				FragColor = vec4(mix(vec4(texture(myTexture, calTexCoord).rgb, 1).rgb, vec3(1, 1, 1), 0.5), 1);
		}

	}
	//make control range
	else if(areaM && onRange == 0)
		FragColor = vec4(mix(vec4(texture(myTexture, calTexCoord).rgb, 1).rgb, vec3(1, 1, 1), 0.2), 1);
	//rest of the image
	else	
		FragColor = vec4(texture(myTexture, calTexCoord).rgb, 1);


}

