package com.openhab.core.db.vo;

import java.sql.Date;

public class SiteInfoVO extends AbstractVO {

	  private int SITE_ID	=	-1;
	  private String	USER_SITE_ID	=	null;
		public String getUSER_SITE_ID() {
		return USER_SITE_ID;
	}
	public void setUSER_SITE_ID(String uSER_SITE_ID) {
		USER_SITE_ID = uSER_SITE_ID;
	}
		private String	F_NAME	=	null;
	      private String	L_NAME	=	null;
	      private String	USER_GRP_ID	=	null;
	      private String	GENDER	=	null;
	      private Date	REGDATE	=	null;
	      private String	PHONE	=	null;
	      private String	DESCRIPTION	=	null;
	      private int INITIALIZED	=	0;
	      
	      
	  public int getINITIALIZED() {
			return INITIALIZED;
		}
		public void setINITIALIZED(int iNITIALIZED) {
			INITIALIZED = iNITIALIZED;
		}
	public int getSITE_ID() {
		return SITE_ID;
	}
	public void setSITE_ID(int sITE_ID) {
		SITE_ID = sITE_ID;
	}
	public String getF_NAME() {
		return F_NAME;
	}
	public void setF_NAME(String f_NAME) {
		F_NAME = f_NAME;
	}
	public String getL_NAME() {
		return L_NAME;
	}
	public void setL_NAME(String l_NAME) {
		L_NAME = l_NAME;
	}
	public String getUSER_GRP_ID() {
		return USER_GRP_ID;
	}
	public void setUSER_GRP_ID(String uSER_GRP_ID) {
		USER_GRP_ID = uSER_GRP_ID;
	}
	public String getGENDER() {
		return GENDER;
	}
	public void setGENDER(String gENDER) {
		GENDER = gENDER;
	}
	public Date getREGDATE() {
		return REGDATE;
	}
	public void setREGDATE(Date rEGDATE) {
		REGDATE = rEGDATE;
	}
	public String getPHONE() {
		return PHONE;
	}
	public void setPHONE(String pHONE) {
		PHONE = pHONE;
	}
	public String getDESCRIPTION() {
		return DESCRIPTION;
	}
	public void setDESCRIPTION(String dESCRIPTION) {
		DESCRIPTION = dESCRIPTION;
	}

}
