//Output
out vec4 FragColor;

//Uniform
uniform vec4 u_light_color;

void main()
{
	FragColor = u_light_color;
}