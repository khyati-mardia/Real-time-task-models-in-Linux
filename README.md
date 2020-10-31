# Real-time-task-models-in-Linux

Objectives:
1. To program real-time tasks on Linux environment, including periodic and aperiodic tasks, event handling, priority setting, etc.
2. To set up software development environment for target Galileo board and to build and run user application program on the target.
3. To use Linux trace tools to view and analyze real-time scheduling.

The task of multithreading is implemented on Linux environment. Threads for periodic and aperiodic tasks are created using pthread library. 

Concepts on semaphores, joins and global variables are used.
The program initially prompts the user for a file .File which is given by user , indicates the number of threads that need to be instantiated . The thread parameters are set in the form of customized struct data type wherein we take the priority, period, min iterations, max iterations, event type . The file inputs are read and the thread parameters are hereby initialized . We have used file handling functions to read the file. We read the file line by line , and parse it by using the string buffer and initialize our parameters.
After reading the file input, which sets the thread parameters . Now , the threads are created using the pthread_create command which hereby initialized all the threads . The periodic threads now begin execution by running the iterations defined in the task body. The aperiodic threads , wait for the mouse thread events to occur to run the task body iterations.
We have created another thread which continuously reads the mouse events from the “/dev/input/event6 “ file where the mouse events get logged with the values of type,code and value. We have used Semaphores for this. We have initialized the condition variable and mutex. Whenever the type,code,value == 1,272,0 (Left button release) and 1,273,0 (right button release) occur , we signal the aperiodic task body to run. We have implemented Semaphores to do this. p_cond_Wait is present in aperioidic task body. Pcond_signal is present in the notifyMethod . This means upon successful reading of mouse event , p_cond_Signal signals the aperiodic task to run.
We have created the threads with the assigned priorities. We have also set the scheduling parameter as FIFO and set the CPU affinity to one core.Therefore the Kernel ensures the thread with the highest priority gets the cpu to run the task body. The FIFO scheduling determines the scheduling of threads in the waiting list whenever two threads have same priorities.
We allow main thread to sleep for 3 secs which allow running of the created threads.After the main thread wakes up, we kill the reading thread with pkill signal. We used the global variable to signal the task bodies to stop execution. We have used p_join for our periodic and aperiodic tasks for successful completion of their tasks and exit.
We have set the CPU Affinity which helps in bounding the threads to a single core. The Kernelshark plots support the above understanding of our project enclosed below .
