//
//  CRayTrace.cpp
//  rt
//
//  Created by Radoslaw Mantiuk on 22/01/2023.
//

#include "rt.h"

/// \fn compPrimaryRayMatrix(CCamera cam, glm::mat3& m)
/// \brief Computation of the projection matrix.
/// \param cam Camera parameters.
/// \param m Output projection matrix.
///
bool CRayTrace::compPrimaryRayMatrix(const CCamera& cam, glm::mat3& m) {
    glm::vec3 look = cam.lookp - cam.eyep;

    glm::vec3 u = glm::normalize(glm::cross(cam.up, look));
    glm::vec3 v = glm::normalize(glm::cross(u, look));
    glm::vec3 o = glm::normalize(look) * (cam.width / (2 * tan(cam.fov / 2.0f))) - (cam.width / 2.0f * u + cam.height / 2.0f * v);

    m = glm::mat3(u, v, o);

    return true;
}


/// \fn rayTrace(CScene scene, CRay& ray, COutput& out)
/// \brief Traces single ray.
/// \param scene Object with all scene components including a camera.
/// \param ray Ray parameter (primary and secondary rays).
/// \param out Object with output color and parameters used in recursion.
///
bool CRayTrace::rayTrace(const CScene& scene, CRay& ray, COutput& out, bool useBVH) {
    float tmin = 1e30f;
    float EPS = 0.0001f;
    bool is_intersection = false;
    CObject* hit_obj = nullptr;
    bool isLit = false;

    if (!useBVH) {
        for (CObject* obj : scene.objectList) {
            float t = obj->intersect(ray);
            if (t > EPS && t < tmin) {
                tmin = t;
                is_intersection = true;
                hit_obj = obj;
            }
        }
    } else {
        float tBVH = 1e30f;
        hit_obj = nullptr;

        if (scene.bvh.root != nullptr) {
            is_intersection = scene.bvh.intersect(ray, tBVH, hit_obj);
            tmin = tBVH;
        }
    }

    if (!is_intersection) {
        return false;
    }

    glm::vec3 p = ray.pos + tmin * ray.dir;
    glm::vec3 n = hit_obj->normal(p);
    glm::vec3 V = -ray.dir;
    glm::vec3 col(0.0f);

    for (const CLight& light : scene.lightList) {
        col += light.color * hit_obj->matAmbient;

        glm::vec3 L = glm::normalize(light.pos - p);
        float cos_angle = glm::dot(n, L);

        CRay shadowRay;
        shadowRay.pos = p;
        shadowRay.dir = L;

        bool in_shadow = false;
        float light_dist = glm::length(light.pos - p);

        if (!useBVH) {
            for (CObject* obj : scene.objectList) {
                float t_shadow = obj->intersect(shadowRay);
                if (t_shadow > EPS && t_shadow < light_dist) {
                    in_shadow = true;
                    break;
                }
            }
        } else {
            if (scene.bvh.root != nullptr) {
                in_shadow = scene.bvh.intersectAny(shadowRay, light_dist);
            }
        }

        if (in_shadow) {
            continue;
        }

        if (cos_angle > 0.001f) {
            isLit = true;
            col += light.color * hit_obj->matDiffuse * cos_angle;

            glm::vec3 h = glm::normalize(L + V);
            float cos_beta = glm::dot(n, h);
            if (cos_beta > 0.001f) {
                col += light.color * hit_obj->matSpecular * pow(cos_beta, hit_obj->matShininess);
            }
        }

    }

    const int MAX_RAY_TREE = 1;
    const double MIN_RAY_ENERGY = 0.01;

    glm::vec3 localColor = glm::clamp(col, 0.0f, 1.0f);

    if (hit_obj->isTexture) {
        glm::vec2 uv = hit_obj->textureMapping(n);
        glm::vec3 tex_col = CImage::getTexel(hit_obj->texture, uv.x, uv.y);
        localColor *= tex_col;
    }

    glm::vec3 reflectedColor(0.0f);
    if (isLit && hit_obj->reflectance > 0 && out.tree < MAX_RAY_TREE && out.energy > MIN_RAY_ENERGY) {
        COutput reflectedOutput;
        reflectedOutput.tree = out.tree + 1;
        reflectedOutput.energy = out.energy * hit_obj->reflectance;
        reflectedOutput.col = glm::vec3(0.0f);

        CRay reflected_ray = reflectedRay(ray, n, p);
        rayTrace(scene, reflected_ray, reflectedOutput, useBVH); // przekazujemy dalej useBVH

        reflectedColor = reflectedOutput.col * hit_obj->reflectance;
    }

    out.col = glm::clamp(localColor + reflectedColor, 0.0f, 1.0f);
    return true;
}




/// \fn reflectedRay(CRay ray, glm::vec3 n, glm::vec3 pos)
/// \brief Computes parameters of the ray reflected at the surface point with given normal vector.
/// \param ray Input ray.
/// \param n Surface normal vector.
/// \param pos Position of reflection point.
/// \return Reflected ray.
///
CRay CRayTrace::reflectedRay(const CRay& ray, const glm::vec3& n, const glm::vec3& pos) {
    CRay reflected_ray;
    glm::vec3 v = ray.dir;
    reflected_ray.pos = pos;
    reflected_ray.dir = glm::normalize(v - 2.0f * glm::dot(v, n) * n);

    return reflected_ray;
}

