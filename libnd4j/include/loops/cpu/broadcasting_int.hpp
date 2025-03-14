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
//  @author raver119@gmail.com
//

#include <system/op_boilerplate.h>
#include <loops/broadcasting_int.h>
#include <loops/legacy_ops.h>
#include <types/types.h>
#include <helpers/LoopKind.h>
#include <helpers/ConstantTadHelper.h>
#include <execution/Threads.h>

using namespace simdOps;

namespace functions {
    namespace broadcast {

        template <typename X>
        void BroadcastInt<X>::exec(const int opNum,
                                   const void *x, const Nd4jLong *xShapeInfo,
                                   const void *y, const Nd4jLong *yShapeInfo,
                                   void *z, const Nd4jLong *zShapeInfo,
                                   int *dimension, int dimensionLength,
                                   const Nd4jLong *xTadShapeInfo, const Nd4jLong *xTadOffset,
                                   const Nd4jLong *zTadShapeInfo, const Nd4jLong *zTadOffset,
                                   uint64_t start, uint64_t stop) {
            DISPATCH_BY_OPNUM_T(exec, PARAMS(x,
                                               xShapeInfo,
                                               y,
                                               yShapeInfo,
                                               z,
                                               zShapeInfo,
                                               dimension,
                                               dimensionLength,
                                               xTadShapeInfo,
                                               xTadOffset,
                                               zTadShapeInfo,
                                               zTadOffset, start, stop), BROADCAST_INT_OPS);
        }

        template <typename X>
        void BroadcastInt<X>::exec(const int opNum,
                             const void *x, const Nd4jLong *xShapeInfo,
                             const void *y, const Nd4jLong *yShapeInfo,
                                   void *z, const Nd4jLong *zShapeInfo) {

            DISPATCH_BY_OPNUM_T(exec, PARAMS(x, xShapeInfo, y, yShapeInfo, z, zShapeInfo), BROADCAST_INT_OPS);
        }

        template <typename X>
        void BroadcastInt<X>::execInverse(const int opNum,
                                          const void *x, const Nd4jLong *xShapeInfo,
                                          const void *y, const Nd4jLong *yShapeInfo,
                                          void *z, const Nd4jLong *zShapeInfo,
                                          int *dimension, int dimensionLength,
                                          const Nd4jLong *xTadShapeInfo, const Nd4jLong *xTadOffset,
                                          const Nd4jLong *zTadShapeInfo, const Nd4jLong *zTadOffset,
                                          uint64_t start, uint64_t stop) {
            DISPATCH_BY_OPNUM_T(execInverse, PARAMS(x,
                                               xShapeInfo,
                                               y,
                                               yShapeInfo,
                                               z,
                                               zShapeInfo,
                                               dimension,
                                               dimensionLength,
                                               xTadShapeInfo,
                                               xTadOffset,
                                               zTadShapeInfo,
                                               zTadOffset, start, stop), BROADCAST_INT_OPS);
        }

