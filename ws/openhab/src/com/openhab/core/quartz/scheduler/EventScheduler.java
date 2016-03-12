package com.openhab.core.quartz.scheduler;

import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;
import org.quartz.CronScheduleBuilder;
import org.quartz.JobBuilder;
import org.quartz.JobDetail;
import org.quartz.Scheduler;
import org.quartz.SimpleScheduleBuilder;
import org.quartz.Trigger;
import org.quartz.TriggerBuilder;
import org.quartz.impl.StdSchedulerFactory;

import com.openhab.core.quartz.eventhandler.QuartzEventHandler;
import com.openhab.core.util.AppPropertyReader;



public class EventScheduler {

	public static EventScheduler	eventScheduler	=	null;
	public static boolean isInitialized	=	false;
	
	private EventScheduler() throws CloudException{
		//initializeScheduler();
		initilizeSimpleTrigger();
	}
	
	public static EventScheduler getEventScheduler() throws CloudException{
		if(eventScheduler==null){
			eventScheduler=	new EventScheduler();
			
			isInitialized	=	true;
		}
		return eventScheduler;
	}
	
	
	private void initializeScheduler1() throws CloudException{
    	try{
			JobDetail job = JobBuilder.newJob(QuartzEventHandler.class)
			.withIdentity("dummyJobName", "group1").build();
	
//	    	JobDataMap jobDataMap	=  job.getJobDataMap();
//	    	
//	    	QuartzEventDTO	data	=	new QuartzEventDTO();
//	    	jobDataMap.put("DATA", data);
	    	AppPropertyReader.getAppPropertyReader();
	    	String cronJob	=	AppPropertyReader.getProperty("cronjob");
	    	
	    	Trigger trigger = TriggerBuilder
			.newTrigger()
			.withIdentity("dummyTriggerName", "group1")
			.withSchedule(
				CronScheduleBuilder.cronSchedule(cronJob))//"0/5 * * * * ?"))
			.build();
	    	
	    	//schedule it
	    	Scheduler scheduler = new StdSchedulerFactory().getScheduler();
	    	scheduler.start();
	    	scheduler.scheduleJob(job, trigger);
    	} catch (Exception e){
    		e.printStackTrace();
    		CloudExceptionManager.throwException(CloudMessageConstants.SCHEDULER_INIT_ERROR, e, null);
    	}
	}

	public void initilizeSimpleTrigger(){
		try{
			System.out.println("\n EventScheduer->Initialize Simple Trigger");
		JobDetail job = JobBuilder.newJob(QuartzEventHandler.class).withIdentity("dummyJobName", "group1").build();
		Trigger trigger = TriggerBuilder
				.newTrigger()
				.withIdentity("dummyTriggerName", "group1")
				.withSchedule(
				    SimpleScheduleBuilder.simpleSchedule()
					.withIntervalInSeconds(10).repeatForever())
				.build();
    	
		Scheduler scheduler = new StdSchedulerFactory().getScheduler();
    	scheduler.start();
    	scheduler.scheduleJob(job, trigger);
    	System.out.println("\n EventScheduer->Initialize Simple Trigger-Done");
		} catch (Exception e){
			e.printStackTrace();
		}

	}
	
	/**
	 * Generate a CRON expression is a string comprising 6 or 7 fields separated by white space.
	 *
	 * @param seconds    mandatory = yes. allowed values = {@code  0-59    * / , -}
	 * @param minutes    mandatory = yes. allowed values = {@code  0-59    * / , -}
	 * @param hours      mandatory = yes. allowed values = {@code 0-23   * / , -}
	 * @param dayOfMonth mandatory = yes. allowed values = {@code 1-31  * / , - ? L W}
	 * @param month      mandatory = yes. allowed values = {@code 1-12 or JAN-DEC    * / , -}
	 * @param dayOfWeek  mandatory = yes. allowed values = {@code 0-6 or SUN-SAT * / , - ? L #}
	 * @param year       mandatory = no. allowed values = {@code 1970–2099    * / , -}
	 * @return a CRON Formatted String.
	 */
	private static String generateCronExpression(final String seconds, final String minutes, final String hours,
	                                             final String dayOfMonth,
	                                             final String month, final String dayOfWeek, final String year)
	{
	  return String.format("%1$s %2$s %3$s %4$s %5$s %6$s %7$s", seconds, minutes, hours, dayOfMonth, month, dayOfWeek, year);
	}
	
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try{
			EventScheduler.getEventScheduler();
			System.out.println("\n Outcome->");
		} catch (CloudException e){
			e.printStackTrace();
		}
	}

	
}
