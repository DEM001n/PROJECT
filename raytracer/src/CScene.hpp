//
//  CScene.hpp
//  rt
//
//  Created by Radoslaw Mantiuk on 22/01/2023.
//

#ifndef CScene_hpp
#define CScene_hpp

class BVHNode {
public:
    CSphere* boundingSphere = nullptr; // kula ograniczająca
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;
    CObject* object = nullptr; // tylko w liściu

    ~BVHNode() {
        delete boundingSphere;
        delete left;
        delete right;
    }

    bool isLeaf() const {
        return object != nullptr;
    }
};

class BVH {
public:
    BVHNode* root = nullptr;

    void build(const std::vector<CObject*>& objs);
    bool intersect(const CRay& ray, float& tClosest, CObject*& hitObj) const;
    bool intersectAny(const CRay& ray, float maxDist) const;

private:
    BVHNode* buildRecursive(std::vector<CObject*>& objs);
    CSphere* computeBoundingSphere(const std::vector<CObject*>& objs);
    bool intersectNode(BVHNode* node, const CRay& ray, float& tClosest, CObject*& hitObj) const;
};

/// \class CCamera
/// \brief Class with camera parameters.
///
class CCamera {
public:
    int width = 0; ///< Horizontal resolution in pixels.
    int height = 0; ///< Vertical resolution in pixels.
    float fov = 0; ///< Field-of-view (<0,180) range)
    glm::vec3 eyep = {0,0,0}; ///< Camera position.
    glm::vec3 lookp = {0,0,0}; ///< Camera target point.
    glm::vec3 up = {0,0,0}; ///< Camera orientation.

};


/// \class CLight
/// \brief Class with light source parameters.
///
class CLight  {
public:
    glm::vec3 pos; ///< Light position.
    glm::vec3 color; ///< Light color.
    
    CLight() {
        color = {1,1,1};
    }
    
    explicit CLight(const glm::vec3& pos) {
        this->pos = pos;
    }
};


/// \class CScene
/// \brief Container for the scene components.
///
class CScene {
public:
    CCamera camL,camR; ///< Camera object.
    std::vector<CObject*> objectList; ///< List of geometric objects.
    std::vector<CLight> lightList; ///< List of the light sources.
    BVH bvh;
    
public:
    void create(float width, float height, float fov, glm::vec3 eyep, glm::vec3 lookp,glm::vec3 up, float n);

    void buidBVH(){bvh.build(objectList);};
};
#endif /* CScene_hpp */
