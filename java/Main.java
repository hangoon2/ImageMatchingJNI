package com.onycom.ImageMatchingJNI;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class Main
{
	static
	{
		System.loadLibrary("ImageMatching");
	}

	public static native double matchImage( String template_image_file_name, Object template_rect,
										    String target_image_file_name,   Object match_rect, boolean show_image );

//    public static void main(String[] args)
//    {
//		String template_image_file_name = "Images/Template/s_17.jpg";
//		String template_roi_file_name   = "Images/Template/s_17_roi.txt";
//		String target_image_file_name   = "Images/Target/z_17.jpg";

//		Scanner scanner = null;
//		try {
//			scanner = new Scanner(new File( template_roi_file_name ));
//		} catch (FileNotFoundException e) {
//			e.printStackTrace();
//		}

//		int x0 = scanner.nextInt();
//		int y0 = scanner.nextInt();
//		int x1 = scanner.nextInt();
//		int y1 = scanner.nextInt();
//		int margin = 10;

//		Rect template_rect = new Rect( x0 - margin, y0 - margin, x1 - x0 + 1 + margin*2, y1 - y0 + 1 + margin*2 );

//		Rect match_rect = new Rect();

//		double match_score = matchImage( template_image_file_name, template_rect, target_image_file_name, match_rect, true );

//		System.out.format( "match_rect = (%d, %d, %d, %d)\n", match_rect.x, match_rect.y, match_rect.width, match_rect.height );
//		System.out.format( "match_score = %f\n", match_score );
//    }
}
