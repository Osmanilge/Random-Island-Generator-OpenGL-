#version 150

in vec4 vNormal;
in vec4 vColor;
out vec4 color;
in vec4 vPosition;
uniform mat4 model_view;
uniform mat4 projection;
uniform mat4 plane;


uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;

uniform vec4 LightPosition;
uniform float Shininess;
void main() 
{	
// Transform vertex  position into eye coordinates
    vec3 pos = (model_view * vPosition).xyz;
	
    vec3 L = normalize( (LightPosition).xyz - pos ); //assuming light position provided in camera coordinates
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );
    // Transform vertex normal into eye coordinates
    vec3 N = normalize( model_view*vec4(vNormal.x,vNormal.y,vNormal.z, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;
    
    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 
	
   gl_Position = projection*model_view*vPosition;
   color = vColor+ambient + diffuse + specular;
    color.a = 1.0;
} 
