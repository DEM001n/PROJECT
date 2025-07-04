//
//  CScene.cpp
//  rt
//
//  Created by Radoslaw Mantiuk on 22/01/2023.
//

#include "rt.h"

#include<fstream>
#include "../3rd/glm/gtx/norm.hpp"

void BVH::build(const std::vector<CObject*>& objs) {
    std::vector<CObject*> copy = objs;
    root = buildRecursive(copy);
}

BVHNode* BVH::buildRecursive(std::vector<CObject*>& objs) {
    BVHNode* node = new BVHNode();
    node->boundingSphere = computeBoundingSphere(objs);

    if (objs.size() == 1) {
        node->object = objs[0];
        return node;
    }

    /*
    std::sort(objs.begin(), objs.end(), [](CObject* a, CObject* b) {
        return a->getCenter().x < b->getCenter().x;
    });
    */

    size_t mid = objs.size() / 2;
    std::vector<CObject*> leftObjs(objs.begin(), objs.begin() + mid);
    std::vector<CObject*> rightObjs(objs.begin() + mid, objs.end());

    node->left = buildRecursive(leftObjs);
    node->right = buildRecursive(rightObjs);

    return node;
}

CSphere* BVH::computeBoundingSphere(const std::vector<CObject*>& objs) {
    glm::vec3 minPt(FLT_MAX);
    glm::vec3 maxPt(-FLT_MAX);

    for (auto obj : objs) {
        if (CTriangle* tri = dynamic_cast<CTriangle*>(obj)) {
            minPt = glm::min(minPt, glm::min(tri->v0, glm::min(tri->v1, tri->v2)));
            maxPt = glm::max(maxPt, glm::max(tri->v0, glm::max(tri->v1, tri->v2)));
        }
    }

    glm::vec3 center = (minPt + maxPt) * 0.5f;
    float radius = glm::length(maxPt - center);

    return new CSphere(center, radius + 0.01f);
}


bool BVH::intersect(const CRay& ray, float& tClosest, CObject*& hitObj) const {
    if (!root) return false;
    tClosest = 1e30f;
    hitObj = nullptr;
    return intersectNode(root, ray, tClosest, hitObj);
}

bool BVH::intersectNode(BVHNode* node, const CRay& ray, float& tClosest, CObject*& hitObj) const {
    if (!node || !node->boundingSphere) return false;

    float t = node->boundingSphere->intersect(ray);
    if (t < 0.0001f) return false;

    if (node->isLeaf()) {
        float tObj = node->object->intersect(ray);
        if (tObj > 0 && tObj < tClosest) {
            tClosest = tObj;
            hitObj = node->object;
            return true;
        }
        return false;
    }

    bool hitL = intersectNode(node->left, ray, tClosest, hitObj);
    bool hitR = intersectNode(node->right, ray, tClosest, hitObj);

    return hitL || hitR;
}

bool intersectAnyNode(BVHNode* node, const CRay& ray, float maxDist) {
    if (!node || !node->boundingSphere) return false;

    float t = node->boundingSphere->intersect(ray);
    if (t < 0.0001f || t > maxDist) return false;

    if (node->isLeaf()) {
        float tObj = node->object->intersect(ray);
        return (tObj > 0.0001f && tObj < maxDist);
    }

    return intersectAnyNode(node->left, ray, maxDist) || intersectAnyNode(node->right, ray, maxDist);
}

bool BVH::intersectAny(const CRay& ray, float maxDist) const {
    return intersectAnyNode(root, ray, maxDist);
}

/// \fn create(void)
/// \brief Adds components to the scene.
///
void CScene::create(float width, float height, float fov, glm::vec3 eyep, glm::vec3 lookp,glm::vec3 up,float n) {

    lightList.clear();
    objectList.clear();
    glm::vec3 ab = eyep - lookp;

    glm::vec3 d(-ab.y, ab.x, 0.0f);

    if (glm::length2(d) < 1e-6f) {
        std::cerr << "Wektor kierunkowy leży w osi Z — dowolna prosta w XY pasuje.\n";
        return;
    }

    d = glm::normalize(d);

    glm::vec3 p1 = eyep + d * n;
    glm::vec3 p2 = eyep - d * n;

    camL.width = width;
    camL.height = height;
    camL.fov = glm::radians(fov);
    camL.eyep = p1;
    camL.lookp = lookp;
    camL.up = up;

    camR.width = width;
    camR.height = height;
    camR.fov = glm::radians(fov);
    camR.eyep = p2;
    camR.lookp = lookp;
    camR.up = up;

}

