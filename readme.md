## 2018 SNU Operating System -- Project 3: Weighted Round-Robin Scheduler

#### Team1: Gao Zhiyuan, Kim Yongchae, Tormod Aase, Dong Xin

---

### Overview

* Objective of project
* Background
* Implementation
* References

---

### Obejective of project

* Add a weighted round-robin scheduler
* serves for `swapper`, `systemd`, `kthread`


---

### Background

scheduler implementation is based on two functions --- the periodic scheduler and the main scheduler function. These distribute CPU time on the basis of the priorities of the available processes;


#### Scheduling class

Scheduling classes are used to decide which task runs next. 

The kernel supports different scheduling policies (completely fair scheduling, real-time scheduling, and scheduling of the idle task when there is nothing to do), and scheduling classes allow for implementing these policies in a modular way: 
Code from one class does not need to interact with code from other classes. When the scheduler is invoked, it queries the scheduler classes which task is supposed to run next.

After a task has been selected to run, a low-level task switch must be performed. This requires close interaction with the underlying CPU.




#### Time Slice 
* base time slice: `10ms`
* weight range between 1 and 20(inclusively)
* task time slice = weight * base time slice
* default weight: 10

#### Change of weight

* Finish task's quantum before weight change
* Weight and `SCHED_WRR` flag should be inherited when forking

#### CPU run queue

* choose the cpu with smallest total weight
* periodic load balancing:
	* tail of run queue moved to head with lowest total weight
	* check if task is eligible
		* keep weight imbalance 
	* every 2000ms
 
#### Linux Scheduling

`fari_clock`: virtual time of tun queue

`wait_runtime`: waiting time of a process. measure for the unfairness

`fair_clock-wait_runtime`: used to sort tasks

`nice` values: different priority levels

When a task is allowed to run, the interval during which it has been running is subtracted from `wait_runtime`.

**When task is running, `fair_clock` is increasing**

**A good way to understand scheduling decisions is to activate scheduler statistics at compile time. This will generate the file `/proc/sched_debug`, which contains information on all aspects of the current state of the scheduler.**


```=clike
<sched.h>
struct task_struct {
	...
	int prio, static_prio, normal_prio;
	unsigned int rt_priority;
	struct list_head run_list;	// only used in SCHED_RR
	const struct sched_class *sched_class;
	struct sched_entity se;
	unsigned int policy;
	cpumask_t cpus_allowed;
	unsigned int time_slice;	// remaining quantum in RR
	...
}
```

#### Priority

`prio` and `normal_prio` indicate the dynamic priorities
`static_prio` the static priority of a process, it's assigned to the process when it was started

`normal_priority` denotes a priority that is computed based on the static priority and the scheduling policy of the process. Identical static priorities will therefore result in different normal priorities depending on whether a process is a regular or a real-time process. When a process forks, the child process will inherit the normal priority.

`prio` used in case kernel needs to boost priority temperarily

`rt_priority` denote the priority of a real-time process(0~99)


#### Group Scheduling

Scheduler works with schedulable entities. An entity is represented by an instance of `sched_entity`.


#### Policy 

`SCHED_NORMAL`. `SCHED_BATCH`, CPU-intensive and not interactive; `SCHED_IDLE` low importance but relative weight minimal.

`SCHED_RR`, round robin; `SCHED_FIFO` . They are handled by real-time scheduler class.

#### Scheduler Classes

```=clike
//<sched.h>
struct sched_class {
	const struct sched_class *next;
	void (*enqueue_task) (struct rq *rq, struct task_struct *p, int wakeup);
	void (*dequeue_task) (struct rq *rq, struct task_struct *p, int sleep);
	void (*yield_task) (struct rq *rq);
	void (*check_preempt_curr) (struct rq *rq, struct task_struct *p);
	struct task_struct * (*pick_next_task) (struct rq *rq);
	void (*put_prev_task) (struct rq *rq, struct task_struct *p);
	void (*set_curr_task) (struct rq *rq);
	void (*task_tick) (struct rq *rq, struct task_struct *p);
	void (*task_new) (struct rq *rq, struct task_struct *p);
};
```

An instance of struct `sched_class` must be provided for each scheduling class.

* `enqueue_task` adds a new process to the run queue. This happens when a process changes from a sleeping into a runnable state.
* `dequeue_task` provides the inverse operation: It takes a process off a run queue.
	* when a process switches from a runnable into an un-runnable state
	* when the kernel decides to take it off the run queue for other reasons -- for instance, because its priority needs to be changed.
