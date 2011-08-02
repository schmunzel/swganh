#include "portal_object2D.h"
#include <glm/core/func_geometric.hpp>

using namespace swganh::portals;
using namespace anh::component;

EntityId PortalObject2D::detect_parent(EntityId current, glm::vec3 pos1, glm::vec3 pos2)
{
	return 0; //@todo fix me
}

bool PortalObject2D::intersects_triangle_(glm::vec3 v1, glm::vec3 v2, glm::vec3 va, glm::vec3 vb, glm::vec3 vc)
{
	glm::vec3 n = glm::cross(vb - va, vc - va);  // normal vector for plane containing triangle

	if(glm::dot(n, v2 - v1) == 0) 
	{
		// line will never intersect plane, both points must be on same side
		return false;
	}

	// t is the distance along the line segment traveled before intersecting, starting at v1, from 0 (on v1) to 1 (on v2)
	float t = (n.x*(v1.x - va.x) + n.y*(v1.y - va.y) + n.z*(v1.z - va.z)) / (n.x*(v1.x - v2.x) + n.y*(v1.y - v2.y) + n.z*(v1.x - v2.z));

	if(t < 0 || t > 1) 
	{
		// intersection occurred beyond this segment
		return false;
	}

	glm::vec3 p; // point of line<->plane intersection
	p.x = v1.x*(1 - t) + t*v2.x;
	p.y = v1.y*(1 - t) + t*v2.y;
	p.z = v1.z*(1 - t) + t*v2.z;

	return same_side_(p, va, vb, vc) && same_side_(p, vb, va, vc) && same_side_(p, vc, va, vb);
}

bool PortalObject2D::same_side_(const glm::vec3 p1, const glm::vec3 p2, const glm::vec3 a, const glm::vec3 b)
{
	return glm::dot(glm::cross(b - a, p1 - a), glm::cross(b - 1.0f, p2 - a)) >= 0;
}