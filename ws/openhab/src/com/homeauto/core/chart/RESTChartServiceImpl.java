package com.homeauto.core.chart;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.openhab.ui.webapp.cloud.exception.CloudException;

import com.homeauto.core.chart.dto.TempChartDTO;

//http://stackoverflow.com/questions/21706191/populate-chartjs-ajax-servlet

@Path(IChartService.PATH_CHARTSERVICE)
public class RESTChartServiceImpl {

	
	@GET
	@Path("/graph")
	@Produces(MediaType.APPLICATION_JSON)
	public TempChartDTO getTempDataJSON(@Context HttpHeaders headers,
    		@QueryParam("sitename") String sitemapname, 
    		@QueryParam("nodeid") String nodeId, 
    		@QueryParam("type") String type,
    		@QueryParam("startDate") String startDate,
    		@QueryParam("endDate") String endDate) {
			TempChartDTO	temp	=	null;	
			System.out.println("\nRESTChartServiceImpl->getTrackInJSON");
			try{
			//TempChartDTO	temp	=	new TempChartDTO();
//			temp.setTitle("RAHUL");
//			String data	=	"January"+","+"February"+","+"March"+","+"April"+","+"May"+","+"June"+","+"July";
//			temp.setX_axis_data(data);
			
			IChartService chartService	=	getChartService(type);
			temp	=	(TempChartDTO)chartService.getChartResponse(sitemapname, nodeId, startDate, endDate);
			} catch (CloudException e){
				e.printStackTrace();
			}
			return temp;
	}
	
	@GET @Path("/{test}")
	@Produces(MediaType.APPLICATION_JSON)
	public String gatGraph(@Context HttpHeaders headers,
    		@QueryParam("sitename") String sitemapname, 
    		@QueryParam("nodeid") String nodeId, 
    		@QueryParam("type") String type,
    		@QueryParam("startDate") String startDate,
    		@QueryParam("endDate") String endDate) {
		
		IChartService chartService	=	getChartService(type);
		try{
			String output = "POST:Jersey say : ";
			String responseType	=	"application/json";
			return "{data1:data2}";//Response.ok(output, responseType).build();
		} catch (Exception e){
			e.printStackTrace();
		}
		return "Failed";
	  }

	public IChartService getChartService(String type){
		IChartService	service	=	null;
		if("temp".equals(type)){
			service	=	new TempChartService();
		}
		return service;
	}

}
