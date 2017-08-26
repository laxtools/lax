#pragma once

namespace lax
{
namespace actor
{

/// actor / cluster level msg category and type 
enum msg_types
{
	type_invalid, 
	type_cluster, 
	type_end, 
};

enum msg_seqs
{
	seq_invalid, 
	seq_cluster_update_state, 
	seq_end, 
};

} // actor 
} // lax
