package com.openhab.core.rest;

import javax.ws.rs.DefaultValue;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.openhab.io.rest.internal.resources.ResponseHelper;
import org.openhab.io.rest.internal.resources.beans.SitemapListBean;


	@Path("/hello")
	public class TestRestService {
		
		@Context UriInfo uriInfo;
		@GET
		@Path("/{param}")
		@Produces( { MediaType.WILDCARD })
		public Response getMsg(@PathParam("param") String msg,    		
				@Context HttpHeaders headers,
	    		@QueryParam("type") String type, 
	    		@QueryParam("jsoncallback") @DefaultValue("callback") String callback) {
	 
			String responseType	=	"application/xml";
			callback	=	"callback";
			
			System.out.println("\ngetMsg->"+msg);
			String output = "Jersey say : " + msg+"->URIINFO->";
			final SitemapListBean content = new SitemapListBean();
			
			final Object responseObject = ResponseHelper.wrapContentIfNeccessary(callback, responseType, content);

			return Response.ok(responseObject, responseType).build();
			//return Response.status(200).entity(output).build();
	 
		}
	 
	}