#include "boost_thread_pool.h"
#include <iostream>



namespace yzx{

	boost_thread_pool::~boost_thread_pool()
	{
		stop();
		std::cout<<"~boost_thread_pool"<<"\n";
	}


	void boost_thread_pool::start()
	{
		size_t i = 0;
		for (; i < m_threadnum; i ++)
		{
			m_thrGp.create_thread(boost::bind(&boost_thread_pool::dequeueCallThread, this));
		}
	}


	void boost_thread_pool::stop()
	{

		size_t i = 0;
		for (; i < m_threadnum; i ++)
		{

			boost::shared_ptr<void> nullsptr;
			enqueue_internal(nullsptr, 0);
		}            

		m_thrGp.join_all();
		m_cbuf.clear();

	}

	//user canot put null pointer
	bool boost_thread_pool::enqueue(SP_VOID_POINTER p, unsigned char ctype)
	{
		if (!p)
		{
			return false;
		}
		return enqueue_internal(p, ctype);
	}


	bool boost_thread_pool::enqueue_internal(SP_VOID_POINTER p, unsigned char ctype)
	{
		BufferItem bufitem = {p ,ctype};

		boost::mutex::scoped_lock lock(m_cbufmutex);
		m_cond_not_full.wait(lock, boost::bind(&boost_thread_pool::isNotFull, this));
		m_cbuf.push_front(bufitem);
		lock.unlock();
		m_cond_not_empty.notify_one();

		return true;
	}


	void boost_thread_pool::dequeueCallThread()
	{
		while (1)
		{
			boost::mutex::scoped_lock lock(m_cbufmutex);
			m_cond_not_empty.wait(lock, boost::bind(&boost_thread_pool::isNotEmpty, this));
			BufferItem item = m_cbuf.back();
			m_cbuf.pop_back();
			lock.unlock();
			m_cond_not_full.notify_one();

			if (item.spvoid)
			{
				m_tf(item.spvoid, item.type);
			}
			else
			{
				std::cout<<"recv null pointer, it will exit"<<"\n";
				break;
			}
		}
	}
}


