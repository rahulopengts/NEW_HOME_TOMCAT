package com.homeauto.core.chart;

import javax.ws.rs.core.Response;

import org.apache.commons.lang.StringUtils;
import org.openhab.ui.webapp.cloud.exception.CloudException;

import com.homeauto.core.chart.dto.TempChartDTO;

public class TempChartService extends ChartService {

	public TempChartService(){
		setTemplateType("temp");
	}
	
	@Override
	public TempChartDTO getChartResponse(String siteName, String nodeId,
			String startDate, String endDate) throws CloudException {
		TempChartDTO	temp	=	null;
		try{
			temp	=	new TempChartDTO();
			temp.setTitle("RAHUL");
			String data	=	"January"+","+"February"+","+"March"+","+"April"+","+"May"+","+"June"+","+"July";
			temp.setX_axis_data(data);
			
			String dataTemp	=	"65"+","+"59"+","+"90"+","+"81"+","+"56"+","+"55"+","+"40"+","+"80";
			temp.setY_axis_data(dataTemp);
		} catch (Exception e){
			throw e;
		}
		return temp;
	}

}
