package com.openhab.core.rest;

import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URLEncoder;
import java.util.Collection;
import java.util.LinkedList;
import java.util.concurrent.TimeUnit;

import javax.ws.rs.DefaultValue;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.WebApplicationException;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriBuilder;
import javax.ws.rs.core.UriInfo;

import org.apache.commons.lang.StringUtils;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.openhab.core.items.Item;
import org.openhab.io.rest.RESTApplication;
import org.openhab.io.rest.internal.resources.ItemResource;
import org.openhab.io.rest.internal.resources.MediaTypeHelper;
import org.openhab.io.rest.internal.resources.ResponseHelper;
import org.openhab.io.rest.internal.resources.ResponseTypeHelper;
import org.openhab.io.rest.internal.resources.SitemapResource;
import org.openhab.io.rest.internal.resources.beans.MappingBean;
import org.openhab.io.rest.internal.resources.beans.PageBean;
import org.openhab.io.rest.internal.resources.beans.SitemapBean;
import org.openhab.io.rest.internal.resources.beans.SitemapListBean;
import org.openhab.io.rest.internal.resources.beans.WidgetBean;
import org.openhab.model.core.ModelRepository;
import org.openhab.model.sitemap.Chart;
import org.openhab.model.sitemap.Frame;
import org.openhab.model.sitemap.Image;
import org.openhab.model.sitemap.LinkableWidget;
import org.openhab.model.sitemap.List;
import org.openhab.model.sitemap.Mapping;
import org.openhab.model.sitemap.Selection;
import org.openhab.model.sitemap.Setpoint;
import org.openhab.model.sitemap.Sitemap;
import org.openhab.model.sitemap.Slider;
import org.openhab.model.sitemap.Switch;
import org.openhab.model.sitemap.Video;
import org.openhab.model.sitemap.Webview;
import org.openhab.model.sitemap.Widget;
import org.openhab.ui.items.ItemUIRegistry;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.example.rest.GeneralBroadcaster;
import com.openhab.core.util.CloudHelperUtil;



//@Path("/sitemaps")
@Path(SitemapResource.PATH_SITEMAPS)
public class CloudRESTService {

	private static final Logger logger = LoggerFactory.getLogger(CloudRESTService.class); 

	protected static final String SITEMAP_FILEEXT = ".sitemap";

	public static final String PATH_SITEMAPS = "sitemaps";

	public static final String ATMOS_TIMEOUT_HEADER = "X-Atmosphere-Timeout";
	
	public static final int DEFAULT_TIMEOUT_SECS = 300;

	
//	@Context UriInfo uriInfo;
//	@GET
//	@Path("/{param}")
//	@Produces( { MediaType.WILDCARD })
//	public Response getSitemaps(@PathParam("param") String msg,    		
//			@Context HttpHeaders headers,
//    		@QueryParam("type") String type, 
//    		@QueryParam("jsoncallback") @DefaultValue("callback") String callback) {

	@Context UriInfo uriInfo;
	@GET
	@Produces( { MediaType.WILDCARD })
	public Response getSitemaps(@Context HttpHeaders headers,
    		@QueryParam("type") String type, 
    		@QueryParam("jsoncallback") @DefaultValue("callback") String callback) {
	
			String responseType	=	"application/xml";
			callback	=	"callback";
			
			System.out.println("\ngetMsg->Got it");
			//String output = "Jersey say : " + msg+"->URIINFO->";
			if (logger.isDebugEnabled()) logger.debug("Received HTTP GET request at '{}' for media type '{}'.", uriInfo.getPath(), type);
			if(responseType!=null) {
				final SitemapListBean content = new SitemapListBean(getSitemapBeans(uriInfo.getAbsolutePathBuilder().build()));
				final Object responseObject = ResponseHelper.wrapContentIfNeccessary(callback, responseType, content);
				System.out.println("\nSitemapReource->getSiteMaps->responseObject->"+responseObject);
				return Response.ok(responseObject, responseType).build();
			} else {
				return Response.notAcceptable(null).build();
			}
	}

