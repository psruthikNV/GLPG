#pragma once

#include <vector>
#include <fstream>
#include <sstream>

#ifdef __linux__
#include <unistd.h>
#endif

namespace GLPG {
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

struct FaceIN{
    std::vector<uint32_t> vertexIndices;
    uint32_t textureIndices[4];
    uint32_t normalIndices[4];
};

bool LoadObjFile(const char* filePath, std::vector<VertexIN>& v_vtx, std::vector<FaceIN>& v_face)
{
    if (filePath == nullptr) {
        std::cerr << "No filepath specified";
        return false;
    }

    std::fstream objFileStream(filePath, std::ios::in);
    if (objFileStream.is_open()) {
        uint32_t faceId = 0U;
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
                v_vtx.push_back(std::move(tempVertex));
            } else if (type == "f") {
                char slash; // K, this shit needs to be removed.
                FaceIN tempFace;
                uint32_t i = 0U;
                uint32_t temp;
                while (!in.eof()) {

                    in >> temp;
                    tempFace.vertexIndices.push_back(temp - 1);
                    in >> slash;
                    in >> tempFace.textureIndices[i];
                    in >> slash;
                    in >> tempFace.normalIndices[i];
                    ++i;
                }
                v_face.push_back(std::move(tempFace));
            }
        }
    } else {
        return false;
    }
    return true;
}

} // namespace
