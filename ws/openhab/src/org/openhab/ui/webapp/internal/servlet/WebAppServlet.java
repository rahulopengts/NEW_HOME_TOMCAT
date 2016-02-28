/**
 * Copyright (c) 2010-2015, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
//org.openhab.model.item ->; singleton:=true
package org.openhab.ui.webapp.internal.servlet;

import java.io.IOException;
import java.util.Date;
import java.util.Dictionary;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Set;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import org.eclipse.emf.common.util.EList;
import org.openhab.binding.mqtt.internal.MqttGenericBindingProvider;
import org.openhab.core.drools.internal.DroolsService;
import org.openhab.core.events.EventPublisher;
import org.openhab.core.internal.events.EventPublisherImpl;
import org.openhab.core.internal.items.ItemRegistryImpl;
import org.openhab.core.items.GenericItem;
import org.openhab.core.items.Item;
import org.openhab.core.items.ItemFactory;
import org.openhab.core.items.ItemNotFoundException;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.items.StateChangeListener;
import org.openhab.core.library.CoreItemFactory;
import org.openhab.core.persistence.PersistenceService;
import org.openhab.core.persistence.internal.PersistenceManager;
import org.openhab.core.scriptengine.ScriptEngine;
import org.openhab.core.types.State;
import org.openhab.io.transport.mqtt.MqttService;
import org.openhab.model.ItemsStandaloneSetup;
import org.openhab.model.SitemapStandaloneSetup;
import org.openhab.model.core.ModelRepository;
import org.openhab.model.core.internal.folder.FolderObserver;
import org.openhab.model.item.binding.BindingConfigReader;
import org.openhab.model.item.internal.GenericItemProvider;
import org.openhab.model.persistence.PersistenceStandaloneSetup;
import org.openhab.model.rule.RulesStandaloneSetup;
import org.openhab.model.rule.internal.engine.RuleEngine;
import org.openhab.model.script.ScriptStandaloneSetup;
import org.openhab.model.script.internal.engine.ItemRegistryProvider;
import org.openhab.model.script.internal.engine.ScriptEngineImpl;
import org.openhab.model.script.scoping.StateAndCommandProvider;
import org.openhab.model.sitemap.Frame;
import org.openhab.model.sitemap.LinkableWidget;
import org.openhab.model.sitemap.Sitemap;
import org.openhab.model.sitemap.SitemapProvider;
import org.openhab.model.sitemap.Widget;
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
import org.openhab.ui.webapp.cloud.session.CloudSessionManager;
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
import org.openhab.ui.webapp.render.RenderException;
import org.openhab.ui.webapp.render.WidgetRenderer;
import org.osgi.service.http.NamespaceException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.openhab.core.cache.AppCacheFactory;
import com.openhab.core.cache.IAppCache;
import com.openhab.core.constant.CloudHomeAutoConstants;
import com.openhab.core.dto.CloudMasterData;
import com.openhab.core.event.messaging.mqtt.MessageBrokerService;
import com.openhab.core.quartz.scheduler.EventScheduler;
import com.openhab.core.threadstore.CloudThreadLocalStorage;
import com.openhab.core.util.AppPropertyReader;

/**
 * This is the main servlet for the WebApp UI. It serves the Html code based on
 * the sitemap model.
 * 
 * @author Kai Kreuzer
 * 
 */
public class WebAppServlet extends BaseServlet {

	// public final static boolean CLOUD_MODE = true;

	private static final Logger logger = LoggerFactory
			.getLogger(WebAppServlet.class);

	/**
	 * timeout for polling requests in milliseconds; if no state changes during
	 * this time, an empty response is returned.
	 */
	public static final long TIMEOUT_IN_MS = 10000L;

	/** the name of the servlet to be used in the URL */
	public static final String SERVLET_NAME = "openhab.app";

	// private PageRenderer renderer;

	// CloudChange
	// private PageRenderer cloudRenderer;

	private EventPublisher cloudEventPublisher = null;
	// private ModelRepository modelRepository = null;

	// CloudChange
	private MessageBrokerService messageBrokerService = null;


