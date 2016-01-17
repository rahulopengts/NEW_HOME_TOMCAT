package com.openhab.core.cache;

import org.openhab.ui.webapp.cloud.exception.CloudException;

public interface IAppCache {

	public void init() throws CloudException;
	public void putIntoCache(String cacheKey, Object cacheType) throws CloudException;
	
	public Object getFromCache(String cacheKey, Object cacheType) throws CloudException;
}
