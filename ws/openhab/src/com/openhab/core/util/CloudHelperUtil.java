package com.openhab.core.util;

import org.openhab.core.items.ItemRegistry;
import org.openhab.model.core.ModelRepository;
import org.openhab.model.sitemap.SitemapProvider;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;
import org.openhab.ui.webapp.internal.render.PageRenderer;

import com.openhab.core.cache.AppCacheFactory;
import com.openhab.core.cache.IAppCache;
import com.openhab.core.dto.CloudMasterData;

public class CloudHelperUtil {

	public static ItemRegistry retrieveItemRegistry(String siteName) throws CloudException{
		ItemRegistry	itemRegirtry	=	null;
		try{
			IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(siteName);
			
			CloudMasterData master	=	(CloudMasterData)cache.getFromCache(siteName, null);
			if(master==null){
				CloudExceptionManager.throwException(CloudMessageConstants.CACHE_EXCEPTION, new Exception(), "");
			} else {
				itemRegirtry	=	master.getItemRegistry();
			}
			return itemRegirtry;
		} catch (CloudException e){
			throw e;
		}
	}
	
	public static ModelRepository retrieveModelRepRegistry(String siteName) throws CloudException{
		ModelRepository	modelRepository	=	null;
		try{
			IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(siteName);
			CloudMasterData master	=	(CloudMasterData)cache.getFromCache(siteName, null);
			if(master==null){
				CloudExceptionManager.throwException(CloudMessageConstants.CACHE_EXCEPTION, new Exception(), "");
			} else {
				modelRepository	=	master.getModelRepository();
			}
			return modelRepository;
		} catch (CloudException e){
			throw e;
		}
	}
	
	
	public static PageRenderer retrievePageRenderer(String siteName) throws CloudException{
		PageRenderer	pageRenderer	=	null;
		try{
			IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(siteName);
			CloudMasterData master	=	(CloudMasterData)cache.getFromCache(siteName, null);
			if(master==null){
				CloudExceptionManager.throwException(CloudMessageConstants.CACHE_EXCEPTION, new Exception(), "");
			} else {
				pageRenderer	=	master.getPageRenderer();
			}
			return pageRenderer;
		} catch (CloudException e){
			throw e;
		}
	}
	

	public static SitemapProvider retrieveSiteMapProvider(String siteName) throws CloudException{
		SitemapProvider	sitemapProvider	=	null;
		try{
			IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(siteName);
			CloudMasterData master	=	(CloudMasterData)cache.getFromCache(siteName, null);
			if(master==null){
				CloudExceptionManager.throwException(CloudMessageConstants.CACHE_EXCEPTION, new Exception(), "");
			} else {
				sitemapProvider	=	master.getSiteMapProvider();
			}
			return sitemapProvider;
		} catch (CloudException e){
			throw e;
		}
	}

}
