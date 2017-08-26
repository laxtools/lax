#include "stdafx.h"

#include "directory.h"

namespace lax
{
namespace actor
{

bool directory::update(ref::ptr ptr)
{
	check(ptr);
	return_if(!ptr, false);

	check(ptr->get_id().is_valid());
	return_if(!ptr->get_id().is_valid(), false);

	if (ptr->get_id().is_node())
	{
		// 없을 때만 추가
		if (!node_index_.find(ptr->get_id().node))
		{
			// try insert 
			auto rc = node_index_.link(ptr->get_id().node, ptr->get_id());

			if (!rc)
			{
				// node는 추가되지 않으면 문제 발생
				return false;
			}
		}
	}

	check(ptr->get_role() > 0);

	// try update
	(void)role_index_.link(ptr->get_role(), ptr->get_id()); 

	// replace
	dir_[ptr->get_id()] = ptr;

	return true;
}

ref::ptr directory::find(id aid) const
{
	auto iter = dir_.find(aid);
	return_if(iter == dir_.end(), ref::ptr());

	return iter->second;
}

directory::refs directory::find_role(ref::role role) const
{
	directory::refs refs;
	role_index::vec vec;

	if (role_index_.find(role, vec))
	{
		for (auto& id : vec)
		{
			auto ptr = find(id);

			if (ptr)
			{
				refs.push_back(ptr);
			}
		}
	}

	return refs;
}

ref::ptr directory::find_node(ref::node node) const
{
	auto rc = node_index_.find(node);

	if (rc)
	{
		return find(rc.value);
	}

	return ref::ptr();
}

} // actor 
} // lax


