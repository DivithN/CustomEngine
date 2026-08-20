/*
	This is the standard fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>



// Constant Buffers
//=================


// Entry Point
//============
#if defined( EAE6320_PLATFORM_D3D )
void main(

	// Input
	//======

	in const float4 i_fragmentPosition : SV_POSITION,
	in float4 i_color : COLOR,

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

)
#elif defined( EAE6320_PLATFORM_GL )
out vec4 o_color;
layout( location = 1 ) in vec4 i_color;
void main()
#endif
{
	
	// Output solid white
	float4 calculatedColor = float4(
		// RGB (color)
		1.0,1.0, 1.0,
		// Alpha (opacity)
		1.0 );


	float4 combinedColor =Multiply(i_color,calculatedColor);
	float a = Multiply(i_color.a, calculatedColor.a);
	float r = Multiply(i_color.r, calculatedColor.r);
	float g = Multiply(i_color.g, calculatedColor.g);
	float b = Multiply(i_color.b, calculatedColor.b);
	o_color = float4(r,g,b,a);
}

