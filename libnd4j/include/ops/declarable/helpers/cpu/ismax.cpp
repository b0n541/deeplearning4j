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
// @author Yurii Shyrma, created on 21.09.2018
// @author raver119@gmail.com
//


#include <helpers/TAD.h>
#include<ops/declarable/helpers/ismax.h>
#include <helpers/ConstantTadHelper.h>
#include <execution/Threads.h>

namespace sd 	  {
namespace ops 	  {
namespace helpers {

template <typename X, typename Z>
static void ismax_(const NDArray* input, NDArray* output, const std::vector<int>& dimensions) {

    if (input->isVector()) {
        int dimensionsLength = dimensions.size();
        int length = input->lengthOf();
        if (!dimensions.empty() && (input->shapeOf())[dimensions[0]] == 1) {
            for (int i = 0; i < length; i++)
                output->p<Z>(i, 1);
        }
        else {
            int eleStride = shape::elementWiseStride(input->shapeInfo());
            if (eleStride == 1) {
                int maxIdx = 0;
                auto currMax = input->e<X>(0);
                if (length < ELEMENT_THRESHOLD) {

                    for (int i = 0; i < length; i++) {
                        if (currMax < input->e<X>(i)) {
                            currMax = input->e<X>(i);
                            maxIdx = i;
                        }
                        output->p<Z>(i, 0);
                    }
                }
                else {

                    {
                        int maxIdxLocal = maxIdx;
                        auto currMaxLocal = currMax;

                        for (int i = 0; i < length; i++) {
                            if (currMaxLocal < input->e<X>(i)) {
                                currMaxLocal = input->e<X>(i);
                                maxIdxLocal = i;
                            }
                            output->p<Z>(i, 0);
                        }

                        PRAGMA_OMP_CRITICAL
                        {
                            if (currMax < currMaxLocal) {
                                currMax = currMaxLocal;
                                maxIdx = maxIdxLocal;
                            }
                        }
                    }
                }
                output->p<Z>(maxIdx, 1);
            }
            else {
                int maxIdx = 0;
                auto currMax = input->e<X>(0);
                if (length < ELEMENT_THRESHOLD) {

                    for (int i = 0; i < length; i++) {
                        if (currMax < input->e<X>(i)) {
                            currMax = input->e<X>(i);
                            maxIdx = i;
                        }
                        output->p<Z>(i, 0.f);
                    }
                }
                else {

                    {
                        int maxIdxLocal = maxIdx;
                        auto currMaxLocal = currMax;
                        for (int i = 0; i < length; i++) {
                            if (currMaxLocal < input->e<X>(i)) {
                                currMaxLocal = input->e<X>(i);
                                       maxIdxLocal = i;
                            }
                            output->p<Z>(i, 0.f);
                        }

                        PRAGMA_OMP_CRITICAL
                        {
                            if (currMax < currMaxLocal) {
                                currMax = currMaxLocal;
                                maxIdx = maxIdxLocal;
                            }
                        }
                    }
                }
                output->p<Z>(maxIdx, 1);
            }
        }
    }
    else {
        int dimensionsLength = dimensions.size();
        //int tads = tad.numTads;
        //decompose in to several sub tads after
        //moving all dimensions (in sorted order)
        //to the back.
        //permuted version of the input shape info for setting up the tad problem
        auto tadPack = sd::ConstantTadHelper::getInstance().tadForDimensions(input->shapeInfo(), const_cast<int*>(dimensions.data()), dimensionsLength);
        auto tadPackZ = sd::ConstantTadHelper::getInstance().tadForDimensions(output->shapeInfo(), const_cast<int*>(dimensions.data()), dimensionsLength);


        auto tadShapeShapeInfo = tadPack.primaryShapeInfo();
        auto tadOffsets = tadPack.primaryOffsets();
        auto zOfsets = tadPackZ.platformOffsets();

        int tadLength = shape::length(tadShapeShapeInfo);
        int tads = tadPack.numberOfTads();

        int tadsPerThread = tads / TAD_THRESHOLD;
        int num_threads = sd::math::nd4j_max<int>(1, tadsPerThread);
        num_threads = sd::math::nd4j_min<int>(num_threads, omp_get_max_threads());

        auto tadEWS = shape::elementWiseStride(tadShapeShapeInfo);
        auto zEWS = shape::elementWiseStride(tadPackZ.primaryShapeInfo());

        int span = (tads / num_threads) + 8;

        auto func = PRAGMA_THREADS_FOR {
            for (auto r = start; r < stop; r++) {
                    auto rX = const_cast<NDArray*>(input)->bufferAsT<X>() + tadOffsets[r];
                    auto rZ = output->bufferAsT<Z>() + zOfsets[r];

                    auto maxValue = rX[0];
                    int maxIdx = 0;
                    if (tadEWS == 1 && zEWS == 1) {
                        for (int i = 0; i < tadLength; i++) {
                            if (rX[i] > maxValue) {
                                maxIdx = i;
                                maxValue = rX[i];
                            }
                        }

                        PRAGMA_OMP_SIMD
                        for (int i = 0; i < tadLength; i++) {
                            rZ[i] = maxIdx == i ? (Z) 1 : (Z) 0;
                        }
                    }
                    else if (tadEWS > 1 && zEWS > 1) {
                        for (int i = 0; i < tadLength; i++) {
                            if (rX[i * tadEWS] > maxValue) {
                                maxIdx = i;
                                maxValue = rX[i * tadEWS];
                            }
                        }

                        PRAGMA_OMP_SIMD
                        for (int i = 0; i < tadLength; i++) {
                            rZ[i * zEWS] = maxIdx == i ? (Z) 1 : (Z) 0;
                        }
                    } else {
                        for (int i = 0; i < tadLength; i++) {
                            auto xOffset = shape::getIndexOffset(i, tadShapeShapeInfo);
                            if (rX[xOffset] > maxValue) {
                                maxIdx = i;
                                maxValue = rX[xOffset];
                            }
                        }

                        PRAGMA_OMP_SIMD
                        for (int i = 0; i < tadLength; i++) {
                            auto zOffset = shape::getIndexOffset(i, tadPackZ.primaryShapeInfo());
                            rZ[zOffset] = maxIdx == i ? (Z) 1 : (Z) 0;
                        }
                    }
            }
        };

        samediff::Threads::parallel_tad(func, 0, tads);
    }
}


ND4J_LOCAL void ismax(sd::LaunchContext * context, const NDArray *input, NDArray *output, const std::vector<int>& dimensions) {
    BUILD_DOUBLE_SELECTOR(input->dataType(), output->dataType(), ismax_, (input, output, dimensions), LIBND4J_TYPES, LIBND4J_TYPES);
}


}
}
}

