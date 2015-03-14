#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/irq.h>
#define IRQT_FALLING IRQ_TYPE_EDGE_FALLING

MODULE_LICENSE ( "Dual BSD/GPL" ) ; 

const int IRQ_NUM = 17;
void *irq_dev_id = (void *)&IRQ_NUM;
struct work_struct workq;

void sample_workqueue(struct work_struct *work)
{
	printk("%s called\n",__func__);
        msleep(3000);
}

irqreturn_t sample_isr(int irq, void *dev_instance)
{
        if (printk_ratelimit()) 
	{
                printk("%s: irq %d (%ld, %ld, %ld)\n", __func__, irq,
                                in_irq(), in_softirq(), in_interrupt());

                schedule_work(&workq);
        }
        return IRQ_NONE;
}


static int sample_isr_init( void ) 
{
	int ret = 0;

        printk("isr driver installed.\n");
 
        INIT_WORK(&workq, sample_workqueue);
        //set_irq_type(IRQ_NUM,IRQT_FALLING);
        ret = request_irq(IRQ_NUM, sample_isr, IRQF_SHARED, "sample_isr", irq_dev_id);
        if (ret) {
                printk("request_irq() failed (%d)\n", ret);
                goto out;
        }
	
out:
        return (ret);

} 

static void sample_isr_exit( void ) 
{
	
	flush_scheduled_work();
        free_irq(IRQ_NUM, irq_dev_id);

        printk("isr driver removed.\n");

} 

module_init ( sample_isr_init ) ; 
module_exit ( sample_isr_exit ) ;