        template <typename X>
        template<typename OpType>
        void BroadcastInt<X>::exec(const void *vx, const Nd4jLong *xShapeInfo,
                                   const void *vy, const Nd4jLong *yShapeInfo,
                                   void *vz, const Nd4jLong *zShapeInfo,
                                   int *dimension, int dimensionLength,
                                   const Nd4jLong *xTadShapeInfo, const Nd4jLong *xTadOffset,
                                   const Nd4jLong *zTadShapeInfo, const Nd4jLong *zTadOffset,
                                   uint64_t start, uint64_t stop) {

                auto x = reinterpret_cast<const X *>(vx);
                auto y = reinterpret_cast<const X *>(vy);
                auto z = reinterpret_cast<X *>(vz);

                //decompose in to several sub tads after
                //moving all dimensions (in sorted order)
                //to the back.
                //permuted version of the x shape info for setting up the tad problem
                auto xTadShapeShapeInfo = xTadShapeInfo;
                auto tadOffsets = xTadOffset;

                if (xTadShapeInfo == nullptr || tadOffsets == nullptr) {
                    auto tadPack = sd::ConstantTadHelper::getInstance().tadForDimensions(xShapeInfo, dimension, dimensionLength);

                    xTadShapeShapeInfo = const_cast<Nd4jLong*>(tadPack.primaryShapeInfo());
                    tadOffsets = const_cast<Nd4jLong*>(tadPack.primaryOffsets());
                }

                //int *resultStride = shape::stride(xTadShapeShapeInfo);
                unsigned int tadLength = shape::length(xTadShapeShapeInfo);//shape::length(xTadShapeShapeInfo);
                unsigned int tads = shape::length(xShapeInfo) / tadLength;

                if (zTadShapeInfo == nullptr) {
                    zTadShapeInfo = xTadShapeShapeInfo;
                    zTadOffset = tadOffsets;
                }

                auto lenZ = shape::length(zTadShapeInfo);
                auto lenY = shape::length(yShapeInfo);

                int tadsPerThread = tads / TAD_THRESHOLD;
                int threads = sd::math::nd4j_max<int>(1, tadsPerThread);
                threads = sd::math::nd4j_min<int>(threads, sd::Environment::getInstance().maxThreads());

                auto xEws = shape::elementWiseStride(xTadShapeShapeInfo);
                auto yEws = shape::elementWiseStride(yShapeInfo);
                auto zEws = shape::elementWiseStride(zTadShapeInfo);

                const sd::LoopKind::Kind kindOfLoop = sd::LoopKind::deduceKindOfLoopXYZ(xTadShapeShapeInfo, yShapeInfo, zTadShapeInfo);

                if (kindOfLoop == sd::LoopKind::EWS1) {
                    for (auto i = start; i < stop; i ++) {
                        auto oX = x + tadOffsets[i];
                        auto oZ = z + zTadOffset[i];

                        PRAGMA_OMP_SIMD
                        for (unsigned int f = 0; f < tadLength; f++)
                            oZ[f] = OpType::op(oX[f], y[f]);
                    };
                }
                else if(kindOfLoop == sd::LoopKind::EWSNONZERO) {
                    for (auto i = start; i < stop; i ++) {
                        auto oX = x + tadOffsets[i];
                        auto oZ = z + zTadOffset[i];

                        PRAGMA_OMP_SIMD
                        for (unsigned int f = 0; f < tadLength; f++)
                            oZ[f * zEws] = OpType::op(oX[f * xEws], y[f * yEws]);
                    };
                }
                else if(shape::haveSameShapeAndStrides(xTadShapeShapeInfo, yShapeInfo) && shape::haveSameShapeAndStrides(xTadShapeShapeInfo, zTadShapeInfo)) {
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    bool canCastX = sd::DataTypeUtils::castShapeInfo(xTadShapeShapeInfo, tadShapeShapeInfoCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oX = x + tadOffsets[i];

                        PRAGMA_OMP_SIMD
                        for (unsigned int f = 0; f < tadLength; f++) {
                            auto offset = shape::indexOffset(f, xTadShapeShapeInfo, tadShapeShapeInfoCast, canCastX);
                            oZ[offset] = OpType::op(oX[offset], y[offset]);
                        }
                    };
                }
                else if(shape::haveSameShapeAndStrides(xTadShapeShapeInfo, yShapeInfo)) {
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    uint tadShapeInfoZCast[MAX_RANK];
                    bool canCastX = sd::DataTypeUtils::castShapeInfo(xTadShapeShapeInfo, tadShapeShapeInfoCast);
                    bool canCastZ = sd::DataTypeUtils::castShapeInfo(zTadShapeInfo, tadShapeInfoZCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oX = x + tadOffsets[i];

                        PRAGMA_OMP_SIMD
                        for (unsigned int f = 0; f < tadLength; f++) {
                            auto offset = shape::indexOffset(f, xTadShapeShapeInfo, tadShapeShapeInfoCast, canCastX);
                            auto zOffset = shape::indexOffset(f, zTadShapeInfo, tadShapeInfoZCast, canCastZ);
                            oZ[zOffset] = OpType::op(oX[offset], y[offset]);
                        }
                    };
                }
                else if(shape::haveSameShapeAndStrides(xTadShapeShapeInfo, zTadShapeInfo)) {
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    uint yShapeInfoCast[MAX_RANK];
                    bool canCastX = sd::DataTypeUtils::castShapeInfo(xTadShapeShapeInfo, tadShapeShapeInfoCast);
                    bool canCastY = sd::DataTypeUtils::castShapeInfo(yShapeInfo, yShapeInfoCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oX = x + tadOffsets[i];

                        PRAGMA_OMP_SIMD
                        for (unsigned int f = 0; f < tadLength; f++) {
                            auto offset = shape::indexOffset(f, xTadShapeShapeInfo, tadShapeShapeInfoCast, canCastX);
                            auto yOffset = shape::indexOffset(f, yShapeInfo, yShapeInfoCast, canCastY);
                            oZ[offset] = OpType::op(oX[offset], y[yOffset]);
                        }
                    };
                }
                else if(shape::haveSameShapeAndStrides(yShapeInfo, zTadShapeInfo)) {
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    uint yShapeInfoCast[MAX_RANK];
                    bool canCastX = sd::DataTypeUtils::castShapeInfo(xTadShapeShapeInfo, tadShapeShapeInfoCast);
                    bool canCastY = sd::DataTypeUtils::castShapeInfo(yShapeInfo, yShapeInfoCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oX = x + tadOffsets[i];

                        PRAGMA_OMP_SIMD
                        for (unsigned int f = 0; f < tadLength; f++) {
                            auto xOffset = shape::indexOffset(f, xTadShapeShapeInfo, tadShapeShapeInfoCast, canCastX);
                            auto offset = shape::indexOffset(f, yShapeInfo, yShapeInfoCast, canCastY);
                            oZ[offset] = OpType::op(oX[xOffset], y[offset]);
                        }
                    };
                }
                else {
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    uint tadShapeInfoZCast[MAX_RANK];
                    uint yShapeInfoCast[MAX_RANK];
                    bool canCastX = sd::DataTypeUtils::castShapeInfo(xTadShapeShapeInfo, tadShapeShapeInfoCast);
                    bool canCastY = sd::DataTypeUtils::castShapeInfo(yShapeInfo, yShapeInfoCast);
                    bool canCastZ = sd::DataTypeUtils::castShapeInfo(zTadShapeInfo, tadShapeInfoZCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oX = x + tadOffsets[i];

                        PRAGMA_OMP_SIMD
                        for (unsigned int f = 0; f < tadLength; f++) {
                            auto xOffset = shape::indexOffset(f, xTadShapeShapeInfo, tadShapeShapeInfoCast, canCastX);
                            auto yOffset = shape::indexOffset(f, yShapeInfo, yShapeInfoCast, canCastY);
                            auto zOffset = shape::indexOffset(f, zTadShapeInfo, tadShapeInfoZCast, canCastZ);
                            oZ[zOffset] = OpType::op(oX[xOffset], y[yOffset]);
                        }
                    };
                }
        }


