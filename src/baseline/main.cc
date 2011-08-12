/*
 This file is part of SWGANH. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2011 The SWG:ANH Team

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <iostream>

#include <glog/logging.h>
#include <boost/thread.hpp>

#include "anh/app/kernel_interface.h"
#include "anh/plugin/bindings.h"
#include "anh/plugin/plugin_manager.h"
#include "anh/service/service_manager.h"

#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/sqlstring.h>

#include "baseline_service.h"
#include "baseline_delta.h"

using namespace anh;
using namespace component;
using namespace app;
using namespace plugin;
using namespace service;
using namespace event_dispatcher;
using namespace std;

extern "C" PLUGIN_API void ExitModule() 
{
    return;
}

extern "C" PLUGIN_API ExitFunc InitializePlugin(shared_ptr<KernelInterface> kernel) 
{
	baseline::baseline_service bs(kernel);
	auto bd = std::make_shared<baseline::baseline_delta>("CREO", 4);
	bs.attach_baseline_delta("CREO4", bd);
	
	bd->attach_updatable(0, [] (std::shared_ptr<Entity> e, ByteBuffer& b) {
		b.write(0);
	});

    return ExitModule;
}
