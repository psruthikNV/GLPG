#pragma once

#include <vector>
#include <fstream>
#include <sstream>

#ifdef __linux__
#include <unistd.h>
#endif

namespace glpg {
    void pause()
    {
#ifdef __linux__
        sleep(5);
#elif defined _WIN32
        system("pause");
#endif
    }

struct VertexIN {
    float pos[3];
    float color[3];
};

bool LoadObjFile(const char* filePath, std::vector<VertexIN>& v_vtx)
{
    if (filePath == nullptr) {
        std::cerr << "No filepath specified";
        return false;
    }

    std::fstream objFileStream(filePath, std::ios::in);
    if (objFileStream.is_open()) {
        for (std::string line; std::getline(objFileStream, line);) {
            std::istringstream in(line);
            std::string type;
            in >> type;

            if (type == "v") {
                float x, y, z;
                in >> x >> y >> z;
                VertexIN tempVertex;
                tempVertex.pos[0] = x;
                tempVertex.pos[1] = y;
                tempVertex.pos[2] = z;
                tempVertex.color[0] = 0.0F;
                tempVertex.color[1] = 0.0F;
                tempVertex.color[2] = 0.0F;
                //tempVertex.pos = { x, y, z };
                //tempVertex.color = { 0.0F, 0.0F, 1.0F };
                v_vtx.push_back(std::move(tempVertex));
                //std::cout << "VTX: " << x << " " << y << " " << z <<"\n";
            }
        }
    } else {
        return false;
    }
    return true;
}

} // namespace
