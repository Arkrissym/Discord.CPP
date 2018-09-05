#include "static.h"

#ifndef _WIN32
#include <pplx/threadpool.h>
#include <boost/asio/deadline_timer.hpp>
#else

#endif

pplx::task<void> waitFor(const std::chrono::milliseconds ms) {
	pplx::task_completion_event<void> tce;

#ifndef _WIN32
	auto &ioService = crossplat::threadpool::shared_instance().service();

	auto timer = std::make_shared<boost::asio::deadline_timer>(ioService);
	timer->expires_from_now(boost::posix_time::milliseconds(ms.count()));
	timer->async_wait([timer, tce](const boost::system::error_code &error) {
		if (error) {
			std::stringstream ss;
			ss << "timer error or cancel, because: " << error.message();
			tce.set_exception(std::runtime_error(ss.str()));
		}
		else {
			tce.set();
		}
	});
#else
	pplx::wait((unsigned int)ms.count());
	tce.set();
#endif

	return pplx::create_task(tce);
}
