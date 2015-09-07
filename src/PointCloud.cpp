// this is a placeholder for the PointCloud class
//#define _TEST_
// it should hold the following elements (at least):
//  - x,y,z,intensity
//  - return number
//  - classification
//  - extra fields, extra field names
//  - projection (UTM, ECEF, lat/lon) (choose one to always work in... I'm thinking ECEF)
//  - pointcount

// and it should have the following functions (at least):
//  - read LAS file (DONE)
//  - write LAS file
//  - read ascii delimited x,y,z,i,c
//  - write ascii delimited x,y,z,i,c
//  - print a summary of data (DONE)
//  - recalculate the x,y,z extents (DONE)
//  - conversion to/from ECEF/latlon/UTM
//  - create/destroy data vectors (DONE)
#include "PointCloud.hpp"

using namespace std;

PointCloud::PointCloud(){
  _xmin = 0; _xmax = 0; _ymin = 0; _ymax = 0; _zmin = 0; _zmax = 0;
  _gpst_min = 0; _gpst_max = 0;
}

PointCloud::PointCloud(unsigned int numpts){
  _x.resize(numpts, 0.0);
  _y.resize(numpts, 0.0);
  _z.resize(numpts, 0.0);

  _xmin = 0; _xmax = 0; _ymin = 0; _ymax = 0; _zmin = 0; _zmax = 0;
  _gpst_min = 0; _gpst_max = 0;
}

PointCloud::~PointCloud(){

}

PointCloud & PointCloud::operator=(const PointCloud & cloud){

  if (this == &cloud) return *this;

  _x = cloud._x;
  _y = cloud._y;
  _z = cloud._z;
  _gpstime = cloud._gpstime;
  _intensity = cloud._intensity;
  _classification = cloud._classification;
  _RGB = cloud._RGB;
  _extradata_names = cloud._extradata_names;
  _extradata = cloud._extradata;

  _xmin = cloud._xmin; _xmax = cloud._xmax;
   _ymin = cloud._ymin; _ymax = cloud._ymax;
   _zmin = cloud._zmin; _zmax = cloud._zmax;
   _gpst_min = cloud._gpst_min; _gpst_max = cloud._gpst_max;

  return *this;
}

PointCloud & PointCloud::operator+=(const PointCloud & cloud){
  // copy all the matching data and increment the pointcount
  _x.insert(_x.end(), cloud._x.begin(), cloud._x.end());
  _y.insert(_y.end(), cloud._y.begin(), cloud._y.end());
  _z.insert(_z.end(), cloud._z.begin(), cloud._z.end());
  if (cloud.gpstime_present() && gpstime_present()) _gpstime.insert(_gpstime.end(), cloud._gpstime.begin(), cloud._gpstime.end());
  else cout << "WARNING: gpstime data is being lost through operator+=" << endl;
  if (cloud.intensity_present() && intensity_present()) _intensity.insert(_intensity.end(), cloud._intensity.begin(), cloud._intensity.end());
  else cout << "WARNING: intensity data is being lost through operator+=" << endl;
  if (cloud.classification_present() && classification_present()) _classification.insert(_classification.end(), cloud._classification.begin(), cloud._classification.end());
  else cout << "WARNING: classification data is being lost through operator+=" << endl;
  if (cloud.RGB_present() && RGB_present()) _RGB.insert(_RGB.end(), cloud._RGB.begin(), cloud._RGB.end());
  else cout << "WARNING: RGB data is being lost through operator+=" << endl;
  for (auto i=0; i<cloud._extradata_names.size(); i++){
    if (extradata_present(cloud._extradata_names.at(i))) _extradata.at(_extradata_names.at(i)).insert(_extradata.at(_extradata_names.at(i)).begin(), 
                        cloud._extradata.at(_extradata_names.at(i)).begin(), cloud._extradata.at(_extradata_names.at(i)).end());
    else cout << "WARNING: extradata " << cloud._extradata_names.at(i) << " data is being lost through operator+=" << endl;
  }

  calc_extents();
  return *this;
}

