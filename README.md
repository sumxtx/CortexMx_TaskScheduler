**WIP**
## Implementing a scheduler ###

- Schedule multiple tasks in a Round robin fashion by carrying out the context switch operation

- Round robin scheduling method time slices are assigned to each task in equal portions and in circular order

- First will use systick handler to carry out the context switch operation between multiple tasks

- Later will we change the code using pendSV handler


#### What is a task ####

A task is a piece of code, a C function, which does a specific job when it is allowed
to run on the CPU

A task has it's own stack to create its local variables when it runs on the CPU.
+ | T1 -> Stack-T1 | T2 -> Stack-T2 | T3 -> Stack-T3 | T4 -> Stack-T4 +

Also when scheduler decides to remove a task from CPU, scheduler first saves the context
(state) of the task in task's private stack

In summary, a running code is called task when it is schedulable and never lose its state
unless it is deleted permanently

#### What is scheduling ####
- Scheduling is an algorithm which takes the decision of pre-empting a running task
from the CPU and takes the decision about which task should run on the CPU next

- The decision could be based on many factors such as system load, the priority
of tasks, shared resource access, or a simple round robin method

#### What is Context switch ####
- Context switching is the procedure of switching out of the currently running task
from the CPU after saving the task's exception context or state and switching in the
next task's to run on the CPU by retrieving the past execution context or state
of the same 

### What is the state of a task ####
What exactly is execution context or state of a task? Understanding of this is very important.

Inside a microcontroller we have a processor or CPU, in this case the CPU is ARM Cortex M4 or M3. 
Inside the processor we have processor core, a collection of various units.

ALU( arithmetic logic unit), General purpose registers, Procesor Core registsers, Status Registers...

To store instruction operands, store intermediate results, execution like it has zero flag, carry flag, and negative flag etc..
That gives a snapshot of the last instruction execution.

Also a processor core has some special registers such as program counter, stack pointer registers, and various
and other things. That is part of the processor core.

For example, let's say you have an instruction to add two numbers. Two numbers will be fetched from the
memory and it will be stored inside the processor core registers. Then, ALU adds them and the result
is stored back into the general purpose registers. After that, the result is return back to the memory.

That's why, a processor core has all these units.

Apart from this a processor also has its own peripherals such as NVIC, 
which controls a interrupts and exceptions coming from various parts of the microcontroller. 
Usually interrupts come from various parts of the microcontroller peripheral such as

USB,Ethernet, SPI, I2C, Timer, etc.. 

And processor also has it's internal exceptions, everything
is controlled by the peripheral NVIC. That is a peripheral of the processor.

And apart from that a processor has memory protection unit in order to guard the memory locations.
It has system control block, it may contain a floating point unit, debug unit, etc.. 
These are also peripherals of the processor. 
When the task is running on the processor it will be using the instructions of the task handler,
will be using various general purpose registers, it could be using a status register and it could be using some special register.

That's why, the collection of general purpose registers, some special registers, and a status register is
collectively called as "State of a Task".

When the scheduler wants to switch out a task, then it should save its intermediate results of the operation
which are already stored in general purpose registers and it should also store the status register
value and some special register values into the private stack of the task, isn't it? 

After that only scheduler can switch out a task.

Let's revisit ARM Cortex Mx processors core registers. We have got R0 to R12, 13 general purpose registers,
this is part of the task's state, after that PC is very important.
Why?
Because, when the Scheduler was preempting or switching out a task, the PC will be holding the
address of the next instruction of that task handler. When the scheduler wants to switch in that task some
time later it should know where it should return. So, that information that address is stored in the PC.
PC has also to be sotred. Scheduler also preserve the value of PC. That's why,
PC is also part of state of a task. And the link register.

One more important thing is the current value or the current state of task private stack,
that is stored in PSP. Because, we are using PSP as a stack pointer for user task.

We have to preserve the value of current state or current consumption state of task private stack

That's why, we also should preserve PSP. After that PSR.
That is also important, because that gives the snapshot of the current state like whether the zero flag
has set or not or whether the negative flag has set or not, 
all those a state is stored in the program status register.

That's why, we have to preserve program status register also. 
After that, all these registers are not important.

Why?
Those are privileged registers.
They are used for some special purpose like to turn off the interrupt globally and for other things.
And most of the time user task maybe running with unprivileged access level. 
Saving of these registers doesn't make any sense.
The kernel or the scheduler is the one who is going to alter these a registers. 
User task should never touch these a registers.



In summary, 
State of a task is the values of general purpose registers from R0 to R12, the stack pointer register
in this case PSP, link register and program counter, and the program status registers. 

- [ ] Create user tasks

4 user tasks, basically never returning functions


- [ ] Stack Pointer Selection

T[1-4] are running in Thread Mode
PSP will be used to track the initialization pointer of these tasks

Handler Mode MSP
The systick/pendSV Handler will be used as the scheduler

Stack Assessment

RAM_START _____________________________________private stacks___________RAM_END
    |                                           |Scheduler  |T4 |T3 |T2 |T1 |
    |                                           |           |   |   |   |   |
    |                                           |           |   |   |   |   |
    |                                           |           |   |   |   |   |
<-------------------------------128kb-Ram------------------------------------->

- [ ] Task Scheduling Policy

Round-robin pre-emptive scheduling
No task priority
We will use systick timer to generate exception for every 1ms to run the scheduler code

- [ ] Case study of context switching

T1 switching out <-
T2 switching in ->

|    Running T1 
v
    Save the context of T1  (PUSH)      |
|    to t1's private stack              |   -> context saving
v                                       |
|    Save the PSP value of T1           |
v

|    Get current PSP value of T2        | 
v                                       |  -> context retrieving
|    Retrieve the context of T2 from    | 
v    T2's Private stack       (POP)     |

|
v    Run T2


