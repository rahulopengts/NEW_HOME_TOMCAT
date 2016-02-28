package com.openhab.core.rest;

//public class CloudRESTItemResourceService {
import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;

import javax.ws.rs.Consumes;
import javax.ws.rs.DefaultValue;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.WebApplicationException;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.Status;
import javax.ws.rs.core.UriBuilder;
import javax.ws.rs.core.UriInfo;

import org.openhab.core.items.GroupItem;
import org.openhab.core.items.Item;
import org.openhab.core.items.ItemNotFoundException;
import org.openhab.core.library.items.RollershutterItem;
import org.openhab.core.library.items.SwitchItem;
import org.openhab.core.library.types.OnOffType;
import org.openhab.core.library.types.UpDownType;
import org.openhab.core.types.Command;
import org.openhab.core.types.State;
import org.openhab.core.types.TypeParser;
import org.openhab.io.rest.RESTApplication;


import org.openhab.io.rest.internal.resources.ResponseHelper;
import org.openhab.io.rest.internal.resources.beans.GroupItemBean;
import org.openhab.io.rest.internal.resources.beans.ItemBean;
import org.openhab.io.rest.internal.resources.beans.ItemListBean;
import org.openhab.ui.items.ItemUIRegistry;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.openhab.core.util.CloudHelperUtil;

/**
 * <p>This class acts as a REST resource for items and provides different methods to interact with them,
 * like retrieving lists of items, sending commands to them or checking a single status.</p>
 * 
 * <p>The typical content types are plain text for status values and XML or JSON(P) for more complex data
 * structures</p>
 * 
 * <p>This resource is registered with the Jersey servlet.</p>
 *
 * @author Kai Kreuzer
 * @since 0.8.0
 */
@Path(CloudRESTItemResourceService.PATH_ITEMS)
public class CloudRESTItemResourceService {

	private static final Logger logger = LoggerFactory.getLogger(CloudRESTItemResourceService.class); 
	
	/** The URI path to this resource */
    public static final String PATH_ITEMS = "items";
    
	@Context UriInfo uriInfo;
	@GET
    @Produces( { MediaType.WILDCARD })
    public Response getItems(
    		@Context HttpHeaders headers,
    		@QueryParam("type") String type, 
    		@QueryParam("jsoncallback") @DefaultValue("callback") String callback) {
		System.out.println("\nItemReource->getItems->");
		String responseType	=	"application/xml";
		if (logger.isDebugEnabled()) logger.debug("Received HTTP GET request at '{}' for media type '{}'.", uriInfo.getPath(), type);
		//final String responseType = MediaTypeHelper.getResponseMediaType(headers.getAcceptableMediaTypes(), type);
		if(responseType!=null) {
			final ItemListBean content = new ItemListBean(getItemBeans());
	    	final Object responseObject = ResponseHelper.wrapContentIfNeccessary(callback, responseType, content); 
	    	return Response.ok(responseObject, responseType).build();
		} else {
			return Response.notAcceptable(null).build();
		}
    }

    @GET @Path("/{itemname: [a-zA-Z_0-9]*}/state") 
	@Produces( { MediaType.TEXT_PLAIN })
    public String getPlainItemState(
    		@PathParam("itemname") String itemname ){ 
    		
    	System.out.println("\nItemReource->getPageItemState->");
    	return null;
    }

    @GET @Path("/{itemname: [a-zA-Z_0-9]*}")
    @Produces( { MediaType.WILDCARD })
    public Response  getItemData(
    		@Context HttpHeaders headers,
    		@PathParam("itemname") String itemname, 
    		@QueryParam("type") String type, 
    		@QueryParam("jsoncallback") @DefaultValue("callback") String callback ){
    		
    	System.out.println("\nItemReource->getItemData->");
		if (logger.isDebugEnabled()) logger.debug("Received HTTP GET request at '{}' for media type '{}'.", uriInfo.getPath(), type);
		return null;
    }
    
    @PUT @Path("/{itemname: [a-zA-Z_0-9]*}/state")
	@Consumes(MediaType.TEXT_PLAIN)	
	public Response putItemState(@PathParam("itemname") String itemname, String value) {
    	System.out.println("\nItemReource->putItemState->");
    	final Item item = getItem(itemname);
    	if(item!=null) {
    		final State state = TypeParser.parseState(item.getAcceptedDataTypes(), value);
    		if(state!=null) {
    			if (logger.isDebugEnabled()) logger.debug("Received HTTP PUT request at '{}' with value '{}'.", uriInfo.getPath(), value);
    			RESTApplication.getEventPublisher().postUpdate(itemname, state);
    			return Response.ok().build();
    		} else {
    			if (logger.isDebugEnabled()) logger.warn("Received HTTP PUT request at '{}' with an invalid status value '{}'.", uriInfo.getPath(), value);
    			return Response.status(Status.BAD_REQUEST).build();
    		}
    	} else {
    		logger.info("Received HTTP PUT request at '{}' for the unknown item '{}'.", uriInfo.getPath(), itemname);
    		throw new WebApplicationException(404);
    	}
	}

