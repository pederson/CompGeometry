#ifndef _POINTCLOUD_H
#define _POINTCLOUD_H

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>

// this is a placeholder for the PointCloud class

// it should hold the following elements (at least):
//  - x,y,z,intensity
//  - return number
//  - classification
//  - extra fields, extra field names
//  - projection (UTM, ECEF, lat/lon) (choose one to always work in... I'm thinking ECEF)
//
// and it should have the following functions (at least):
//  - read LAS file
//  - write LAS file
//  - read ascii delimited x,y,z,i,c
//  - write ascii delimited x,y,z,i,c
//  - print a summary of data
//  - recalculate the x,y,z extents
//  - conversion to/from ECEF/latlon/UTM


// forward declaration
struct rgb48;


class PointCloud{
public:

  PointCloud();                       // ctor
  PointCloud(unsigned int numpts);    // ctor
  PointCloud(const PointCloud & cloud)  // copy ctor
  : _x(cloud._x),
    _y(cloud._y),
    _z(cloud._z),
    _gpstime(cloud._gpstime),
    _intensity(cloud._intensity),
    _classification(cloud._classification),
    _RGB(cloud._RGB),
    _extradata_names(cloud._extradata_names),
    _extradata(cloud._extradata)
    {_xmin = cloud._xmin; _xmax = cloud._xmax;
     _ymin = cloud._ymin; _ymax = cloud._ymax;
     _zmin = cloud._zmin; _zmax = cloud._zmax;
     _gpst_min = cloud._gpst_min; _gpst_max = cloud._gpst_max;}
  ~PointCloud();                      // dtor

  // operators
  PointCloud & operator=(const PointCloud & cloud);
  PointCloud & operator+=(const PointCloud & cloud);

  // terminal output
  void print_summary() const;
  void print_detailed() const;

  // metadata inspectors
  unsigned int pointcount() const {return _x.size();};
  double xmax() const {return _xmax;};
  double xmin() const {return _xmin;};
  double ymax() const {return _ymax;};
  double ymin() const {return _ymin;};
  double zmax() const {return _zmax;};
  double zmin() const {return _zmin;};
  bool gpstime_present() const {if (_gpstime.size()>0) return true; else return false;};
  bool intensity_present() const {if (_intensity.size()>0) return true; else return false;};
  bool classification_present() const {if (_classification.size()>0) return true; else return false;};
  bool RGB_present() const {if (_RGB.size()>0) return true; else return false;};
  bool extradata_present(std::string fieldname) const {for (auto i=0; i<_extradata.size(); i++) {if (_extradata_names.at(i).compare(fieldname)==0) return true;} return false;};


  // main data accessors
  const double & x() const {return _x.front();};
  const double & y() const {return _y.front();};
  const double & z() const {return _z.front();};

  // optional member data accessors
  const double & gpstime() const {return _gpstime.front();};
  const unsigned short & intensity() const {return _intensity.front();};
  const unsigned char & classification() const {return _classification.front();};
  const rgb48 & RGB() const {return _RGB.front();};

  // user-defined member data accessors
  const double & data(std::string field) const {return _extradata.at(field).front();};

  // mutators
  PointCloud subset(const bool & keep);
  PointCloud subset(const unsigned int & keep_inds, const unsigned int keep_count);

  static PointCloud read_LAS(std::string filename, unsigned int byte_offset=0);
  void write_LAS(std::string filename);


protected:

private:
  // metadata
  double _xmin, _xmax, _ymin, _ymax, _zmin, _zmax, _gpst_min, _gpst_max;
  unsigned int _pointcount;

  // required data
  std::vector<double> _x, _y, _z;

  // optional data
  std::vector<double> _gpstime;
  std::vector<unsigned short> _intensity;
  std::vector<unsigned char> _classification;
  std::vector<rgb48> _RGB;

  // user-defined data
  std::vector<std::string> _extradata_names;
  std::map<std::string, std::vector<double>> _extradata;

  // initializing optional data fields
  void add_intensity();
  void add_classification();
  void add_gpstime();
  void add_RGB();  
  void add_extradata(std::string fieldname);

  void calc_extents();  

  void read_LAS_internal(std::string filename, unsigned int byte_offset=0);

};


#pragma pack(push,1)
struct rgb48{
  unsigned short R;
  unsigned short G;
  unsigned short B;
};

struct las_pt_0{
  int X;
  int Y;
  int Z;
  unsigned short Intensity;
  unsigned char Return_Info;
  unsigned char Classification;
  char Scan_Angle_Rank;
  unsigned char User_Data;
  unsigned short Point_Source_ID;
};

struct las_pt_1{
  int X;
  int Y;
  int Z;
  unsigned short Intensity;
  unsigned char Return_Info;
  unsigned char Classification;
  char Scan_Angle_Rank;
  unsigned char User_Data;
  unsigned short Point_Source_ID;
  double GPSTime;
};

struct las_pt_2{
  int X;
  int Y;
  int Z;
  unsigned short Intensity;
  unsigned char Return_Info;
  unsigned char Classification;
  char Scan_Angle_Rank;
  unsigned char User_Data;
  unsigned short Point_Source_ID;
  unsigned short Red;
  unsigned short Green;
  unsigned short Blue;
};

struct las_pt_3{
  int X;
  int Y;
  int Z;
  unsigned short Intensity;
  unsigned char Return_Info;
  unsigned char Classification;
  char Scan_Angle_Rank;
  unsigned char User_Data;
  unsigned short Point_Source_ID;
  double GPSTime;
  unsigned short Red;
  unsigned short Green;
  unsigned short Blue;
};

struct las_pt_4{
  int X;
  int Y;
  int Z;
  unsigned short Intensity;
  unsigned char Return_Info;
  unsigned char Classification;
  char Scan_Angle_Rank;
  unsigned char User_Data;
  unsigned short Point_Source_ID;
  double GPSTime;
  unsigned char WaveP_Desc_Idx;
  unsigned long WaveP_Byte_Offset;
  unsigned int WaveP_Size;
  float WaveF_Location;
  float X_t;
  float Y_t;
  float Z_t;
};

struct las_pt_5{
  int X;
  int Y;
  int Z;
  unsigned short Intensity;
  unsigned char Return_Info;
  unsigned char Classification;
  char Scan_Angle_Rank;
  unsigned char User_Data;
  unsigned short Point_Source_ID;
  double GPSTime;
  unsigned short Red;
  unsigned short Green;
  unsigned short Blue;
  unsigned char WaveP_Desc_Idx;
  unsigned long WaveP_Byte_Offset;
  unsigned int WaveP_Size;
  float WaveF_Location;
  float X_t;
  float Y_t;
  float Z_t;
};
#pragma pack(pop)

#endif
