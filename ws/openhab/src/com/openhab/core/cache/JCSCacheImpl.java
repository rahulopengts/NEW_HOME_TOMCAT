package com.openhab.core.cache;

import org.apache.commons.jcs.JCS;
import org.apache.commons.jcs.access.CacheAccess;
import org.apache.commons.jcs.access.exception.CacheException;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;

public class JCSCacheImpl extends AbstractAppCache {

	private CacheAccess<String, Object> cache	=	null;
	
	@Override
	public void putIntoCache(String cacheKey, Object cacheType) throws CloudException {
		// TODO Auto-generated method stub
		try{
			cache.put(cacheKey, cacheType);	
		} catch (CacheException e){
			CloudExceptionManager.throwException(CloudMessageConstants.CACHE_EXCEPTION, e, "");
		}
		
	}

	@Override
	public Object getFromCache(String cacheKey, Object cacheType)  throws CloudException {
		// TODO Auto-generated method stub
		return cache.get(cacheKey);
	}

	
	public void init() throws CloudException {
		try{
			cache = JCS.getInstance( "default" );
//			final String configFilename = "D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\openhab\\cache.ccf";
//
//		    if (configFilename != null)
//		    {
//		        try (FileReader f = new FileReader(configFilename))
//		        {
//		            Properties props = new Properties();
//		            props.load(f);
//		            CompositeCacheManager ccm = CompositeCacheManager.getUnconfiguredInstance();
//		            ccm.configure(props);
//		            cache = JCS.getInstance( "default" );
//		        } catch (FileNotFoundException e){
//		        	e.printStackTrace();
//		        } catch (IOException e){
//		        	e.printStackTrace();
//		        }
//		    }
		} catch (Exception	e){
			e.printStackTrace();
			CloudExceptionManager.throwException(CloudMessageConstants.SESSION_NULL, null,"");
		}
		
	}
}
