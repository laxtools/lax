#pragma once 
#include <lax/net/protocol/bits/bits_message.hpp>

namespace lax
{
namespace net
{

/// ������ �޼����� ������ ���� �޼����� �����Ѵ�.
class bits_message_factory
{
public:
	using creator = std::function<bits_message::ptr()>;

	static bits_message_factory& inst();

	/// add a creator for a protocol
	void add(const packet::topic_t& topic, creator c);

	/// create a protocol
	bits_message::ptr create(const packet::topic_t& topic) const;

	template <typename Msg>
	std::shared_ptr<Msg> create(const packet::topic_t& topic) const
	{
		auto ptr = create(name);

		return std::static_pointer_cast<Msg>(ptr);
	}

private:
	using map = std::map<const packet::topic_t, creator>;

	map map_;
};

} // net
} // lax