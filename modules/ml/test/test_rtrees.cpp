// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "test_precomp.hpp"

namespace opencv_test { namespace {

TEST(ML_RTrees, getVotes)
{
    int n = 12;
    int count, i;
    int label_size = 3;
    int predicted_class = 0;
    int max_votes = -1;
    int val;
    // RTrees for classification
    Ptr<ml::RTrees> rt = cv::ml::RTrees::create();

    //data
    Mat data(n, 4, CV_32F);
    randu(data, 0, 10);

    //labels
    Mat labels = (Mat_<int>(n,1) << 0,0,0,0, 1,1,1,1, 2,2,2,2);

    rt->train(data, ml::ROW_SAMPLE, labels);

    //run function
    Mat test(1, 4, CV_32F);
    Mat result;
    randu(test, 0, 10);
    rt->getVotes(test, result, 0);

    //count vote amount and find highest vote
    count = 0;
    const int* result_row = result.ptr<int>(1);
    for( i = 0; i < label_size; i++ )
    {
        val = result_row[i];
        //predicted_class = max_votes < val? i;
        if( max_votes < val )
        {
            max_votes = val;
            predicted_class = i;
        }
        count += val;
    }

    EXPECT_EQ(count, (int)rt->getRoots().size());
    EXPECT_EQ(result.at<float>(0, predicted_class), rt->predict(test));
}

TEST(ML_RTrees, 11142_sample_weights_regression)
{
    int n = 3;
    // RTrees for regression
    Ptr<ml::RTrees> rt = cv::ml::RTrees::create();
    //simple regression problem of x -> 2x
    Mat data = (Mat_<float>(n,1) << 1, 2, 3);
    Mat values = (Mat_<float>(n,1) << 2, 4, 6);
    Mat weights = (Mat_<float>(n, 1) << 10, 10, 10);

    Ptr<TrainData> trainData = TrainData::create(data, ml::ROW_SAMPLE, values);
    rt->train(trainData);
    double error_without_weights = round(rt->getOOBError());
    rt->clear();
    Ptr<TrainData> trainDataWithWeights = TrainData::create(data, ml::ROW_SAMPLE, values, Mat(), Mat(), weights );
    rt->train(trainDataWithWeights);
    double error_with_weights = round(rt->getOOBError());
    // error with weights should be larger than error without weights
    EXPECT_GE(error_with_weights, error_without_weights);
}

TEST(ML_RTrees, 11142_sample_weights_classification)
{
    int n = 12;
    // RTrees for classification
    Ptr<ml::RTrees> rt = cv::ml::RTrees::create();

    Mat data(n, 4, CV_32F);
    randu(data, 0, 10);
    Mat labels = (Mat_<int>(n,1) << 0,0,0,0, 1,1,1,1, 2,2,2,2);
    Mat weights = (Mat_<float>(n, 1) << 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10);

    rt->train(data, ml::ROW_SAMPLE, labels);
    rt->clear();
    double error_without_weights = round(rt->getOOBError());
    Ptr<TrainData> trainDataWithWeights = TrainData::create(data, ml::ROW_SAMPLE, labels, Mat(), Mat(), weights );
    rt->train(data, ml::ROW_SAMPLE, labels);
    double error_with_weights = round(rt->getOOBError());
    std::cout << error_without_weights << std::endl;
    std::cout << error_with_weights << std::endl;
    // error with weights should be larger than error without weights
    EXPECT_GE(error_with_weights, error_without_weights);
}



}} // namespace
