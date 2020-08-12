#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <thread>

#include "match_image.h"

int detect_num_logical_processors();
void find_matches(cv::Mat image_scaled, cv::Mat templ, double *templ_scales, int i0, int i1, double *match_values, cv::Rect *match_rects);

double match_image(const char *ref_image_file_name, cv::Rect templ_rect, const char *target_image_file_name, cv::Rect &match_rect, bool show_image) {
    int64 tick0 = cv::getTickCount();

    cv::Mat image_ref = imread(ref_image_file_name, cv::IMREAD_COLOR);
    if( image_ref.empty() ) {
        std::cout << "match_image: cannot open template_image_file_name : " << ref_image_file_name << std::endl;
        return 0;
    }

    cv::Mat image = imread(target_image_file_name, cv::IMREAD_COLOR);
    if( image.empty() ) {
        std::cout << "match_image: cannot open target_image_file_name : " << target_image_file_name << std::endl;
        return 0;
    }

    if( templ_rect.x < 0 || templ_rect.x >= image_ref.cols ||
        templ_rect.y < 0 || templ_rect.y >= image_ref.rows ||
        templ_rect.x + templ_rect.width - 1 < 0 || templ_rect.x + templ_rect.width - 1 >= image_ref.cols ||
        templ_rect.y + templ_rect.height - 1 < 0 || templ_rect.y + templ_rect.height - 1 >= image_ref.rows ||
        templ_rect.width <= 0 || templ_rect.height <= 0 ) {
        std::cout << "match_image: template rect is out of range" << std::endl;
        return 0;
    }

    cv::Mat templ = image_ref( templ_rect );
    float image_scale;
    cv::Mat image_scaled;

    if( std::min( image.rows, image.cols ) > 1000 ) {
        image_scale = 0.5;
        cv::resize( image, image_scaled, cv::Size( 0, 0 ), image_scale, image_scale, cv::INTER_AREA );
    } else {
        image_scale = 1;
        image.copyTo( image_scaled );
    }

    double rough_templ_scale = image_scale * std::min( image.rows, image.cols ) / std::min( image_ref.rows, image_ref.cols );
    std::vector<double> templ_scales;

    for( double templ_scale = rough_templ_scale * 0.76; templ_scale < rough_templ_scale * 1.2; templ_scale *= 1.04 ) {
        templ_scales.push_back( templ_scale );
    }

    std::vector<double> match_values( templ_scales.size() );
    std::vector<cv::Rect> match_rects( templ_scales.size() );

    int nThreads = detect_num_logical_processors();
    std::cout << "Detect Logical Processors : " << nThreads << std::endl;

    std::vector<std::thread> threads( nThreads );
    int div = (int)templ_scales.size() / nThreads;
    int rem = (int)templ_scales.size() % nThreads;

    for( int i0 = 0, task_size, i = 0; i < nThreads; ++i, i0 += task_size ) {
        task_size = ( i < rem ) ? ( div + 1 ) : div;
        threads[i] = std::thread( find_matches, image_scaled, templ, templ_scales.data(), i0, i0 + task_size, match_values.data(), match_rects.data() );
    }

    for( auto &thread:threads ) {
        thread.join();
    }

    double match_val = -DBL_MAX;

    for( int i = 0; i < (int)templ_scales.size(); ++i ) {
        if( match_values[i] > match_val ) {
            match_val = match_values[i];
            match_rect = match_rects[i];
        }
    }

    int image_scale_back = (int)( 1 / image_scale + 0.5 );

    match_rect.x *= image_scale_back;
    match_rect.y *= image_scale_back;
    match_rect.width *= image_scale_back;
    match_rect.height *= image_scale_back;

    int64 tick1 = cv::getTickCount();
    std::cout << "Computation time = " << (double)( tick1 - tick0 ) / cv::getTickFrequency() << std::endl;

    if( show_image ) {
        std::cout << "match_rect x = " << match_rect.x << ", y = " << match_rect.y << ", width = " << match_rect.width << ", height = " << match_rect.height << std::endl;
        std::cout << "match_score = " << match_val << std::endl;

        cv::rectangle( image, match_rect, CV_RGB( 255, 0, 0 ), 2 );
        cv::Mat image_result;
        double scale = image.rows > 1000 ? 1000. / image.rows : 1;
        cv::resize( image, image_result, cv::Size( 0, 0 ), scale, scale, cv::INTER_AREA );
        cv::imshow( "image_result", image_result );
        cv::waitKey();
    }

/*
    double result_val = -DBL_MAX;
    cv::Mat image_result;
    for( int i = 0, j = 0; i < (int)templ_scales.size(); ++i ) {
        cv::Rect rect = match_rects[i];

        if( match_values[i] > result_val ) {
            result_val = match_values[i];

            std::cout << "match_rect x = " << rect.x << ", y = " << rect.y << ", width = " << rect.width << ", height = " << rect.height << std::endl;
            std::cout << "match_score = " << result_val << std::endl;

            cv::rectangle( image, rect, CV_RGB( 255, 0, 0 ), 2 );
            double scale = image.rows > 1000 ? 1000. / image.rows : 1;
            cv::resize( image, image_result, cv::Size( 0, 0 ), scale, scale, cv::INTER_AREA );
        }
    }

    if( !image_result.empty() )
        imwrite( "./result.png", image_result );

    std::cout << "return value = " << match_val << std::endl;
    */

    return match_val;
}

int detect_num_logical_processors() {
    int numCPU = std::thread::hardware_concurrency();

    return numCPU;
}

void find_matches(cv::Mat image_scaled, cv::Mat templ, double *templ_scales, int i0, int i1, double *match_values, cv::Rect *match_rects) {
    for( int i = 0; i < i1; ++i ) {
        cv::Mat templ_scaled;
        int interpolation = templ_scales[i] < 1 ? cv::INTER_AREA : cv::INTER_LINEAR;
        cv::resize( templ, templ_scaled, cv::Size( 0, 0 ), templ_scales[i], templ_scales[i], interpolation );

        int score_rows = image_scaled.rows - templ_scaled.rows + 1;
        int score_cols = image_scaled.cols - templ_scaled.cols + 1;

        if( score_rows > 0 && score_cols > 0 ) {
            cv::Mat score( image_scaled.rows - templ_scaled.rows + 1, image_scaled.cols - templ_scaled.cols + 1, CV_32FC1 );

            cv::matchTemplate( image_scaled, templ_scaled, score, cv::TM_CCOEFF_NORMED );

            double minVal;
            double maxVal;
            cv::Point minLoc;
            cv::Point maxLoc;

            cv::minMaxLoc( score, &minVal, &maxVal, &minLoc, &maxLoc );

            match_values[i] = maxVal;
            match_rects[i] = cv::Rect( maxLoc.x, maxLoc.y, templ_scaled.cols, templ_scaled.rows );
        } else {
            match_values[i] = 0;
        }
    }
}