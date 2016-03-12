package com.openhab.core.util;

import java.util.Dictionary;
import java.util.Hashtable;

import org.openhab.binding.mqtt.internal.MqttGenericBindingProvider;
import org.openhab.core.drools.internal.DroolsService;
import org.openhab.core.internal.items.ItemRegistryImpl;
import org.openhab.core.items.ItemFactory;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.library.CoreItemFactory;
import org.openhab.core.persistence.PersistenceService;
import org.openhab.core.persistence.internal.PersistenceManager;
import org.openhab.io.transport.mqtt.MqttService;
import org.openhab.model.ItemsStandaloneSetup;
import org.openhab.model.SitemapStandaloneSetup;
import org.openhab.model.core.ModelRepository;
import org.openhab.model.item.binding.BindingConfigReader;
import org.openhab.model.item.internal.GenericItemProvider;
import org.openhab.model.persistence.PersistenceStandaloneSetup;
import org.openhab.model.rule.RulesStandaloneSetup;
import org.openhab.model.script.ScriptStandaloneSetup;
import org.openhab.model.sitemap.SitemapProvider;
import org.openhab.model.sitemap.internal.SitemapProviderImpl;
import org.openhab.persistence.rrd4j.internal.RRD4jService;
import org.openhab.ui.internal.items.GenericItemUIProvider;
import org.openhab.ui.internal.items.ItemUIRegistryImpl;
import org.openhab.ui.items.ItemUIProvider;
import org.openhab.ui.items.ItemUIRegistry;
import org.openhab.ui.webapp.cloud.CloudFolderObserver;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;
import org.openhab.ui.webapp.internal.render.ChartRenderer;
import org.openhab.ui.webapp.internal.render.ColorpickerRenderer;
import org.openhab.ui.webapp.internal.render.FrameRenderer;
import org.openhab.ui.webapp.internal.render.GroupRenderer;
import org.openhab.ui.webapp.internal.render.ImageRenderer;
import org.openhab.ui.webapp.internal.render.ListRenderer;
import org.openhab.ui.webapp.internal.render.PageRenderer;
import org.openhab.ui.webapp.internal.render.SelectionRenderer;
import org.openhab.ui.webapp.internal.render.SetpointRenderer;
import org.openhab.ui.webapp.internal.render.SliderRenderer;
import org.openhab.ui.webapp.internal.render.SwitchRenderer;
import org.openhab.ui.webapp.internal.render.TextRenderer;
import org.openhab.ui.webapp.internal.render.VideoRenderer;
import org.openhab.ui.webapp.internal.render.WebviewRenderer;
import org.openhab.ui.webapp.render.WidgetRenderer;

