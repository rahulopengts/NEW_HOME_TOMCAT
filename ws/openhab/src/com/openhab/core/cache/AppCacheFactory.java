package com.openhab.core.cache;

import org.apache.jcs.JCS;
import org.apache.jcs.access.exception.CacheException;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;

public class AppCacheFactory {

	private static AppCacheFactory	appCacheInstance	=	null;
	private static Object lock	=	new Object();


	private AppCacheFactory() {
	}

	
	public static AppCacheFactory getAppCacheInstance() throws CloudException {
		
		if(appCacheInstance==null){
			synchronized (lock) {
				appCacheInstance	=	new AppCacheFactory();
				
			}
		}
		return appCacheInstance;
	}
	
	
	public IAppCache getCacheImpl(String cacheType) throws CloudException{
		IAppCache	cache	=	new JCSCacheImpl();
		cache.init();
		return cache;
	}
	
}
