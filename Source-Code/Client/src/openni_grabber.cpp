#include "ofMain.h"
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/openni_grabber.h>
#include <pcl/io/openni_camera/openni_device_oni.h>
#include <pcl/common/time.h>
#include <pcl/visualization/cloud_viewer.h>
#include <iostream>
#include <pcl/io/pcd_io.h>

#include <pcl/io/openni_grabber.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/statistical_outlier_removal.h>

 class SimpleOpenNIViewer {
   public:
     SimpleOpenNIViewer () : viewer ("PCL OpenNI Viewer") {}

     void cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr &cloud) {
       if (!viewer.wasStopped()) {
			//pcl::PointCloud<pcl::PointXYZRGBA> cloud_filtered;
			//pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZRGBA>);
			//pcl::VoxelGrid<pcl::PointXYZRGBA> sor; 
			//pcl::StatisticalOutlierRemoval<pcl::PointXYZRGBA> sor2;
			//sor2.setInputCloud (cloud);
			////sor.setLeafSize (0.01f, 0.01f, 0.01f);
			//sor2.setMeanK (25);
			//sor2.setStddevMulThresh (1.0);
			//sor2.filter (*cloud_filtered);

			viewer.showCloud (cloud);
	   }
     }

     void run () {
       //pcl::Grabber* interface = new pcl::OpenNIGrabber("onivideo_2.oni");
	   //pcl::Grabber* interface = new pcl::OpenNIGrabber("test.oni");
	   pcl::Grabber * interface = new pcl::OpenNIGrabber("#1");

       boost::function<void (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr&)> f = boost::bind (&SimpleOpenNIViewer::cloud_cb_, this, _1);

       interface->registerCallback (f);

       interface->start ();

       while (!viewer.wasStopped()) {
         boost::this_thread::sleep (boost::posix_time::seconds (0.1));
       }

       interface->stop ();
	   
     }

     pcl::visualization::CloudViewer viewer;
 };

 int main () {
   SimpleOpenNIViewer v;
   v.run ();
   return 0;
 }