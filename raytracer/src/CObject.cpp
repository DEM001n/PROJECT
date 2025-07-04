//
//  CObject.cpp
//  rt
//
//  Created by Radoslaw Mantiuk on 22/01/2023.
//

#include "rt.h"
#include "CObject.hpp"


/// \fn intersect(CRay ray)
/// \brief Computes intersection between ray and sphere.
/// \param ray Ray parameters.
/// \return Distance from camera position to the closest intersection point, or negative value.
///
float CSphere::intersect(const CRay& ray) {
    float t = -1;

    glm::vec3 v = ray.pos - pos;

    float delta = ( glm::dot(v, ray.dir) *  glm::dot(v, ray.dir)) - glm::dot(ray.dir, ray.dir) * (glm::dot(v, v) - r * r);

    if (delta < 0) return t;

    float t0 = (-glm::dot(v, ray.dir) - std::sqrt(delta)) / glm::dot(ray.dir, ray.dir);
    float t1 = (-glm::dot(v, ray.dir) + std::sqrt(delta)) / glm::dot(ray.dir, ray.dir);

    if (t0 > 0 && t1 > 0) {
        t = std::min(t0, t1);
    } else if (t0 > 0) {
        t = t0;
    } else if (t1 > 0) {
        t = t1;
    }

    return t;
}


/// Normal vector to the sphere surface
/// \fn normal(glm::vec3 hit_pos)
/// \brief Surface normal vector at the intersection point.
/// \param hit_pos Intersection point.
/// \return Normal vector parameters.
///
glm::vec3 CSphere::normal(const glm::vec3& hit_pos) {
    glm::vec3 n = {0,0,0};

    n = glm::normalize(hit_pos - pos);

    return n;
}

/// Computes texture mapping coordinates (u,v).
/// \param normal_vec Normalized normal vector at intersection point.
/// \return (u,v) texture coordinates in <0,1> range.
glm::vec2 CSphere::textureMapping(const glm::vec3& normal_vec) {
    glm::vec2 uv = {0.5 + asin(normal_vec[0])/CV_PI,0.5 - asin(normal_vec[1])/CV_PI};

    return uv;
}



/// \fn intersect(CRay ray)
/// \brief Computes intersection between triangle and sphere.
/// \param ray Ray parameters.
/// \return Distance from camera position to the closest intersection point, or negative value.
///
float CTriangle::intersect(const CRay& ray) {
    float t = -1;
    glm::vec3 temp = {0,0,-1};

    glm::intersectRayTriangle(ray.pos,ray.dir,v0,v1,v2,temp);

    if(temp.z >= 0)
        t = temp.z;

    return t;
}


/// \fn normal(glm::vec3 hit_pos)
/// \brief Surface normal vector at the intersection point.
/// \param hit_pos Intersection point (not used for triangle).
/// \return Normal vector parameters.
///
glm::vec3 CTriangle::normal(const glm::vec3& hit_pos) {
    glm::vec3 n = {0,0,0};

    glm::vec3 u = v1 - v0;
    glm::vec3 v = v2 - v0;

    n = glm::normalize(glm::cross(u,v));
        
    return n;
}





