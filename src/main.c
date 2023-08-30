# if !defined(__SOFT_FP__)... blablab

#include <stdio.h>

void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);
void init_systick_timer(uint32_t tick_hz);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void init_tasks_stack(void);
void enable_processor_faults(void);
__attribute__((naked)) void switch_sp_to_psp(void);
uint32_t get_psp_value(void);

uint32_t psp_of_tasks[MAX_TASKS] = {T1_STACK_START,T2_STACK_START,T3_STACK_START,T4_STACK_START};
uint32_t task_handlers[MAX_TASKS];
uint8_t current_task = 0; // task1 is running

int main(void)
{

  enable_processor_faults();

  init_scheduler_stack(SCHED_STACK_START); //MSP

  //catch addresses of different task handlers
  task_handlers[0] = (uint32_t)task1_handler;
  task_handlers[1] = (uint32_t)task2_handler;
  task_handlers[2] = (uint32_t)task3_handler;
  task_handlers[3] = (uint32_t)task4_handler;

  // task stack initialization to store the summy frames
  init_tasks_stack();

  // geenerate systick time exception
  init_systick_timer(TICK_HZ); // -> up to here the code is using MSP stack pointer

  switch_sp_to_psp();

  // calling a task
  task1_handler();
  for(;;;)
}
void task1_handler(void)
{
  while(1)
  {
    printf("This is task1\n");
  }
}
void task2_handler(void)
{
  while(1)
  {
    printf("This is task1\n");
  }
}
void task3_handler(void)
{
  while(1)
  {
    printf("This is task1\n");
  }
}
void task4_handler(void)
{
  while(1)
  {
    printf("This is task1\n");
  }
}

void init_systick_timer(uint32_t tick_hz)
{
  uint32_t *pSRVR = (uint32_t*)0xE000E014;
  uint32_t *pSCSR = (uint32_t*)0xE000E010;

  uint32_t count_value = (SYSTICK_TIMER_CLK/tick_hz)-;

  //Clear the value in to SVR
  *pSRVR &= ~(0x00FFFFFFFF);

  //Load the value in to SVR
  *pSRVR |= count_value;

  //Do some settings
  *pSCSR |= (1 << 1); //Enables SysTick Exception Request
  *pSCSR |= (1 << 2); //Indicates the clock source, processor clock source

  //Enable The Systick
  *pSCSR |= (1 << 0); //Enables the counter

}

__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack)
{
  __asm volatile("MSR MSP,%0": : "r" (sched_top_of_stack) : );
  __asm volatile("BX LR");
}

void init_tasks_stack(void)
{
  uint32_t *pPSP;

  for(int i = 0; i < MAX_TASKS; i++)
  {
    pPSP = (uint32_t*) psp_of_tasks[i];
    pPSP--;
    *pPSP = DUMMY_XPSR;//0x01000000

    pPSP--;//PC
    *pPSP = task_handlers[i];

    pPSP--;//LR
    *pPSP = 0xFFFFFFFD;

    for(int j=0; j < 13; j++)
    {
      pPSP--;
      *pPSP = 0;
    }

    psp_of_tasks[i] = (uint32_t)pPSP;

  }
}

void enable_processor_faults(void)
{
  uint32_t * pSHCSR = (uint32_t*) 0xE000ED24;

  *pSHCSR |= ( 1 << 16 ); //mem manage
  *pSHCSR |= ( 1 << 17 ); //bus fault
  *pSHCSR |= ( 1 << 18 ); //usage fault

}

uint32_t get_psp_value(void)
{
  return psp_of_tasks[current_task];
}

__attribute__((naked)) void switch_sp_to_psp(void)
{
  //1. initialize the PSP with TASK1 stack start addresses

  //get the value of psp of current_task , BL branch with link 
  __asm volatile ("PUSH {LR}");//preserve LR -> connect back to main
  __asm volatile ("BL get_psp_value");
  __asm volatile ("MSR PSP,R0");//initialize psp
  __asm volatile ("POP {LR}"); //pops back LR value
  
  //2.change SP to PSP using CONTROL register
  __asm volatile ("MOV R0,#0x02");
  __asm volatile ("MSR CONTROL,R0");
  __asm volatile ("BX LR");

}

void SysTick_Handler(void)
{
  /* Save the content od curret task */

  // Get the current running task's PSP value 
  // Using that PSP value store SF2(R4 to R11) 
  // Save the current value of PSP 

  /* Retrieve the context of next task */
  // Decide next task to run 
  // get its past PSP value 
  // Using that PSP value retrieve SF2(R4 to R11) 
  // Update PSP and exit
}

void HardFault_Handler(void)
{
  printf("Exception : Hardfault\n");
  while(1);
}
void MemManage_Handler(void)
{
  printf("Exception : MemManage\n");
  while(1);
}
void BusFault_Handler(void)
{
  printf("Exception : BusFault\n");
  while(1);
}