	@Override
	public void init(ServletConfig config) throws ServletException {
		// TODO Auto-generated method stub
		super.init(config);
		try {
			if (FolderObserver.CLOUD_MODE) {
				initializeApp();
				// cloudEventPublisher =

				System.out.println("\nWebAppServler->Init");
			}
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}

	protected void activate() {
		try {
			Hashtable<String, String> props = new Hashtable<String, String>();
			httpService.registerServlet(WEBAPP_ALIAS + SERVLET_NAME, this,
					props, createHttpContext());
			httpService.registerResources(WEBAPP_ALIAS, "web", null);
			logger.info("Started Classic UI at " + WEBAPP_ALIAS + SERVLET_NAME);
		} catch (NamespaceException e) {
			logger.error("Error during servlet startup", e);
		} catch (ServletException e) {
			logger.error("Error during servlet startup", e);
		}
	}

	protected void deactivate() {
		httpService.unregister(WEBAPP_ALIAS + SERVLET_NAME);
		httpService.unregister(WEBAPP_ALIAS);
		logger.info("Stopped Classic UI");
	}

	/**
	 * {@inheritDoc}
	 */
	public void service(ServletRequest req, ServletResponse res)
			throws ServletException, IOException {
		logger.debug("Servlet request received!");
		// System.out.println("Servlet request received!");
		// read request parameters
		PageRenderer renderer = null;
		CloudMasterData masterData = null;
		SitemapProvider sitemapProvider = null;

		String sitemapName = (String) req.getParameter("sitemap");
		String widgetId = (String) req.getParameter("w");
		boolean async = "true".equalsIgnoreCase((String) req
				.getParameter("__async"));
		boolean poll = "true".equalsIgnoreCase((String) req
				.getParameter("poll"));

		if (CloudHomeAutoConstants.CLOUD_MODE) {
			masterData = handleHttpRequest((HttpServletRequest) req,
					(HttpServletResponse) res, sitemapName);
			sitemapProvider = masterData.getSiteMapProvider();
			System.out.println("\n WebAppServler - > service -> siteMapName "+sitemapName);
			renderer = masterData.getPageRenderer();
		}
		if (sitemapName == null)
			sitemapName = "default";

		StringBuilder result = new StringBuilder();
		Sitemap sitemap = sitemapProvider.getSitemap(sitemapName);

		// **************************
		// System.out.println(" SitemapNameAA"+sitemapName+" Sitemap-- ");//);
		logger.debug(" async " + async + " poll " + poll);
		logger.info(" async info " + async + " poll " + poll);
		// **************************

		try {
			if (sitemap == null) {
				throw new RenderException("Sitemap '" + sitemapName
						+ "' could not be found");
			}
			logger.debug("reading sitemap {}", sitemap.getName());
			// System.out.println("\n Sitemap Name : "+sitemap.getName());
			if (widgetId == null || widgetId.isEmpty()
					|| widgetId.equals("Home")) {
				// we are at the homepage, so we render the children of the
				// sitemap root node
				String label = sitemap.getLabel() != null ? sitemap.getLabel()
						: sitemapName;
				EList<Widget> children = sitemap.getChildren();
				int childSize = children.size();

				if (FolderObserver.CLOUD_MODE) {
					if (poll
							&& CloudWebAppServletHelper.waitForChanges(
									children, (HttpServletRequest) req) == false) {
						// we have reached the timeout, so we do not return any
						// content as nothing has changed
						res.getWriter().append(getTimeoutResponse()).close();
						return;
					}

				} else {
					if (poll && waitForChanges(children) == false) {
						// we have reached the timeout, so we do not return any
						// content as nothing has changed
						res.getWriter().append(getTimeoutResponse()).close();
						return;
					}

				}
				// System.out.println("\n WebAppServler - > service -> request for IF-WidgetId Null");
				// System.out.println("\n WebAppServler - > sitemap.getChildren()-> "+sitemap.getChildren());
				StringBuilder testBuilder = renderer.processPage("Home",
						sitemapName, label, sitemap.getChildren(), async);
				// System.out.println("Chile Size : \n "+ childSize);
				result.append(testBuilder);

			} else if (!widgetId.equals("Colorpicker")) {
				// we are on some subpage, so we have to render the children of
				// the widget that has been selected
				Widget w = renderer.getItemUIRegistry().getWidget(sitemap,
						widgetId);
				if (w != null) {
					if (!(w instanceof LinkableWidget)) {
						throw new RenderException("Widget '" + w
								+ "' can not have any content");
					}
					EList<Widget> children = renderer.getItemUIRegistry()
							.getChildren((LinkableWidget) w);
					int childSize = children.size();
					// System.out.println("\n Child Size "+childSize);
					if (FolderObserver.CLOUD_MODE) {
						if (poll
								&& CloudWebAppServletHelper.waitForChanges(
										children, (HttpServletRequest) req) == false) {
							// we have reached the timeout, so we do not return
							// any content as nothing has changed
							res.getWriter().append(getTimeoutResponse())
									.close();
							// System.out.println("\n WebAppServlet->waitforChanges->Nothing Changed:");
							return;
						}
					} else {
						if (poll && waitForChanges(children) == false) {
							// we have reached the timeout, so we do not return
							// any content as nothing has changed
							res.getWriter().append(getTimeoutResponse())
									.close();
							// System.out.println("\n WebAppServlet->waitforChanges->Nothing Changed:");
							return;
						}

					}
					// System.out.println("\n WebAppServlet->waitforChanges->Changed:");
					String label = renderer.getItemUIRegistry().getLabel(w);
					if (label == null)
						label = "undefined";
					result.append(renderer.processPage(renderer
							.getItemUIRegistry().getWidgetId(w), sitemapName,
							label, children, async));
					// System.out.println("\n WebAppServler - > service -> request for ELSE-WidgetId :"+w.getItem());
				}

			}
		} catch (RenderException e) {
			e.printStackTrace();
			throw new ServletException(e.getMessage(), e);
		} catch (CloudException e) {
			e.printStackTrace();
			throw new ServletException(e.getMessage(), e);
		}
		if (async) {
			res.setContentType("application/xml;charset=UTF-8");
		} else {
			res.setContentType("text/html;charset=UTF-8");
		}
		System.out.println("Testing : \n " + result.toString());
		res.getWriter().append(result);
		res.getWriter().close();
	}

	/**
	 * Defines the response to return on a polling timeout.
	 * 
	 * @return the response of the servlet on a polling timeout
	 */
	private String getTimeoutResponse() {
		return "<root><part><destination mode=\"replace\" zone=\"timeout\" create=\"false\"/><data/></part></root>";
	}

	/**
	 * This method only returns when a change has occurred to any item on the
	 * page to display
	 * 
	 * @param widgets
	 *            the widgets of the page to observe
	 */
	private boolean waitForChanges(EList<Widget> widgets) {
		// System.out.println("\nWebAppServlet->waitForChanges->Adding Listeners Now");
		long startTime = (new Date()).getTime();
		boolean timeout = false;
		BlockingStateChangeListener listener = new BlockingStateChangeListener();
		// let's get all items for these widgets
		Set<GenericItem> items = getAllItems(widgets);
		// System.out.println("\nWebAppServlet->waitForChanges->items.size()"+items.size());
		for (GenericItem item : items) {
			// System.out.println("\nWebAppServlet->waitForChanges->"+items.toString()+"->Listener->"+listener.getClass().getName());
			item.addStateChangeListener(listener);
		}
		do {
			timeout = (new Date()).getTime() - startTime > TIMEOUT_IN_MS;
			try {
				Thread.sleep(500);
			} catch (InterruptedException e) {
				timeout = true;
				break;
			}
		} while (!listener.hasChangeOccurred() && !timeout);
		for (GenericItem item : items) {
			item.removeStateChangeListener(listener);
		}
		return !timeout;
	}

	/**
	 * Collects all items that are represented by a given list of widgets
	 * 
	 * @param widgets
	 *            the widget list to get the items for
	 * @return all items that are represented by the list of widgets
	 */
	private Set<GenericItem> getAllItems(EList<Widget> widgets) {
		Set<GenericItem> items = new HashSet<GenericItem>();
		if (itemRegistry != null) {
			for (Widget widget : widgets) {
				String itemName = widget.getItem();
				if (itemName != null) {
					try {
						Item item = itemRegistry.getItem(itemName);
						if (item instanceof GenericItem) {
							final GenericItem gItem = (GenericItem) item;
							items.add(gItem);
						}
					} catch (ItemNotFoundException e) {
						// ignore
					}
				} else {
					if (widget instanceof Frame) {
						items.addAll(getAllItems(((Frame) widget).getChildren()));
					}
				}
			}
		}
		return items;
	}

	/**
	 * This is a state change listener, which is merely used to determine, if a
	 * state change has occurred on one of a list of items.
	 * 
	 * @author Kai Kreuzer
	 * 
	 */
	private static class BlockingStateChangeListener implements
			StateChangeListener {

		private boolean changed = false;

		/**
		 * {@inheritDoc}
		 */
		public void stateChanged(Item item, State oldState, State newState) {
			changed = true;
		}

		/**
		 * determines, whether a state change has occurred since its creation
		 * 
		 * @return true, if a state has changed
		 */
		public boolean hasChangeOccurred() {
			return changed;
		}

		/**
		 * {@inheritDoc}
		 */
		public void stateUpdated(Item item, State state) {
			changed = true;
		}

	}

	// CloudChange
	public CloudMasterData handleHttpRequest(HttpServletRequest req,
			HttpServletResponse res, String sitemapName) {
		CloudMasterData masterData = null;
		PageRenderer cloudRenderer = null;
		SitemapProvider cloudSitemapProvider = null;

		try {
			HttpSession session = CloudSessionManager.getSession(req, res,
					sitemapName);
//			cloudRenderer = (PageRenderer) CloudSessionManager.getAttribute(
//					session, CloudSessionManager.PAGERRENDERER);

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

			// Adding for ScriptStandaloneProvider
			StateAndCommandProvider stateAndCommandProvider;
			ItemRegistryProvider itemRegistryProvider = new ItemRegistryProvider();

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
//				CloudSessionManager.setAttribute(session,
//						CloudSessionManager.PAGERRENDERER, cloudRenderer);
//				CloudSessionManager.setAttribute(session,
//						CloudSessionManager.MODELREPO, localModelRepository);
//				CloudSessionManager
//						.setAttribute(session,
//								CloudSessionManager.EVENTPUBLISHER,
//								cloudEventPublisher);
				
//				CloudSessionManager.setAttribute(session,
//						CloudSessionManager.CLOUDMASTERDATA, masterData);
				
				
			} else {
//				masterData = (CloudMasterData) CloudSessionManager
//						.getAttribute(session,
//								CloudSessionManager.CLOUDMASTERDATA);
				System.out.println("\n WebAppServlet->handleHttpRequest->Existing Session for site->"+sitemapName);
				return masterData;
			}

			// masterData = new CloudMasterData();
			masterData.setItemRegistry(cloudItemRegistry);
			masterData.setModelRepository(localModelRepository);
			// masterData.setTopicName(topicName);
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
					localModelRepository,sitemapName);

			cloudSitemapProvider = new SitemapProviderImpl();

			cloudSitemapProvider.setModelRepository(localModelRepository);
			// sitemapProvider = cloudSitemapProvider;
			masterData.setSiteMapProvider(cloudSitemapProvider);

//			CloudSessionManager.setAttribute(session,
//					CloudSessionManager.ITEMREGISTRY, cloudItemRegistry);
			
			addPageRenderers(cloudRenderer, cloudUIItemRegistry,
					localModelRepository);

			PersistenceManager persistenceManager = initilizeModelWithStoredData(
					cloudItemRegistry, localModelRepository,sitemapName);

			DroolsService	drools	=	initializeDroolsService(cloudItemRegistry, localModelRepository,sitemapName);
//			CloudSessionManager.setAttribute(session,
//					CloudSessionManager.PERSISTENCEMANAGER, persistenceManager);

			// RuleEngine ruleEngine =
			// initializeRuleEngine(cloudItemRegistry,localModelRepository);
			//
			// CloudSessionManager.setAttribute(session,
			// CloudSessionManager.RULEENGINE, ruleEngine);

			CloudSessionManager.setAttribute(session,
					CloudSessionManager.SITEMAPNAME, sitemapName);

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

	private DroolsService initializeDroolsService(ItemRegistry cloudItemRegistry,
			ModelRepository localModelRepository,String siteName) {
			DroolsService	drools	=	new DroolsService();
			drools.setSiteName(siteName);
			drools.setItemRegistry(cloudItemRegistry);
			drools.activate();
			return drools;
	}
	
	private RuleEngine initializeRuleEngine(ItemRegistry cloudItemRegistry,
			ModelRepository localModelRepository) {
		RuleEngine ruleEngine = new RuleEngine();
		System.out
				.println("\n CloudEventPublisheImpl->Initialize RuleEngine->2");
		ruleEngine.setItemRegistry(cloudItemRegistry);
		System.out
				.println("\n CloudEventPublisheImpl->Initialize RuleEngine->3");
		ScriptEngine scriptEngine = new ScriptEngineImpl();
		((ScriptEngineImpl) scriptEngine).activate();
		System.out
				.println("\n CloudEventPublisheImpl->Initialize RuleEngine->4");
		ruleEngine.setScriptEngine(scriptEngine);
		System.out
				.println("\n CloudEventPublisheImpl->Initialize RuleEngine->5");
		ruleEngine.setModelRepository(localModelRepository);
		System.out
				.println("\n CloudEventPublisheImpl->Initialize RuleEngine->6");
		ruleEngine.activate();
		return ruleEngine;
	}

	private void addPageRenderers(PageRenderer cloudRenderer,
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

	private PersistenceManager initilizeModelWithStoredData(
			ItemRegistry cloudItemRegistry, ModelRepository localModelRepository,String siteMapName) {
		// This to initialize the stored data into the modelreposotory.

		PersistenceService persistenceService = new RRD4jService(siteMapName);
		RRD4jService rRD4jService = (RRD4jService) persistenceService;
		rRD4jService.setItemRegistry(cloudItemRegistry);
		PersistenceManager persistenceManager = new PersistenceManager();
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

	public void validateAndGetSession(HttpServletRequest req,
			HttpServletResponse resp) {
		if (req != null) {
			HttpSession session = req.getSession(false);
			if (session == null) {
				// Create New Sssion
				session = req.getSession(true);
			} else {

			}
		}

	}

	MqttService mqttService = null;

	private void initializeApp() throws CloudException {
		try {
			// initializeCloudMqttService();
			mqttService = new MqttService();
			mqttService.activate();
			mqttService.updatedCloud(getMqttServiceDictionary());
			// cloudEventPublisher = mqttService.getEventPublisher();
			cloudEventPublisher = new EventPublisherImpl();
			mqttService.setEventPublisher(cloudEventPublisher);
			messageBrokerService = MessageBrokerService.getInstance();
			AppPropertyReader.getAppPropertyReader();
			EventScheduler.getEventScheduler();
			// AdminEventImpl manager = new AdminEventImpl();
			// manager.initializeBus();

			// System.out.println("\nWebAppServlet->initilize->cloudEventPublisher->"+cloudEventPublisher);
		} catch (Exception e) {
			e.printStackTrace();
			CloudExceptionManager.throwException(
					CloudMessageConstants.MQTT_SERVICE_ERROR, null,
					"could not initilize mqtt");
		}
	}

	private Dictionary<String, ?> getMqttServiceDictionary() {
		Dictionary dict = new Hashtable();
		dict.put("mosquitto.async", "false");
		dict.put("mosquitto.async", "false");
		dict.put("mosquitto.clientId", "openHAB");
		dict.put("mosquitto.qos", "1");
		dict.put("mosquitto.retain", "false");
		dict.put("mosquitto.url", "tcp://localhost:1883");
		return dict;
		// MqttService->updated->property->async
		// MqttService->updated->property->clientId
		// MqttService->updated->property->qos
		// MqttService->updated->property->retain

	}

	public void intitializeMQTTBinding(GenericItemProvider genericItemProvider,
			ModelRepository modelRepo,String homeName) {
		BindingConfigReader bindingConfigReader = new MqttGenericBindingProvider();
		// System.out.println("\nWebAppServlet->initialzeMqttBinding->"+bindingConfigReader);
		((MqttGenericBindingProvider) bindingConfigReader)
				.setMqttService(mqttService);
		((MqttGenericBindingProvider) bindingConfigReader)
		.setHomeName(homeName);
		genericItemProvider.addBindingConfigReader(bindingConfigReader);
	}
}