void PointCloud::print_summary() const{
  cout << " " << endl;
  cout << "********** Point Cloud Summary **********" << endl;
  cout << "  pointcount: " << pointcount() << endl;
  cout << "  existing fields: x" << endl;
  cout << "                   y" << endl;
  cout << "                   z" << endl;
  if (gpstime_present()) cout << "                   gpstime" << endl;
  if (intensity_present()) cout << "                   intensity" << endl;
  if (classification_present()) cout << "                   classification" << endl;
  if (RGB_present()) cout << "                   RGB" << endl;
  if (_extradata.size() > 0){
    for (unsigned int i=0; i<_extradata.size(); i++){
      cout << "                   " << _extradata_names.at(i) << endl;
    }
  }
  cout << "  data extents:" << endl;
  cout << "       x:[" << _xmin << ", \t" << _xmax << "]" << endl;
  cout << "       y:[" << _ymin << ", \t" << _ymax << "]" << endl;
  cout << "       z:[" << _zmin << ", \t" << _zmax << "]" << endl;
  if (gpstime_present()) cout << "       gpstime:[" << _gpst_min << ", \t" << _gpst_max << "]" << endl;
  cout << "******************************************" << endl;
  cout << " " << endl;

  return;
}

void PointCloud::print_detailed() const{
  cout << " " << endl;
  cout << "********** Point Cloud Details **********" << endl;
  cout << "  pointcount: " << pointcount() << endl;
  cout << "  existing fields: x" << endl;
  cout << "                   y" << endl;
  cout << "                   z" << endl;
  if (gpstime_present()) cout << "                   gpstime" << endl;
  if (intensity_present()) cout << "                   intensity" << endl;
  if (classification_present()) cout << "                   classification" << endl;
  if (RGB_present()) cout << "                   RGB" << endl;
  if (_extradata.size() > 0){
    for (unsigned int i=0; i<_extradata.size(); i++){
      cout << "                   " << _extradata_names.at(i) << endl;
    }
  }
  cout << "  data extents:" << endl;
  cout << "       x:[" << _xmin << ", \t" << _xmax << "] \tRange: " << _xmax-_xmin << endl;
  cout << "       y:[" << _ymin << ", \t" << _ymax << "] \tRange: " << _ymax-_ymin << endl;
  cout << "       z:[" << _zmin << ", \t" << _zmax << "] \tRange: " << _zmax-_zmin << endl;
  if (gpstime_present()) cout << "       gpstime:[" << _gpst_min << ", \t" << _gpst_max << "]   \t\t\tRange: " << _gpst_max-_gpst_min << endl;
  cout << "******************************************" << endl;
  cout << " " << endl;

  return;
}

void PointCloud::calc_extents(){
  if (pointcount()==0) return;

  _xmin = *min_element(_x.begin(), _x.end());
  _xmax = *max_element(_x.begin(), _x.end());
  _ymin = *min_element(_y.begin(), _y.end());
  _ymax = *max_element(_y.begin(), _y.end());
  _zmin = *min_element(_z.begin(), _z.end());
  _zmax = *max_element(_z.begin(), _z.end());
  if (gpstime_present()) {
    _gpst_min = *min_element(_gpstime.begin(), _gpstime.end());
    _gpst_max = *max_element(_gpstime.begin(), _gpstime.end());
  }

  return;
}

void PointCloud::add_intensity(){
  /*if (intensity != NULL) cout << "intensity already exists!" << endl;
  else intensity = new unsigned short[_pointcount];
  */
  if (intensity_present()) return;
  _intensity.resize(pointcount(), 0);
}

void PointCloud::add_classification(){
  /*if (classification != NULL) cout << "classification already exists!" << endl;
  else classification = new unsigned char[_pointcount];
  */
  if (classification_present()) return;
  _classification.resize(pointcount(), 0);
}

void PointCloud::add_gpstime(){
  /*
  if (gpstime != NULL) cout << "gpstime already exists!" << endl;
  else gpstime = new double[_pointcount];
  */
  if (gpstime_present()) return;
  _gpstime.resize(pointcount(), 0.0);
}

void PointCloud::add_RGB(){
  /*
  if (RGB != NULL) cout << "RGB already exists!" << endl;
  else RGB = new rgb48[_pointcount];
  */
  if (RGB_present()) return;
  _RGB.resize(pointcount(), {0,0,0});
}

void PointCloud::add_extradata(std::string fieldname){
  /*extra_data[fieldname] = new double[_pointcount];
  extra_data_names.push_back(fieldname);
  return;
  */
  if (extradata_present(fieldname)){
    cout << fieldname << " is already present in the extra data!" << endl;
    return;
  }
  _extradata[fieldname] = {};
  _extradata[fieldname].resize(pointcount(), 0.0);
}



