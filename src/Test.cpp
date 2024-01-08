#include "../include/Int.hpp"
#include "../include/ObjectPool7.hpp"
#include <iostream>
#include <string>
using namespace std;

void funa(KeyedObjectPool<std::string, Int> &mypool)
{
    auto pa = mypool.acquire("yhpInt",std::string("yhpInt"), 1);
    if (pa)
        pa->Print();

    auto pb = mypool.acquire("xssInt",std::string("xssInt"), 2);
    if (pb)
        pb->Print();
}
void funb(KeyedObjectPool<std::string, Int> &mypool)
{
    auto pa = mypool.acquire("yhInt",std::string("yhInt"), 3);
    if (pa)
        pa->Print();

    auto pb = mypool.acquire("xssInt",std::string("xssInt"), 4);
    if (pb)
        pb->Print();
}
void func(KeyedObjectPool<std::string, Int> &mypool)
{
    auto pa = mypool.acquire("zfInt",std::string("zfInt"), 5);
    if (pa)
        pa->Print();

    auto pb = mypool.acquire("xssInt",std::string("xssInt"), 6);
    if (pb)
        pb->Print();
}

void fund(KeyedObjectPool<std::string, Int> &mypool)
{
    auto pa = mypool.acquire("zfInt",std::string("zfInt"), 7);
    if (pa)
        pa->Print();

    auto pb = mypool.acquire("yhpInt",std::string("yhpInt"), 8);
    if (pb)
        pb->Print();
}
void fune(KeyedObjectPool<std::string, Int> &mypool)
{
    auto pa = mypool.acquire("yhpInt",std::string("yhpInt"), 9);
    if (pa)
        pa->Print();

    auto pb = mypool.acquire("yhInt", std::string("yhInt"),10);
    if (pb)
        pb->Print();
}
void funf(KeyedObjectPool<std::string, Int> &mypool)
{
    auto pa = mypool.acquire("yhpInt", std::string("yhpInt"),11);
    if (pa)
        pa->Print();

    auto pb = mypool.acquire("yhInt",std::string("yhInt"), 12);
    if (pb)
        pb->Print();
}
void fung(KeyedObjectPool<std::string, Int> &mypool)
{
    auto pa = mypool.acquire("yhpInt", std::string("yhpInt"),13);
    if (pa)
        pa->Print();

    auto pb = mypool.acquire("yhInt", std::string("yhInt"),14);
    if (pb)
        pb->Print();
}
void funh(KeyedObjectPool<std::string, Int> &mypool)
{
    auto pa = mypool.acquire("yhpInt", std::string("yhpInt"),15);
    if (pa)
        pa->Print();

    auto pb = mypool.acquire("xssInt", std::string("xssInt"),16);
    if (pb)
        pb->Print();
}

int main()
{
    KeyedObjectPool<std::string, Int> mypool(12, 8, 4, 5);
    std::thread tha(funa, std::ref(mypool));
    std::thread thb(funb, std::ref(mypool));
    std::thread thc(func, std::ref(mypool));
    std::thread thd(fund, std::ref(mypool));
    std::thread the(fune, std::ref(mypool));
    std::thread thf(funf, std::ref(mypool));
    std::thread thg(fung, std::ref(mypool));
    std::thread thh(funh, std::ref(mypool));

    tha.join();
    thb.join();
    thc.join();
    thd.join();
    the.join();
    thf.join();
    thg.join();
    thh.join();

    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;
    std::this_thread::sleep_for(std::chrono::seconds(20));
    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;

    printf("\n_____________________________________\n");
    cout<<"yhp obj : "<<mypool.getKeyedIdleNum("yhpInt")<<endl;
    cout<<"xss obj : "<<mypool.getKeyedIdleNum("xssInt")<<endl;
    cout<<"yh obj : "<<mypool.getKeyedIdleNum("yhInt")<<endl;
    cout<<"zf obj : "<<mypool.getKeyedIdleNum("zfInt")<<endl;
    return 0;
}

#if 0 
void funa(ObjectPool<Int> &pool)
{
    auto pa = pool.acquire(1);
    if(pa)
    {
        pa->Print();
    }

    auto pb = pool.acquire(2);
    if(pb) 
    {
        pb->Print();
    }

    auto pc = pool.acquire(3);
    if(pa)
    {
        pc->Print();
    }

    auto pd = pool.acquire(4);
    if(pd) 
    {
        pd->Print();
    }
    cout<<"funa end ..."<<endl;
    cout<<"total Object num : "<<pool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<pool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<pool.getIdleObjectNum()<<endl;
 
}

