// Header
#include "missile.hpp"

#include <cmath>
#include <iostream>

Texture Missile::missile_texture;

bool Missile::init()
{    
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	// Reads the salmon mesh from a file, which contains a list of vertices and indices
	FILE* mesh_file = fopen(mesh_path("missile.mesh"), "r");
	if (mesh_file == nullptr)
		return false;

	// Reading vertices and colors
	size_t num_vertices;
	fscanf(mesh_file, "%zu\n", &num_vertices);
	for (size_t i = 0; i < num_vertices; ++i)
	{
		float x, y, z;
		float _u[3]; // unused
		int r, g, b;
		fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _u, _u+1, _u+2, &r, &g, &b);
		Vertex vertex;
		vertex.position = { x, y, -z }; 
		vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
		vertices.push_back(vertex);
	}

	// Reading associated indices
	size_t num_indices;
	fscanf(mesh_file, "%zu\n", &num_indices);
	for (size_t i = 0; i < num_indices; ++i)
	{
		int idx[3];
		fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
		indices.push_back((uint16_t)idx[0]);
		indices.push_back((uint16_t)idx[1]);
		indices.push_back((uint16_t)idx[2]);
	}

	// Done reading
	fclose(mesh_file);
  
    // Clearing errors
    gl_flush_errors();
    
	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl")))
		return false;

    // Setting initial values
    m_scale.x = -5.f * ViewHelper::getRatio();
    m_scale.y = 5.f * ViewHelper::getRatio();
    m_is_alive = true;
    m_position = { 450.f * ViewHelper::getRatio(), 450.f * ViewHelper::getRatio()};
    m_speed = {0.f, 0.f};
    useMissileOnPlayer = 0;
    onPlayerPos = {0.f, 0.f};

	m_num_indices = indices.size();
	m_rotation = -1.2;
    mass = 1.f;
    
    return true;
}

void Missile::draw(const mat3& projection)
{
    // Transformation code, see Rendering and Transformation in the template specification for more info
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
    transform_begin();
    transform_translate(m_position);
    transform_scale(m_scale);
    transform_rotate(m_rotation);
    transform_end();
    
    // Setting shaders
    glUseProgram(effect.program);
    
    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    
    // Setting vertices and indices
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    
    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    // GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");

    glEnableVertexAttribArray(in_position_loc);
    //glEnableVertexAttribArray(in_texcoord_loc);
	glEnableVertexAttribArray(in_color_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
    
    // Enabling and binding texture to slot 0
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, missile_texture.id);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
	glDrawElements(GL_TRIANGLES, (GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);
}

/*
void Missile::draw(const mat3& projection)
{
	transform_begin();

	transform_translate({ m_position });
	transform_rotate(m_rotation);
	transform_scale(m_scale);

	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	//GLint light_up_uloc = glGetUniformLocation(effect.program, "light_up");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	float color[] = { 1.f, 1.f, 1.f };

	glUniform3fv(color_uloc, 1, color);
	
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES,(GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);
}
*/

void Missile::set_position(vec2 position)
{
	//std::cout << "Missile created" << "\n";

    m_position = position;
}

void Missile::set_scale(vec2 scale)
{
    m_scale = scale;
}

vec2 Missile::get_position()const
{
    return m_position;
}

float Missile::get_mass() const
{
    return mass;
}

void Missile::set_rotation(float radians)
{
    m_rotation = radians;
}

bool Missile::is_alive()const
{
    return m_is_alive;
}


void Missile::set_speed(vec2 newspeed){
    m_speed = newspeed;
    
}
vec2 Missile::get_speed()const{
    return m_speed;
}

void Missile::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteProgram(effect.program);
}



vec2 Missile::get_bounding_box()const
{
    float missilewidth = 2.f;
    float missileheight = 8.f;
    // fabs is to avoid negative scale due to the facing direction
    return { std::fabs(m_scale.x) * missilewidth, std::fabs(m_scale.y) * missileheight };
}

void Missile::move(vec2 pos) {
    m_position += pos;
}

bool Missile::collides_with(const Missile& missile)
{
    float dx = m_position.x - missile.get_position().x;
    float dy = m_position.y - missile.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(missile.get_bounding_box().x, missile.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

float Missile::get_force(float mass1, float speed, vec2 objPosition)
{
    //float blastArea = 3.14*(200.f*200.f);
    float blastRadius = 100.f;
    float force = 0;
    float dist = distance(objPosition, get_position());
    //fprintf(stderr, "distance %f\n", dist);
    if (dist < blastRadius)
    {
        //if (dist < 40.f)
        //    dist = 40.f;
        //fprintf(stderr, "distance %f\n", dist);
        //fprintf(stderr, "speed %f\n", speed);
        force = ((speed*speed)/(dist*mass1));//+400;
        //fprintf(stderr, "speed %f\n", speed);
        //if (force > 950.0)
        //    force = 950;
    }
    return force;
    
}

bool Missile::checkPoint() {
    
    float dx = m_position.x - onPlayerPos.x;
    float dy = m_position.y - onPlayerPos.y;
    float d_sq = dx * dx + dy * dy;
    
    float other_r = 72;
    
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
    
    /*if (m_position.x > 1280 || m_position.x < 0 || m_position.y > 720 || m_position.y < 0)
        return true;
    return false;*/

}

