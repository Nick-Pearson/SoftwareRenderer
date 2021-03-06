#ifndef SCENE_H
#define SCENE_H

#include "../mesh/mesh.h"

#include <memory>
#include <vector>

#include <glm/glm.hpp>

class Camera;
class Cubemap;
class Light;
class ImplicitSurface;
class Material;
class KDNode;
class Triangle;

using glm::mat4;
using glm::vec3;
using glm::mat3;

struct Intersection
{
	Vertex vertexData;
	float distance = FLT_MAX;

  Intersection() {}

  Intersection(const Vertex& inVertexData, float inDist, std::shared_ptr<Mesh> inMesh, int inTriIdx) :
    vertexData(inVertexData), distance(inDist), mesh(inMesh), triangleIndex(inTriIdx)
  {}

  Intersection(const Vertex& inVertexData, float inDist, std::shared_ptr<ImplicitSurface> inSurf, vec3 inNormal) :
    vertexData(inVertexData), distance(inDist), surf(inSurf), normal(inNormal)
  {}

  inline bool isValid() const { return mesh || surf; }
  std::shared_ptr<Material> GetMaterial() const;
  void GetNormals(vec3& outNormal, vec3& outTangent, vec3& outBitangent) const;
  vec3 GetNormal() const;
  int triangleIndex = -1;

private:

	std::shared_ptr<Mesh> mesh;

  std::shared_ptr<ImplicitSurface> surf;
  vec3 normal;
};

class Scene
{
public:

	Scene();
	~Scene();

	void Update(float DeltaSeconds);

	// returns the closest facing object along the Ray
	bool ClosestIntersection(const vec3& start, const vec3& dir, Intersection& closestIntersection, bool checkBackfaces = true) const;

	// returns any intersecting object along the ray
  bool ShadowIntersection(const vec3& start, const vec3& dir, Intersection& firstIntersection) const;

	void AddMesh(std::shared_ptr<Mesh> mesh) { if(mesh) Meshes.push_back(mesh); }
	inline const std::vector<std::shared_ptr<Mesh>>* GetMeshes() const { return &Meshes; }

  void AddLight(std::shared_ptr<Light> light) { if(light) Lights.push_back(light); }
  inline const std::vector<std::shared_ptr<Light>>* GetLights() const { return &Lights; }

  void AddSurface(std::shared_ptr<ImplicitSurface> Surface);
  inline const std::vector<std::shared_ptr<ImplicitSurface>>* GetSurfaces() const { return &Surfaces; }

	vec3 GetEnvironmentColour(const vec3& dir) const;

	Camera* camera;

	Cubemap* environment;

private:

	std::vector<std::shared_ptr<Mesh>> Meshes;
  std::vector<std::shared_ptr<Light>> Lights;
  std::vector<std::shared_ptr<ImplicitSurface>> Surfaces;

	// querys the scene for intersections, will return if the predicate function returns true
	template<typename Func>
	bool IntersectScene_Internal(const vec3& start, vec3 dir, Func Predicate, Intersection& outIntersection, bool terminateOnValidIntersection = false, bool checkBackfaces = true) const;

  template<typename Func>
	bool IntersectScene_Internal_KDNode(const vec3& start, vec3 dir, const std::shared_ptr<Mesh> mesh, Func Predicate, Intersection& outIntersection, const KDNode* node, bool terminateOnValidIntersection = false,  bool checkBackfaces = true) const;

	template<typename Func>
	static bool CalcIntersectionInternal (const glm::vec3& start, const glm::vec3& direction, const std::shared_ptr<Mesh> mesh, Func Predicate, const Triangle& triangle, Intersection &intersection, bool checkBackfaces /*= true*/);
};
#endif
