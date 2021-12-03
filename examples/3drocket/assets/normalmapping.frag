#version 410

in vec2 fragTexCoord;

in vec3 fragPObj;
in vec3 fragTObj;
in vec3 fragBObj;
in vec3 fragNObj;
in vec3 fragLEye;
in vec3 fragVEye;

uniform mat3 normalMatrix;

// Light properties
uniform vec4 Ia, Id, Is;

// Material properties
uniform vec4 Ka, Kd, Ks;
uniform float shininess;

// Diffuse map sampler
uniform sampler2D diffuseTex;

// Normal map sampler
uniform sampler2D normalTex;

out vec4 outColor;

// Compute matrix to transform from camera space to tangent space
mat3 ComputeTBN(vec3 TObj, vec3 BObj, vec3 NObj) {
  vec3 TEye = normalMatrix * normalize(TObj);
  vec3 BEye = normalMatrix * normalize(BObj);
  vec3 NEye = normalMatrix * normalize(NObj);
  return mat3(TEye.x, BEye.x, NEye.x, TEye.y, BEye.y, NEye.y, TEye.z, BEye.z,
              NEye.z);
}

// Blinn-Phong reflection model
vec4 BlinnPhong(vec3 N, vec3 L, vec3 V, vec2 texCoord) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    V = normalize(V);
    vec3 H = normalize(L + V);
    float angle = max(dot(H, N), 0.0);
    specular = pow(angle, shininess);
  }

  vec4 map_Kd = texture(diffuseTex, texCoord);
  vec4 map_Ka = map_Kd;

  vec4 diffuseColor = map_Kd * Kd * Id * lambertian;
  vec4 specularColor = Ks * Is * specular;
  vec4 ambientColor = map_Ka * Ka * Ia;

  return ambientColor + diffuseColor + specularColor;
}

// Planar mapping
vec2 PlanarMappingXUV(vec3 P) { return vec2(1.0 - P.z, P.y); }
mat3 PlanarMappingXTBN(vec3 P) {
  vec3 T = vec3(0, 0, -1);
  vec3 N = fragNObj;
  vec3 B = cross(N, T);
  return ComputeTBN(T, B, N);
}

vec2 PlanarMappingYUV(vec3 P) { return vec2(P.x, 1.0 - P.z); }
mat3 PlanarMappingYTBN(vec3 P) {
  vec3 T = vec3(1, 0, 0);
  vec3 N = fragNObj;
  vec3 B = cross(N, T);
  return ComputeTBN(T, B, N);
}

vec2 PlanarMappingZUV(vec3 P) { return P.xy; }
mat3 PlanarMappingZTBN(vec3 P) {
  vec3 T = vec3(1, 0, 0);
  vec3 N = fragNObj;
  vec3 B = cross(N, T);
  return ComputeTBN(T, B, N);
}

#define PI 3.14159265358979323846

// Cylindrical mapping
vec2 CylindricalUV(vec3 P) {
  float longitude = atan(P.x, P.z);
  float height = P.y;

  float u = longitude / (2.0 * PI) + 0.5;  // From [-pi, pi] to [0, 1]
  float v = height - 0.5;                  // Base at y = -0.5

  return vec2(u, v);
}

mat3 CylindricalTBN(vec3 P) {
  vec3 T = vec3(P.z, 0, -P.x);
  vec3 N = fragNObj;
  vec3 B = cross(N, T);
  return ComputeTBN(T, B, N);
}

// Spherical mapping
vec2 SphericalUV(vec3 P) {
  float longitude = atan(P.x, P.z);
  float latitude = asin(P.y / length(P));

  float u = longitude / (2.0 * PI) + 0.5;  // From [-pi, pi] to [0, 1]
  float v = latitude / PI + 0.5;           // From [-pi/2, pi/2] to [0, 1]

  return vec2(u, v);
}

mat3 SphericalTBN(vec3 P) {
  vec3 T = vec3(P.z, 0, -P.x);
  vec3 N = fragNObj;
  vec3 B = cross(N, T);
  return ComputeTBN(T, B, N);
}

void main() {
  vec4 color;

  vec2 texCoord = fragTexCoord;
  mat3 TBN = ComputeTBN(fragTObj, fragBObj, fragNObj);

  // Compute tangent space vectors
  vec3 LTan = TBN * normalize(fragLEye);
  vec3 VTan = TBN * normalize(fragVEye);
  vec3 NTan = texture(normalTex, texCoord).xyz;
  NTan = normalize(NTan * 2.0 - 1.0);  // From [0, 1] to [-1, 1]

  color = BlinnPhong(NTan, LTan, VTan, texCoord);

  if (gl_FrontFacing) {
    outColor = color;
  } else {
    float i = (color.r + color.g + color.b) / 3.0;
    outColor = vec4(i, 0, 0, 1.0);
  }
}
