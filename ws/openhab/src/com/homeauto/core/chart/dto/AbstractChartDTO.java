package com.homeauto.core.chart.dto;

import com.homeauto.core.chart.IChartService;

public class AbstractChartDTO implements IChartDTO {

	String title	=	null;
	
	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	String x_axis_data	=	null;

	public String getX_axis_data() {
		return x_axis_data;
	}

	public void setX_axis_data(String x_axis_data) {
		this.x_axis_data = x_axis_data;
	}

	public String getY_axis_data() {
		return y_axis_data;
	}

	public void setY_axis_data(String y_axis_data) {
		this.y_axis_data = y_axis_data;
	}

	String y_axis_data	=	null;

}
