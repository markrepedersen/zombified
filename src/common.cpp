#include "common.hpp"
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image/stb_image.h"

// stlib
#include <vector>
#include <sstream>
#include <iostream>
#include <math.h>

void gl_flush_errors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
	}

	return true;
}

float dot(vec2 l, vec2 r)
{
	return l.x * r.x + l.y * r.y;
}

float dot(vec3 l, vec3 r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

mat3 mul(const mat3 & l, const mat3 & r)
{
	mat3 l_t = { { l.c0.x, l.c1.x, l.c2.x},
	{ l.c0.y, l.c1.y, l.c2.y } ,
	{ l.c0.z, l.c1.z, l.c2.z } };

	mat3 ret;
	ret.c0.x = dot(l_t.c0, r.c0);
	ret.c0.y = dot(l_t.c1, r.c0);
	ret.c0.z = dot(l_t.c2, r.c0);

	ret.c1.x = dot(l_t.c0, r.c1);
	ret.c1.y = dot(l_t.c1, r.c1);
	ret.c1.z = dot(l_t.c2, r.c1);

	ret.c2.x = dot(l_t.c0, r.c2);
	ret.c2.y = dot(l_t.c1, r.c2);
	ret.c2.z = dot(l_t.c2, r.c2);
	return ret;
}

vec2 normalize(vec2 v)
{
	float hyp = sqrt(v.x*v.x + v.y*v.y);
	v.x /= hyp;
	v.y /= hyp;
	return {v.x, v.y};
}

vec2 scale(float c, vec2 v) {
    float x = c*v.x;
    float y = c*v.y;
    return {x, y};
}

float distance(vec2 v1, vec2 v2) {
	float diffY = v1.y - v2.y;
	float diffX = v1.x - v2.x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}


vec2 direction(vec2 v1, vec2 v2) {
	return {v2.x - v1.x, v2.y - v1.y};
}

//returns true if point r lies on line pq after checking
//that they are collinear
bool onSegment(vec2 p, vec2 q, vec2 r) {
		if (r.x <= fmax(p.x, q.x) && r.x >= fmin(p.x, q.x) &&
		r.y <= fmax(p.y, q.y) && r.y >= fmin(p.y, q.y)) {
			return true;
		}
		return false;
}

//returns orientation of ordered triplet (a, b, c)
//returns 0 -> a, b, and c are collinear
//returns 1 -> clockwise
//returns 2 -> counter-clockwise
int orientation(vec2 a, vec2 b, vec2 c) {

	int value = (b.y - a.y) * (c.x - b.x) -
				(b.x - a.x) * (c.y - b.y);

	if (value == 0) return 0;
	return (value > 0)? 1 : 2;
}

bool intersect(vec2 p1, vec2 q1, vec2 p2, vec2 q2) {

	//four orientations of all line segment
	int o1 = orientation (p1, q1, p2);
	int o2 = orientation (p1, q1, q2);
	int o3 = orientation (p2, q2, p1);
	int o4 = orientation (p2, q2, q1);

	if (o1 != o2 && o3 != o4) return true;

	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false;

}

//return true if point point is inside the polygon represented by poly
bool isInsidePolygon(std::vector<vec2> poly, vec2 point) {

	//base case where vector of points doesn't make a polygon
	//assume that the first vector on the list is connected to
	//the last vector using a line
	if (poly.size() < 2 ) return false;

	vec2 extreme = (10000, point.y);

	int intersectionCount = 0;

	for (int i = 0 ; i < poly.size() ; i++) {
		vec2 poly1 = poly[i];
		vec2 poly2 = ((i >= poly.size() - 1)? poly[0] : poly[i+1]);

		if (intersect(poly1, poly2, point, extreme))
		{
			if (orientation(poly1, point, poly2) == 0) {
				return onSegment(poly1, point, poly2);
			}

			intersectionCount++;
		}
	}

	return (intersectionCount % 2 == 1);
}


vec2 getRandomPointInMap(std::vector<vec2> mapCollisionPoints, vec2 screen) {

  srand((unsigned)time(0));

	vec2 randomPoint = {(float)((rand() % (int)screen.x)),
						(float)((rand() % (int)screen.y))};

	while(!isInsidePolygon(mapCollisionPoints, randomPoint)) {
		randomPoint = {(float)((rand() % (int)screen.x)),
		        (float)((rand() % (int)screen.y))};

	}

	return randomPoint;

}



float getDistance(vec2 v1, vec2 v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

float getDistancePointToLine(vec2 a, vec2 b, vec2 c) {

	float x = (b.y - a.y) * c.x;
	float y = (b.x - a.x) * c.y;
	float z = (b.x * a.y) - (b.y * a.x);
	float xyz = std::fabs(x - y + z);

	float d1 = pow((b.y - a.y), 2);
	float d2 = pow((b.x - a.x), 2);
	float denominator = sqrt(d1 + d2);

	return (xyz/denominator);
} //line ab to point c

vec2 subtractVectors(vec2 a, vec2 b)
{
	return {a.x - b.x, a.y - b.y};
}

vec2 sumVectors(vec2 a, vec2 b)
{
	return {a.x + b.x, a.y + b.y};
}

vec2 multiplyScalarVector(float a, vec2 b)
{
	return {a * b.x, a * b.y};
}

Texture::Texture()
{

}

Texture::~Texture()
{
	if (id != 0) glDeleteTextures(1, &id);
}

bool Texture::load_from_file(const char* path)
{
	if (path == nullptr)
		return false;
	
	stbi_uc* data = stbi_load(path, &width, &height, NULL, 4);
	if (data == NULL)
		return false;

	gl_flush_errors();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);
	return !gl_has_errors();
}

bool Texture::is_valid()const
{
	return id != 0;
}

namespace
{
	bool gl_compile_shader(GLuint shader)
	{
		glCompileShader(shader);
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint log_len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetShaderInfoLog(shader, log_len, &log_len, log.data());
			glDeleteShader(shader);

			fprintf(stderr, "GLSL: %s", log.data());
			return false;
		}

		return true;
	}
}

bool Effect::load_from_file(const char* vs_path, const char* fs_path)
{
	gl_flush_errors();

	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);

	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path, fs_path);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);

	// Compiling
	// Shaders already delete if compilation fails
	if (!gl_compile_shader(vertex))
		return false;

	if (!gl_compile_shader(fragment))
	{
		glDeleteShader(vertex);
		return false;
	}

	// Linking
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	{
		GLint is_linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(program, log_len, &log_len, log.data());

			release();
			fprintf(stderr, "Link error: %s", log.data());
			return false;
		}
	}

	if (gl_has_errors())
	{
		release();
		fprintf(stderr, "OpenGL errors occured while compiling Effect");
		return false;
	}

	return true;
}

void Effect::release()
{
	glDeleteProgram(program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Renderable::transform_begin()
{
	transform = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} };
}

void Renderable::transform_scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	transform = mul(transform, S);
}

void Renderable::transform_rotate(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	transform = mul(transform, R);
}

void Renderable::transform_translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	transform = mul(transform, T);
}

void Renderable::transform_end()
{
	//
}