#include "stm32f10x.h"
#include "bat_test.h"

int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  Bat_Test();
  
	while(1)
  {
    
	}
}
