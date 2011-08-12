#ifndef BASELINE_CREATURE_H
#define BASELINE_CREATURE_H

#include <memory>
#include <baseline/baseline_service.h>

namespace baseline { namespace baseline_deltas {

	void init_creo1(std::shared_ptr<baseline::baseline_service>& serv);
	void init_creo4(std::shared_ptr<baseline::baseline_service>& serv);
	void init_creo3(std::shared_ptr<baseline::baseline_service>& serv);
	void init_creo6(std::shared_ptr<baseline::baseline_service>& serv);

	void init_creature(std::shared_ptr<baseline::baseline_service> serv);

}
}

#endif