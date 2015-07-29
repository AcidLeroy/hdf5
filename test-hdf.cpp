#include <hdf5/hdffile.hpp>
#include "gtest/gtest.h"

class Hdf5Test : public ::testing::Test {

 protected:
  const std::string file_name_{"test.h5"};
  const std::string file_name_struct_{"teststruct.h5"};
};

TEST_F(Hdf5Test, CreateFile) {
  hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);
}

TEST_F(Hdf5Test, CreateDataset) {
  hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);
  std::vector<hsize_t> dims;
  dims.resize(2);
  dims[0] = 20;
  dims[1] = 5;
  hdf::Slab<2> filespace(dims);
  boost::shared_ptr<hdf::HDFDataSet<> > datasetint =
      file.createDataset<int>("/test", filespace);
  boost::shared_ptr<hdf::HDFDataSet<> > datasetfloat =
      file.createDataset<float>("/test-float", filespace);
  boost::shared_ptr<hdf::HDFDataSet<> > datasetdouble =
      file.createDataset<double>("/test-double", filespace);
}

TEST_F(Hdf5Test, OpenDataset) {
  hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);
  {
    std::vector<hsize_t> dims;
    dims.resize(2);
    dims[0] = 15;
    dims[1] = 5;
    hdf::Slab<2> filespace(dims);
    boost::shared_ptr<hdf::HDFDataSet<> > datasetint =
        file.createDataset<int>("/test", filespace);
  }
  boost::shared_ptr<hdf::HDFDataSet<> > dataset = file.openDataset("/test");
}

TEST_F(Hdf5Test, CreateGroup) {
  hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);
  boost::shared_ptr<hdf::HDFGroup<> > group = file.createGroup("/test-group");
}

TEST_F(Hdf5Test, OpenGroup) {
  hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);
  {
    boost::shared_ptr<hdf::HDFGroup<> > group = file.createGroup("/test-group");
  }
  boost::shared_ptr<hdf::HDFGroup<> > group = file.openGroup("/test-group");
}

TEST_F(Hdf5Test, CreateAttribute) {
  hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);
  boost::shared_ptr<hdf::HDFGroup<> > group = file.createGroup("/test-group");
  std::vector<hsize_t> dims;
  dims.resize(2);
  dims[0] = 1;
  dims[1] = 3;

  group->createAttribute<int>("attr", dims);
}

TEST_F(Hdf5Test, OpenAttribute) {
  hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);
  boost::shared_ptr<hdf::HDFGroup<> > group = file.createGroup("/test-group");
  std::vector<hsize_t> dims;
  dims.resize(2);
  dims[0] = 1;
  dims[1] = 3;
  group->createAttribute<int>("attr", dims);
  group->openAttribute(std::string("attr"));
}

TEST_F(Hdf5Test, WriteAttribute) {
  hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);
  boost::shared_ptr<hdf::HDFGroup<> > group = file.createGroup("/test-group");
  std::vector<hsize_t> dims(1, 1);

  group->createAttribute<int>("attr", dims);
  boost::shared_ptr<hdf::HDFAttribute<> > attr =
      group->openAttribute(std::string("attr"));
  attr->writeData(10);
  int val;
  attr->readData(val);
  assert(val == 10);
}

TEST_F(Hdf5Test, WriteDataset) {
  {
    hdf::HDFFile<> file(file_name_, hdf::HDFFile<>::truncate);

    std::vector<hsize_t> dims;
    dims.resize(2);
    dims[0] = 3;
    dims[1] = 2;
    hdf::Slab<2> filespace(dims);
    boost::shared_ptr<hdf::HDFDataSet<> > datasetint =
        file.createDataset<int>("/test", filespace);
  }
  hdf::HDFFile<> file(file_name_);

  boost::shared_ptr<hdf::HDFDataSet<> > dataset = file.openDataset("/test");

  std::vector<int> testdata(6);
  testdata[0] = 1;
  testdata[1] = 2;
  testdata[2] = 3;
  testdata[3] = 4;
  testdata[4] = 5;
  testdata[5] = 6;
  dataset->writeData(testdata);

  {
    std::vector<hsize_t> dims;
    dims.resize(2);
    dims[0] = 2;
    dims[1] = 3;
    hdf::Slab<2> filespace(dims);
    boost::shared_ptr<hdf::HDFDataSet<> > datasetint =
        file.createDataset<int>("/test2", filespace);
    datasetint->writeData(testdata);

    std::vector<int> readData2;
    datasetint->readData(readData2);
    ASSERT_TRUE(readData2.size() == 6);
    ASSERT_TRUE(readData2[0] == 1);
    ASSERT_TRUE(readData2[1] == 2);
    ASSERT_TRUE(readData2[2] == 3);
    ASSERT_TRUE(readData2[3] == 4);
  }

  std::vector<int> readData;
  dataset->readData(readData);
  ASSERT_TRUE(readData.size() == 6);
  ASSERT_TRUE(readData[0] == 1);
  ASSERT_TRUE(readData[1] == 2);
  ASSERT_TRUE(readData[2] == 3);
  ASSERT_TRUE(readData[3] == 4);
}

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

struct test_point {
  test_point() = default;
  test_point(float x, float y, float z) : x(x), y(y), z(z) {}
  float x, y, z;

  bool operator==(const test_point& t) const {
    return x == t.x && y == t.y && z == t.z;
  }
};

BOOST_FUSION_ADAPT_STRUCT(test_point, (float, x)(float, y)(float, z));

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/std_pair.hpp>

TEST_F(Hdf5Test, WriteStructure) {
  {
    hdf::HDFFile<> file(file_name_struct_, hdf::HDFFile<>::truncate);
    std::vector<test_point> points(4);
    points[0] = test_point(0, 0, 1);
    points[1] = test_point(1, 0, 0);
    points[2] = test_point(1, 0, 1);
    points[3] = test_point(0, 1, 0);
    std::cout << "Writing points" << std::endl;
    boost::shared_ptr<hdf::HDFDataSet<> > datasetpoints =
        file.writeDataset("/struct", points);
  }

  hdf::HDFFile<> file(file_name_struct_);
  boost::shared_ptr<hdf::HDFDataSet<> > datasetpoints =
      file.openDataset("/struct");
  std::vector<test_point> read;
  datasetpoints->readData(read);
  ASSERT_EQ(12, read.size());
  ASSERT_EQ(test_point(0, 0, 1), read[0]);
  ASSERT_EQ(test_point(1, 0, 0), read[1]);
}

TEST_F(Hdf5Test, WritePairs) {
  hdf::HDFFile<> file(file_name_struct_, hdf::HDFFile<>::truncate);
  std::vector<std::pair<int, float> > pairs(2);
  pairs[0] = std::make_pair(1, 2.5f);
  pairs[1] = std::make_pair(1, 3.5f);
  file.writeDataset("/pairs", pairs);
  boost::shared_ptr<hdf::HDFDataSet<> > datasetpairs =
      file.openDataset("/pairs");

  std::vector<std::pair<int, float> > actual(2);
  datasetpairs->readData(actual);
  ASSERT_EQ(2, actual.size());
  ASSERT_EQ(std::make_pair(1, 2.5f), actual[0]);
}
