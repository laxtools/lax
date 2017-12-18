#include "stdafx.h"
#include <lax/channel/topic.hpp>

namespace lax
{
namespace channel
{

bool topic::use_domain_field = true;
uint8_t topic::domain_bits = 16;

} // channel
} // lax