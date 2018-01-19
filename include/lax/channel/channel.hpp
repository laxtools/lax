#pragma once 

#include <lax/channel/message.hpp>
#include <lax/channel/sub.hpp>
#include <lax/channel/detail/sub_map.hpp>
#include <lax/util/concurrent_queue.hpp>
#include <atomic>
#include <functional>

namespace lax
{
namespace channel
{

/// �޼��� ���� ä�� 
/** 
 * channel�� �޼����� �����ϴ� ��θ� ����
 *  topic(uint32_t)�� lambda �Լ� (std::function)�� ���
 *  ä�ΰ� ���ᵵ �Լ��� ����Ѵ�. 
 *  ������ Ű ��ü, Ÿ�ٿ� ���ؼ��� subscribe �� �� �ִ�. 
 *  �������� �� Ű ��ü, Ÿ�ٿ� ���ؼ��� ������ �õ��Ѵ�.
 *
 * ����: 
 *  channel::create(name) 
 * 
 * ���: 
 *  auto ch = channel::find( "key" );
 *  auto skey = ch->sub();
 *  ch->unsub(skey); 
 *
 * ��������: 
 *  ä���� �����ϴ� ������ post() �ֱ����� ȣ��
 *  
 * �Ҹ�: 
 *  channel::destroy(name)
 * 
 * ���(sub):
 *   ���� ������ ���� 
 *   topic - �ް��� �ϴ� ���� (����) 
 *   cond - ���� üũ (���� ���͸�) 
 *   cb - �ݹ� 
 * 
 * ���� (post) ���: 
 *   ���(immediate) ���� push()�� �� ����. 
 *   ����(delayed) ���� post() ȣ�� �� ����
 */
class channel
{
public:
	using ptr = std::shared_ptr<channel>;
	using key_t = sub::channel_key_t;
	using cond_t = sub::cond_t;
	using cb_t = sub::cb_t;

	/// config 
	struct config
	{
		/// limit post count in a post loop
		std::size_t loop_post_limit = 2048;

		/// ���� �޼��� ���� ���� �ð��� �� �ð��� ���� ��� �α� ����
		float time_to_log_when_post_time_over = 0.02f;

		/// post() ������ ��ü ���� ���� �ð��� �� �ð��� ���� ��� �α� ����
		float time_to_log_when_post_loop_timeover = 0.2f;

		/// shared lock ��� ���� ����
		bool use_lock = true;

		/// ���ȿ� ��ϵ� �ݹ��� ���� ��� �α� ����
		bool log_no_sub_when_post = false;

		/// �Ҹ��ڿ��� ������ ��ü ó���ϰ� ������ �� ����
		bool post_all_when_destroyed = true;

		/// ť�� ���� �ʿ䰡 �ִ� �� ���� üũ
		bool check_delayed_sub_before_enqueue = true;
	};

	/// thread-unsafe stat block
	struct stat
	{
		std::size_t post_time_over_count = 0;
		std::size_t post_loop_time_over_count = 0;

		std::size_t total_post_count = 0;
		std::size_t total_immediate_post_count = 0;
		std::size_t last_post_count = 0;
	};

public:
	/// create a channel and register it to a map
	/**
	 * @return ������ ä��. �ߺ��Ǹ� ������ channel::ptr() ����.
	 */
	static ptr create(const key_t& key, const config& cfg);

	/// find a channel with a key
	static ptr find(const key_t& key);

	/// destroy a channel
	static bool destroy(const key_t& key);

public:
	channel(const key_t& key)
		: channel(key, config())
	{
	}

	/// ������ 
	/** 
	 * key�� ���� ������ ����
	 *
	 * @param key - ä���� ���� �̸� 
	 * @param cfg - ���� ���� 
	 */
	channel(const key_t& key, const config& cfg);

	/// �Ҹ���
	~channel();

	/// push 
	/**
	 * �޼����� �������ε� ������. 
	 * ť�� ���� �ְ� �������� ����.
	 * @param m - message
	 *
	 * @return number of dispatched count
	 */
	std::size_t push(message::ptr m);

	/// push with a separate topic from message's topic
	/**
	 * �ܺο��� �����ϴ� �������� ������. 
	 * ���� �޼����� �������ε� ������. 
	 * ť�� ���� �ְ� �������� ����.
	 *
	 * @param topic - topic to dispatch first
	 * @param m - message
	 *
	 * dispatch with a topic first. then, call push(m) inside.
	 *
	 * @return number of dispatched cont
	 */
	std::size_t push(const message::topic_t& topic, message::ptr m);

	/// subscribe to a topic with a condition 
	/**
	 * @param topic - a topic to subscribe
	 * @param cond - condition to check before dispatch
	 * @param cb - callback function
	 * @param mode - dispatch mode
	 *
	 * @return the key to the subscription
	 */
	sub::key_t subscribe(
		const message::topic_t& topic,
		cond_t cond, 
		cb_t cb, 
		sub::mode mode = sub::mode::delayed
	);

	/// subscribe to a topic without condition 
	/**
	 * @param topic - a topic to subscribe
	 * @param cb - callback function
	 *
	 * @return the key to the subscription
	 */
	sub::key_t subscribe(
		const message::topic_t& topic,
		cb_t cb, 
		sub::mode mode = sub::mode::delayed
	);

	/// unsubscribe 
	/**
	 * @param key - a subscription key
	 *
	 * @return ��ϵ� �� ����� true, ������ false
	 */
	bool unsubscribe(sub::key_t key);

	/// post all messages in queue
	/**
	 * sub::mode::delayed subscriptions are handled for messages
	 */
	std::size_t post();

	/// Ű�� ������
	const key_t& get_key() const 
	{
		return key_;
	}

	/// queue size (unsafe)
	std::size_t get_queue_size() const;

	/// stat
	const stat& get_stat() const
	{
		return stat_;
	}

private:
	using mq = util::concurrent_queue<message::ptr>;

	void enqueue_checked(const message::topic_t& topic, message::ptr m);

private:
	key_t   key_;
	mq		q_;
	sub_map map_;
	config  config_;
	stat	stat_;
};

} // channel
} // lax