	@Context UriInfo localUriInfo;
    @POST @Path("/{itemname: [a-zA-Z_0-9]*}")
	@Consumes(MediaType.TEXT_PLAIN)	
	public Response postItemCommand(@PathParam("itemname") String itemname, String value) {
    	System.out.println("\nItemReource->postItemsResource->");
    	final Item item = getItem(itemname);
    	Command command = null;
    	if(item!=null) {
    		// support for TOGGLE, see https://code.google.com/p/openhab/issues/detail?id=336
    		if("toggle".equalsIgnoreCase(value) && 
    				(item instanceof SwitchItem || 
    				 item instanceof RollershutterItem)) {
    			if(OnOffType.ON.equals(item.getStateAs(OnOffType.class))) command = OnOffType.OFF;
    			if(OnOffType.OFF.equals(item.getStateAs(OnOffType.class))) command = OnOffType.ON;
    			if(UpDownType.UP.equals(item.getStateAs(UpDownType.class))) command = UpDownType.DOWN;
    			if(UpDownType.DOWN.equals(item.getStateAs(UpDownType.class))) command = UpDownType.UP;
    		} else {
    			command = TypeParser.parseCommand(item.getAcceptedCommandTypes(), value);
    		}
    		if(command!=null) {
    			logger.debug("Received HTTP POST request at '{}' with value '{}'.", uriInfo.getPath(), value);
    			RESTApplication.getEventPublisher().postCommand(itemname, command);
    			return Response.created(localUriInfo.getAbsolutePathBuilder().path("state").build()).build();
    		} else {
    			logger.warn("Received HTTP POST request at '{}' with an invalid status value '{}'.", uriInfo.getPath(), value);
    			return Response.status(Status.BAD_REQUEST).build();
    		}
    	} else {
    		logger.info("Received HTTP POST request at '{}' for the unknown item '{}'.", uriInfo.getPath(), itemname);
    		throw new WebApplicationException(404);
    	}
	}

    public static ItemBean createItemBean(Item item, boolean drillDown, String uriPath) {
    	ItemBean bean;
    	if(item instanceof GroupItem && drillDown) {
    		GroupItem groupItem = (GroupItem) item;
    		GroupItemBean groupBean = new GroupItemBean();
    		Collection<ItemBean> members = new HashSet<ItemBean>();
    		for(Item member : groupItem.getMembers()) {
    			members.add(createItemBean(member, false, uriPath));
    		}
    		groupBean.members = members.toArray(new ItemBean[members.size()]);
    		bean = groupBean;
    	} else {
    		 bean = new ItemBean();
    	}
    	bean.name = item.getName();
    	bean.state = item.getState().toString();
    	bean.type = item.getClass().getSimpleName();
    	bean.link = UriBuilder.fromUri(uriPath).path(CloudRESTItemResourceService.PATH_ITEMS).path(bean.name).build().toASCIIString();
    	
    	return bean;
    }
    
    static public Item getItem(String itemname) {
        ItemUIRegistry registry = null;//RESTApplication.getItemUIRegistry();
        try{
        	registry = CloudHelperUtil.retrieveItemUIRegistry("demo");
        } catch (CloudException e){
        	e.printStackTrace();
        }
        if(registry!=null) {
        	try {
        		
				Item item = registry.getItem(itemname);
				return item;
			} catch (ItemNotFoundException e) {
				logger.debug(e.getMessage());
			}
        }
        return null;
    }

	private List<ItemBean> getItemBeans() {
		List<ItemBean> beans = new LinkedList<ItemBean>();
		ItemUIRegistry registry = null;//RESTApplication.getItemUIRegistry();
        try{
        	registry = CloudHelperUtil.retrieveItemUIRegistry("demo");
        } catch (CloudException e){
        	e.printStackTrace();
        }

		for(Item item : registry.getItems()) {
			beans.add(createItemBean(item, false, uriInfo.getBaseUri().toASCIIString()));
		}
		return beans;
	}

	private ItemBean getItemDataBean(String itemname) {
		final Item item = getItem(itemname);
		if(item!=null) {
			return createItemBean(item, true, uriInfo.getBaseUri().toASCIIString());
		} else {
			logger.info("Received HTTP GET request at '{}' for the unknown item '{}'.", uriInfo.getPath(), itemname);
			throw new WebApplicationException(404);
		}
	}
}