* `sched_yield` triggers `yield_task` to be called in the kernel for a process to give up control of the processor voluntarily
* `check_preempt_curr`
* `put_prev_task` called before the current task is replaced
* `pick_next_task` selects the next task that is supposed to run
* `set_curr_task` is called when the scheduling policy of a task is changed
* `task_tick` is called by the periodic scheduler each time it is activated
* `new_task` notifies the scheduler whenever fork is called

When a process is registered on a run queue, the `on_rq` element of the embedded `sched_entity` instance is set to 1, otherwise to 0.


It is the kernel's job to provide an appropriate mapping between these constants and the available scheduling classes. `SCHED_NORMAL` , `SCHED_BATCH` , and `SCHED_IDLE` are mapped to `fair_sched_class` , while `SCHED_RR` and `SCHED_FIFO` are associated with `rt_sched_class` . Both `fair_sched_class` and `rt_sched_class` are instances of struct `sched_class` that represent, respectively, the completely fair and the realtime scheduler.

#### Run Queues

Each CPU has its own run queue, and each active process appears on just one run queue.

```=clike
//kernel/sched.c
struct rq {
	unsigned long nr_running;
	#define CPU_LOAD_IDX_MAX 5
	unsigned long cpu_load[CPU_LOAD_IDX_MAX];
	...
	struct load_weight load;
	struct cfs_rq cfs;
	struct rt_rq rt;
	struct task_struct *curr, *idle;
	u64 clock;
	...
};
```

* `nr_running`: number of runnable processes on the queue
* `load`: current load on the run queue. The virtual per-run queue clock is based on this information
* `cpu_load` track past load behavior
* `cfs` and `rt` are sub-run queues for CF & RT scheduler
* `idle` points to the task structure of idle process called when no other runnable process is available
* `clock` and `prev_raw_clock` used to implement per-run queue clock. The value of clock is updated each time the periodic scheduler is called. Additionally, the kernel provides the standard function `update_rq_clock` that is called from many places in the scheduler that manipulate the run queue, for instance, when a new task is woken up in `wakeup_new_task`.

```
kernel/sched.c
#define		cpu_rq(cpu)	(&per_cpu(runqueues, (cpu)))
#define		this_rq()	(&__get_cpu_var(runqueues))
#define		task_rq(p)	cpu_rq(task_cpu(p))
#define		cpu_curr(cpu)	(cpu_rq(cpu)->curr)
```
#### Scheduling Entities

```=clike
//<sched.h>
struct sched_entity {
	struct load_weight load; 	/* for load-balancing */
	struct rb_node run_node;
	unsigned int on_rq;

	u64	exec_start;
	u64	sum_exec_runtime;
	u64	vruntime;
	u64	prev_sum_exec_runtime;
	...
}
```

* `load` specifies a weight for each entity that contributes to the total load of the queue.
* `run_node`: a tree element that allows the entity to be sorted on a rb tree
* `on_rq`: whether the entity is scheduled on a run queue or not
* `sum_exec_runtime`: the consumed CPU time
* `update_curr`:At each invocation, the difference between the current time and `exec_start` is computed, and `exec_start` is updated to the current time. The difference interval is added to `sum_exec_runtime`.
* `vruntime`: the amount of time that has elapsed on the virtual clock during process execution
* `prev_exec_runtime` When a process is taken off the CPU, its current `sum_exec_runtime` value is preserved in `prev_exec_runtime` . The data will later be required in the context of process preemption.


#### Priorities

```=clike
// set other prio based on static_prio
static int effective_prio(struct task_struct *p)
{
	p->normal_prio = normal_prio(p);
	/*
	* If we are RT tasks or we were boosted to RT priority,
	* keep the priority unchanged. Otherwise, update priority
	* to the normal priority:
	*/
	if (!rt_prio(p->prio))		
		/* rt_prio requires non-real-time temporarily boosted, RT-Mutexes */
		/* not a rt */
		return p->normal_prio;
	return p->prio;
}
```

`rt_prio(p->prio)` checks if `p->prio` is smaller than `RT_RT_PRIO`

if we are dealing with a non-real-time task, `normal_prio` = `static_prio`

```=clike
static inline int normal_prio(struct task_struct *p)
{
	int prio;
	if (task_has_rt_policy(p))
		prio = MAX_RT_PRIO-1 - p->rt_priority;	// 100 -1 -p->rt_priority
	else
		prio = __normal_prio(p);	// non realtime
	return prio;
}
```

```=clike
static inline int __normal_prio(struct task_struct *p)
{
	return p->static_prio;
}
```

	Now one can certainly wonder why an extra function is used for this purpose. There is a historical reason:
	Computing the normal priority in the old O(1) scheduler was a much trickier business. 
	Interactive tasks had to be detected and their priority boosted, while non-interactive tasks had to be penalized to obtain good interactive behavior of the system. 
	This required numerous heuristic calculations that either did the job well --- or failed at it. The new scheduler, thankfully, does not require such magical calculations anymore

