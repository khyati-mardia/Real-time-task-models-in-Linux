//============================================================================
// Name        : AssignmentOne.c
// Version     :
// Copyright   : Your copyright notice
// Description : Assignment 1 ESP LAB By Chandrika and Khyati Mardia
// ASU ID Chandrika : 1215321133
// ASU ID Khyati : 1215346587
//============================================================================

//#include <stdio.h>
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <sched.h>
#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <errno.h>
#include <ctime>
#include <unistd.h>
#include <ctype.h>
#include <sched.h>
#include<vector>
#include <sstream>
#include <bits/stdc++.h>
#include<linux/input.h>
#include "logger.h"
void setThreadAttributes(int, pthread_attr_t);
int numberOfTasks;

#define MOUSEFILE "/dev/input/event6"
void* periodic_task(void* periodicParameters);
void* generic_task(void* periodicParameters);
void *aperiodic_task(void* functionParameters);
#define REL     0
using namespace std;
void* notifyWhenMouseClickOccurs(void* params);

pthread_mutex_t aperiodic_mutex;
pthread_cond_t button_release;
pthread_cond_t MAX_PRIORITY_TASK;

/**
 * Handles incase of error
 */
#define handle_error_en(en, msg) \
		;do { errno = en; perror(msg);} while (0)

int flag = 1; // Global Variable , changes after 3 sec.
struct PeriodicParameters {
	int minimumIterations;
	int maximumIterations;
	int periodOfFunction;
	int event; // 0 for left click release	// 1 for right click release
	int priority;
};
/**
 * Setting the parameters of the above structure which get initialised on reading the file
 */
void setFunctionParameters(int min, int max, int period,int event,
		PeriodicParameters* tempPeriod) {
	std::time_t result = std::time(NULL);
	write_text_to_log_file("Entering setFunctionParameters method for "
			"thread's period of" + std::to_string(period) + "and iterations of range" + std::to_string(min) + " - "
			+ std::to_string(max) + "or aperiodic event of " + std::to_string(event)
	+ " at a time of " +string (std::asctime(std::localtime(&result))));
	write_text_to_log_file("after logging thing");
	if (period == 0) {
		tempPeriod->event = event;
	}
	write_text_to_log_file("after checking period as zero");
	tempPeriod->minimumIterations = min;
	write_text_to_log_file(std::to_string(tempPeriod->minimumIterations));
	tempPeriod->maximumIterations = max;
	write_text_to_log_file(std::to_string(tempPeriod->maximumIterations));
	tempPeriod->periodOfFunction = period;
	write_text_to_log_file(std::to_string(tempPeriod->periodOfFunction));
	write_text_to_log_file("Exiting setFunctionParameters method for "
			"thread's period of" + std::to_string(period) + "and iterations of range" + std::to_string(min) + " - "
			+ std::to_string(max) + "and aperiodic event of " + std::to_string(event)
	+ " at a time of " +string (std::asctime(std::localtime(&result))));
}
/**
 * Sets the thread attribute parameters
 */
void setThreadAttrParameters(int priority,pthread_attr_t threadAttribute[], int i) {
	std::time_t result = std::time(NULL);
	write_text_to_log_file("Entering setThreadAttrParameters method for thread's priority "
			+ std::to_string(priority) + "at time " + string (std::asctime
					(std::localtime(&result))));
	int ret;
	sched_param thread_1_param;
	thread_1_param.__sched_priority = priority;
	ret = pthread_attr_init(&threadAttribute[i]);
	if (ret != 0) {
		printf("ERROR; return code from pthread_attr_init() is %d\n", ret);
		exit(-1);
	}
	ret = pthread_attr_setschedpolicy(&threadAttribute[i], SCHED_FIFO);
	if (ret != 0) {
		printf("ERROR; return code from pthread_attr_setschedpolicy() is %d\n",
				ret);
		exit(-1);
	}
	ret = pthread_attr_setschedparam(&threadAttribute[i], &thread_1_param);
	if (ret != 0) {
		printf("ERROR; return code from pthread_attr_setschedpolicy() is %d\n",
				ret);
		exit(-1);
	}
	write_text_to_log_file("Exiting setThreadAttrParameters method for thread's priority  "
			"priority of " + std::to_string(priority) + "at time " + string (std::asctime
					(std::localtime(&result))));
}
/**
 * THis method is used for reading the file inputs
 */