import com.openhab.core.cache.AppCacheFactory;
import com.openhab.core.cache.IAppCache;
import com.openhab.core.constants.CloudAppConstants;
import com.openhab.core.dto.CloudMasterData;
import com.openhab.core.threadstore.CloudThreadLocalStorage;

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

	public static ItemUIRegistry retrieveItemUIRegistry(String siteName) throws CloudException{
		ItemUIRegistry	itemUIRegistry	=	null;
		try{
			IAppCache	cache	=	AppCacheFactory.getAppCacheInstance(siteName);
			
			CloudMasterData master	=	(CloudMasterData)cache.getFromCache(siteName, null);
			if(master==null){
				CloudExceptionManager.throwException(CloudMessageConstants.CACHE_EXCEPTION, new Exception(), "");
			} else {
				itemUIRegistry	=	master.getItemUIRegistry();
			}
			return itemUIRegistry;
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

	public static String getHomeId(String receievedMessage) throws CloudException {
		if(receievedMessage==null){
			CloudExceptionManager.throwException(CloudMessageConstants.NULL_RECEIVEDMESAGE,new Exception(), null);
		}
		
		String homeId	=	receievedMessage.substring(CloudAppConstants.STARTINDEX_HOMEID, CloudAppConstants.ENDINDEX_HOMEID);
		return homeId;
	}
	
	
	
	public static CloudMasterData initializeSite(String sitemapName,MqttService mqttService) {
		CloudMasterData masterData = null;
		PageRenderer cloudRenderer = null;
		SitemapProvider cloudSitemapProvider = null;
		try {
				IAppCache cache = AppCacheFactory.getAppCacheInstance(sitemapName);
				masterData = (CloudMasterData) cache
						.getFromCache(sitemapName, null);
				System.out.println("\n WebAppServlet->handleHttpRequest->getting MasterData for site->"+sitemapName);
				if (masterData != null) {
					System.out.println("\n WebAppServlet->handleHttpRequest->Existing Session for site->"+sitemapName);
					return masterData;
				}
				
				
				
				Dictionary dict = new Hashtable();
				// String[] s = {"10","items"};
				String s = "10,items";
				dict.put("items", s);

				String s1 = "10,persist";
				dict.put("persistence", s1);

				String s2 = "10,rules";
				dict.put("rules", s2);

				String s3 = "10,script";
				dict.put("scripts", s3);

				String s4 = ",service.pid";
				dict.put("service.pid", s4);

				String s5 = "10,sitemap";
				dict.put("sitemaps", s5);

				ModelRepository localModelRepository = null;
				CloudFolderObserver cloudFolderObserver = null;
				ItemRegistryImpl cloudItemRegistry = null;// new ItemRegistryImpl();
				ItemUIRegistryImpl cloudUIItemRegistry = null;
				ItemUIProvider cloudItemUIProvider = null;
				if (cloudRenderer == null) {

					cloudItemRegistry = new ItemRegistryImpl();
					cloudUIItemRegistry = new ItemUIRegistryImpl();
					cloudItemUIProvider = new GenericItemUIProvider();

					SitemapStandaloneSetup.doSetup();
					ItemsStandaloneSetup.doSetup();
					PersistenceStandaloneSetup.doSetup();
					RulesStandaloneSetup.doSetup();
					ScriptStandaloneSetup.doSetup();

					masterData = new CloudMasterData();
					masterData.setItemRegistry(cloudItemRegistry);
					masterData.setModelRepository(localModelRepository);
					masterData.setItemUIRegistry(cloudUIItemRegistry);
					// masterData.setTopicName(topicName);
					CloudThreadLocalStorage.setCloudMasterData(masterData);

					cloudRenderer = new PageRenderer();

					masterData.setPageRenderer(cloudRenderer);

					cloudFolderObserver = new CloudFolderObserver();
					cloudFolderObserver.setHomeName(sitemapName);

					cloudFolderObserver.updated(dict);
					localModelRepository = cloudFolderObserver.getModelRepository();
					
					
				} else {
					System.out.println("\n WebAppServlet->handleHttpRequest->Existing Session for site->"+sitemapName);
					return masterData;
				}
				masterData.setItemRegistry(cloudItemRegistry);
				masterData.setModelRepository(localModelRepository);
				CloudThreadLocalStorage.setCloudMasterData(masterData);

				cloudRenderer.setItemUIRegistry(cloudUIItemRegistry);
				// ItemRegistryImpl cloudItemRegistry = new ItemRegistryImpl();
				// ItemUIRegistryImpl depends on ItemRegistryImpl and ItemUIProvider
				// ItemUIProvider cloudItemUIProvider = new GenericItemUIProvider();
				cloudUIItemRegistry.setItemRegistry(cloudItemRegistry);
				((GenericItemUIProvider) cloudItemUIProvider)
						.setModelRepository(localModelRepository);
				cloudUIItemRegistry.addItemUIProvider(cloudItemUIProvider);
				// ItemRegistry Depends on ItemProvider-GenericItemProvider
				GenericItemProvider cloudGenericItemProvider = new GenericItemProvider();
				// Depends on ModelRepositoryImpl, ItemFactory,BindingConfigReader
				// --MOVED-ModelRepository localModelRepository1 =
				// cloudFolderObserver.getModelRepository();
				localModelRepository.setName("rahul");
				System.out.println("\nWebAppServlet->ModelRepositoryImpl->this->"
						+ localModelRepository);

				cloudGenericItemProvider.setModelRepository(localModelRepository);

				// cloudGenericItemProvider.addItemFactory(factory)
				ItemFactory itemFactory = new CoreItemFactory();
				cloudGenericItemProvider.addItemFactory(itemFactory);
				cloudItemRegistry.addItemProvider(cloudGenericItemProvider);

				intitializeMQTTBinding(cloudGenericItemProvider,
						localModelRepository,sitemapName,mqttService);

				cloudSitemapProvider = new SitemapProviderImpl();

				cloudSitemapProvider.setModelRepository(localModelRepository);
				// sitemapProvider = cloudSitemapProvider;
				masterData.setSiteMapProvider(cloudSitemapProvider);

				
				addPageRenderers(cloudRenderer, cloudUIItemRegistry,
						localModelRepository);

				PersistenceManager persistenceManager = initilizeModelWithStoredData(
						cloudItemRegistry, localModelRepository,sitemapName);

				DroolsService	drools	=	initializeDroolsService(cloudItemRegistry, localModelRepository,sitemapName);

				//PUT THE OBJECTS INTO CACHE
				masterData.setItemRegistry(cloudItemRegistry);
				masterData.setModelRepository(localModelRepository);
				masterData.setPageRenderer(cloudRenderer);
				masterData.setSiteMapProvider(cloudSitemapProvider);
				masterData.setPersistenceManager(persistenceManager);
				masterData.setDroolsService(drools);
				cache.putIntoCache(sitemapName, masterData);
				System.out.println("\n WebAppServlet->Thread:"
						+ Thread.currentThread().getId());

			} catch (Throwable e) {
				e.printStackTrace();
				// throw e;
			}

			return masterData;
	}


	private static void intitializeMQTTBinding(GenericItemProvider genericItemProvider,ModelRepository modelRepo,String homeName,MqttService mqttService) {
		BindingConfigReader bindingConfigReader = new MqttGenericBindingProvider();
		// System.out.println("\nWebAppServlet->initialzeMqttBinding->"+bindingConfigReader);
		((MqttGenericBindingProvider) bindingConfigReader)
				.setMqttService(mqttService);
		((MqttGenericBindingProvider) bindingConfigReader)
		.setHomeName(homeName);
		genericItemProvider.addBindingConfigReader(bindingConfigReader);
	}


	private static void addPageRenderers(PageRenderer cloudRenderer,
			ItemUIRegistry cloudUIItemRegistry,
			ModelRepository localModelRepository) {

		WidgetRenderer groupRenderer = new GroupRenderer();
		groupRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer frameRenderer = new FrameRenderer();
		frameRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer switchRenderer = new SwitchRenderer();
		switchRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer selectionRenderer = new SelectionRenderer();
		selectionRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer listRenderer = new ListRenderer();
		listRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer textRenderer = new TextRenderer();
		textRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer imageRenderer = new ImageRenderer();
		imageRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer sliderRenderer = new SliderRenderer();
		sliderRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer chartRenderer = new ChartRenderer();
		chartRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer videoRenderer = new VideoRenderer();
		videoRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer webviewRenderer = new WebviewRenderer();
		webviewRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer setpointRenderer = new SetpointRenderer();
		setpointRenderer.setItemUIRegistry(cloudUIItemRegistry);

		WidgetRenderer colorpickerRenderer = new ColorpickerRenderer();
		colorpickerRenderer.setItemUIRegistry(cloudUIItemRegistry);

		cloudRenderer.addWidgetRenderer(groupRenderer);
		cloudRenderer.addWidgetRenderer(frameRenderer);
		cloudRenderer.addWidgetRenderer(switchRenderer);
		cloudRenderer.addWidgetRenderer(selectionRenderer);
		cloudRenderer.addWidgetRenderer(listRenderer);
		cloudRenderer.addWidgetRenderer(textRenderer);
		cloudRenderer.addWidgetRenderer(imageRenderer);
		cloudRenderer.addWidgetRenderer(sliderRenderer);
		cloudRenderer.addWidgetRenderer(chartRenderer);
		cloudRenderer.addWidgetRenderer(videoRenderer);
		cloudRenderer.addWidgetRenderer(webviewRenderer);
		cloudRenderer.addWidgetRenderer(setpointRenderer);
		cloudRenderer.addWidgetRenderer(colorpickerRenderer);
		// initilizeModelWithStoredData(cloudUIItemRegistry,localModelRepository);

	}

	private static PersistenceManager initilizeModelWithStoredData(
			ItemRegistry cloudItemRegistry, ModelRepository localModelRepository,String siteMapName) {
		// This to initialize the stored data into the modelreposotory.

		PersistenceService persistenceService = new RRD4jService(siteMapName);
		RRD4jService rRD4jService = (RRD4jService) persistenceService;
		rRD4jService.setItemRegistry(cloudItemRegistry);
		PersistenceManager persistenceManager = new PersistenceManager();
		persistenceManager.setUserSiteId(siteMapName);
		persistenceManager.setItemRegistry(cloudItemRegistry);
		persistenceManager.setModelRepository(localModelRepository);
		persistenceManager.addPersistenceService(persistenceService);
		
		
		return persistenceManager;

		// ---------------------NEW DB-----------------------//
		/*
		 * ICloudDAO cloudDAO = new CloudItemDAO();
		 * cloudDAO.initialzeItem(localModelRepository, cloudUIItemRegistry);
		 */
	}

	private static DroolsService initializeDroolsService(ItemRegistry cloudItemRegistry,
			ModelRepository localModelRepository,String siteName) {
			DroolsService	drools	=	new DroolsService();
			drools.setSiteName(siteName);
			drools.setItemRegistry(cloudItemRegistry);
			drools.activate();
			return drools;
	}

}
