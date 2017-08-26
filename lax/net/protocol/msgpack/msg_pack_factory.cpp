#include "stdafx.h"

#include "msg_pack_factory.h"
#include <lax/util/logger.h>

namespace lax
{
namespace net
{

msg_pack_factory& msg_pack_factory::instance()
{
	// thread-unsafe singleton

	static msg_pack_factory instance_;

	return instance_;
}

msg_pack_factory::result msg_pack_factory::add(msg_pack::ptr mp)
{
	check(mp);

	return_if(
		mp->get_key() == 0, 
		result(false, reason::fail_invalid_parameter)
	);

	auto iter = prototypes_.find(mp->get_key());

	return_if(
		iter != prototypes_.end(), 
		result(false, reason::fail_msg_pack_duplicate_add)
	);

	prototypes_[mp->get_key()] = mp;

	return result(true, reason::success);
}

msg_pack::ptr msg_pack_factory::create(uint32_t key)
{
	check(key > 0);

	auto iter = prototypes_.find(key);
	return_if(iter == prototypes_.end(), msg_pack::ptr());
	
	return iter->second->create();
}

} // net
} // lax
