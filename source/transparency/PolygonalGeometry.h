#pragma once

#include <vector>
#include <glm/fwd.hpp>


class PolygonalGeometry
{
public:
    const std::vector<unsigned int> & indices() const;

    void setIndices(const std::vector<unsigned int> & indices);
    void setIndices(std::vector<unsigned int> && indices);

    const std::vector<glm::vec3> & vertices() const;

    void setVertices(const std::vector<glm::vec3> & vertices);
    void setVertices(std::vector<glm::vec3> && vertices);

    bool hasNormals() const;
    const std::vector<glm::vec3> & normals() const;

    void setNormals(const std::vector<glm::vec3> & normals);
    void setNormals(std::vector<glm::vec3> && normals);

private:
    std::vector<unsigned int> m_indices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
};
