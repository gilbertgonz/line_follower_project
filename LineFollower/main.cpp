#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#define KEY_ESC 27

using Pixel = cv::Vec3b; // B,G,R

uchar gray(Pixel p) {
    return 0.3*p[2] + 0.59*p[1] + 0.11*p[0];
}

void plotRow(cv::Mat &image, std::vector<uchar> row_colors, int y, float scale, cv::Scalar color) {
    std::vector<cv::Point> points;
    for (int x = 0; x < row_colors.size(); ++x)
        points.push_back( cv::Point(x, y - scale*row_colors[x]) );
    cv::polylines(image, points, false, color, 2);
}

int main(int argc, char** argv) {
    // Load image from command line arguments
    std::string filename;
    if (argc >= 2)
        filename = argv[1];
    else
        filename = "../Images/line.mp4";

    if (filename.find(".mp4") != std::string::npos || filename.find(".avi") != std::string::npos) { 
        // Load video from file
        cv::VideoCapture cap(filename);
        if (!cap.isOpened() || cap.get(cv::CAP_PROP_FRAME_COUNT) == 0) {
            std::cout << "Video '" << filename << "' not found or contains no frames!" << std::endl;
            return -1;
        }

        // Create windows with trackbar
        cv::namedWindow("EML4840");
        // Trackers
        int track_row = 70;     // Percentage
        int track_scale = 40;   // Percentage
        int track_resize = 100;
        cv::createTrackbar("Row", "EML4840", &track_row, 100);
        cv::createTrackbar("Scale", "EML4840", &track_scale, 100);
        cv::createTrackbar("Resize", "EML4840", &track_resize, 100);
        // Menu
        bool show_red = true;
        bool show_blue = true;
        bool show_green = true;
        bool show_gray = true;
        std::cout << "Press:" << std::endl
                << "s            : to save image" << std::endl
                << "r, g, b, or k: to show colors" << std::endl
                << "q or ESC     : to quit" << std::endl;
        // Create vectors to store the graphs
        std::vector<uchar>graph_red, graph_green, graph_blue, graph_gray;
        // Loop through frames
        cv::Mat frame;
        while (cap.read(frame)) {
            // Resize frame
            cv::resize(frame, frame, cv::Size(), track_resize/100.0, track_resize/100.0);
            // Split image into color channels
            std::vector<cv::Mat> bgr_channels;
            cv::split(frame, bgr_channels);
            // Clear graphs
            graph_red.clear();
            graph_green.clear();
            graph_blue.clear();
            graph_gray.clear();
            // Fill graphs
            for (int x = 0; x < frame.cols; ++x) {
                uchar b = bgr_channels[0].at<uchar>(cv::Point(x, track_row/100.0*frame.rows));
                uchar g = bgr_channels[1].at<uchar>(cv::Point(x, track_row/100.0*frame.rows));
                uchar r = bgr_channels[2].at<uchar>(cv::Point(x, track_row/100.0*frame.rows));
                Pixel pixel{b, g, r};
                graph_red.push_back(pixel[2]);
                graph_green.push_back(pixel[1]);
                graph_blue.push_back(pixel[0]);
                graph_gray.push_back(gray(pixel));
            }
            // Plot graphs
            cv::Mat plot = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
            if (show_red)
                plotRow(plot, graph_red, plot.rows, track_scale/100.0, cv::Scalar(0, 0, 255));
            if (show_green)
                plotRow(plot, graph_green, plot.rows, track_scale/100.0, cv::Scalar(0, 255, 0));
            if (show_blue)
                plotRow(plot, graph_blue, plot.rows, track_scale/100.0, cv::Scalar(255, 0, 0));
            if (show_gray)
                plotRow(plot, graph_gray, plot.rows, track_scale/100.0, cv::Scalar(255, 255, 255));
            // Display image
            cv::imshow("EML4840", plot);
            // Handle keyboard input
            char key = cv::waitKey(1);
            if (key == 's') {
                std::string filename = "plot_" + std::to_string(cap.get(cv::CAP_PROP_POS_FRAMES)) + ".jpg";
                cv::imwrite(filename, plot);
                std::cout << "Saved plot to " << filename << std::endl;
            } else if (key == 'r') {
                show_red = !show_red;
            } else if (key == 'g') {
                show_green = !show_green;
            } else if (key == 'b') {
                show_blue = !show_blue;
            } else if (key == 'k') {
                show_gray = !show_gray;
            } else if (key == 'q' || key == KEY_ESC) {
                break;
            }
        }
            // Release video capture and destroy windows
            cap.release();
            cv::destroyAllWindows();
    }
    else {
        // Load image from file
        const cv::Mat image = cv::imread(filename);
        if (image.empty()) {
            std::cout << "Image '" << filename << "' not found!" << std::endl;
            return -1;
        }
        // Check the if image type is compatible with CV_8UC3
        std::string type = cv::typeToString( image.type() );
        std::cout << type << std::endl
                << image.rows << "x" << image.cols
                << std::endl;
        if (type != "CV_8UC3") {
            std::cout << "Ops, format '" << type << "' not supported!" << std::endl;
            return 1;
        }
        // Show image
        // cv::imshow("EML4840", image);
        // Create windows with trackbar
        cv::namedWindow("EML4840");
        // Trackers
        int track_row = 70;     // Percentage
        int track_scale = 40;   // Percentage
        int track_resize = 100;
        cv::createTrackbar("Row", "EML4840", &track_row, 100);
        cv::createTrackbar("Scale", "EML4840", &track_scale, 100);
        cv::createTrackbar("resize", "EML4840", &track_resize, 100);
        // Menu
        bool show_red = true;
        bool show_blue = true;
        bool show_green = true;
        bool show_gray = true;
        std::cout << "Press:" << std::endl
                << "s            : to save image" << std::endl
                << "r, g, b, or k: to show colors" << std::endl
                << "q or ESC     : to quit" << std::endl;
        // Create vectors to store the graphs
        std::vector<uchar> r, g, b, k;
        // Run until 'q' is pressed...
        bool running = true;
        while( running ) {
            // Clear colors
            r.clear();
            g.clear();
            b.clear();
            k.clear();
            // Update scale
            float scale = 0.01*track_scale;
            // Pixel scanine
            int y = 0.01*track_row*(image.rows-1);
            for (int x = 0; x < image.cols; ++x ) {
                Pixel pixel = image.at<Pixel>( cv::Point(x, y) );
                r.push_back( pixel[2] );
                g.push_back( pixel[1] );
                b.push_back( pixel[0] );
                k.push_back( gray(pixel) );
            }
            // clone image and keep the orginal for processing!
            cv::Mat canvas = image.clone();
            if (show_red)   plotRow(canvas, r, y, scale, cv::Scalar(0,0,255));
            if (show_green) plotRow(canvas, g, y, scale, cv::Scalar(0,255,0));
            if (show_blue)  plotRow(canvas, b, y, scale, cv::Scalar(255,0,0));
            if (show_gray)  plotRow(canvas, k, y, scale, cv::Scalar(0,0,0));
            cv::line(canvas, cv::Point(0, y), cv::Point(image.cols, y), cv::Scalar(0,0,0), 2);
            // Menu
            int key = cv::waitKey(10);
            switch(key) {
            case 's':
                cv::imwrite("../output.jpg", image);
                break;
            case 'q':
            case KEY_ESC:
                running = false;
                break;
            case 'r':
                show_red = !show_red;
                break;
            case 'g':
                show_green = !show_green;
                break;
            case 'b':
                show_blue = !show_blue;
                break;
            case 'k':
                show_gray = !show_gray;
                break;
            }
            // Show image
            cv::resize(canvas, canvas, cv::Size(), 0.01*track_resize, 0.01*track_resize);
            cv::imshow("EML4840", canvas);
        }
    }

    return 0;
}