PeriodicParameters* readFileInputs(string path) {
	//Initialising the variables
	int terminationTime;
	ifstream inFile;
	inFile.open(path);
	std::string line;
	int count = 0;
	string typeOfEvent;
	int period;
	int min;
	int max;
	int event;
	int priority;
	string str;
	bool firstTime = true;
	PeriodicParameters* parameters = NULL;
	while (inFile.good()) {
		std::time_t result = std::time(NULL);
		write_text_to_log_file("Reading the contents of the file " + string (std::asctime
				(std::localtime(&result))));
		if (!inFile.eof()) {
			if (std::getline(inFile, line)) { //Gets the file line by line
				std::istringstream myStreamThing(line);
				if (firstTime) {
					//THis will be invoked only for the first line
					write_text_to_log_file("Reading the first line of file at time " + string (std::asctime
							(std::localtime(&result))));
					myStreamThing >> numberOfTasks;
					myStreamThing >> terminationTime;
					write_text_to_log_file(std::to_string(numberOfTasks));
					write_text_to_log_file(std::to_string(terminationTime));
					firstTime = false;
					parameters = new PeriodicParameters[numberOfTasks];
				} else {
					//THis portion will be invoked from second line onwards
					write_text_to_log_file("Reading the next line of file at time " + string (std::asctime
							(std::localtime(&result))));
					write_text_to_log_file("Reading event data");
					myStreamThing >> typeOfEvent;
					if (typeOfEvent == "P") {
						write_text_to_log_file("Periodic event");
						myStreamThing >> priority;
						parameters[count].priority = priority;
						myStreamThing >> period;
						parameters[count].periodOfFunction = period;
						myStreamThing >> min;
						parameters[count].minimumIterations = min;
						myStreamThing >> max;
						parameters[count].maximumIterations = max;
						parameters[count].event = 2;
						count++;
						myStreamThing.clear();
						line.clear();
					} else if (typeOfEvent == "A") {
						write_text_to_log_file("Aperiodic Event");
						myStreamThing >> priority;
						parameters[count].priority = priority;
						myStreamThing >> event;
						parameters[count].event = event;
						myStreamThing >> min;
						parameters[count].minimumIterations = min;
						myStreamThing >> max;
						parameters[count].maximumIterations = max;
						parameters[count].periodOfFunction = 0;
						count++;
						myStreamThing.clear();
						line.clear();
					}
				}
			}
		}
	}
	inFile.close();
	return parameters;
}


bool leftButtonRelease(const input_event& eventToBeNOted) {
	return eventToBeNOted.type == 1 && eventToBeNOted.code == 272
			&& eventToBeNOted.value == 0 ;
}

bool periodic(int i, PeriodicParameters* parameters) {
	return parameters[i].periodOfFunction > 0 && parameters[i].event == 2;
}

ssize_t readForMouseButtonReleases(int fd, input_event& eventToBeNOted) {
	return read(fd, &eventToBeNOted, sizeof(struct input_event));
}

bool rightButtonRelease(const input_event& eventToBeNOted) {
	return eventToBeNOted.type == 1 && eventToBeNOted.code == 273
			&& eventToBeNOted.value == 0 ;
}
/**
 * Setting the Environmental Variables of LOG_FILE , MOUSE_FILE and EXECUTION_ENV
 */
void setEnvironmentalVariables() {
	int s;
	//Setting environment Variables
	s = setenv("LOG_FILE", "log_file.txt", 1);
	if (s != 0) {
		handle_error_en(s, "setenv");
	}
	s = setenv("MOUSE_FILE", "/dev/input/event6", 1);
	if (s != 0) {
		handle_error_en(s, "setenv");
	}
	s = setenv("EXECTION_ENV", "HOST", 1);
	if (s != 0) {
		handle_error_en(s, "setenv");
	}
}

