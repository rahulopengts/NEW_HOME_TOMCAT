package com.sample;

public class Test {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String msgFilter = "^[0-9]*$";
		String msg	=	"20s";
		boolean b	=	msg.matches(msgFilter);
		System.out.println("\n -> Result->"+b);
	}

}