void funb(ObjectPool<Int> &pool)
{
    auto pa = pool.acquire(5);
    if(pa)
    {
        pa->Print();
    }

    auto pb = pool.acquire(6);
    if(pb) 
    {
        pb->Print();
    }

    auto pc = pool.acquire(7);
    if(pa)
    {
        pc->Print();
    }

    auto pd = pool.acquire(8);
    if(pd) 
    {
        pd->Print();
    }
    cout<<"funb end ..."<<endl;
    cout<<"total Object num : "<<pool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<pool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<pool.getIdleObjectNum()<<endl;
  
}
int main()
{
    ObjectPool<Int> mypool(8,4,2,5);
    std::thread tha(funa,std::ref(mypool));
    std::thread thb(funb,std::ref(mypool));

    tha.join();
    thb.join();
    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;
    std::this_thread::sleep_for(std::chrono::seconds(60));
    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;
}

#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

const int n = 10;
std::mutex mtx1;
std::mutex mtx2;

void PrintChar(char ch)
{
	for (int i = 0; i < n; ++i)
	{	
		//std::lock_guard<std::mutex> lock(mtx1);
		std::scoped_lock<std::mutex> lock(mtx1,mtx2);
		for (int j = 0; j < n; ++j)
		{
			printf("%c", ch);
		}
		printf("\n");
	}
	printf("\n");
	printf("\n______________________________\n");

}
int main()
{
	std::thread thar[5] = {};
	for (int i = 0; i < 5; ++i)
	{
		thar[i] = std::thread(PrintChar, 'A' + i);
	}
	for (int i = 0; i < 5; ++i)
	{
		thar[i].join();
	}

	return 0;

}

#include "../include/Int.hpp"
#include "../include/ObjectPool6.hpp"
#include <iostream>
using namespace std;

void funa(ObjectPool<Int> &pool)
{
    auto pa = pool.acquire(1);
    if(pa)
    {
        pa->Print();
    }

    auto pb = pool.acquire(2);
    if(pb) 
    {
        pb->Print();
    }

    auto pc = pool.acquire(3);
    if(pa)
    {
        pc->Print();
    }

    auto pd = pool.acquire(4);
    if(pd) 
    {
        pd->Print();
    }
    cout<<"funa end ..."<<endl;
    cout<<"total Object num : "<<pool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<pool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<pool.getIdleObjectNum()<<endl;
 
}

void funb(ObjectPool<Int> &pool)
{
    auto pa = pool.acquire(5);
    if(pa)
    {
        pa->Print();
    }

    auto pb = pool.acquire(6);
    if(pb) 
    {
        pb->Print();
    }

    auto pc = pool.acquire(7);
    if(pa)
    {
        pc->Print();
    }

    auto pd = pool.acquire(8);
    if(pd) 
    {
        pd->Print();
    }
    cout<<"funb end ..."<<endl;
    cout<<"total Object num : "<<pool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<pool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<pool.getIdleObjectNum()<<endl;
  
}
int main()
{
    ObjectPool<Int> mypool(8,4,2,5);
    std::thread tha(funa,std::ref(mypool));
    std::thread thb(funb,std::ref(mypool));

    tha.join();
    thb.join();
    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;
    std::this_thread::sleep_for(std::chrono::seconds(60));
    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;
}

void func()
{
    static int num = 0;
    cout << "func num: " << ++num << endl;
}
class ObjectPool
{
public:
    void Loop()
    {
        static int num = 0;
        cout << "ObjectPool::Loop num: " << ++num << endl;
    }
};
int main()
{
    ObjectPool objpool;
    Timer t1, t2;
    t1.init();
    t1.set_timer(func, 2000);

    t2.init();
    t2.set_timer(std::bind(&ObjectPool::Loop, objpool), 4000);

    TimerMap tmap;
    tmap.init(1000);
    tmap.add_timer(t1);
    tmap.add_timer(t2);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}

int main()
{
    ObjectPool objpool;
    Timer t1,t2;
    t1.init();
    t1.set_timer(func,2000);

    t2.init();
    t2.set_timer(std::bind(&ObjectPool::Loop,objpool),4000);

    TimerMap tmap;
    tmap.init();
    tmap.add_timer(t1);
    tmap.add_timer(t2);

    while(1)
    {
        tmap.loop(1000);
    }

}

