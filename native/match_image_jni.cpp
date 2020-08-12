#include <iostream>
#include <opencv2/core/core.hpp>

#include "match_image_jni.h"
#include "match_image.h"

JNIEXPORT jdouble JNICALL Java_com_onycom_ImageMatchingJNI_Main_matchImage(
    JNIEnv *env, 
    jclass, 
    jstring templateImageFileName, 
    jobject templateRect, 
    jstring targetImageFileName, 
    jobject matchRect, 
    jboolean showImage)
{
    char *template_image_file_name = (char*) env->GetStringUTFChars(templateImageFileName, 0);
    char *target_image_file_name = (char*) env->GetStringUTFChars(targetImageFileName, 0);

    jclass templRectClass = env->GetObjectClass(templateRect);

    jfieldID templ_xID = env->GetFieldID(templRectClass, "x", "I");
    jfieldID templ_yID = env->GetFieldID(templRectClass, "y", "I");
    jfieldID templ_wID = env->GetFieldID(templRectClass, "width", "I");
    jfieldID templ_hID = env->GetFieldID(templRectClass, "height", "I");

    cv::Rect templ_rect( env->GetIntField(templateRect, templ_xID),
                        env->GetIntField(templateRect, templ_yID),
                        env->GetIntField(templateRect, templ_wID),
                        env->GetIntField(templateRect, templ_hID) );
    
    cv::Rect match_rect;

    bool show_image = showImage == 0 ? false : true;
    double match_score = match_image(template_image_file_name, templ_rect, target_image_file_name, match_rect, show_image);

    jclass matchRectClass = env->GetObjectClass(matchRect);

    jfieldID match_xID = env->GetFieldID(matchRectClass, "x", "I");
    jfieldID match_yID = env->GetFieldID(matchRectClass, "y", "I");
    jfieldID match_wID = env->GetFieldID(matchRectClass, "width", "I");
    jfieldID match_hID = env->GetFieldID(matchRectClass, "height", "I");

    env->SetIntField(matchRect, match_xID, match_rect.x);
    env->SetIntField(matchRect, match_yID, match_rect.y);
    env->SetIntField(matchRect, match_wID, match_rect.width);
    env->SetIntField(matchRect, match_hID, match_rect.height);

    env->ReleaseStringUTFChars(templateImageFileName, template_image_file_name);
    env->ReleaseStringUTFChars(targetImageFileName, target_image_file_name);

    return match_score;
}