        template <typename X>
        template<typename OpType>
        void BroadcastInt<X>::execInverse(const void *vx, const Nd4jLong *xShapeInfo,
                                          const void *vy, const Nd4jLong *yShapeInfo,
                                          void *vz, const Nd4jLong *zShapeInfo,
                                          int *dimension, const int dimensionLength,
                                          const Nd4jLong *yTadShapeInfo, const Nd4jLong *yTadOffset,
                                          const Nd4jLong *zTadShapeInfo, const Nd4jLong *zTadOffset,
                                          uint64_t start, uint64_t stop) {

                auto x = reinterpret_cast<const X *>(vx);
                auto y = reinterpret_cast<const X *>(vy);
                auto z = reinterpret_cast<X *>(vz);

                //decompose in to several sub tads after
                //moving all dimensions (in sorted order)
                //to the back.
                //permuted version of the x shape info for setting up the tad problem
                auto yTadShapeShapeInfo = yTadShapeInfo;
                auto tadOffsets = yTadOffset;

                if (yTadShapeInfo == nullptr || tadOffsets == nullptr) {
                    auto tadPack = sd::ConstantTadHelper::getInstance().tadForDimensions(yShapeInfo, dimension, dimensionLength);

                    yTadShapeShapeInfo = const_cast<Nd4jLong*>(tadPack.primaryShapeInfo());
                    tadOffsets = const_cast<Nd4jLong*>(tadPack.primaryOffsets());
                }

                //int *resultStride = shape::stride(yTadShapeShapeInfo);
                unsigned int tadLength = shape::length(yTadShapeShapeInfo);
                unsigned int tads = shape::length(yShapeInfo) / tadLength;

                if (zTadShapeInfo == nullptr) {
                    zTadShapeInfo = yTadShapeShapeInfo;
                    zTadOffset = tadOffsets;
                }

                auto lenZ = shape::length(zTadShapeInfo);
                auto lenX = shape::length(xShapeInfo);

                int tadsPerThread = tads / TAD_THRESHOLD;
                int threads = sd::math::nd4j_max<int>(1, tadsPerThread);
                threads = sd::math::nd4j_min<int>(threads, sd::Environment::getInstance().maxThreads());

                auto yEws = shape::elementWiseStride(yTadShapeShapeInfo);
                auto xEws = shape::elementWiseStride(xShapeInfo);
                auto zEws = shape::elementWiseStride(zTadShapeInfo);

                const sd::LoopKind::Kind kindOfLoop = sd::LoopKind::deduceKindOfLoopXYZ(yTadShapeShapeInfo, xShapeInfo, zTadShapeInfo);

                if (kindOfLoop == sd::LoopKind::EWS1) {
                    for (auto i = start; i < stop; i ++) {
                        auto oY = y + tadOffsets[i];
                        auto oZ = z + zTadOffset[i];

                        PRAGMA_OMP_SIMD
                        for (unsigned int f = 0; f < tadLength; f++)
                            oZ[f] = OpType::op(x[f], oY[f]);
                    };
                }
                else if(kindOfLoop == sd::LoopKind::EWSNONZERO) {
                    for (auto i = start; i < stop; i ++) {
                        auto oY = y + tadOffsets[i];
                        auto oZ = z + zTadOffset[i];

                        PRAGMA_OMP_SIMD
                        for (uint f = 0; f < tadLength; f++)
                            oZ[f * zEws] = OpType::op(x[f * xEws], oY[f * yEws]);
                    };
                }
                else if(shape::haveSameShapeAndStrides(yTadShapeShapeInfo, xShapeInfo) && shape::haveSameShapeAndStrides(yTadShapeShapeInfo, zTadShapeInfo)) {
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    bool canCastY = sd::DataTypeUtils::castShapeInfo(yTadShapeShapeInfo, tadShapeShapeInfoCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oY = y + tadOffsets[i];
                        auto oZ = z + zTadOffset[i];

                        PRAGMA_OMP_SIMD
                        for (uint f = 0; f < tadLength; f++) {
                            auto offset = shape::indexOffset(f, yTadShapeShapeInfo, tadShapeShapeInfoCast, canCastY);
                            oZ[offset] = OpType::op(x[offset], oY[offset]);
                        }
                    };
                }
                else if(shape::haveSameShapeAndStrides(yTadShapeShapeInfo, xShapeInfo)) {

                    uint tadShapeShapeInfoCast[MAX_RANK];
                    uint tadShapeInfoZCast[MAX_RANK];
                    bool canCastY = sd::DataTypeUtils::castShapeInfo(yTadShapeShapeInfo, tadShapeShapeInfoCast);
                    bool canCastZ = sd::DataTypeUtils::castShapeInfo(zTadShapeInfo, tadShapeInfoZCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oY = y + tadOffsets[i];

                        for (uint f = 0; f < tadLength; f++) {
                            auto offset = shape::indexOffset(f, yTadShapeShapeInfo, tadShapeShapeInfoCast, canCastY);
                            auto zOffset = shape::indexOffset(f, zTadShapeInfo, tadShapeInfoZCast, canCastZ);
                            oZ[zOffset] = OpType::op(x[offset], oY[offset]);
                        }
                    };
                }
                else if(shape::haveSameShapeAndStrides(yTadShapeShapeInfo, zTadShapeInfo)) {
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    uint xShapeInfoCast[MAX_RANK];
                    bool canCastX = sd::DataTypeUtils::castShapeInfo(xShapeInfo, xShapeInfoCast);
                    bool canCastY = sd::DataTypeUtils::castShapeInfo(yTadShapeShapeInfo, tadShapeShapeInfoCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oY = y + tadOffsets[i];

                        PRAGMA_OMP_SIMD
                        for (uint f = 0; f < tadLength; f++) {
                            auto offset = shape::indexOffset(f, yTadShapeShapeInfo, tadShapeShapeInfoCast, canCastY);
                            auto xOffset = shape::indexOffset(f, xShapeInfo, xShapeInfoCast, canCastX);
                            oZ[offset] = OpType::op(x[xOffset], oY[offset]);
                        }
                    };
                }
                else if(shape::haveSameShapeAndStrides(xShapeInfo, zTadShapeInfo)) {
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    uint xShapeInfoCast[MAX_RANK];
                    bool canCastX = sd::DataTypeUtils::castShapeInfo(xShapeInfo, xShapeInfoCast);
                    bool canCastY = sd::DataTypeUtils::castShapeInfo(yTadShapeShapeInfo, tadShapeShapeInfoCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oY = y + tadOffsets[i];

                        PRAGMA_OMP_SIMD
                        for (uint f = 0; f < tadLength; f++) {
                            auto yOffset = shape::indexOffset(f, yTadShapeShapeInfo, tadShapeShapeInfoCast, canCastY);
                            auto offset = shape::indexOffset(f, xShapeInfo, xShapeInfoCast, canCastX);
                            oZ[offset] = OpType::op(x[offset], oY[yOffset]);
                        }
                    };
                }
                else {
                    uint xShapeInfoCast[MAX_RANK];
                    uint tadShapeShapeInfoCast[MAX_RANK];
                    uint tadShapeInfoZCast[MAX_RANK];
                    bool canCastX = sd::DataTypeUtils::castShapeInfo(xShapeInfo, xShapeInfoCast);
                    bool canCastY = sd::DataTypeUtils::castShapeInfo(yTadShapeShapeInfo, tadShapeShapeInfoCast);
                    bool canCastZ = sd::DataTypeUtils::castShapeInfo(zTadShapeInfo, tadShapeInfoZCast);

                    for (auto i = start; i < stop; i ++) {
                        auto oZ = z + zTadOffset[i];
                        auto oY = y + tadOffsets[i];

                        PRAGMA_OMP_SIMD
                        for (uint f = 0; f < tadLength; f++) {
                            auto xOffset = shape::indexOffset(f, xShapeInfo, xShapeInfoCast, canCastX);
                            auto yOffset = shape::indexOffset(f, yTadShapeShapeInfo, tadShapeShapeInfoCast, canCastY);
                            auto zOffset = shape::indexOffset(f, zTadShapeInfo, tadShapeInfoZCast, canCastZ);
                            oZ[zOffset] = OpType::op(x[xOffset], oY[yOffset]);
                        }
                    };
                }
        }

////////////////////////////////////////////////////////////////////////
template <typename X, typename OpType>
static void execRank1(const X *x, const Nd4jLong *xShapeInfo, const X *y, const Nd4jLong *yShapeInfo, X* z, const Nd4jLong *zShapeInfo) {

    uint     zAxis0 = shape::sizeAt(zShapeInfo,   0);
    Nd4jLong xStrd0 = shape::strideAt(xShapeInfo, 0);
    Nd4jLong yStrd0 = shape::strideAt(yShapeInfo, 0);
    Nd4jLong zStrd0 = shape::strideAt(zShapeInfo, 0);

    auto func = PRAGMA_THREADS_FOR{

        if(zStrd0 == 1 && xStrd0 == 1 && yStrd0 == 0) {
            for (auto i0 = start; i0 < stop; ++i0)
                z[i0] = OpType::op(x[i0], *y);
        }
        else if(zStrd0 == 1 && xStrd0 == 0 && yStrd0 == 1) {
            for (auto i0 = start; i0 < stop; ++i0)
                z[i0] = OpType::op(*x, y[i0]);
        }
        else if(zStrd0 == 1 && xStrd0 == 1 && yStrd0 == 1) {
            for (auto i0 = start; i0 < stop; ++i0)
                z[i0] = OpType::op(x[i0], y[i0]);
        }
        else {
            for (auto i0 = start; i0 < stop; ++i0)
                z[i0 * zStrd0] = OpType::op(x[i0 * xStrd0], y[i0 * yStrd0]);
        }
    };
    samediff::Threads::parallel_tad(func, 0, zAxis0);
}

////////////////////////////////////////////////////////////////////////
template <typename X, typename OpType>
static void execRank2(const X *x, const Nd4jLong *xShapeInfo, const X *y, const Nd4jLong *yShapeInfo, X* z, const Nd4jLong *zShapeInfo) {

    uint     zAxis0 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 0 : 1);
    Nd4jLong xStrd0 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 1);
    Nd4jLong yStrd0 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 1);
    Nd4jLong zStrd0 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 1);

    uint     zAxis1 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 1 : 0);
    Nd4jLong xStrd1 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 0);
    Nd4jLong yStrd1 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 0);
    Nd4jLong zStrd1 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 0);

    auto func = PRAGMA_THREADS_FOR{

        for (auto i0 = start; i0 < stop; ++i0) {

            auto x0 = x + i0 * xStrd0;
            auto y0 = y + i0 * yStrd0;
            auto z0 = z + i0 * zStrd0;

            if(zStrd1 == 1 && xStrd1 == 1 && yStrd1 == 0)
                for (uint i1 = 0; i1 < zAxis1; ++i1)
                    z0[i1] = OpType::op(x0[i1], *y0);
            else if(zStrd1 == 1 && xStrd1 == 0 && yStrd1 == 1)
                for (uint i1 = 0; i1 < zAxis1; ++i1)
                    z0[i1] = OpType::op(*x0, y0[i1]);
            else if(zStrd1 == 1 && xStrd1 == 1 && yStrd1 == 1)
                for (uint i1 = 0; i1 < zAxis1; ++i1)
                    z0[i1] = OpType::op(x0[i1], y0[i1]);
            else
                for (uint i1 = 0; i1 < zAxis1; ++i1)
                    z0[i1 * zStrd1] = OpType::op(x0[i1 * xStrd1], y0[i1 * yStrd1]);
        }
    };

    samediff::Threads::parallel_tad(func, 0, zAxis0);
}