	public Collection<SitemapBean> getSitemapBeans(URI uri) {
		Collection<SitemapBean> beans = new LinkedList<SitemapBean>();
		if (logger.isDebugEnabled()) logger.debug("Received HTTP GET request at '{}'.", UriBuilder.fromUri(uri).build().toASCIIString());
		System.out.println("\nSitemapReource->getSitemapBeans->");
		ModelRepository modelRepository = null;
		
		try{
			modelRepository = CloudHelperUtil.retrieveModelRepRegistry("demo");
		} catch (CloudException e){
			e.printStackTrace();
		}
		
		for(String modelName : modelRepository.getAllModelNamesOfType("sitemap")) {
			Sitemap sitemap = (Sitemap) modelRepository.getModel(modelName);
			if(sitemap!=null) {
				SitemapBean bean = new SitemapBean();
				bean.name = StringUtils.removeEnd(modelName, SITEMAP_FILEEXT);
				bean.icon = sitemap.getIcon();
				bean.label = sitemap.getLabel();
				bean.link = UriBuilder.fromUri(uri).path(bean.name).build().toASCIIString();
				bean.homepage = new PageBean();
				bean.homepage.link = bean.link + "/" + sitemap.getName();
				beans.add(bean);
			}
		}
		return beans;
	}
	
	//http://stackoverflow.com/questions/2945947/path-and-regular-expression-jersey-rest
	
	@GET @Path("/{sitemapname: [a-zA-Z_0-9]*}")
	//@GET @Path("/{sitemapname: [a-zA-Z_0-9_/]*}")
    @Produces( { MediaType.WILDCARD })
    public Response getSitemapData(
    		@Context HttpHeaders headers,
    		@PathParam("sitemapname") String sitemapname, 
    		@QueryParam("type") String type, 
    		@QueryParam("jsoncallback") @DefaultValue("callback") String callback) {

		String responseType	=	"application/xml";
		callback	=	"callback";
		//*********************//
		String newSiteMapName	=	sitemapname.substring(0,sitemapname.indexOf("/"));
		sitemapname	=	newSiteMapName;
		//*********************//
		System.out.println("\nSitemapReource->getSitemapData->"+sitemapname);
		if (logger.isDebugEnabled()) logger.debug("Received HTTP GET request at '{}' for media type '{}'.", uriInfo.getPath(), type);
		//final String responseType = MediaTypeHelper.getResponseMediaType(headers.getAcceptableMediaTypes(), type);
		if(responseType!=null) {
			final SitemapBean content = getSitemapBean(sitemapname, uriInfo.getBaseUriBuilder().build());
			final Object responseObject = ResponseHelper.wrapContentIfNeccessary(callback, responseType, content);
			System.out.println("\nSitemapReource->getSitemapData->content->"+content.toString());
			return Response.ok(responseObject, responseType).build();
		} else {
			System.out.println("\nSitemapReource->getSitemapData->else->"+Response.notAcceptable(null).build());
			return Response.notAcceptable(null).build();
		}
    }

	public SitemapBean getSitemapBean(String sitemapname, URI uri) {
		System.out.println("\nSitemapReource->getSitemapBean->");
		final Sitemap sitemap = getSitemap(sitemapname);
		if(sitemap!=null) {
			return createSitemapBean(sitemapname, sitemap, uri);
		} else {
			logger.info("Received HTTP GET request at '{}' for the unknown sitemap '{}'.", uriInfo.getPath(), sitemapname);
			throw new WebApplicationException(404);
		}
	}

	private SitemapBean createSitemapBean(String sitemapName, Sitemap sitemap, URI uri) {
    	final SitemapBean bean = new SitemapBean();
		
    	bean.name = sitemapName;
		bean.icon = sitemap.getIcon();
		bean.label = sitemap.getLabel();

    	bean.link = UriBuilder.fromUri(uri).path(SitemapResource.PATH_SITEMAPS).path(bean.name).build().toASCIIString();
    	bean.homepage = createPageBean(sitemap.getName(), sitemap.getLabel(), sitemap.getIcon(), sitemap.getName(), sitemap.getChildren(), true, false, uri);
    	System.out.println("\nSitemapReource->createSitemapBean->"+bean.homepage);
    	return bean;
    }
    
