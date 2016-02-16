package org.openhab.core.transform;

import org.openhab.core.transform.internal.service.JAVATransformationService;
import org.osgi.framework.BundleContext;

import com.openhab.core.constants.CloudAppConstants;

public class CloudTransformationHelper extends TransformationHelper {

	//~I90199991001119292#
	
	public final static String DIMMERTYPEITEM	=	"DI";
	public final static String SWITCHTYPEITEM	=	"SI";
	
	public final static String CONTACTTYPEITEM	=	"CI";
	
	static public TransformationService getTransformationService(String transformationType) {
		TransformationService	transformationService	=	null;
		if(transformationType!=null && transformationType.equals(CloudAppConstants.JAVA_TRANSFORMATION)){
			transformationService	=	new JAVATransformationService();
		}
		
		return transformationService;
	}


}