#### Load Weights

The general idea is that every process that changes the priority by one nice level down gets 10 percent more CPU power, while changing one nice level up gives 10 percent CPU power less.

```
static void set_load_weight(struct task_struct *p)
{
	if (task_has_rt_policy(p)) {
		p->se.load.weight = prio_to_weight[0] * 2;
		p->se.load.inv_weight = prio_to_wmult[0] >> 1;
		return;
	}
	/*
	* SCHED_IDLE tasks get minimal weight:
	*/
	if (p->policy == SCHED_IDLE) {
		p->se.load.weight = WEIGHT_IDLEPRIO;
		p->se.load.inv_weight = WMULT_IDLEPRIO;
		return;
	}
	p->se.load.weight = prio_to_weight[p->static_prio - MAX_RT_PRIO];
	p->se.load.inv_weight = prio_to_wmult[p->static_prio - MAX_RT_PRIO];
}
```

run queues are associated with a load weight. **Every time a process is added to a run queue, the kernel calls `inc_nr_running` . This not only ensures that the run queue keeps track of how many processes are running, but also adds the process weight to the weight of the run queue:**

```
static inline void update_load_add(struct load_weight *lw, unsigned long inc)
{
	lw->weight += inc;
}
static inline void inc_load(struct rq *rq, const struct task_struct *p)
{
	update_load_add(&rq->load, p->se.load.weight);
}
static void inc_nr_running(struct task_struct *p, struct rq *rq)
{
	rq->nr_running++;
	inc_load(rq, p);
}
```


---


### Implementation 

My implementation reside in 
	
	kernel/sched/wrr.c

I read samples from

	kernel/sched/rt.c
	kernel/sched/fair.c
	kernel/sched/core.c
	kernel/sched/sched.h
	include/uapi/linux/scheh.h

Scheduling class name: `wrr_sched_class`
Policy name:	`SCHED_WRR`	`6`

##### modify `include/linux/init_task.h`

`SCHED_WRR` should be made the default scheduler class of `swapper`

##### modify `kernel/kthread.c`

change default scheduling policy of `kthreadd`

`If a process' scheduler is set to SCHED_WRR after previously being set to another scheduler, its weight should be the default weight. `
`Tasks using the SCHED_WRR policy should take priority over tasks using the SCHED_NORMAL policy, but not over tasks using the SCHED_RR or SCHED_FIFO policies of real time scheduler`

##### Enable debug in `arch/arm/configs/tizen_tm1_defconfig`

Enable `CONFIG_SCHED_DEBUG` and `CONFIG_SCHEDSTATS`	

then we could modify `sched/debug.c` and `sched/stats.c`

the `debugfs` can be mounted with `mount -t debugfs none /sys/kernel/debug`

##### modify `./fs/debugfs`

Scheduler debugging information can be found in `/proc/sched_debug` and `/proc/schedstat`


##### Manually configuring tasks with `sched_setscheduler()` syscall 156

kernel log from last crash exist in `/proc/last_kmsg`

---

### Work with multicores

Your scheduler must be capable of working on both uniprocessor systems and multiprocessor systems (like the ARTIK device). All cores should be utilized on multiprocessor systems. Proper synchronization and locking is crucial for an SMP scheduler, but not easy. Pay close attention to the kind of locking used in existing kernel schedulers.

### `sched_setweight` and `sched_getweight`

```
/*
 * Set the SCHED_WRR weight of process, as identified by 'pid'.
 * If 'pid' is 0, set the weight for the calling process.
 * System call number 380.
 */
int sched_setweight(pid_t pid, int weight);

/*
 * Obtain the SCHED_WRR weight of a process as identified by 'pid'.
 * If 'pid' is 0, return the weight of the calling process.
 * System call number 381.
 */
int sched_getweight(pid_t pid);

```

`sched_setweight` only root
`sched_getweight` any user

The system calls should be implemented in kernel/sched.c

---

### Investigate


---

### Improve WRR Scheduler


For details please refer to `EXTRA.md`.


---

### Traps and pitfalls

* `cpus_allowed` in a `task_struct`



---

### Test and Verify [source code]()

```=clike
int main(int args, char**argv){
}
```

#### Transmit using a USB

```=bash
echo 0 > /sys/devices/usb.1/12000000.dwc3/id
dmesg | tail
mount /dev/sda1 /mnt   # sda may subject to change
# cp files 
umount /dev/sda1
```

---

### Reference

---


