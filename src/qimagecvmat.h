#ifndef QIMAGECVMAT_H
#define QIMAGECVMAT_H

/* credit to https://github.com/WangHongshuo/QImageMatConvert */

#include <QImage>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <assert.h>

/** WARNING!
 *  When the src QImage format is QImage::Format_RGB8888 and the enableDeepCopy == false,
 *  the ConvertQImageToMat() will change the pixel order of src QImage(RGB to BGR).
 *  ShadowCopy for show image(faster), DeepCopy for process image data(safer).
 */
QImage cvmat2qimage(const cv::Mat& src, bool enableDeepCopy = true);
cv::Mat qimage2cvmat(const QImage& src, bool enableDeepCopy = true);

#endif // QIMAGECVMAT_H
