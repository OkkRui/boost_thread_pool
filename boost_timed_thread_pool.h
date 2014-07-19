#ifndef BOOST_TIMED_THREAD_POOL_YZX
#define BOOST_TIMED_THREAD_POOL_YZX

#include "boost_thread_pool.h"


#include <set>


namespace yzx
{


	class boost_timed_thread_pool
	{
	public:
		boost_timed_thread_pool(size_t nthreadnum, size_t cirbufersize)
			:m_tworkthreadpool(nthreadnum, cirbufersize)
		{
			m_btimerthreadstop = false;
		}

		~boost_timed_thread_pool()
		{
			stop();
		}

		void start()
		{
			m_timecheckerthread.create_thread(boost::bind(&boost_timed_thread_pool::dispatchthread, this));
			m_tworkthreadpool.start();
		}

		void stop()
		{
			notifice_thread_exit();
			m_tworkthreadpool.stop();
			m_timecheckerthread.join_all();
			m_timeentryset.clear();
		}


		//later
		bool enqueue(SP_VOID_POINTER spvoid, boost::system_time &wait_until);

		//setworking thread
		void setThreadCallBack(Thread_Func f)
		{
			m_tworkthreadpool.setThreadCallBack(f);
		}

		void dispatchthread();

	private:

		void	notifice_thread_exit()
		{
			m_btimerthreadstop = true;
			boost::mutex::scoped_lock  locktimer(m_timermutex);
			m_condtioncheck.notify_one();
		}

		bool								m_btimerthreadstop;


		boost_thread_pool			m_tworkthreadpool;
		boost::thread_group	    m_timecheckerthread;

		Thread_Func					m_workingtf;


		boost::mutex					m_timermutex;
		boost::condition			m_condtioncheck;

		typedef std::pair<boost::system_time, SP_VOID_POINTER>  TIMEENTRY;

		std::set<TIMEENTRY>     m_timeentryset;
	};



}


#endif