PointCloud PointCloud::read_LAS(string filename, unsigned int byte_offset){
  PointCloud cloud = PointCloud();
  cloud.read_LAS_internal(filename, byte_offset);
  return cloud;
}

void PointCloud::read_LAS_internal(string filename, unsigned int byte_offset){
  // define vars
  bool fieldexist=false;
  char signature[4];
  unsigned char ver_major, ver_minor, point_format_id, dummy_1;
  unsigned short header_size, point_record_bytes, dummy_2;
  unsigned int point_offset, dummy_4;
  unsigned int pt_count;
  double x_scale, y_scale, z_scale, x_offset, y_offset, z_offset;
  double x_min, y_min, z_min, x_max, y_max, z_max, dummy_8;
  int fd;
  char * lasmap;
  size_t res;

  // open the file
  FILE * fid;
  fid = fopen(filename.c_str(), "rb");

  // do a size check
  fseek(fid, 0L, SEEK_END);
  size_t sz = ftell(fid);
  fseek(fid, 0L, SEEK_SET);
  //cout << "the file is " << sz << " bytes long" << endl;
  
  // move to byte_offset
  fseek(fid, byte_offset, SEEK_SET);

  // check LASF signature
  res = fread(&signature, 4, 1, fid); // "LASF" signature

  //cout << "signature: " << signature[0] << signature[1] << signature[2] << signature[3] << endl;

  // collect header info
  res = fread(&dummy_2, 2, 1, fid);     // File Source ID
  res = fread(&dummy_2, 2, 1, fid);     // Global Encoding
  res = fread(&dummy_4, 4, 1, fid);     // Project ID - GUID data 1
  res = fread(&dummy_2, 2, 1, fid);     // Project ID - GUID data 2
  res = fread(&dummy_2, 2, 1, fid);     // Project ID - GUID data 3
  res = fread(&dummy_8, 8, 1, fid);     // Project ID - GUID data 4
  res = fread(&ver_major, 1, 1, fid);   // Version Major
  res = fread(&ver_minor, 1, 1, fid);   // Version Minor
  for (int i=0; i<32; i++) res = fread(&dummy_1, 1, 1, fid); // System Identifier
  for (int i=0; i<32; i++) res = fread(&dummy_1, 1, 1, fid); // Generating Software
  res = fread(&dummy_2, 2, 1, fid);     // File Creation Day of Year
  res = fread(&dummy_2, 2, 1, fid);     // File Creation Year
  res = fread(&header_size, 2, 1, fid); // Header Size
  res = fread(&point_offset, 4, 1, fid);// Offset to Point Data
  res = fread(&dummy_4, 4, 1, fid);     // Number of Variable Length Records
  res = fread(&point_format_id, 1, 1, fid); // Point Data Format ID
  res = fread(&point_record_bytes, 2, 1, fid);  // Point Data Record Length
  res = fread(&pt_count, 4, 1, fid);    // Number of point records
  for (int i=0; i<5; i++) res = fread(&dummy_4, 4, 1, fid); // Number of points by return
  if (ver_minor > 2) res = fread(&dummy_8, 8, 1, fid); // extra points by return 
  res = fread(&x_scale, 8, 1, fid);     // X Scale Factor
  res = fread(&y_scale, 8, 1, fid);     // Y Scale Factor
  res = fread(&z_scale, 8, 1, fid);     // Z Scale Factor
  res = fread(&x_offset, 8, 1, fid);    // X Offset
  res = fread(&y_offset, 8, 1, fid);    // Y Offset
  res = fread(&z_offset, 8, 1, fid);    // Z Offset
  res = fread(&x_max, 8, 1, fid);       // X Max
  res = fread(&x_min, 8, 1, fid);       // X Min
  res = fread(&y_max, 8, 1, fid);       // Y Max
  res = fread(&y_min, 8, 1, fid);       // Y Min
  res = fread(&z_max, 8, 1, fid);       // Z Max
  res = fread(&z_min, 8, 1, fid);       // Z Min

  // verify that the number of points match up
  //cout << "we have mapped" << point_offset + point_record_bytes*pt_count << " bytes" << endl;
  //cout << "the file should have " << (sz-point_offset)/point_record_bytes << " points" << endl;
  unsigned int realsize = (sz-point_offset - byte_offset)/point_record_bytes;
  if (pt_count > realsize){
    cout << "WARNING: byte count doesn't match up with reported point count" << endl;
    cout << "WARNING: proceeding with calculated byte count" << endl;
    pt_count = realsize;
  }

  // read projection info from Variable Length Records (VLRs)

  // close the file
  fclose(fid);

  // check the version number and alert the user if it is not fully supported
  if (ver_minor > 2 || ver_major > 1){
    cout << "Some features of LAS version " << ver_major << "." << ver_minor ;
    cout << " may not be supported" << endl;
  }

  // check the record format number and alert the user if it is not supported
  if (point_format_id > 3){
    cout << "Some features of point format " << point_format_id ;
    cout << " may not be supported" << endl;
  }
  if (point_format_id > 5){
    cout << "Point format " << point_format_id << " is not supported" << endl;
    throw -1;
  }

  /*
  cout << "LAS version " << int(ver_major) << "." << int(ver_minor) << endl;
  cout << "point format id: " << int(point_format_id) << endl;
  cout << "bytes in point record: " << point_record_bytes << endl;
  cout << "pointcount: " << pt_count << endl;
  cout << "point offset: " << point_offset << endl;
  cout << "xoffset: " << x_offset << " yoffset:" << y_offset << " zoffset:" << z_offset << endl;
  cout << "xscale: " << x_scale << " yscale:" << y_scale << " zscale:" << z_scale << endl;
  cout << "xmax: " << x_max << " xmin: " << x_min << endl;
  cout << "ymax: " << y_max << " ymin: " << y_min << endl;
  cout << "zmax: " << z_max << " zmin: " << z_min << endl;
  */

  // memory map the point data
  fd = open(filename.c_str(), O_RDONLY);
  if (fd < 0){
    cout << "Error opening file in readLas" << endl;
    throw -1;
  }
  lseek(fd, byte_offset, SEEK_SET);
  lasmap = (char *)mmap(NULL, point_offset + point_record_bytes*pt_count, PROT_READ, MAP_PRIVATE, fd, 0);
  if (lasmap == MAP_FAILED){
    cout << "Failed to map las file" << endl;
    throw -1;
  }  

  // initialize some stuff
  _x.resize(pt_count, 0.0);
  _y.resize(pt_count, 0.0);
  _z.resize(pt_count, 0.0);
  add_intensity();
  add_classification();
  _xmin = x_min;
  _xmax = x_max;
  _ymin = y_min;
  _ymax = y_max;
  _zmin = z_min;
  _zmax = z_max;


  // do a switch for point record format and memcpy the points
  switch (point_format_id)
  {
    case 0:
      // initialize array of structs
      las_pt_0 *laspts0;
      laspts0 = new las_pt_0[pt_count];

      //cout << "about to memcpy " << sizeof(las_pt_0)*pt_count << " " << " " << point_record_bytes*pt_count << endl;

      // copy from memory map into array of structs
      memcpy(laspts0, &lasmap[point_offset], point_record_bytes*pt_count);

      //cout << "about to extract points" << endl;
      // loop over structs to extract the points
      for (unsigned int i=0; i<pt_count; i++){
        _x[i] = double(laspts0[i].X)*x_scale + x_offset;    // X
        _y[i] = double(laspts0[i].Y)*y_scale + y_offset;    // Y
        _z[i] = double(laspts0[i].Z)*z_scale + z_offset;    // Z
        _intensity[i] = laspts0[i].Intensity;     // Intensity
        _classification[i] = laspts0[i].Classification;   // Classification
      }

      delete[] laspts0;
      //cout << "successfully deleted" << endl;
      break;

    case 1:
      // initialize array of structs
      las_pt_1 *laspts1;
      laspts1 = new las_pt_1[pt_count];

      // copy from memory map into array of structs
      memcpy(laspts1, &lasmap[point_offset-1], point_record_bytes*pt_count);

      add_gpstime();
      // loop over structs to extract the points
      for (unsigned int i=0; i<pt_count; i++){
        _x[i] = double(laspts1[i].X)*x_scale + x_offset;    // X
        _y[i] = double(laspts1[i].Y)*y_scale + y_offset;    // Y
        _z[i] = double(laspts1[i].Z)*z_scale + z_offset;    // Z
        _intensity[i] = laspts1[i].Intensity;                // Intensity
        _classification[i] = laspts1[i].Classification;      // Classification
        _gpstime[i] = laspts1[i].GPSTime;
      }
      _gpst_min = _gpstime[0];
      _gpst_max = _gpstime[pt_count-1];

      delete[] laspts1;
      break;

    case 2:
      // initialize array of structs
      las_pt_2 *laspts2;
      laspts2 = new las_pt_2[pt_count];

      //cout << "about to memcpy" << sizeof(las_pt_2)*pt_count << " " << " " << point_record_bytes*pt_count << endl;
      // copy from memory map into array of structs
      memcpy(laspts2, &lasmap[point_offset], point_record_bytes*pt_count);

      //cout << "about to copy stuff" << endl;
      add_RGB();
      // loop over structs to extract the points
      for (unsigned int i=0; i<pt_count; i++){
        _x[i] = double(laspts2[i].X)*x_scale + x_offset;    // X
        _y[i] = double(laspts2[i].Y)*y_scale + y_offset;    // Y
        _z[i] = double(laspts2[i].Z)*z_scale + z_offset;    // Z
        _intensity[i] = laspts2[i].Intensity;                // Intensity
        _classification[i] = laspts2[i].Classification;      // Classification
        _RGB[i].R = laspts2[i].Red;
        _RGB[i].G = laspts2[i].Green;
        _RGB[i].B = laspts2[i].Blue;
      }

      delete[] laspts2;
      break;

    case 3:
      // initialize array of structs
      las_pt_3 *laspts3;
      laspts3 = new las_pt_3[pt_count];

      // copy from memory map into array of structs
      memcpy(laspts3, &lasmap[point_offset-1], point_record_bytes*pt_count);

      add_gpstime();
      add_RGB();
      // loop over structs to extract the points
      for (unsigned int i=0; i<pt_count; i++){
        _x[i] = double(laspts3[i].X)*x_scale + x_offset;    // X
        _y[i] = double(laspts3[i].Y)*y_scale + y_offset;    // Y
        _z[i] = double(laspts3[i].Z)*z_scale + z_offset;    // Z
        _intensity[i] = laspts3[i].Intensity;                // Intensity
        _classification[i] = laspts3[i].Classification;      // Classification
        _gpstime[i] = laspts3[i].GPSTime;
        _RGB[i].R = laspts3[i].Red;
        _RGB[i].G = laspts3[i].Green;
        _RGB[i].B = laspts3[i].Blue;
      }
      _gpst_min = _gpstime[0];
      _gpst_max = _gpstime[pt_count-1];

      delete[] laspts3;
      break;

    case 4:
      // initialize array of structs
      las_pt_4 *laspts4;
      laspts4 = new las_pt_4[pt_count];

      // copy from memory map into array of structs
      memcpy(laspts4, &lasmap[point_offset-1], point_record_bytes*pt_count);

      add_gpstime();
      // loop over structs to extract the points
      for (unsigned int i=0; i<pt_count; i++){
        _x[i] = double(laspts4[i].X)*x_scale + x_offset;    // X
        _y[i] = double(laspts4[i].Y)*y_scale + y_offset;    // Y
        _z[i] = double(laspts4[i].Z)*z_scale + z_offset;    // Z
        _intensity[i] = laspts4[i].Intensity;                // Intensity
        _classification[i] = laspts4[i].Classification;      // Classification
        _gpstime[i] = laspts4[i].GPSTime;
      }
      _gpst_min = _gpstime[0];
      _gpst_max = _gpstime[pt_count-1];

      delete[] laspts4;
      break;

    case 5:
      // initialize array of structs
      las_pt_5 *laspts5;
      laspts5 = new las_pt_5[pt_count];

      // copy from memory map into array of structs
      memcpy(laspts5, &lasmap[point_offset-1], point_record_bytes*pt_count);

      add_gpstime();
      add_RGB();
      // loop over structs to extract the points
      for (unsigned int i=0; i<pt_count; i++){
        _x[i] = double(laspts5[i].X)*x_scale + x_offset;    // X
        _y[i] = double(laspts5[i].Y)*y_scale + y_offset;    // Y
        _z[i] = double(laspts5[i].Z)*z_scale + z_offset;    // Z
        _intensity[i] = laspts5[i].Intensity;                // Intensity
        _classification[i] = laspts5[i].Classification;      // Classification
        _gpstime[i] = laspts5[i].GPSTime;
        _RGB[i].R = laspts5[i].Red;
        _RGB[i].G = laspts5[i].Green;
        _RGB[i].B = laspts5[i].Blue;
      }
      _gpst_min = _gpstime[0];
      _gpst_max = _gpstime[pt_count-1];

      delete[] laspts5;
      break;

    default:
      cout << "this should not be possible" << endl;
  }

  // unmap the data
    //cout << "trying to unmap" << endl;

  if (munmap(lasmap, point_offset + point_record_bytes*pt_count) < 0){
    cout << "ruh roh! problem unmapping LAS file" << endl;
    throw -1;
  }
  close(fd);
  //cout << "finished unmapping" << endl;
  

  // check to see intensity and classification contain actual info
  for (unsigned int i=0; i<pt_count; i++){
    if (_intensity[i] != 0) {
      fieldexist = true;
      break;
    }
  }
  if (!fieldexist){
    cout << "clearing intensity" << endl;
    _intensity.clear();
    //delete[] intensity;
    //intensity = NULL;
  }
  fieldexist = false;
  for (unsigned int i=0; i<pt_count; i++){
    if (_classification[i] != 0) {
      fieldexist = true;
      break;
    }
  }
  if (!fieldexist){
    cout << "clearing classification" << endl;
    _classification.clear();
    //delete[] classification;
    //classification = NULL;
  }
  fieldexist = false;

  return;
}


