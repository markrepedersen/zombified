////
//// Created by Mark Pedersen on 2018-03-20.
////
//
//#include "terrain.h"
//#include <cmath>
//
//Texture Terrain::terrain_texture;
//
//bool Terrain::init(int x_off, int y_off, int text_num,Effect eff)
//{
//    nodes.clear();
//    // Load shared texture
//    if (!terrain_texture.is_valid())
//    {
//        if (!terrain_texture.load_from_file(textures_path("")))
//        {
//            fprintf(stderr, "Failed to load terrain texture!");
//            return false;
//        }
//    }
//
//    int terrain_columns = 21;
//    int terrain_rows = 23;
//
//    float row = (float)ceil(text_num / terrain_columns);
//    float column = text_num - (row*terrain_columns)-1;
//
//    if (column < 0)
//        return false;
//    // The position corresponds to the center of the texture
//    float wr = terrain_texture.width / terrain_rows * 0.5f;
//    float hr = terrain_texture.height / terrain_rows * 0.5f;
//
//    float length = terrain_texture.width*0.001f / 14.112000000000000000000000183456f;
//    float height = terrain_texture.height*0.001f / 16.9f;
//
//    float left = length * column;
//    float right = length * (column + 1);
//    float top = height * (row + 1);
//    float bottom = height * row;
//
//    TexturedVertex vertices[4];
//    vertices[0].position = { -wr, +hr, -0.02f };
//    vertices[1].position = { +wr, +hr, -0.02f };
//    vertices[2].position = { +wr, -hr, -0.02f };
//    vertices[3].position = { -wr, -hr, -0.02f };
//
//    //top left
//    vertices[0].texcoord = { left, top };
//    //top right
//    vertices[1].texcoord = { right, top };
//    //bottom right
//    vertices[2].texcoord = { right, bottom };
//    //bottom left
//    vertices[3].texcoord = { left, bottom };
//
//    // counterclockwise as it's the default opengl front winding direction
//    uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };
//
//    // Clearing errors
//    gl_flush_errors();
//
//    // Vertex Buffer creation
//    glGenBuffers(1, &mesh.vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);
//
//    // Index Buffer creation
//    glGenBuffers(1, &mesh.ibo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);
//
//    // Vertex Array (Container for Vertex + Index buffer)
//    glGenVertexArrays(1, &mesh.vao);
//    if (gl_has_errors())
//        return false;
//
//    // Loading shaders -- PASS FROM TILEMAP
//    //if (!eff.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
//    //	return false;
//
//    m_scale.x = 1.5f;
//    m_scale.y = 1.5f;
//
//    m_position = { (float)terrain_texture.width * m_scale.x / 2 + (x_off*28), (float)terrain_texture.height * m_scale.y / 2 + (y_off * 28) };
//
//    float wt = (float)terrain_texture.width * m_scale.x / 2;
//    float ht = (float)terrain_texture.height * m_scale.y / 2;
//
//    float wm = (float)terrain_texture.width * (m_scale.x-0.2) / 2;
//    float hm = (float)terrain_texture.height * (m_scale.y-0.2) / 2;
//
//    // create interpolated version of object to prevent inner edges
//    //mini_nodes.push_back({ m_position.x + wm, m_position.y + hm });
//    //mini_nodes.push_back({ m_position.x + wm, m_position.y - hm });
//    //mini_nodes.push_back({ m_position.x - wm, m_position.y + hm });
//    //mini_nodes.push_back({ m_position.x - wm, m_position.y - hm });
//
//    //// get vertices ready for world
//    //nodes.push_back({ m_position.x + wt, m_position.y + ht});
//    //nodes.push_back({ m_position.x + wt, m_position.y - ht });
//    //nodes.push_back({ m_position.x - wt, m_position.y + ht });
//    //nodes.push_back({ m_position.x - wt, m_position.y - ht });
//    //for (int i = 0; i < 20; i++) {
//    //	nodes.push_back({ m_position.x + wt + i * 20, m_position.y + ht + i * 20});
//    //}
//
//    is_active = true;
//    return true;
//}
//
//void Terrain::destroy()
//{
//    glDeleteBuffers(1, &mesh.vbo);
//    glDeleteBuffers(1, &mesh.ibo);
//    glDeleteBuffers(1, &mesh.vao);
//
//    glDeleteShader(eff.vertex);
//    glDeleteShader(eff.fragment);
//    glDeleteShader(eff.program);
//}
//
//void Terrain::draw(const mat3& projection)
//{
//    // Transformation code, see Rendering and Transformation in the template specification for more info
//    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
//    transform_begin();
//    transform_scale(m_scale);
//    transform_translate(m_position);
//    transform_end();
//
//    // Setting shaders
//    glUseProgram(eff.program);
//
//    // Enabling alpha channel for textures
//    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glDisable(GL_DEPTH_TEST);
//
//    // Getting uniform locations for glUniform* calls
//    GLint transform_uloc = glGetUniformLocation(eff.program, "transform");
//    GLint color_uloc = glGetUniformLocation(eff.program, "fcolor");
//    GLint projection_uloc = glGetUniformLocation(eff.program, "projection");
//
//    // Setting vertices and indices
//    glBindVertexArray(mesh.vao);
//    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
//
//    // Input data location as in the vertex buffer
//    GLint in_position_loc = glGetAttribLocation(eff.program, "in_position");
//    GLint in_texcoord_loc = glGetAttribLocation(eff.program, "in_texcoord");
//    glEnableVertexAttribArray(in_position_loc);
//    glEnableVertexAttribArray(in_texcoord_loc);
//    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
//    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
//
//    // Enabling and binding texture to slot 0
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, terrain_texture.id);
//
//    // Setting uniform values to the currently bound program
//    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
//    float color[] = { 1.f, 1.f, 1.f };
//    glUniform3fv(color_uloc, 1, color);
//    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
//
//    // Drawing!
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
//}
//
//vector<vec2> Terrain::get_nodes()const
//{
//    return nodes;
//}
//
//vector<vec2> Terrain::get_mini_nodes()const
//{
//    return mini_nodes;
//}
//
//bool Terrain::return_state()
//{
//    return is_active;
//}
//
//void Terrain::clear()
//{
//    nodes.clear();
//    mini_nodes.clear();
//}
//
//void Terrain::getEffect(Effect effe) {
//    eff = effe;
//}
//
