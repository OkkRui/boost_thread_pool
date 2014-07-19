#include "boost_timed_thread_pool.h"


namespace yzx
{

	bool boost_timed_thread_pool::enqueue(SP_VOID_POINTER spvoid, boost::system_time &wait_until)
	{
		boost::mutex::scoped_lock  locktimer(m_timermutex);

		m_timeentryset.insert(std::make_pair(wait_until, spvoid));

		m_condtioncheck.notify_one();

		return true;
	}


	void boost_timed_thread_pool::dispatchthread()
	{

		boost::int64_t microsecondsdelay = 10000000;

		while(!m_btimerthreadstop)
		{
			boost::mutex::scoped_lock  lock(m_timermutex);
			boost::system_time nextwakeup = boost::get_system_time();
			nextwakeup = nextwakeup + boost::posix_time::microsec(microsecondsdelay);

			if (!m_condtioncheck.timed_wait(lock, nextwakeup))
			{
				if (!m_timeentryset.empty())
				{
					boost::system_time systimenow = boost::get_system_time();

					size_t   tTimeoutSize = 0;

					std::set<TIMEENTRY>::iterator itr = m_timeentryset.begin();
					for(; itr != m_timeentryset.end(); ++itr)
					{
						//check timeout
						if(itr->first <= systimenow) 
						{
							tTimeoutSize++;    
							//enqueue into working thread
							m_tworkthreadpool.enqueue(itr->second, 0);
						}
						else 
							break;
					}

					if(tTimeoutSize)  
					{   
						//delete already timeout entry
						m_timeentryset.erase(m_timeentryset.begin(), itr);              
					}
				}
			}

			if (!m_timeentryset.empty())
			{               
				//find next timeout entry
				boost::system_time systimenow = boost::get_system_time();
				boost::posix_time::millisec_posix_time_system_config::time_duration_type timediff;
				timediff = m_timeentryset.begin()->first - systimenow;  
				boost::int64_t mircosec = timediff.total_microseconds();
				microsecondsdelay = (mircosec > 0  ? mircosec : 0);
			}
			else
			{
				microsecondsdelay = 10000000;
			}
		}
		std::cout<<"timer dispatch thread exit"<<"\n";

	}



}

