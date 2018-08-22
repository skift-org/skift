#summary Adding Multitasking capability to your OS
#labels Phase-Implementation,Featured

# Introduction

*_Suggested Reading:_* [http://osdever.net/bkerndev/Docs/intro.htm Bran's Kernel Development Tutorial]

*_Examples_* [http://code.google.com/p/onyxkernel/downloads/list Download the latest Stable Release of Kryos]. You can find the multitasking code in \core\task\

*_This page is currently a work in progress_*
So, you have made your own Operating System, but you've finally come to a stopping point. After hours of reading and research and banging your head on your keyboard, you have decided to give it up. Soon, you will be able to switch tasks easily.

First off, you need to understand that you cannot really run more than one _thread/task_ at a time. Your computer's OS is just switching between the different processes for you.

If you have read Bran's Kernel Tutorial, or have written a kernel based off of it, then you know a little bit about the Global Descriptor Tables (GDT) and more importantly, the Interrupt Descriptor Table (IDT). The ISR (Interrupt Service Routine)/IDT handler that Bran so kindly provided us with is a small example of what software multi-tasking can do. When an interrupt occurs, we save the current registers and processor state (we're going to call this the _context_) to the stack (ESP). We then `jmp` to our handler and execute the required Interrupt Stub (Timer, floppy, ATA disk, etc), and then we return to our ISR/IDT code (where we saved the context) and restore the context back to the processor. Basically, this allows us to sort of trick the processor into thinking that we never even switched to our handler code. We are going to do the exact same thing now, just with a little more power

## Step 1: Writing a Task Structure

Before we go running off and start switching contexts, we need to make a structure to save our task's state and data in. Here is a basic structure that you can expand to your needs.
```c
typedef struct task
{
	uint32 pid;       // Process ID
	uint32 stack;	  // Task's stack
	uint32 state : 1; // Is task alive?
	uint8 name[32];   // Task's name

	void (*thread)(); // Actual thread entry
}task_t;

task_t tasks[2];          // Our tasklist
int pid = -1;             // Current Process ID
```
The main field that we need is the `task_t.stack` field. This is where we will save the context state of the processor and task.

## Step 2: Writing a `CreateTask` Routine =

Now that we have a task structure, we need a way to fill out the structure. This is pretty basic, if you understand that the Intel 80x86 family of processors has a stack that _expands downward_.

```c
void CreateTask(int id, unsigned char name[32], void (*thread)())
{
	asm volatile("cli");
	unsigned int *stack;
	task_t *task = (task_t*) kmalloc(sizeof(task_t));
	task->stack = kmalloc_a(0x1000) + 0x1000;	// Allocate 4 kilobytes of space

	stack = (unsigned int*)task->stack;

        // Expand down stack
	// processor data
	*--stack = 0x202;	// EFLAGS
	*--stack = 0x08;	// CS
	*--stack = (unsigned int) thread;	// EIP

	// pusha
	*--stack = 0;		// EDI
	*--stack = 0;		// ESI
	*--stack = 0;		// EBP
	*--stack = 0;		// NULL
	*--stack = 0;		// EBX
	*--stack = 0;		// EDX
	*--stack = 0;		// ECX
	*--stack = 0;		// EAX

	// data segments
	*--stack = 0x10;	// DS
	*--stack = 0x10;	// ES
	*--stack = 0x10;	// FS
	*--stack = 0x10;	// GS

	task->state = 1;
	task->stack = (unsigned int) stack;
	task->thread = thread;
	strncpy(task->name, name, strlen(name));

	tasks[id] = *task;

        pid = id; // enable task switching

	asm volatile("sti");

}
```

There. Now we can create new tasks easily.

## Step 3: Writing a Task Scheduler =

I am going to provide you with just a basic round-robin scheduler, one that can switch between just 2 tasks. If you want more tasks, just take out the switch statement and increment `pid` until it goes above the number of created tasks, and loop back down to zero.

```c
unsigned int sched(unsigned int context)  // Our assembly code provides us with the context
{
         tasks[pid].stack = context;      // save the old context into current task
         switch(pid)
         {
         case 0:
              pid = 1;
              break;
         case 1:
              pid = 0;
              break;
         }
         return tasks[pid].stack;         // Return new task's context.
}
```
Now we have to write a small assembly function to do the actual setup.

## Step 4: Writing a `task switch` Function =

Now, we have to write the actual switching function. This doesn't distinguish between tasks, it just retrieves the next task's

```
extern sched			; core/sched.c

global irq0
irq0:
		pusha		; push general registers onto stack
		push ds		; push ds onto stack
		push es		; push es
		push fs		; push fs
		push gs		; push gs

		mov eax, 0x10	; save selector
		mov ds, eax		
		mov es, eax
		mov fs, eax
		mov gs, eax		

		mov eax, esp	; stack => eax
		push eax	; push eax (esp)

		call sched	; sched(eax) returns stack of next task
		mov esp, eax	; return value => stack

		mov al, 0x20	; ack IRQ
		out 0x20, al

		pop gs		; pop gs off task stack
		pop fs		; pop fs
		pop es		; pop es
		pop ds		; pop ds
		popa		; pop general registers

		iret		; interrupt return
```

Just install irq0 into your interrupt request table, and each time that the system timer interrupt (Interrupt Request 0, Int 32) interrupts, you will switch tasks!

## Task Setup

I have prepared a simple task for you. Just remember whenever you create a task to make sure that it loops. Just call `CreateTask(0, "Idle Task", IdleTask);`
```c
void IdleTask(void)
{
	printf("Idle Task\n");
	for(;;);				// Critical, will page fault if no loop
}
```
And that's it!

Enjoy! Let me know what you think of this tutorial

*_Examples_* [http://code.google.com/p/onyxkernel/downloads/list Download the latest Stable Release of Kryos]. You can find the multitasking code in \core\task\

<wiki:gadget url="http://onyxkernel.googlecode.com/svn/wiki/adsense.xml" height="60" width="468"/>
