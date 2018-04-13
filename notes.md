## Project 2 


Test
---

* Selector: 

	* while(1)
		* read argument
		* take write lock
		* write argument integer to file "integer"
		* release lock
	
* trial
	* while(1)
		* argument is an identifier, as we would like multiple trials running
		* acquire a read lock when device in a certain rotation([0, 180])
		* open integer, calculate prime number(*Trial Division*), write to std


Read/writes Lock
---


If a reader holds a lock and a writer wants to take the lock, no
more readers can take the lock

manipulating wait list, acquired list, take care of 4 cores
we should also use the lock

exit, release the lock

block processes, 
	* set_current_state
	* prepare_to_wait() and finish_wait()
	* wait event 
	* 


 screen -S artik10 /dev/ttyUSB0 115200 cs8 ixoff


direct_set_debug.sh --sdb-set

sdb -d root on
sdb shell
