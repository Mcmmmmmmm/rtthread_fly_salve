#include "bsp_led.h"

#define rta_output_n(...) rt_kprintf(__VA_ARGS__); 

static struct rt_timer      timer_test;
static rt_tick_t timeout_value = 10;
uint8_t led_state;

void led_on(void)
{
    rt_pin_mode(7,PIN_MODE_OUTPUT);
    rt_pin_write(7,PIN_LOW);
}

void led_run(void* p)
{
    rt_timer_start(&timer_test);
}

void led_timer_callback(void* p)
{
      timeout_value +=10;
    
//    rt_timer_control(timer_test, RT_TIMER_CTRL_SET_TIME, (void *)&timeout_value);
    
    if(timeout_value == 10)
    {
        rt_pin_mode (7,PIN_MODE_OUTPUT);
        rt_pin_write(7,PIN_LOW);
        led_state = 1;
//        rt_timer_stop(timer_test);
         rta_output_n("[led_state   ON ]\n");
    }
    if(timeout_value >= 110)
    {
        rt_pin_mode (7,PIN_MODE_OUTPUT);
        rt_pin_write(7,PIN_HIGH);
//        rt_timer_start(timer_test);
        led_state = 0;
        timeout_value = 0;
        rta_output_n("[led_state   OFF]\n");
     //   rt_timer_control(timer_test, RT_TIMER_CTRL_SET_TIME, (void *)&timeout_value);
    }
}

int rt_led_init(void)
{
    rt_thread_t thread = RT_NULL;
    
    rt_timer_init(&timer_test,
                 "timer_test",
           led_timer_callback,
                      RT_NULL,
           RT_TICK_PER_SECOND,
    RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    thread = rt_thread_create("led", 
							led_run, 
							RT_NULL, 
							   2048, 
								  2, 
								50);
    
    if(thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    
    return RT_EOK;

}


INIT_APP_EXPORT(rt_led_init);



