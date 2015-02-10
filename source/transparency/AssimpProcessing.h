#pragma once

#include <vector>

struct aiMesh;
struct aiScene;

class PolygonalGeometry;

class AssimpProcessing
{
public:
    static std::vector<PolygonalGeometry> convertToGeometries(const aiScene * scene);
    static PolygonalGeometry convertToGeometry(const aiMesh * mesh);
};