////////////////////////////////////////////////////////////////////////
template <typename X, typename OpType>
static void execRank3(const X *x, const Nd4jLong *xShapeInfo, const X *y, const Nd4jLong *yShapeInfo, X* z, const Nd4jLong *zShapeInfo) {

    uint     zAxis0 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 0 : 2);
    Nd4jLong xStrd0 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 2);
    Nd4jLong yStrd0 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 2);
    Nd4jLong zStrd0 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 2);

    uint     zAxis1 = shape::sizeAt(zShapeInfo,   1);
    Nd4jLong xStrd1 = shape::strideAt(xShapeInfo, 1);
    Nd4jLong yStrd1 = shape::strideAt(yShapeInfo, 1);
    Nd4jLong zStrd1 = shape::strideAt(zShapeInfo, 1);

    uint     zAxis2 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 2 : 0);
    Nd4jLong xStrd2 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 2 : 0);
    Nd4jLong yStrd2 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 2 : 0);
    Nd4jLong zStrd2 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 2 : 0);

      auto func = PRAGMA_THREADS_FOR_2D {

        for (auto i0 = start_x; i0 < stop_x; ++i0) {
            for (auto i1 = start_y; i1 < stop_y; ++i1) {

                auto x1 = x + i0 * xStrd0 + i1 * xStrd1;
                auto y1 = y + i0 * yStrd0 + i1 * yStrd1;
                auto z1 = z + i0 * zStrd0 + i1 * zStrd1;

                if(zStrd2 == 1 && xStrd2 == 1 && yStrd2 == 0)
                    for (uint i2 = 0; i2 < zAxis2; ++i2)
                        z1[i2] = OpType::op(x1[i2], *y1);
                else if(zStrd2 == 1 && xStrd2 == 0 && yStrd2 == 1)
                    for (uint i2 = 0; i2 < zAxis2; ++i2)
                        z1[i2] = OpType::op(*x1, y1[i2]);
                else if(zStrd2 == 1 && xStrd2 == 1 && yStrd2 == 1)
                    for (uint i2 = 0; i2 < zAxis2; ++i2)
                        z1[i2] = OpType::op(x1[i2], y1[i2]);
                else
                    for (uint i2 = 0; i2 < zAxis2; ++i2)
                        z1[i2 * zStrd2] = OpType::op(x1[i2 * xStrd2], y1[i2 * yStrd2]);
            }
        }
    };

    samediff::Threads::parallel_for(func, 0,zAxis0,1,  0,zAxis1,1);
}

