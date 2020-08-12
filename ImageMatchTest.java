package com.onycom.test;

import java.awt.Rectangle;

import com.onycom.ImageMatchingJNI.Main;

public class ImageMatchTest {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Rectangle templRect = new Rectangle(60, 558, 45, 25);
		Rectangle rect = new Rectangle();
		double ret = Main.matchImage("ref_image.png", templRect, "sample.png", rect, false);
		
		System.out.println("Result: " + ret + ", RECT(" + rect.x + ", " + rect.y +
				", " + rect.width + ", " + rect.height + ")");
	}

}
