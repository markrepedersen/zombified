////
//// Created by Mark Pedersen on 2018-03-20.
////
//
//#include "tilemap.h"
//
//void tilemap::init()
//{
//    //for (int i = 0; i < 930; i++) {
//    //	for (int j = 0; j < 1055; j++) {
//    //		coordMap[{float(i), float(j)}] = 1;
//    //	}
//    //}
//}
//
//void tilemap::addCollision(vec2 tile) {
//
//}
//
//bool tilemap::check_collision(vec2 next_pos)
//{
//    vec2 next_tile = { ((next_pos.x + multiple / 2) / multiple) * multiple,((next_pos.y + multiple / 2) / multiple) * multiple };
//    if (coordMap[next_tile] != NULL) {
//        return coordMap[next_tile] == 1;
//    }
//    return false;
//}
//
//void tilemap::parseMap() {
//
//    effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
//
//    fflush(stdout);
//    std::ifstream input_file(data_path + std::string("/tilemap.txt"));
//
//    std::string tile_data;
//    if (input_file.is_open()) {
//        while (!input_file.eof()) {
//            std::string temp;
//            input_file >> temp;
//            tile_data.append(temp);
//        }
//    }
//    input_file.close();
//
//    // split for each 16x16 tile
//    std::string s = tile_data;
//    std::string delimiter = "</chunk>";
//
//    vector<string> tileGroup = tileSplit(s, delimiter);
//
//    std::regex rgx1("x\=\".*?(?=\")");
//    std::regex rgx2("y\=\".*?(?=\")");
//    std::regex rgx3("\"\>.*");
//    for (auto const tiles : tileGroup) {
//        std::smatch matchx;
//        std::smatch matchy;
//        std::smatch matcht;
//        vector<string> tileBlock;
//        string x;
//        string y;
//        string t;
//        int x_off;
//        int y_off;
//        if (std::regex_search(tiles.begin(), tiles.end(), matchx, rgx1)) {
//            x = matchx[0];
//            x_off = stoi(x.substr(3));
//        }
//        else break;
//        if (std::regex_search(tiles.begin(), tiles.end(), matchy, rgx2)) {
//            y = matchy[0];
//            y_off = stoi(y.substr(3));
//        }
//        else break;
//        if (std::regex_search(tiles.begin(), tiles.end(), matcht, rgx3)) {
//            t = matcht[0];
//            t = t.substr(2);
//            std::string delimiter = ",";
//            tileBlock = tileSplit(t,delimiter);
//        }
//        else break;
//        for (int i = 0; i < 16; i++) {
//            for (int j = 0; j < 16; j++) {
//                Terrain terrain = Terrain();
//                if (terrain.init(x_off, y_off, stoi(tileBlock[i * 16 + j]),effect)) {
//                    terrain.getEffect(effect);
//                    terrain_pieces.emplace_back(terrain);
//                    if ((std::find(unpassable.begin(), unpassable.end(), stoi(tileBlock[i * 16 + j]))) != unpassable.end()) {
//                        addCollision({float(x_off), float(y_off)});
//                    }
//                }
//                x_off += 1;
//            }
//            x_off = stoi(x.substr(3));
//            y_off += 1;
//        }
//
//    }
//}
//
//vector<string> tilemap::tileSplit(string s, string delimiter) {
//    size_t pos = 0;
//    std::string token;
//    vector<string> tempGroup;
//    while ((pos = s.find(delimiter)) != std::string::npos) {
//        token = s.substr(0, pos);
//        tempGroup.push_back(token);
//        s.erase(0, pos + delimiter.length());
//    }
//    tempGroup.push_back(s);
//    return tempGroup;
//}
//
//void tilemap::draw(const mat3 &projection) {
//    for (auto t : terrain_pieces) {
//        t.draw(projection);
//    }
//}
//
//void tilemap::destroy() {
//    for (auto t : terrain_pieces) {
//        t.destroy();
//    }
//}
//