////////////////////////////////////////////////////////////////////////
template <typename X, typename OpType>
static void execRank4(const X *x, const Nd4jLong *xShapeInfo, const X *y, const Nd4jLong *yShapeInfo, X* z, const Nd4jLong *zShapeInfo) {

    uint     zAxis0 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 0 : 3);
    Nd4jLong xStrd0 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 3);
    Nd4jLong yStrd0 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 3);
    Nd4jLong zStrd0 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 3);

    uint     zAxis1 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 1 : 2);
    Nd4jLong xStrd1 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 2);
    Nd4jLong yStrd1 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 2);
    Nd4jLong zStrd1 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 2);

    uint     zAxis2 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 2 : 1);
    Nd4jLong xStrd2 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 2 : 1);
    Nd4jLong yStrd2 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 2 : 1);
    Nd4jLong zStrd2 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 2 : 1);

    uint     zAxis3 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 3 : 0);
    Nd4jLong xStrd3 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 3 : 0);
    Nd4jLong yStrd3 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 3 : 0);
    Nd4jLong zStrd3 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 3 : 0);

     auto func = PRAGMA_THREADS_FOR_3D {

        for (auto i0 = start_x; i0 < stop_x; ++i0) {
            for (auto i1 = start_y; i1 < stop_y; ++i1) {
                for (auto i2 = start_z; i2 < stop_z; ++i2) {

                    auto x2 = x + i0 * xStrd0 + i1 * xStrd1 + i2 * xStrd2;
                    auto y2 = y + i0 * yStrd0 + i1 * yStrd1 + i2 * yStrd2;
                    auto z2 = z + i0 * zStrd0 + i1 * zStrd1 + i2 * zStrd2;

                    if(zStrd3 == 1 && xStrd3 == 1 && yStrd3 == 0)
                        for (uint i3 = 0; i3 < zAxis3; ++i3)
                            z2[i3] = OpType::op(x2[i3], *y2);
                    else if(zStrd3 == 1 && xStrd3 == 0 && yStrd3 == 1)
                        for (uint i3 = 0; i3 < zAxis3; ++i3)
                            z2[i3] = OpType::op(*x2, y2[i3]);
                    else if(zStrd3 == 1 && xStrd3 == 1 && yStrd3 == 1)
                        for (uint i3 = 0; i3 < zAxis3; ++i3)
                            z2[i3] = OpType::op(x2[i3], y2[i3]);
                    else
                        for (uint i3 = 0; i3 < zAxis3; ++i3)
                            z2[i3 * zStrd3] = OpType::op(x2[i3 * xStrd3], y2[i3 * yStrd3]);
                }
            }
        }
    };

    samediff::Threads::parallel_for(func,  0,zAxis0,1,  0,zAxis1,1,  0,zAxis2,1);
}

