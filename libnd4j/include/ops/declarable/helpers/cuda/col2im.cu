/* ******************************************************************************
 *
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License, Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0.
 *
 *  See the NOTICE file distributed with this work for additional
 *  information regarding copyright ownership.
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

//
// @author raver119@gmail.com, created on 30.11.17.
// @author Yurii Shyrma (iuriish@yahoo.com)
//

#include <ops/declarable/helpers/col2im.h>
#include <helpers/PointersManager.h>

namespace sd    {
namespace ops     {
namespace helpers {

//////////////////////////////////////////////////////////////////////////
// columns [bS, iC, kH, kW, oH, oW] to be de-convoluted to image [bS, iC, iH, iW]
template <typename T>
static __global__ void col2imCuda(const void* columns, const Nd4jLong* colShapeInfo, void* image, const Nd4jLong* imShapeInfo, const int sH, const int sW, const int pH, const int pW, const int dH, const int dW) {

    const T* col = reinterpret_cast<const T*>(columns);
          T* im = reinterpret_cast<T*>(image);

    __shared__ uint kH, kW, oH, oW, *sharedMem;
    __shared__ Nd4jLong imLen;

    if (threadIdx.x == 0) {
        extern __shared__ unsigned char shmem[];
        sharedMem = reinterpret_cast<uint*>(shmem);

        kH = dH * (colShapeInfo[3] - 1) + 1;
        kW = dW * (colShapeInfo[4] - 1) + 1;

        oH = colShapeInfo[5];
        oW = colShapeInfo[6];

        imLen  = shape::length(imShapeInfo);
    }
    __syncthreads();

    auto coords = sharedMem + threadIdx.x * 6;

    const auto tid = blockIdx.x * blockDim.x + threadIdx.x;

    for (Nd4jLong i = tid; i < imLen; i += gridDim.x * blockDim.x) {

        shape::index2coords(i, imShapeInfo, coords);

        const auto imOffset = shape::getOffset(imShapeInfo, coords);

        const auto bSiCoffset = coords[0] * colShapeInfo[7] + coords[1] * colShapeInfo[8];

        const uint imH = coords[2] + pH;
        const uint imW = coords[3] + pW;

        const uint colHstart = (imH < kH) ? 0 : (imH - kH) / sH + 1;
        const uint colWstart = (imW < kW) ? 0 : (imW - kW) / sW + 1;

        const uint colHend = sd::math::nd4j_min<uint>(imH / sH + 1, oH);
        const uint colWend = sd::math::nd4j_min<uint>(imW / sW + 1, oW);

        T val = 0;

        for(coords[4] = colHstart; coords[4] < colHend; ++coords[4]) {
            coords[2] = imH - coords[4] * sH;
            if(coords[2] % dH != 0) continue;

            for(coords[5] = colWstart; coords[5] < colWend; ++coords[5]) {
                coords[3] = imW - coords[5] * sW;
                if(coords[3] % dW != 0) continue;

                val += col[bSiCoffset + (coords[2]/dH)*colShapeInfo[9] + (coords[3]/dW)*colShapeInfo[10] + coords[4]*colShapeInfo[11] + coords[5]*colShapeInfo[12]];
            }
        }
        im[imOffset] = val;
    }
}

////////////////////////////////////////////////////////////////////////
// columns [bS, iC, kH, kW, oH, oW] to be de-convoluted to image [bS, iC, iH, iW]
template<typename T>
__global__ static void col2imCuda2(const void *columns, void *image, const Nd4jLong *colShapeInfo, const Nd4jLong *imShapeInfo, const int sH, const int sW, const int pH, const int pW, const int dH, const int dW) {

          const auto col = reinterpret_cast<const T*>(columns);
          auto im = reinterpret_cast<T*>(image);

          auto colShape = shape::shapeOf(const_cast<Nd4jLong *>(colShapeInfo));
          auto colStride = shape::stride(const_cast<Nd4jLong *>(colShapeInfo));

          int colStride0 = colStride[0];
          int colStride1 = colStride[1];
          int colStride2 = colStride[2];
          int colStride3 = colStride[3];
          int colStride4 = colStride[4];
          int colStride5 = colStride[5];

          int kH = colShape[2];
          int kW = colShape[3];

          auto imShape  = shape::shapeOf(const_cast<Nd4jLong *>(imShapeInfo));
          auto imOrder  = shape::order(const_cast<Nd4jLong *>(imShapeInfo));
          auto imStride = shape::stride(const_cast<Nd4jLong *>(imShapeInfo));

          int bS = imShape[0];
          int iC = imShape[1];
          int iH = imShape[2];
          int iW = imShape[3];

          int oH = colShape[4];//(iH + 2 * pH - kH) / sW + 1;
          int oW = colShape[5];//(iW + 2 * pW - kW) / sH + 1;

          int n = bS * iC * iH * iW;

          //Effective kernel size, accounting for dilation
          int kHeff = kH + (kH - 1) * (dH - 1);
          int kWeff = kW + (kW - 1) * (dW - 1);

          for (int i = (blockDim.x * blockIdx.x) + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
              T val = 0;

              int w_im = i % iW + pW;
              int h_im = (i / iW) % iH + pH;
              int c_im = i / (iW * iH);
              int b = c_im / iC;
              int c = c_im % iC;

              // compute the start and end of the output
              // These are the indexes for dimensions ??? in the 6d col matrix
              int w_col_start = (w_im < kWeff) ? 0 : (w_im - kWeff) / sW + 1;
              int w_col_end = sd::math::nd4j_min<int>(w_im / sW + 1, oW);

              int h_col_start = (h_im < kHeff) ? 0 : (h_im - kHeff) / sH + 1;
              int h_col_end = sd::math::nd4j_min<int>(h_im / sH + 1, oH);

              //Iterate over col entries in the 6d array... these are added up
              for (int colH = h_col_start; colH < h_col_end; colH += 1) {
                  for (int colW = w_col_start; colW < w_col_end; colW += 1) {
                      int kRow = (h_im - colH * sH);
                      int kCol = (w_im - colW * sW);

                      if(kRow % dH == 0 && kCol % dW == 0){
                          kRow /= dH;
                          kCol /= dW;

                          int data_col_index = b * colStride0 + c * colStride1 + kRow * colStride2 + kCol * colStride3 + colH * colStride4 + colW * colStride5;
                          val += col[data_col_index];
                      }
                  }
              }

              int i_f = 0;
              int i_c = i;
              for (int dim = 3; dim >= 0; dim--) {
                  i_f += (i_c % imShape[dim])  * imStride[dim];
                  i_c = i_c / imShape[dim];
              }

              im[i_f] = val;
          }
}

//////////////////////////////////////////////////////////////////////////
template <typename T>
static void col2imCudaLauncher(const int blocksPerGrid, const int threadsPerBlock, const int sharedMem, const cudaStream_t *stream,
                               const void* columns, const Nd4jLong* colShapeInfo,
                                     void* image, const Nd4jLong* imShapeInfo,
                               const int sH, const int sW, const int pH, const int pW, const int dH, const int dW) {

    // col2imCuda2<T><<<512, 512, 1024, *stream>>>(columns, image, colShapeInfo, imShapeInfo, sH, sW, pH, pW, dH, dW);
    col2imCuda<T><<<blocksPerGrid, threadsPerBlock, sharedMem, *stream>>>(columns, colShapeInfo, image, imShapeInfo, sH, sW, pH, pW, dH, dW);
}

//////////////////////////////////////////////////////////////////////////
ND4J_LOCAL void col2im(sd::LaunchContext& context, const NDArray& col, NDArray& im, const int sH, const int sW, const int pH, const int pW, const int iH, const int iW, const int dH, const int dW) {

    PointersManager manager(&context, "col2im");

    const int threadsPerBlock = MAX_NUM_THREADS / 2;
    const int blocksPerGrid = (im.lengthOf() + threadsPerBlock - 1) / threadsPerBlock;
    const int sharedMem = col.rankOf() * sizeof(uint) * threadsPerBlock  + 256;

    NDArray::prepareSpecialUse({&im}, {&col});
    BUILD_SINGLE_SELECTOR(im.dataType(), col2imCudaLauncher, (blocksPerGrid, threadsPerBlock, sharedMem, context.getCudaStream(), col.specialBuffer(), col.specialShapeInfo(), im.specialBuffer(), im.specialShapeInfo(), sH, sW, pH, pW, dH, dW), FLOAT_TYPES);
    NDArray::registerSpecialUse({&im}, {&col});

    manager.synchronize();
}



}
}
}