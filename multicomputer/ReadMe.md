## Introduction of Multiprocessor and Multicomputer

_Multiprocessor_

I. A Multiprocessor is a computer system with two or more central processing units (CPUs) share full access to a common RAM. The main objective of using a multiprocessor is to boost the system’s execution speed, with other objectives being fault tolerance and application matching. There are two types of multiprocessors, one is called shared memory multiprocessor and another is distributed memory multiprocessor. In shared memory multiprocessors, all the CPUs shares the common memory but in a distributed memory multiprocessor, every CPU has its own private memory.

![image](/multicomputer/images/design.png)

_Applications of Multiprocessor_

* As a uniprocessor, such as single instruction, single data stream (SISD).
* As a multiprocessor, such as single instruction, multiple data stream (SIMD), which is usually used for vector processing.
* Multiple series of instructions in a single perspective, such as multiple instruction, single data stream (MISD), which is used for describing hyper-threading or pipelined processors.
* Inside a single system for executing multiple, individual series of instructions in multiple perspectives, such as multiple instruction, multiple data stream (MIMD).

_Benefits of using a Multiprocessor_

* Enhanced performance.
* Multiple applications.
* Multi-tasking inside an application.
* High throughput and responsiveness.
* Hardware sharing among CPUs.

_Advantages_

* Improved performance: Multiprocessor systems can execute tasks faster than single-processor systems, as the workload can be distributed across multiple processors.
* Better scalability: Multiprocessor systems can be scaled more easily than single-processor systems, as additional processors can be added to the system to handle increased workloads.
* Increased reliability: Multiprocessor systems can continue to operate even if one processor fails, as the remaining processors can continue to execute tasks.
* Reduced cost: Multiprocessor systems can be more cost-effective than building multiple single-processor systems to handle the same workload.
* Enhanced parallelism: Multiprocessor systems allow for greater parallelism, as different processors can execute different tasks simultaneously.

_Disadvantages_

* Increased complexity: Multiprocessor systems are more complex than single-processor systems, and they require additional hardware, software, and management resources.
* Higher power consumption: Multiprocessor systems require more power to operate than single-processor systems, which can increase the cost of operating and maintaining the system.
* Difficult programming: Developing software that can effectively utilize multiple processors can be challenging, and it requires specialized programming skills.
* Synchronization issues: Multiprocessor systems require synchronization between processors to ensure that tasks are executed correctly and efficiently, which can add complexity and overhead to the system.
* Limited performance gains: Not all applications can benefit from multiprocessor systems, and some applications may only see limited performance gains when running on a multiprocessor system.

![image](/multicomputer/images/interconnect.png)

II. A multicomputer system is a computer system with multiple processors that are connected together to solve a problem. Each processor has its own memory and it is accessible by that particular processor and those processors can communicate with each other via an interconnection network.As the multicomputer is capable of messages passing between the processors, it is possible to divide the task between the processors to complete the task. Hence, a multicomputer can be used for distributed computing. It is cost effective and easier to build a multicomputer than a multiprocessor. Difference between multiprocessor and Multicomputer:

* Multiprocessor is a system with two or more central processing units (CPUs) that is capable of performing multiple tasks where as a multicomputer is a system with multiple processors that are attached via an interconnection network to perform a computation task.
* A multiprocessor system is a single computer that operates with multiple CPUs where as a multicomputer system is a cluster of computers that operate as a singular computer.
* Construction of multicomputer is easier and cost effective than a multiprocessor.
* In multiprocessor system, program tends to be easier where as in multicomputer system, program tends to be more difficult.
* Multiprocessor supports parallel computing, Multicomputer supports distributed computing.

_Advantages_

* Improved performance: Multicomputer systems can execute tasks faster than single-computer systems, as the workload can be distributed across multiple computers.
* Better scalability: Multicomputer systems can be scaled more easily than single-computer systems, as additional computers can be added to the system to handle increased workloads.
* Increased reliability: Multicomputer systems can continue to operate even if one computer fails, as the remaining computers can continue to execute tasks.
* Reduced cost: Multicomputer systems can be more cost-effective than building a single large computer system to handle the same workload.
* Enhanced parallelism: Multicomputer systems allow for greater parallelism, as different computers can execute different tasks simultaneously.

_Disadvantages_

* Increased complexity: Multicomputer systems are more complex than single-computer systems, and they require additional hardware, software, and management resources.
* Higher power consumption: Multicomputer systems require more power to operate than single-computer systems, which can increase the cost of operating and maintaining the system.
* Difficult programming: Developing software that can effectively utilize multiple computers can be challenging, and it requires specialized programming skills.
* Synchronization issues: Multicomputer systems require synchronization between computers to ensure that tasks are executed correctly and efficiently, which can add complexity and overhead to the system.
* Network latency: Multicomputer systems rely on a network to communicate between computers, and network latency can impact system performance

### Documents references

* Expired Patent [literature](https://patents.google.com/?inventor=Lawrence+S.+Mok)
* Multicomputing [DSP](https://www.semanticscholar.org/paper/A-multicomputer-type-DSP-system-for-super-signal-Ono-Kanayama/ee09a9bf444c2bd962d704a834e61a0ef8cb44b4)