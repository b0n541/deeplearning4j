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
// @author raver119@gmail.com
//

#include <helpers/TAD.h>
#include <helpers/ConstantTadHelper.h>
#include <execution/Threads.h>
#include "../one_hot.h"

namespace sd {
    namespace ops {
        namespace helpers {
            template <typename Z, typename I>
            static void onehot_(void *voutput, Nd4jLong const* zShapeInfo, void const* vindices, Nd4jLong const* iShapeInfo, int axis, double on, double off) {
                auto output = reinterpret_cast<Z*>(voutput);
                auto indices = reinterpret_cast<I const*>(vindices);

                auto tadPack = sd::ConstantTadHelper::getInstance().tadForDimensions(zShapeInfo, {axis});

                auto iLen = static_cast<unsigned int>(shape::length(iShapeInfo));
                auto tLen = static_cast<unsigned int>(shape::length(tadPack.primaryShapeInfo()));
                auto numTads = static_cast<unsigned int>(tadPack.numberOfTads());
                auto tadEws = shape::elementWiseStride(tadPack.primaryShapeInfo());

                if (iLen != numTads)
                    throw std::runtime_error("OneHot: number of TADs should be equal to number of indices");

                if (shape::elementWiseStride(zShapeInfo) != 1 || shape::elementWiseStride(iShapeInfo) != 1)
                    throw std::runtime_error("OneHot: op expects output and indices to have elementWiseStride to be equal to 1");

                Z zero = static_cast<Z>(off);
                Z one = static_cast<Z>(on);

                if (tadEws >= 1) {
                    auto func = PRAGMA_THREADS_FOR {
                        for (auto e = 0; e < stop; e++) {
                            auto cO = output + tadPack.primaryOffsets()[e];

                            auto idx = static_cast<int>(indices[e]);
                            if (idx < 0 || idx >= tLen) {
                                PRAGMA_OMP_SIMD
                                for (unsigned int t = 0; t < tLen; t++) {
                                    cO[t * tadEws] = zero;
                                }
                            } else {
                                PRAGMA_OMP_SIMD
                                for (unsigned int t = 0; t < tLen; t++) {
                                    cO[t * tadEws] = idx == t ? one : zero;
                                }
                            }
                        }
                    };

                    samediff::Threads::parallel_tad(func, 0, numTads);
                } else {
                    auto func = PRAGMA_THREADS_FOR {
                        for (auto e = start; e < stop; e++) {
                            auto cO = output + tadPack.primaryOffsets()[e];

                            auto idx = static_cast<int>(indices[e]);
                            if (idx < 0 || idx >= tLen) {
                                PRAGMA_OMP_SIMD
                                for (unsigned int t = 0; t < tLen; t++) {
                                    cO[shape::getIndexOffset(t, tadPack.primaryShapeInfo())] = zero;
                                }
                            } else {
                                PRAGMA_OMP_SIMD
                                for (unsigned int t = 0; t < tLen; t++) {
                                    cO[shape::getIndexOffset(t, tadPack.primaryShapeInfo())] = idx == t ? one : zero;
                                }
                            }
                        }
                    };

                    samediff::Threads::parallel_tad(func, 0, numTads);
                }
            }

            ND4J_LOCAL void onehot(const sd::LaunchContext* context, const NDArray *indices, NDArray *output, const uint axis, const uint depth, const double on, const double off) {
                auto zType = output->dataType();
                auto iType = indices->dataType();

                BUILD_DOUBLE_SELECTOR(zType, iType, onehot_, (output->buffer(), output->shapeInfo(), indices->buffer(), indices->shapeInfo(), axis, on, off), LIBND4J_TYPES, LIBND4J_TYPES);
            }
        }
    }
}