    static private PageBean createPageBean(String sitemapName, String title, String icon, String pageId, EList<Widget> children, boolean drillDown, boolean isLeaf, URI uri) {
    	PageBean bean = new PageBean();
    	bean.id = pageId;
    	bean.title = title;
    	bean.icon = icon;
		bean.leaf = isLeaf;
    	bean.link = UriBuilder.fromUri(uri).path(PATH_SITEMAPS).path(sitemapName).path(pageId).build().toASCIIString();
    	if(children!=null) {
    		int cntWidget = 0;
	    	for(Widget widget : children) {
	    		String widgetId = pageId + "_" + cntWidget;
	    		WidgetBean subWidget = createWidgetBean(sitemapName, widget, drillDown, uri, widgetId);
				if(subWidget != null)
	    		bean.widgets.add(subWidget);
	    		cntWidget++;
	    	}
    	} else {
    		bean.widgets = null;
    	}
    	System.out.println("\nSitemapReource->createSitemapBean->"+bean.toString());
		return bean;
	}

	static private WidgetBean createWidgetBean(String sitemapName, Widget widget, boolean drillDown, URI uri, String widgetId) {
		ItemUIRegistry itemUIRegistry = null;
		try{
			itemUIRegistry = CloudHelperUtil.retrieveItemUIRegistry("demo");
			
		} catch (CloudException e){
			e.printStackTrace();
		}
		// Test visibility
		if(itemUIRegistry.getVisiblity(widget) == false)
			return null;

    	WidgetBean bean = new WidgetBean();
    	if(widget.getItem()!=null) {
    		Item item = ItemResource.getItem(widget.getItem());
        	if(item!=null) {
        		bean.item = ItemResource.createItemBean(item, false, UriBuilder.fromUri(uri).build().toASCIIString());
        	}
    	}
    	bean.widgetId = widgetId;
    	bean.icon = itemUIRegistry.getIcon(widget);
		bean.labelcolor = itemUIRegistry.getLabelColor(widget);
		bean.valuecolor = itemUIRegistry.getValueColor(widget);
    	bean.label = itemUIRegistry.getLabel(widget);
    	bean.type = widget.eClass().getName();
    	if (widget instanceof LinkableWidget) {
			LinkableWidget linkableWidget = (LinkableWidget) widget;
			EList<Widget> children = itemUIRegistry.getChildren(linkableWidget);
    		if(widget instanceof Frame) {
    			int cntWidget=0;
    			for(Widget child : children) {
    				widgetId += "_" + cntWidget;
					WidgetBean subWidget = createWidgetBean(sitemapName, child, drillDown, uri, widgetId);
					if(subWidget != null) {
						bean.widgets.add(subWidget);
    	    		cntWidget++;
    			}
				}
    		} else if(children.size()>0)  {
				String pageName = itemUIRegistry.getWidgetId(linkableWidget);
				bean.linkedPage = createPageBean(sitemapName, itemUIRegistry.getLabel(widget), itemUIRegistry.getIcon(widget), pageName, 
						drillDown ? children : null, drillDown, isLeaf(children), uri);
    		}
		}
    	if(widget instanceof Switch) {
    		Switch switchWidget = (Switch) widget;
    		for(Mapping mapping : switchWidget.getMappings()) {
    			MappingBean mappingBean = new MappingBean();
				// Remove quotes - if they exist
				if(mapping.getCmd() != null) {
					if(mapping.getCmd().startsWith("\"") && mapping.getCmd().endsWith("\"")) {
						mappingBean.command = mapping.getCmd().substring(1, mapping.getCmd().length()-1);
					}
					else {
						mappingBean.command = mapping.getCmd();
					}
				}
				else {
					mappingBean.command = mapping.getCmd();
				}
				mappingBean.label = mapping.getLabel();
				bean.mappings.add(mappingBean);
			}
		}
		if (widget instanceof Selection) {
			Selection selectionWidget = (Selection) widget;
			for (Mapping mapping : selectionWidget.getMappings()) {
				MappingBean mappingBean = new MappingBean();
				// Remove quotes - if they exist
				if(mapping.getCmd() != null) {
					if(mapping.getCmd().startsWith("\"") && mapping.getCmd().endsWith("\"")) {
						mappingBean.command = mapping.getCmd().substring(1, mapping.getCmd().length()-1);
					}
					else {
						mappingBean.command = mapping.getCmd();
					}				
				}
				else {
					mappingBean.command = mapping.getCmd();
				}
    			mappingBean.label = mapping.getLabel();
    			bean.mappings.add(mappingBean);
    		}
    	}
    	if(widget instanceof Slider) {
    		Slider sliderWidget = (Slider) widget;
    		bean.sendFrequency = sliderWidget.getFrequency();
    		bean.switchSupport = sliderWidget.isSwitchEnabled();
    	}
    	if(widget instanceof List) {
    		List listWidget = (List) widget;
    		bean.separator = listWidget.getSeparator();
    	}
    	if (widget instanceof Image ||
    		widget instanceof Video ||
    		widget instanceof Webview) {

        	if(widget instanceof Image) {
        		Image imageWidget = (Image) widget;
        		if(imageWidget.getRefresh() > 0) {
        			bean.refresh = imageWidget.getRefresh(); 
        		}
        		bean.url = imageWidget.getUrl();
        	}
        	else if (widget instanceof Video) {
        		Video videoWidget = (Video) widget;
        		if(videoWidget.getEncoding() != null) {
        			bean.encoding = videoWidget.getEncoding();
        		}
        		bean.url = videoWidget.getUrl();
        	}
        	else {
				Webview webViewWidget = (Webview) widget;
				bean.height = webViewWidget.getHeight();
				bean.url = webViewWidget.getUrl();
        	}

			String wId = itemUIRegistry.getWidgetId(widget);

			StringBuilder sbBaseUrl = new StringBuilder();
			sbBaseUrl.append(uri.getScheme()).append("://").append(uri.getHost());
			if (uri.getPort() >= 0 && uri.getPort() != 80) {
				sbBaseUrl.append(":").append(uri.getPort());
			}
			StringBuilder sb = new StringBuilder();
			sb.append("/proxy?");
			sb.append("sitemap=").append(sitemapName).append(".sitemap&");
			sb.append("widgetId=").append(wId);
			if (bean.url != null && bean.url.startsWith("/")) {
	        	try {
	        		sb.append("&").append("baseUrl=").append(URLEncoder.encode(sbBaseUrl.toString(), "UTF-8"));
	        	}
				catch (UnsupportedEncodingException ex) {
					throw new RuntimeException(ex.getMessage(), ex);
				}
			}
			sbBaseUrl.append(sb.toString());
			bean.url = sbBaseUrl.toString();
    	}
    	if(widget instanceof Chart) {
    		Chart chartWidget = (Chart) widget;
    		bean.service = chartWidget.getService();
    		bean.period = chartWidget.getPeriod();
    		if(chartWidget.getRefresh()>0) {
    			bean.refresh = chartWidget.getRefresh(); 
    		}
    	}
    	if(widget instanceof Setpoint) {
    		Setpoint setpointWidget = (Setpoint) widget;
    		bean.minValue = setpointWidget.getMinValue();
    		bean.maxValue = setpointWidget.getMaxValue();
    		bean.step = setpointWidget.getStep();
    	}
    	System.out.println("\nSitemapReource->createWidgetBean->"+bean.toString());
		return bean;
	}