int main(void) {
	//Reading file path to read the input file.
	string path;
	cout << "Please enter the file path,give a valid complete path inclusive of file name \n";
	cin >> path;
	//Reading the file and populating the structs value for each thread.
	PeriodicParameters* parameters = readFileInputs(path);
	pthread_t threads[numberOfTasks];
	pthread_attr_t threadAttribute[numberOfTasks];
	//For Time-stamp
	std::time_t result = std::time(NULL);
	write_text_to_log_file("Inside main function"+ string (std::asctime
			(std::localtime(&result))));
	void *ret;
	int s;
	//Setting environment Variables
	setEnvironmentalVariables();
	//Creation of threads - both periodic and aperiodic
	write_text_to_log_file("Initialising all threads at time" + string (std::asctime
			(std::localtime(&result))));
	//Thread creations
	for (int i=0;i < numberOfTasks; i++) {
		setThreadAttrParameters(parameters[i].priority, threadAttribute, i);
		s =pthread_create(&threads[i], &threadAttribute[i], generic_task, &parameters[i]);
		if (s != 0) {
			handle_error_en(s, "pthread_create");
		}
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		s = CPU_SET(0, &cpuset);
		// setting affinity
		s = pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpuset);
		if (s != 0) {
			handle_error_en(s, "pthread_setaffinity_np");
		}
		/* Check the actual affinity mask assigned to the thread */
		s = pthread_getaffinity_np(threads[i], sizeof(cpu_set_t), &cpuset);
		if (s != 0)
			handle_error_en(s, "pthread_getaffinity_np");
		printf("Set returned by pthread_getaffinity_np() contained:\n");
		for (int j = 0; j < CPU_SETSIZE; j++)
			if (CPU_ISSET(j, &cpuset))
				printf("    CPU %d\n", j);

		exit(EXIT_SUCCESS);
	}
	//Signalling on reading a mouse event.
	pthread_mutex_init(&aperiodic_mutex, NULL);
	//Creating the reading thread.
	pthread_t readingThread;
	s = pthread_create(&readingThread, NULL, notifyWhenMouseClickOccurs, ret);
	if (s != 0) {
		handle_error_en(s, "pthread_create");
	}
	// Making the main thread sleep for three seconds.
	sleep(3);
	write_text_to_log_file("flag has been set to zero... time to return");
	flag = 0;
	//Killing the reading thread after the three seconds.
	pthread_kill(readingThread, 21);
	//Once 3 secs are up, finishing up all the threads by calling the join
	for (int i=0; i<numberOfTasks; i++) {
		s = pthread_join(threads[i], &ret);
		write_text_to_log_file("Joining of threads has begun for thread" + i);
		if (s != 0) {
			handle_error_en(s, "pthread_join");
		};
	}
	return 0;
}
/**
 * This method will be invoked by both periodic and aperiodic threads.
 */
void *generic_task(void* functionParameters) {
	PeriodicParameters* fnc ;
	fnc = (PeriodicParameters*)functionParameters;
	if (fnc->periodOfFunction >0 && fnc->event == 2) {
		periodic_task(functionParameters);
	}
	else if (fnc->periodOfFunction == 0 && fnc->event <2) {
		aperiodic_task(functionParameters);
	}
	return 0;
}
/**
 * THis method notifies the aperiodic Function waiting on this reading signal about the left button and right Button mouse releases.
 */
void* notifyWhenMouseClickOccurs(void* dummy) {
	std::time_t result = std::time(NULL);
	write_text_to_log_file("Entering notifyWhenMouseClickOccurs at " + string (std::asctime(std::localtime(&result))));
	int fd;
	input_event mouseEvent;
	if((fd = open(MOUSEFILE, O_RDONLY)) == -1) {
		perror("opening device");
		exit(EXIT_FAILURE);
	}
	pthread_mutex_lock(&aperiodic_mutex);
	while (readForMouseButtonReleases(fd, mouseEvent)) {
		if (leftButtonRelease(mouseEvent) || rightButtonRelease(mouseEvent)) {
			pthread_cond_signal(&button_release);
			pthread_mutex_unlock(&aperiodic_mutex);
			sleep(3);
			write_text_to_log_file("Done reading");
		}
	}
	write_text_to_log_file("Exiting notifyWhenMouseClickOccurs at " + string (std::asctime(std::localtime(&result))));
	return 0;
}
/**
 * Periodic Function's Task body.
 */