////////////////////////////////////////////////////////////////////////
template <typename X, typename OpType>
static void execRank5(const X *x, const Nd4jLong *xShapeInfo, const X *y, const Nd4jLong *yShapeInfo, X* z, const Nd4jLong *zShapeInfo) {

    uint     zAxis0 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 0 : 4);
    Nd4jLong xStrd0 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 4);
    Nd4jLong yStrd0 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 4);
    Nd4jLong zStrd0 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 0 : 4);

    uint     zAxis1 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 1 : 3);
    Nd4jLong xStrd1 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 3);
    Nd4jLong yStrd1 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 3);
    Nd4jLong zStrd1 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 1 : 3);

    uint     zAxis2 = shape::sizeAt(zShapeInfo,   2);
    Nd4jLong xStrd2 = shape::strideAt(xShapeInfo, 2);
    Nd4jLong yStrd2 = shape::strideAt(yShapeInfo, 2);
    Nd4jLong zStrd2 = shape::strideAt(zShapeInfo, 2);

    uint     zAxis3 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 3 : 1);
    Nd4jLong xStrd3 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 3 : 1);
    Nd4jLong yStrd3 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 3 : 1);
    Nd4jLong zStrd3 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 3 : 1);

    uint     zAxis4 = shape::sizeAt(zShapeInfo,   shape::order(zShapeInfo) == 'c' ? 4 : 0);
    Nd4jLong xStrd4 = shape::strideAt(xShapeInfo, shape::order(zShapeInfo) == 'c' ? 4 : 0);
    Nd4jLong yStrd4 = shape::strideAt(yShapeInfo, shape::order(zShapeInfo) == 'c' ? 4 : 0);
    Nd4jLong zStrd4 = shape::strideAt(zShapeInfo, shape::order(zShapeInfo) == 'c' ? 4 : 0);

    auto func = PRAGMA_THREADS_FOR_3D {

        for (auto i0 = start_x; i0 < stop_x; ++i0) {
            for (auto i1 = start_y; i1 < stop_y; ++i1) {
                for (auto i2 = start_z; i2 < stop_z; ++i2) {
                    for (uint i3 = 0; i3 < zAxis3; ++i3) {

                        auto x3 = x + i0 * xStrd0 + i1 * xStrd1 + i2 * xStrd2 + i3 * xStrd3;
                        auto y3 = y + i0 * yStrd0 + i1 * yStrd1 + i2 * yStrd2 + i3 * yStrd3;
                        auto z3 = z + i0 * zStrd0 + i1 * zStrd1 + i2 * zStrd2 + i3 * zStrd3;

                       if(zStrd4 == 1 && xStrd4 == 1 && yStrd4 == 0)
                            for (uint i4 = 0; i4 < zAxis4; ++i4)
                                z3[i4] = OpType::op(x3[i4], *y3);
                        else if(zStrd4 == 1 && xStrd4 == 0 && yStrd4 == 1)
                            for (uint i4 = 0; i4 < zAxis4; ++i4)
                                z3[i4] = OpType::op(*x3, y3[i4]);
                        else if(zStrd4 == 1 && xStrd4 == 1 && yStrd4 == 1)
                            for (uint i4 = 0; i4 < zAxis4; ++i4)
                                z3[i4] = OpType::op(x3[i4], y3[i4]);
                        else
                            for (uint i4 = 0; i4 < zAxis4; ++i4)
                                z3[i4 * zStrd4] = OpType::op(x3[i4 * xStrd4], y3[i4 * yStrd4]);
                    }
                }
            }
        }
    };

    samediff::Threads::parallel_for(func,  0,zAxis0,1,  0,zAxis1,1,  0,zAxis2,1);
}

