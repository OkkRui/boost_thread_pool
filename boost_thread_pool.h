#ifndef BOOST_THREAD_POOL_YZX
#define BOOST_THREAD_POOL_YZX


#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>


namespace yzx
{



	typedef boost::shared_ptr<void>  SP_VOID_POINTER;
	typedef boost::function<void (SP_VOID_POINTER, unsigned char) >  Thread_Func;

	typedef struct 
	{    
		SP_VOID_POINTER  spvoid; 
		unsigned char type;

	}BufferItem;


	class boost_thread_pool
	{   
	public:
		boost_thread_pool(size_t nthreadnum, size_t cirbufersize):m_threadnum(nthreadnum),
			m_cbufsize(cirbufersize), m_cbuf(cirbufersize)
		{


		}

		~boost_thread_pool();

		void start();


		void stop();

		//block until success into circular buffer
		bool enqueue(SP_VOID_POINTER p, unsigned char ctype);

		//wait most
		bool timeEnqueue(SP_VOID_POINTER p, unsigned char ctype, size_t millisec); 

		void dequeueCallThread ();

		void setThreadCallBack(Thread_Func tf)
		{
			m_tf = tf;
		}


		bool isNotEmpty()
		{
			return m_cbuf.size() > 0;
		}

		bool isNotFull()
		{
			return m_cbuf.size() < m_cbufsize ;
		}


	private:

		bool enqueue_internal(SP_VOID_POINTER p, unsigned char ctype);


		size_t       m_threadnum;
		size_t       m_cbufsize;

		boost::mutex        m_cbufmutex;
		boost::condition    m_cond_not_full;
		boost::condition    m_cond_not_empty;
		boost::circular_buffer<BufferItem> m_cbuf;


		Thread_Func  m_tf;
		boost::thread_group m_thrGp;

	};
}


#endif