void PointCloud::write_LAS(string filename){
  cout << "writing not quite supported yet" << endl;
}

PointCloud PointCloud::subset(const bool & keepref){
  // declare vars
  unsigned int subset_count=0, ct;
  PointCloud cloud_subset;
  const bool * keep = &keepref;

  for (unsigned int i=0; i<pointcount(); i++){
    if (keep[i]) subset_count++;
  }

  cloud_subset = PointCloud(subset_count);

  // copy x, y, z data
  ct=0;
  for (unsigned int i=0; i<pointcount(); i++){
    if (keep[i]){
      cloud_subset._x[ct] = _x[i];
      cloud_subset._y[ct] = _y[i];
      cloud_subset._z[ct] = _z[i];
      ct++;
    }
  }

  // copy intensity data
  ct=0;
  if (intensity_present()){
    cloud_subset.add_intensity();
    for (unsigned int i=0; i<pointcount(); i++){
      if (keep[i]){
        cloud_subset._intensity[ct] = _intensity[i];
        ct++;
      }
    }
  }

  // copy classification data
  ct=0;
  if (classification_present()){
    cloud_subset.add_classification();
    for (unsigned int i=0; i<pointcount(); i++){
      if (keep[i]){
        cloud_subset._classification[ct] = _classification[i];
        ct++;
      }
    }
  }

  // copy gpstime data
  ct=0;
  if (gpstime_present()){
    cloud_subset.add_gpstime();
    for (unsigned int i=0; i<pointcount(); i++){
      if (keep[i]){
        cloud_subset._gpstime[ct] = _gpstime[i];
        ct++;
      }
    }
  }

  // copy RGB data
  ct=0;
  if (RGB_present()){
    cloud_subset.add_RGB();
    for (unsigned int i=0; i<pointcount(); i++){
      if (keep[i]){
        cloud_subset._RGB[ct].R = _RGB[i].R;
        cloud_subset._RGB[ct].G = _RGB[i].G;
        cloud_subset._RGB[ct].B = _RGB[i].B;
        ct++;
      }
    }
  }

  cloud_subset.calc_extents();

  return cloud_subset;
}

