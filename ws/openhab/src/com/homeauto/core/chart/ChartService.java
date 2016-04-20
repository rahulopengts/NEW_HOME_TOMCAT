package com.homeauto.core.chart;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;

import org.apache.commons.io.IOUtils;
import org.openhab.ui.webapp.cloud.exception.CloudException;
import org.openhab.ui.webapp.cloud.exception.CloudExceptionManager;
import org.openhab.ui.webapp.cloud.exception.CloudMessageConstants;
import org.openhab.ui.webapp.render.RenderException;

public abstract class ChartService implements IChartService {

	private String templateType	=	null;
	
	public String getTemplateType() {
		return templateType;
	}

	public void setTemplateType(String templateType) {
		this.templateType = templateType;
	}

	protected synchronized String getSnippet(String elementType) throws CloudException {
		elementType = elementType.toLowerCase();
		String snippet = null;
		String snippetLocation = TEMPLATE_LOCATION+"\\"+getTemplateType()+".html";
		System.out.println("\nAbstractWidgetRenderer->getAnippets->snippetLocation->"+getTemplateType());
		//URL entry = WebAppActivator.getContext().getBundle().getEntry(snippetLocation);
		//snippetLocation = "D:\\Home_Auto\\NEW_HOME_TOMCAT\\ws\\openhab\\WebContent\\"+getTemplateType()+".html";
		//String location	=	"";
		File file = null;
		URL fileUrl = null;
		try{	
			file = new File(snippetLocation);

			fileUrl = file.toURI().toURL();
			System.out.println("URI:" + fileUrl);			
		} catch (MalformedURLException e){
			e.printStackTrace();
		}
		
		//entry	=	fileUri;
		//if(entry!=null) {
		if(fileUrl!=null) {
			try {
				//snippet = IOUtils.toString(entry.openStream());
				snippet = IOUtils.toString(fileUrl.openStream());
			} catch (IOException e) {
				CloudExceptionManager.throwException(CloudMessageConstants.CHART_TEMPLATE_NOT_FOUND,e, null);
			}
		} else {
			CloudExceptionManager.throwException(CloudMessageConstants.CHART_TEMPLATE_NOT_FOUND,null, null);
		}
		return snippet;
	}
 

}