////////////////////////////////////////////////////////////////////////
template <typename X, typename OpType>
static void execDefault(const X *x, const Nd4jLong *xShapeInfo, const X *y, const Nd4jLong *yShapeInfo, X* z, const Nd4jLong *zShapeInfo) {

    const bool xzSameOffsets = shape::haveSameShapeAndStrides(xShapeInfo, zShapeInfo);
    const bool yzSameOffsets = shape::haveSameShapeAndStrides(yShapeInfo, zShapeInfo);

    auto func = PRAGMA_THREADS_FOR{

        int coords[MAX_RANK];
        Nd4jLong xOffset, yOffset, zOffset;

        for (auto i = start; i < stop; ++i) {

            shape::getOffsetBroadcast(start, i, zShapeInfo, xShapeInfo, yShapeInfo, xzSameOffsets, yzSameOffsets, coords, zOffset, xOffset, yOffset);

            z[zOffset] = OpType::op(x[xOffset], y[yOffset]);
        }
    };

    samediff::Threads::parallel_for(func, 0, shape::length(zShapeInfo));
}

////////////////////////////////////////////////////////////////////////
template <typename X>
template<typename OpType>
void BroadcastInt<X>::exec(const void *vx, const Nd4jLong *xShapeInfo,
                           const void *vy, const Nd4jLong *yShapeInfo,
                                 void *vz, const Nd4jLong *zShapeInfo) {

    const X* x = reinterpret_cast<const X*>(vx);
    const X* y = reinterpret_cast<const X*>(vy);
          X* z = reinterpret_cast<X*>(vz);

    const int rank   = shape::rank(zShapeInfo);    // xRank = yRank = zRank

    switch (rank) {

        case 1:
            execRank1<X, OpType>(x, xShapeInfo, y, yShapeInfo, z, zShapeInfo);
            break;
        case 2:
            execRank2<X, OpType>(x, xShapeInfo, y, yShapeInfo, z, zShapeInfo);
            break;
        case 3:
            execRank3<X, OpType>(x, xShapeInfo, y, yShapeInfo, z, zShapeInfo);
            break;
        case 4:
            execRank4<X, OpType>(x, xShapeInfo, y, yShapeInfo, z, zShapeInfo);
            break;
        case 5:
            execRank5<X, OpType>(x, xShapeInfo, y, yShapeInfo, z, zShapeInfo);
            break;
        default:
            execDefault<X, OpType>(x, xShapeInfo, y, yShapeInfo, z, zShapeInfo);
    }
}

//BUILD_SINGLE_TEMPLATE(template class ND4J_LOCAL BroadcastInt, , INTEGER_TYPES);
}
}