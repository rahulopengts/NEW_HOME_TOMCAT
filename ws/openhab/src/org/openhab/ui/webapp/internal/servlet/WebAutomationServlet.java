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
import javax.sql.DataSource;

import org.eclipse.emf.common.util.EList;
import org.openhab.core.events.EventPublisher;
import org.openhab.core.internal.events.EventPublisherImpl;
import org.openhab.core.items.GenericItem;
import org.openhab.core.items.Item;
import org.openhab.core.items.ItemNotFoundException;
import org.openhab.core.items.ItemRegistry;
import org.openhab.core.items.StateChangeListener;
import org.openhab.core.types.State;
import org.openhab.io.transport.mqtt.MqttService;
import org.openhab.model.core.internal.folder.FolderObserver;
import org.openhab.model.sitemap.Frame;
import org.openhab.model.sitemap.LinkableWidget;
import org.openhab.model.sitemap.Sitemap;
import org.openhab.model.sitemap.SitemapProvider;
import org.openhab.model.sitemap.Widget;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;
import org.openhab.ui.webapp.cloud.session.CloudSessionManager;
import org.openhab.ui.webapp.internal.render.PageRenderer;
import org.openhab.ui.webapp.render.RenderException;
import org.osgi.service.http.NamespaceException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.homeauto.db.core.TestDB;
import com.homeauto.db.core.dbprocessors.SiteDataDAO;
import com.openhab.core.cache.AppCacheFactory;
import com.openhab.core.cache.IAppCache;
import com.openhab.core.constant.CloudHomeAutoConstants;
import com.openhab.core.dto.CloudMasterData;
import com.openhab.core.event.messaging.mqtt.MessageBrokerService;
import com.openhab.core.quartz.scheduler.EventScheduler;
import com.openhab.core.util.AppPropertyReader;
import com.openhab.core.util.CloudHelperUtil;

public class WebAutomationServlet extends BaseServlet {

	private static final Logger logger = LoggerFactory
			.getLogger(WebAutomationServlet.class);

	/**
	 * timeout for polling requests in milliseconds; if no state changes during
	 * this time, an empty response is returned.
	 */
	public static final long TIMEOUT_IN_MS = 10000L;

	/** the name of the servlet to be used in the URL */
	public static final String SERVLET_NAME = "openhab.app";

	private EventPublisher cloudEventPublisher = null;
	// CloudChange
	//private MessageBrokerService messageBrokerService = null;

	public MqttService mqttService = null;
	@Override
	public void init(ServletConfig config) throws ServletException {
		// TODO Auto-generated method stub
		super.init(config);
		try {
			if (FolderObserver.CLOUD_MODE) {
				initializeApp();
				System.out.println("\nWebAppServler->Init");
			}
		} catch (Throwable e) {
			e.printStackTrace();
		}

	}

	protected void activate1() {
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

	protected void deactivate1() {
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
		testDB();
		
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
							// System.out.println("\n WebAutomationServlet->waitforChanges->Nothing Changed:");
							return;
						}
					} else {
						if (poll && waitForChanges(children) == false) {
							// we have reached the timeout, so we do not return
							// any content as nothing has changed
							res.getWriter().append(getTimeoutResponse())
									.close();
							// System.out.println("\n WebAutomationServlet->waitforChanges->Nothing Changed:");
							return;
						}

					}
					// System.out.println("\n WebAutomationServlet->waitforChanges->Changed:");
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
		// System.out.println("\nWebAutomationServlet->waitForChanges->Adding Listeners Now");
		long startTime = (new Date()).getTime();
		boolean timeout = false;
		AutomationBlockingStateChangeListener listener = new AutomationBlockingStateChangeListener();
		// let's get all items for these widgets
		Set<GenericItem> items = getAllItems(widgets);
		// System.out.println("\nWebAutomationServlet->waitForChanges->items.size()"+items.size());
		for (GenericItem item : items) {
			// System.out.println("\nWebAutomationServlet->waitForChanges->"+items.toString()+"->Listener->"+listener.getClass().getName());
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
	private static class AutomationBlockingStateChangeListener implements
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
			IAppCache cache = AppCacheFactory.getAppCacheInstance(sitemapName);
			System.out.println("\n QuartzEventHandler->cache->WebAuto"+cache);
			masterData = (CloudMasterData) cache
					.getFromCache(sitemapName, null);
			
			CloudSessionManager.setAttribute(session,
					CloudSessionManager.SITEMAPNAME, sitemapName);
			System.out.println("\n WebAutomationServlet->handleHttpRequest->getting MasterData for site->"+sitemapName);
			if (masterData != null) {
				System.out.println("\n WebAutomationServlet->handleHttpRequest->Existing Session for site->"+sitemapName);
				return masterData;
			} else {
				masterData	=	CloudHelperUtil.initializeSite(sitemapName,mqttService);
			}

			ItemRegistry itemRegistry	=	 masterData.getItemRegistry();
//			SiteDataDAO	siteInfo	=	new SiteDataDAO();
//			siteInfo.initilizeSiteItemInfo(null, itemRegistry);
			
		} catch (CloudException e){
			e.printStackTrace();
		}
		
		
		return masterData;
	}
	

	private void initializeApp() throws CloudException {
		try {
			// initializeCloudMqttService();
			mqttService = new MqttService();
			mqttService.activate();
			mqttService.updatedCloud(getMqttServiceDictionary());
			// cloudEventPublisher = mqttService.getEventPublisher();
			cloudEventPublisher = new EventPublisherImpl();
			mqttService.setEventPublisher(cloudEventPublisher);
			
			AppPropertyReader.getAppPropertyReader();
			IAppCache cache = AppCacheFactory.getAppCacheInstance("");
			System.out.println("\n QuartzEventHandler->cache->WebAuto"+cache);

			EventScheduler.getEventScheduler();
			
			System.out.println("\nWebAutomationServlet->Done with Scheduler");
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


	private void testDB(){
		try{
			System.out.print("\n Derby DB Calling");
			SiteDataDAO siteInfo	=	new SiteDataDAO();
			//siteInfo.fetchSiteInfo("");
			
//			TestDB t	=	new TestDB();
//			DataSource	data	=	t.init();
//			t.doGet(data);
		} catch (Exception e){
			e.printStackTrace();
		}
//		} catch (CloudException e){
//			e.printStackTrace();
//		}
	}
}
