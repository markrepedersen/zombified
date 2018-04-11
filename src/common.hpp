#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <vector>

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>
//#include "../ext/glfw/include/GLFW/glfw3.h"

#define DISABLE_SDL

// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
#ifdef _MSC_VER
  #define data_path "../data"
  #define shader_path(name) "../shaders/" name
#else
#define data_path "data"
#define shader_path(name) "shaders/" name
#endif
#define textures_path(name) data_path "/textures/" name
#define p1_textures_path(name) data_path "/textures/p1/" name
#define p2_textures_path(name) data_path "/textures/p2/" name
#define zombie_textures_path(name) data_path "/textures/ai/" name
#define tools_textures_path(name) data_path "/textures/tools/" name
#define effects_textures_path(name) data_path "/textures/effects/" name
#define startworld_textures_path(name) data_path "/textures/start screen/" name
#define background_textures_path(name) data_path "/textures/background/" name
#define audio_path(name) data_path "/audio/" name
#define mesh_path(name) data_path "/meshes/" name

// Not much math is needed and there are already way too many libraries linked (:
// If you want to do some overloads..
struct vec2 {
	float x, y;
	vec2(float x = 0, float y = 0) {
		this->x = x;
		this->y = y;
	}
	bool operator==(vec2 const& p) const {
		return this->x == p.x && this->y == p.y;
	}

	bool operator!=(vec2 const& p) const {
		return this->x != p.x || this->y != p.y;
	}

	bool operator<=(vec2 const& p) const {
		return this->x <= p.x && this->y <= p.y;
	}
    vec2& operator-(vec2 const& p) {
        this->x -= p.x;
        this->y -=p.y;
        return *this;
    }
	vec2& operator+=(vec2 const& p) {
		this->x += p.x;
		this->y += p.y;
		return *this;
	}
};
template<>
struct std::hash<vec2> {
	size_t operator()(const vec2 &pt) const {
		return std::hash<float>()(pt.x) ^ std::hash<float>()(pt.y);
	}
};

struct vec3 { float x, y, z; };
struct mat3 { vec3 c0, c1, c2; };

// Utility functions
float dot(vec2 l, vec2 r);
float dot(vec3 l, vec3 r);
mat3  mul(const mat3& l, const mat3& r);
vec2  normalize(vec2 v);
vec2 scale(float, vec2);
vec2 direction(vec2, vec2);
float distance(vec2, vec2);
float getDistance(vec2, vec2);
float getDistancePointToLine(vec2 a, vec2 b, vec2 c); //line ab to point c
vec2 subtractVectors(vec2 a, vec2 b);
vec2 sumVectors(vec2 a, vec2 b);
vec2 multiplyScalarVector(float a, vec2 b);
bool onSegment(vec2, vec2, vec2);
int orientation(vec2, vec2, vec2);
bool intersect(vec2, vec2);
bool isInsidePolygon(std::vector<vec2>, vec2);
std::vector<vec2> getIntersectionWithPoly(std::vector<vec2> poly, vec2 point, vec2 oldpoint);
vec2 getRandomPointInMap(std::vector<vec2> mapCollisionPoints, vec2 screen);



// OpenGL utilities
// cleans error buffer
void gl_flush_errors();
bool gl_has_errors();

// Single Vertex Buffer element for non-textured meshes (colored.vs.glsl)
struct Vertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Texture wrapper
struct Texture
{
	Texture();
	~Texture();

	GLuint id;
	int width;
	int height;
	
	// Loads texture from file specified by path
	bool load_from_file(const char* path);
	bool is_valid()const; // True if texture is valid
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and 
// an Index Buffer
struct Mesh
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
};

// Container for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Effect
{
	bool load_from_file(const char* vs_path, const char* fs_path);
	void release();

	GLuint vertex;
	GLuint fragment;
	GLuint program;
};

// Helper container for all the information we need when rendering an object together
// with its transform.
struct Renderable
{
	Mesh mesh;
	Effect effect;
	mat3 transform;
	vec2 m_position;

	// projection contains the orthographic projection matrix. As every Renderable::draw()
	// renders itself it needs it to correctly bind it to its shader.
	virtual void draw(const mat3& projection) = 0;

	// gl Immediate mode equivalent, see the Rendering and Transformations section in the
	// specification pdf
	void transform_begin();
	void transform_scale(vec2 scale);
	void transform_rotate(float radians);
	void transform_translate(vec2 pos);
	void transform_end();
};