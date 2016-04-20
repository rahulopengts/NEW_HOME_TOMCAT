package com.homeauto.core.chart;

import javax.ws.rs.core.Response;

import org.openhab.ui.webapp.cloud.exception.CloudException;

import com.homeauto.core.chart.dto.IChartDTO;

public interface IChartService {

	public static final String PATH_CHARTSERVICE	=	"chart";

	public static final String TEMPLATE_LOCATION	=	"D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\openhab\\WebContent\\charttemplate";
	public IChartDTO	getChartResponse(String siteName,String nodeId,String startDate,String endDate) throws CloudException;
	
}