	private static boolean isLeaf(EList<Widget> children) {
		for(Widget w : children) {
			if(w instanceof Frame) {
				if(isLeaf(((Frame) w).getChildren())) {
					return false;
				}
			} else if(w instanceof LinkableWidget) {
				LinkableWidget linkableWidget = (LinkableWidget) w;
				ItemUIRegistry itemUIRegistry = null;
				try{
					itemUIRegistry = CloudHelperUtil.retrieveItemUIRegistry("demo");
				} catch (CloudException e){
					e.printStackTrace();
				}
						//RESTApplication.getItemUIRegistry();
				if(itemUIRegistry.getChildren(linkableWidget).size() > 0) {
					return false;
				}
			}
		}
		return true;
	}

	static public Sitemap getSitemap(String sitemapname) {
        ModelRepository repo =	null;
        //sitemapname	=	"demo";
        try{
        	repo =		CloudHelperUtil.retrieveModelRepRegistry(sitemapname);
        } catch (CloudException e){
        	e.printStackTrace();
        }
        		//RESTApplication.getModelRepository();
        if(repo!=null) {
			Sitemap sitemap = (Sitemap) repo.getModel(sitemapname + SITEMAP_FILEEXT);
			return sitemap;
        }
        return null;
    }

	@GET @Path("/{sitemapname: [a-zA-Z_0-9]*}/{pageid: [a-zA-Z_0-9]*}")
	@Produces( { MediaType.WILDCARD })
	//public SuspendResponse<Response> getPageData(
	public Response getPageData(
	@Context HttpHeaders headers,
	@PathParam("sitemapname") String sitemapname,
	@PathParam("pageid") String pageId,
	@QueryParam("type") String type, 
	@QueryParam("jsoncallback") @DefaultValue("callback") String callback){
	//@Context AtmosphereResource resource) {
	logger.debug("Received HTTP GET request at '{}' for media type '{}'.", uriInfo.getPath(), type);	
	System.out.println("\nSitemapReource->getPageData->siteMapname->"+sitemapname+"->pageId->"+pageId+"-type->"+type+"->callback->"+callback);
			
	//if(TRANSPORT.UNDEFINED.equals(resource.transport())) {
	final String responseType = MediaTypeHelper.getResponseMediaType(headers.getAcceptableMediaTypes(), type);
	Object responseObject = null;
	if(responseType!=null) {
			final PageBean content = getPageBean(sitemapname, pageId, uriInfo.getBaseUriBuilder().build());
			responseObject = ResponseHelper.wrapContentIfNeccessary(callback, responseType, content);
			System.out.println("\nSitemapReource->getPageData->responseObject->"+responseObject.toString());
	    }
	
	Response r	=	Response.ok(responseObject, responseType).build();
	System.out.println("\nSitemapReource->getPageData->r->"+r);
		return Response.ok(responseObject, responseType).build();
	}

