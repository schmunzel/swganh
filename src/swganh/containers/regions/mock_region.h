#ifndef MOCK_REGION_H
#define MOCK_REGION_H

#include <api/region_interface.h>

namespace
{
	class mock_region : public container_system::region_interface
	{
	public:
		
		MOCK_METHOD0(verts, const std::vector<glm::vec3>&());
		MOCK_METHOD1(within_extent, bool(const glm::vec3&));
		
		MOCK_METHOD1(on_appear, void(std::shared_ptr<anh::component::Entity>));
		MOCK_METHOD1(on_disappear, void(std::shared_ptr<anh::component::Entity>));
		MOCK_METHOD1(on_enter, void(std::shared_ptr<anh::component::Entity>));
		MOCK_METHOD1(on_exit, void(std::shared_ptr<anh::component::Entity>));

	};
};

#endif