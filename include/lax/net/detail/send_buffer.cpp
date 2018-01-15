#include "stdafx.h"
#include "send_buffer.hpp"

template <std::size_t Length>
typename send_buffer<Length>::pool send_buffer<Length>::pool_;
