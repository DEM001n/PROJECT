
#include <iostream>


#include "rt.h"
#include <chrono>

extern void draw_robot();
extern void draw_triangles();

unsigned int parseVertexIndex(const std::string& token) {
    size_t slashPos = token.find('/');
    if (slashPos != std::string::npos) {
        return std::stoi(token.substr(0, slashPos));
    }
    return std::stoi(token);
}

bool loadOBJtoScene(const std::string& filename, CScene& scene,
                    glm::vec3 matAmbient,
                    glm::vec3 matDiffuse,
                    glm::vec3 matSpecular,
                    float matShininess,
                    float reflectance,
                    bool isTexture) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku: " << filename << std::endl;
        return false;
    }

    std::vector<glm::vec3> vertices;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);

        } else if (prefix == "f") {
            std::string v1_str, v2_str, v3_str;
            iss >> v1_str >> v2_str >> v3_str;

            unsigned int i1 = parseVertexIndex(v1_str);
            unsigned int i2 = parseVertexIndex(v2_str);
            unsigned int i3 = parseVertexIndex(v3_str);

            glm::vec3 v0 = vertices[i1 - 1];
            glm::vec3 v1 = vertices[i2 - 1];
            glm::vec3 v2 = vertices[i3 - 1];

            CTriangle* triangle = new CTriangle(v0, v1, v2);
            triangle->matAmbient = matAmbient;
            triangle->matDiffuse = matDiffuse;
            triangle->matSpecular = matSpecular;
            triangle->matShininess = matShininess;
            triangle->reflectance = reflectance;
            triangle->isTexture = isTexture;

            scene.objectList.push_back(triangle);
        }
    }

    file.close();
    return true;
}
//==========================================================================

int main (int argc, char * const argv[]) {

    std::cout << "ray tracing ... \n";

    CScene scene;
    scene.create(500,400,50,{0,-4,10},{0,0,0},{0,1,0},0.5); // defines sample scene parameters
    if(scene.camL.width == 0 || scene.camR.width == 0) {
        std::cout << "WARNING: scene not defined" << std::endl;
        return -1;
    }

    CRayTrace rt;
    CRay ray;
    COutput results;

    glm::mat3 ray_matrix1,ray_matrix2;
    CRayTrace::compPrimaryRayMatrix(scene.camL, ray_matrix1);
    CRayTrace::compPrimaryRayMatrix(scene.camR, ray_matrix2);

    CImage image1(scene.camL.width, scene.camR.height);

    CLight light1(glm::vec3(-3,-2,8));
    light1.color = {0.6,0.6,0.6};
    scene.lightList.push_back(light1);

    CLight light2(glm::vec3(5,-2,8));
    light2.color = {0.3,0.3,0.3};
    scene.lightList.push_back(light2);

    if (loadOBJtoScene("I:\\GIW2025_220B_Mahaliuk_Dmytro\\raytracer\\obj\\loco.obj", scene,{0.1,0.1,0.1},{0.4,0.4,0.4},{0,0,0},0,0,false))
        //I:\\GIW2025_220B_Mahaliuk_Dmytro\\raytracer\\obj\\loco.obj
        // /home/md55722/GIW2025_220B_Mahaliuk_Dmytro/raytracer/obj/loco.obj
        {
        std::cout << "Zaladowano: " << scene.objectList.size() << " trojkatow.\n";
    } else {
        std::cerr << "Blad wczytywania modelu.\n";
        return -1;
    }

    std::vector<CObject*> triangleObjects;

    for (CObject* obj : scene.objectList) {
        if (dynamic_cast<CTriangle*>(obj))
            triangleObjects.push_back(obj);
    }

    scene.bvh.build(triangleObjects);

    auto start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < scene.camL.height; j++) {
        for(int i = 0; i < scene.camL.width; i++) {
            float   fx = (float)i + 0.5f,
                    fy = (float)j + 0.5f;

            glm::vec3 f(fx, fy, 1.0f);

            ray.pos = scene.camL.eyep;
            ray.dir = glm::normalize(ray_matrix1 * f);

            results.col = {0.0,0.0,0.0};
            results.energy = 1.0f;
            results.tree = 0;

            rt.rayTrace(scene,ray,results);

            if(results.col.x > 1 || results.col.y > 1 || results.col.z > 1 )
                results.col = {1,1,1};


            image1.setPixel(i,j,{results.col.x,0,0});
        }   //0.30, 10.4, 45.6, 107, 212 //Y = 2.2894
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Czas wykonania pętli_L: " << duration.count() << " ms\n";

    auto start1 = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < scene.camR.height; j++) {
        for(int i = 0; i < scene.camR.width; i++) {
            float   fx = (float)i + 0.5f,
                    fy = (float)j + 0.5f;

            glm::vec3 f(fx, fy, 1.0f);

            ray.pos = scene.camR.eyep;
            ray.dir = glm::normalize(ray_matrix2 * f);

            results.col = {0.0,0.0,0.0};
            results.energy = 1.0f;
            results.tree = 0;

            rt.rayTrace(scene,ray,results, true);

            if(results.col.x > 1 || results.col.y > 1 || results.col.z > 1 )
                results.col = {1,1,1};

            image1.setPixel(i,j,{0,results.col.y,results.col.z});//{0,results.col.y,results.col.z}
        }
    }
    auto end1= std::chrono::high_resolution_clock::now();
    auto duration1= std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

    std::cout << "Czas wykonania pętli_R: " << duration1.count() << " ms\n";

    //image1.save("/home/md55722/GIW2025_220B_Mahaliuk_Dmytro/raytracer/images/testBVH.png", true);
    image1.save("I:\\GIW2025_220B_Mahaliuk_Dmytro\\raytracer\\images\\final.png", true);
    cv::imshow("image1", image1.getImage());

    cv::waitKey();


    std::cout << std::endl << std::endl;
    return 0;
}
