#include "boost_thread_pool.h"
#include "boost_timed_thread_pool.h"
#include <iostream>
#include <string>
#include <map>


std::map<std::string , int > g_stringmap;
boost::mutex						  g_mutex;


void threadfunc1(yzx::SP_VOID_POINTER p, unsigned char ctype)
{

	boost::shared_ptr<std::string> s = boost::static_pointer_cast<std::string>(p);

	if (s)
		std::cout<<"ctype: "<<(int)ctype<<"  string: "<< *s <<"\n";


	boost::mutex::scoped_lock lock(g_mutex);
	int &count = g_stringmap[*s];
	count++;

}




void test_thread_pool()
{
	{
		yzx::boost_thread_pool btp(1, 1024);
		btp.setThreadCallBack(boost::bind(threadfunc1, _1, _2));

		btp.start();

		unsigned char i = 0;
		for (; i < 255; i++)
		{
			boost::shared_ptr<std::string> p1 (new std::string("this is string1"));
			btp.enqueue(p1, i);
		}
	}

	{
		yzx::boost_thread_pool btp(2, 1024);
		btp.setThreadCallBack(boost::bind(threadfunc1, _1, _2));

		btp.start();

		unsigned char i = 0;
		for (; i < 255; i++)
		{
			boost::shared_ptr<std::string> p1 (new std::string("this is string2"));
			btp.enqueue(p1, i);
		}
	}

	{
		yzx::boost_thread_pool btp(2, 1024);
		btp.setThreadCallBack(boost::bind(threadfunc1, _1, _2));

		btp.start();
	}


}


void test_timed_thread_pool()
{
	{
		yzx::boost_timed_thread_pool btp(1, 1024);
		btp.setThreadCallBack(boost::bind(threadfunc1, _1, _2));

		btp.start();

		unsigned char i = 0;
		for (; i < 255; i++)
		{
			boost::shared_ptr<std::string> p1 (new std::string("this is timed1 string"));

			boost::system_time timenow = boost::get_system_time();
			timenow += boost::posix_time::seconds(1);

			btp.enqueue(p1, timenow);
		}
		boost::this_thread::sleep(boost::posix_time::seconds(2));
	}

	{
		yzx::boost_timed_thread_pool btp(2, 1024);
		btp.setThreadCallBack(boost::bind(threadfunc1, _1, _2));

		btp.start();

		unsigned char i = 0;
		for (; i < 255; i++)
		{
			boost::shared_ptr<std::string> p1 (new std::string("this is timed2 string"));

			boost::system_time timenow = boost::get_system_time();
			timenow += boost::posix_time::seconds(1);

			btp.enqueue(p1, timenow);
		}
		boost::this_thread::sleep(boost::posix_time::seconds(2));
	}

	{
		yzx::boost_timed_thread_pool btp(2, 1024);
		btp.setThreadCallBack(boost::bind(threadfunc1, _1, _2));

		btp.start();
	}


}



struct printmap
{
	void operator () (std::pair<std::string , int > it)
	{
		std::cout<<it.first<<" count: "<<it.second<<"\n";
	}
};


int main()
{
	test_thread_pool();

	test_timed_thread_pool();


	struct printmap pm;

	std::for_each(g_stringmap.begin(), g_stringmap.end(), pm);


	return 0;
}