void func()
{
    static int num = 0;
    cout<<"func num: "<<++num<<endl;
}
int main()
{
    Timer tv;
    tv.init();
    tv.set_timer(func,2000);
    for(int i = 0;i<10;++i)
    {
        tv.handle_event();
    }
    return 0;
}

#include "../include/Timestamp.hpp"
#include <iostream>
using namespace std;

int main()
{
    Timestamp te;
    te.now();
    cout<<te.toString()<<endl;
    cout<<te.toFormattedString()<<endl;
    return 0;
}

#include "../include/Int.hpp"
#include "../include/ObjectPool4.hpp"

#include <thread>
using namespace std;

void funa(ObjectPool<Int> &mypool)
{
    auto pa = mypool.acquire(1);
    if (pa)
    {
        cout << *pa << endl;
    }

    auto pb = mypool.acquire(2);
    if (pb)
    {
        cout << *pb << endl;
    }
    auto pc = mypool.acquire(1);
    if (pc)
    {
        cout << *pc << endl;
    }

    auto pd = mypool.acquire(2);
   // if (pd)
    {
        cout << *pd << endl;
    }
    auto pe = mypool.acquire(1);
   // if (pe)
    {
        cout << *pe << endl;
    }


    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;

}
void funb(ObjectPool<Int> &mypool)
{
    auto pa = mypool.acquire(4);
    if (pa)
    {
        cout << *pa << endl;
    }

    auto pb = mypool.acquire(5);
  //  if (pb)
    {
        cout << *pb << endl;
    }
    auto pc = mypool.acquire(1);
   // if (pc)
    {
        cout << *pc << endl;
    }

    auto pd = mypool.acquire(2);
    //if (pd)
    {
        cout << *pd << endl;
    }
    auto pe = mypool.acquire(1);
   // if (pe)
    {
        cout << *pe << endl;
    }
    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;

}
int main()
{
    ObjectPool<Int> mypool(4,2);
    std::thread tha(funa,std::ref(mypool));
    std::thread thb(funb,std::ref(mypool));

    tha.join();
    thb.join();

    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;
    return 0;

}


int main()
{
    ObjectPool<Int> mypool(4, 2);
    {
        auto pa = mypool.acquire(1);
        if(pa)
        {
            pa->SetValue(1);
            cout << *pa << endl;
            pa->Clear();
        }

        auto pb = mypool.acquire(2);
        if(pb)
        {
            pb->SetValue(2);
            cout << *pb << endl;
            pb->Clear();
        }

        auto pc = mypool.acquire(3);
        if(pc)
        {
            cout << *pc << endl;
        }

        auto pd = mypool.acquire(4);
        if(pd)
        {
            cout << *pd << endl;
        }

        auto pe = mypool.acquire(3);
        if(pe)
        {
            cout << *popen << endl;
        }


    }

    cout<<"total Object num : "<<mypool.getTotalObjectNum()<<endl;
    cout<<"active Object num: "<<mypool.getActivateObejctNum()<<endl;
    cout<<"idle Object num  : "<<mypool.getIdleObjectNum()<<endl;
}


int main()
{
    ObjectPool<Int> mypool;

    {
        auto pa = mypool.acquire(1);
        pa->SetValue(1);
        cout << *pa << endl;
        pa->Clear();

        auto pb = mypool.acquire(2);

        cout << *pb << endl;

        auto pc = mypool.acquire(3, 4);
        cout << *pc << endl;

        auto pd = mypool.acquire(4, 5);
        cout << *pd << endl;
    }
    cout<<"idle object : "<<mypool.getsize()<<endl;
    {
        auto pa = mypool.acquire();
        pa->SetValue(100);
        cout<<*pa<<endl;
        pa->Clear();

    }

    

    return 0;
}

int main()
{
    ObjectPool<Int>  mypool;
    {
        auto pa = mypool.acquire(1);

        cout<<*pa<<endl;

        auto pb = mypool.acquire(2);

        cout<<*pb<<endl;

        auto pc = mypool.acquire(3,4);
        cout<<*pc<<endl;
        
        auto pd = mypool.acquire(4,5);
        cout<<*pd<<endl;
    }
    cout<<"idle object : "<<mypool.getsize()<<endl;
    {
        auto pc = mypool.acquire();
        cout<<*pc<<endl;
        
        auto pd = mypool.acquire();
        cout<<*pd<<endl;

        cout<<"idle object : "<<mypool.getsize()<<endl;
    }

    cout<<"idle object : "<<mypool.getsize()<<endl;

}
#endif