void *periodic_task(void* functionParameters) {
	PeriodicParameters* fnc ;
	fnc = (PeriodicParameters*)functionParameters;
	std::time_t result = std::time(NULL);
	write_text_to_log_file("Entering periodic task whose priority is "
			+ std::to_string((fnc->priority)) + "at time" + string (std::asctime(std::localtime(&result))) + "\n");
	struct timespec begin, end;
	double elapsed;
	int i,j = 0;
	while (flag) {
		clock_gettime(CLOCK_MONOTONIC, &begin);
		for (i = 0; i < fnc->maximumIterations; i++)
		{
			j = j + i;
		}
		clock_gettime(CLOCK_MONOTONIC, &end);
		elapsed = end.tv_sec - begin.tv_sec;
		double timeLeftForSLeep = fnc->periodOfFunction - elapsed;
		sleep(timeLeftForSLeep/1000000000);
	}
	write_text_to_log_file("Exiting periodic task whose priority is "
			+ std::to_string(fnc->priority) + string (std::asctime
					(std::localtime(&result))));
	return 0;
}
/**
 * Aperiodic Function's Task Body
 */
void *aperiodic_task(void* functionParameters) {
	std::time_t result = std::time(NULL);
	PeriodicParameters* fnc ;
	fnc = (PeriodicParameters*)functionParameters;
	write_text_to_log_file("Entering aperiodic task whose event is "
			+ std::to_string((fnc->event)) + "at time" + string (std::asctime(std::localtime(&result))) + "\n");

	do {
		std::time_t result = std::time(NULL);
		pthread_mutex_lock(&aperiodic_mutex);
		pthread_cond_wait(&button_release, &aperiodic_mutex);
		write_text_to_log_file("Starting aperiodic Iterations at " + string (std::asctime(std::localtime(&result))) + "\n");
		int i,j = 0;
		for (i = 0; i < fnc->maximumIterations; i++)
		{
			j = j + i;
		}
		write_text_to_log_file("Done with aperiodic iterations at ");
		pthread_mutex_unlock(&aperiodic_mutex);
		write_text_to_log_file("Exiting aperiodic task whose event is button release of "
				+  std::to_string(fnc->event) + string (std::asctime
						(std::localtime(&result))));
	}while(flag);
	return 0;
}
/**
 * THis method sets the thread attributes.
 */
void setThreadAttributes(int priority, pthread_attr_t threadAttribute) {
	std::time_t result = std::time(NULL);
	write_text_to_log_file("Entering setThreadPriority method for thread's priority with "
			"priority of " + std::to_string(priority) + string (std::asctime
					(std::localtime(&result))));
	int ret;
	sched_param thread_1_param;
	ret = pthread_attr_init(&threadAttribute);
	if (ret !=0) {
		printf("ERROR; return code from pthread_create() is %d\n", ret);
		exit(-1);
	}
	ret = pthread_attr_getschedparam(&threadAttribute, &thread_1_param);
	if (ret !=0) {
		printf("ERROR; return code from pthread_create() is %d\n", ret);
		exit(-1);
	}
	thread_1_param.sched_priority = priority;
	ret = pthread_attr_setschedpolicy(&threadAttribute, SCHED_FIFO);
	if (ret !=0) {
		printf("ERROR; return code from pthread_create() is %d\n", ret);
		exit(-1);
	}
	ret = pthread_attr_setschedparam(&threadAttribute, &thread_1_param);
	if (ret !=0) {
		printf("ERROR; return code from pthread_create() is %d\n", ret);
		exit(-1);
	}

	write_text_to_log_file("Exiting setThreadPriority method for thread's priority with "
			"priority of " + std::to_string(priority) + string (std::asctime
					(std::localtime(&result))));
}

