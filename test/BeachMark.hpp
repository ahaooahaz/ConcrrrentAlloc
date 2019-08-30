#pragma once
#include "Common.h"
#include <iostream>
#include <time.h>
#include <thread>
#include <vector>
#include "ConcurrentAlloc.h"

using std::cout;
using std::endl;


namespace AHAOAHA{
	void TChandler(size_t count, size_t bytes)
	{
		std::vector<void*> v_ptr;
		for (size_t i = 0; i < count; ++i)
		{
			v_ptr.push_back(ConcurrentAlloc(bytes));	//ÉêÇëÄÚ´æ
		}

		//TODO	ÊÍ·Å
		for (auto & e : v_ptr)
		{
			ConcurrentFree(e);
		}
	}

	void MLhandler(size_t count, size_t bytes)
	{
		std::vector<void*> v_ptr;
		for (size_t i = 0; i < count; ++i)
		{
			v_ptr.push_back(malloc(bytes));	//ÉêÇëÄÚ´æ
		}

		//TODO	ÊÍ·Å
		for(auto & e : v_ptr)
		{
			free(e);
		}
	}

	void TCBeachMark(size_t thr_num, size_t bytes, size_t count)
	{
		std::vector<std::thread> v_thr;
		for(size_t i =0; i < thr_num; ++i)
		{
			v_thr.push_back(std::thread(&TChandler, count, bytes));
		}


		for(size_t i = 0; i < v_thr.size(); ++i)
		{
			v_thr[i].join();
		}
	}

	void MLBeachMark(size_t thr_num, size_t bytes, size_t count)
	{
		std::vector<std::thread> v_thr;
		for (size_t i = 0; i < thr_num; ++i)
		{
			v_thr.push_back(std::thread(&MLhandler, count, bytes));
		}

		for (size_t i = 0; i < v_thr.size(); ++i)
		{
			v_thr[i].join();
		}
	}
	void BeachMark(size_t thr_num, size_t bytes, size_t count)
	{
		clock_t tc_beign = clock();
		TCBeachMark(thr_num, bytes, count);
		clock_t tc_end = clock();

		clock_t ml_begin = clock();
		MLBeachMark(thr_num, bytes, count);
		clock_t ml_end = clock();

		cout << "TCTIME: " << tc_end - tc_beign << endl;
		cout << "MLTIME: " << ml_end - ml_begin << endl;
	}
}