    static public PageBean getPageBean(String sitemapName, String pageId, URI uri) {
		//ItemUIRegistry itemUIRegistry = RESTApplication.getItemUIRegistry();
    	System.out.println("\nSitemapReource->getPageBean->siteMap->"+sitemapName);
    	ItemUIRegistry itemUIRegistry = null;
    	try{
    		itemUIRegistry = CloudHelperUtil.retrieveItemUIRegistry(sitemapName);
    	} catch (CloudException e){
    		e.printStackTrace();
    	}
		
		Sitemap sitemap = getSitemap(sitemapName);
		if(sitemap!=null) {
			if(pageId.equals(sitemap.getName())) {
				return createPageBean(sitemapName, sitemap.getLabel(), sitemap.getIcon(), sitemap.getName(), sitemap.getChildren(), false, isLeaf(sitemap.getChildren()), uri);
			} else {
			Widget pageWidget = itemUIRegistry.getWidget(sitemap, pageId);
				if(pageWidget instanceof LinkableWidget) {
				EList<Widget> children = itemUIRegistry.getChildren((LinkableWidget) pageWidget);
				PageBean pageBean = createPageBean(sitemapName, itemUIRegistry.getLabel(pageWidget), itemUIRegistry.getIcon(pageWidget), 
							pageId, children, false, isLeaf(children), uri);
				EObject parentPage = pageWidget.eContainer();
					while(parentPage instanceof Frame) {
						parentPage = parentPage.eContainer();
					}
					if(parentPage instanceof Widget) {
					String parentId = itemUIRegistry.getWidgetId((Widget) parentPage);
						pageBean.parent = getPageBean(sitemapName, parentId, uri);
					pageBean.parent.widgets = null;
					pageBean.parent.parent = null;
				} else if(parentPage instanceof Sitemap) {
						pageBean.parent = getPageBean(sitemapName, sitemap.getName(), uri);
						pageBean.parent.widgets = null;
					}
					System.out.println("\nSitemapReource->getPageBean->"+pageBean.toString());
				return pageBean;
			} else {
					if(logger.isDebugEnabled()) {
						if(pageWidget==null) {
		    			logger.debug("Received HTTP GET request at '{}' for the unknown page id '{}'.", uri, pageId);
						} else {
			    			logger.debug("Received HTTP GET request at '{}' for the page id '{}'. " + 
		    					"This id refers to a non-linkable widget and is therefore no valid page id.", uri, pageId);
					}
					}
	    		throw new WebApplicationException(404);
			}
		}
	} else {
		logger.info("Received HTTP GET request at '{}' for the unknown sitemap '{}'.", uri, sitemapName);
		throw new WebApplicationException(404);
	}
    }
}