PointCloud PointCloud::subset(const unsigned int & keep_inds_ref, const unsigned int keep_count){
  // declare vars
  bool * keep;
  const unsigned int * keep_inds = &keep_inds_ref;
  PointCloud cloud_subset;

  keep = new bool[pointcount()];

  for (unsigned int i=0; i<pointcount(); i++) keep[i] = false;
  for (unsigned int j=0; j<keep_count; j++) keep[keep_inds[j]] = true;

  cloud_subset = subset(*keep);

  delete[] keep;

  return cloud_subset;
}


#ifdef _TEST_

// compile with:
// g++ -std=c++11 PointCloud.cpp -o pointcloud_test

int main(int argc, char * argv[]){
  // declare vars
  PointCloud cloud, cloud_sub;
  unsigned int * keep_inds;

  // take one command line argument as the las file name and read it
  cloud = PointCloud::read_LAS("testfiles/ComplexSRSInfo.las");
  cloud.print_summary();
  cloud.print_detailed();

  // test the subset function
  keep_inds = new unsigned int[4];
  keep_inds[0] = 1;
  keep_inds[1] = 100;
  keep_inds[2] = 200;
  keep_inds[3] = 300;
  cloud_sub = cloud.subset(keep_inds[0], 4);
  cloud_sub.print_summary();
  delete[] keep_inds;

  return 0;
}
#endif