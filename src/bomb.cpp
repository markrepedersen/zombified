// Header
#include "bomb.hpp"

#include <cmath>
#include <iostream>

Texture Bomb::bomb_texture;

bool Bomb::init()
{    
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	// Reads the salmon mesh from a file, which contains a list of vertices and indices
	FILE* mesh_file = fopen(mesh_path("bomb.mesh"), "r");
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
    m_scale.x = -15.f * ViewHelper::getRatio();
    m_scale.y = 15.f * ViewHelper::getRatio();
    m_is_alive = true;
    m_position = { 450.f * ViewHelper::getRatio(), 450.f * ViewHelper::getRatio()};

	m_num_indices = indices.size();
	m_rotation = 3.f;
    speed = {0.f, 0.f};
    mass = 0.15;
    
    return true;
}

void Bomb::draw(const mat3& projection)
{
    // Transformation code, see Rendering and Transformation in the template specification for more info
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
    transform_begin();
    transform_translate(m_position);
	transform_rotate(m_rotation);
    transform_scale(m_scale);
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

    if (speed.x + speed.y < 100.f) {
        
    }
    
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

void Bomb::set_position(vec2 position)
{
	// std::cout << "Bomb created" << "\n";

    m_position = position;
}

void Bomb::set_scale(vec2 scale)
{
    m_scale = scale;
}

vec2 Bomb::get_position()const
{
    return m_position;
}

float Bomb::get_mass() const
{
    return mass;
}

bool Bomb::is_alive()const
{
    return m_is_alive;
}

void Bomb::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}



vec2 Bomb::get_bounding_box()const
{
    // fabs is to avoid negative scale due to the facing direction
    float bombradius = 5.f;
    return { std::fabs(m_scale.x) * bombradius, std::fabs(m_scale.y) * bombradius};
}

bool Bomb::collides_with(const Bomb& bomb)
{
    float dx = m_position.x - bomb.get_position().x;
    float dy = m_position.y - bomb.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(bomb.get_bounding_box().x, bomb.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

void Bomb::move(vec2 pos) {
    m_position += pos;
}

void Bomb::checkBoundaryCollision(float width, float height, float ms) {
    
    float radius = (bomb_texture.width/2);
    if (m_rotation > 18.f)
        m_rotation = 0.f;
    else
        m_rotation += speed.x *(0.001);
    
    if (m_position.x > width-radius)
    {
        m_position.x = width-radius;
        speed.x *= -1;
        move({speed.x *(ms/1000), speed.y*(ms/1000)});
    }
    else if (m_position.x < 200-radius)
    {
        m_position.x = 200-radius;
        speed.x *= -1;
        move({speed.x *(ms/1000), speed.y*(ms/1000)});
    }
    else if (m_position.y > height-radius)
    {
        m_position.y = height-radius;
        speed.y *= -1;
        move({speed.x *(ms/1000), speed.y*(ms/1000)});
    }
    else if (m_position.y < 70-radius)
    {
        m_position.y = 70-radius;
        speed.y *= -1;
        move({speed.x *(ms/1000), speed.y*(ms/1000)});
    }
}

vec2 Bomb::get_speed() const
{
    return speed;
}
void Bomb::set_speed(vec2 newSpeed)
{
    speed = newSpeed;
}

float Bomb::get_force(float mass1, float speed, vec2 objPosition)
{
    //float blastArea = 3.14*(200.f*200.f);
    float blastRadius = 200.f;
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

void Bomb::checkCollision(Bomb other, float ms) {
    
    // Get distances between the balls components
    float diffY = other.get_position().y - m_position.y; //m_position.y - other.get_position().y;
    float diffX = other.get_position().x - m_position.x; //m_position.x - other.get_position().x;
    vec2 distanceVect = {diffX, diffY};
    
    // Calculate magnitude of the vector separating the balls
    float fDistance = sqrt(distanceVect.x*distanceVect.x
                                 + distanceVect.y*distanceVect.y);
    
    // Minimum distance before they are touching
    float minDistance = 32.5f;//bomb_texture.width;
    //fprintf(stderr, "min distance: %f \n", minDistance);
    //fprintf(stderr, "distvect: %f \n", distanceVectMag);
    
    if (fDistance < minDistance) {
        //fprintf(stderr, "collided \n");
        //float radius = 32.f/2;
        float dt = (ms/1000);
        float mass1 = 0.15;
        float mass2 = 0.15;
        // Normal
        //vec2 normal = normalize({other.get_position().x - m_position.x,
        //    other.get_position().y - m_position.y});
        float nx = diffX/fDistance; //(other.get_position().x - m_position.x) / fDistance;
        float ny = diffY/fDistance; //(other.get_position().y - m_position.y) / fDistance;

        //float nx = normal.x;
        //float ny = normal.y;
        // Tangent
        float tx = -ny;
        float ty = nx;
        
        // Dot Product Tangent
        float dpTan1 = speed.x * tx + speed.y * ty;
        float dpTan2 = other.get_speed().x * tx + other.get_speed().y * ty;
        
        // Dot Product Normal
        float dpNorm1 = speed.x * nx + speed.y * ny;
        float dpNorm2 = other.get_speed().x * nx + other.get_speed().y * ny;
        
        // Conservation of momentum in 1D
        float m1 = (dpNorm1 * (mass1 - mass2) + 2.0f * mass2 * dpNorm2) / (mass1 + mass2);
        float m2 = (dpNorm2 * (mass2 - mass1) + 2.0f * mass1 * dpNorm1) / (mass1 + mass2);
        
        set_speed({tx * dpTan1 + nx * m1, ty * dpTan1 + ny * m1});
        other.set_speed({tx * dpTan2 + nx * m2, ty * dpTan2 + ny * m2});
        
        move({speed.x*dt, speed.y*dt});
        other.move({other.get_speed().x*dt, other.get_speed().y*dt});
        
        //fprintf(stderr, "b1x %f \n", speed.x);
        //fprintf(stderr, "b1y %f \n", speed.y);
        //fprintf(stderr, "b2x %f \n", other.get_speed().x);
        //fprintf(stderr, "b2y %f \n", other.get_speed().y);
        
        
    }
}


