package com.openhab.core.quartz.eventhandler;

import org.openhab.core.drools.dto.CommonDroolsDTO;
import org.openhab.core.drools.dto.IDroolsDTO;
import org.openhab.core.drools.internal.DroolsService;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

import com.openhab.core.cache.AppCacheFactory;
import com.openhab.core.cache.IAppCache;
import com.openhab.core.constants.CloudAppConstants;
import com.openhab.core.dto.CloudMasterData;
import com.openhab.core.util.AppPropertyReader;

public class QuartzEventHandler extends AbstractEventHandler {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

	@Override
	public void execute(JobExecutionContext arg0) throws JobExecutionException {
		// TODO Auto-generated method stub
		try{
			
			String siteNames	=	AppPropertyReader.getProperty("schedule");
			//****************************************//
			String homeId	=	"demo";
			System.out.println("\n QuartzEventHandler->Firing->"+siteNames);
			
			
			IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(homeId);
			CloudMasterData	masterData	=	(CloudMasterData)cache.getFromCache(homeId, null);
			
			CommonDroolsDTO	commondDroolsDTO	=	new	CommonDroolsDTO();
			commondDroolsDTO.setIS_TIMETYPE(homeId.toUpperCase()+"."+CloudAppConstants.YES);
			
			DroolsService	droolsService	=	masterData.getDroolsService();
			droolsService.fireScheduledRule(commondDroolsDTO);
			commondDroolsDTO.setIS_TIMETYPE("");
			//****************************************//
			System.out.println("\n QuartzEventHandler->"+siteNames);
		} catch (CloudException e){
			e.printStackTrace();
		} catch (Exception e){
			e.printStackTrace();
		}
		
	}

}
