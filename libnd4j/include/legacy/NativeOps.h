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
// Created by agibsonccc on 2/21/16.
//

#ifndef NATIVEOPERATIONS_NATIVEOPS_H
#define NATIVEOPERATIONS_NATIVEOPS_H

/*
#ifndef thread_local
# if __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#  define thread_local _Thread_local
# elif defined _WIN32 && ( \
       defined _MSC_VER || \
       defined __ICL || \
       defined __DMC__ || \
       defined __BORLANDC__ )
#  define thread_local __declspec(thread)
// note that ICC (linux) and Clang are covered by __GNUC__ 
# elif defined __GNUC__ || \
       defined __SUNPRO_C || \
       defined __xlC__
#  define thread_local __thread
# else
#  error "Cannot define thread_local"
# endif
#endif
*/

#include <system/pointercast.h>
#include <types/float16.h>
#include <cnpy/cnpy.h>

//DO NOT REMOVE: THIS IS AN EDITOR SEMANTICS THING FOR CLION
//IT DEFINES THE EXPORT MACRO FOR THE EDITOR AND THEN
//RE ADDS THE DEFINITION VIA dll.h
#ifdef  _WIN32
#define ND4J_EXPORT __declspec(dllexport)
#else
#define ND4J_EXPORT
#endif
#include <system/dll.h>

/*
int tad_threshold = 1;
int element_threshold = 32;

bool debug = false;
bool verbose = false;
*/

#include <array/ShapeList.h>
#include <array/ConstantDescriptor.h>
#include <helpers/ConstantShapeHelper.h>
#include <array/ConstantDataBuffer.h>
#include <array/InteropDataBuffer.h>
#include <helpers/ConstantHelper.h>
#include <array/TadPack.h>
#include <graph/VariablesSet.h>
#include <graph/GraphState.h>
#include <graph/execution/LogicExecutor.h>
#include <graph/ResultWrapper.h>
#include <helpers/DebugInfo.h>
#include <memory/MemoryCounter.h>

typedef sd::InteropDataBuffer OpaqueDataBuffer;

extern "C" {

/**
 * This function returns last error code stored,
 * @return non-zero if something bad happened
 */
ND4J_EXPORT int lastErrorCode();

/**
 * This function returns last error message, if last error code > 0
 * @return
 */
ND4J_EXPORT const char* lastErrorMessage();

/**
 *
 * @param p
 * @param len
 */
ND4J_EXPORT void tryPointer(Nd4jPointer extra, Nd4jPointer p, int len);

/**
 *
 * @param num
 */
ND4J_EXPORT void setElementThreshold(int num);

/**
 *
 * @param num
 */
ND4J_EXPORT void setTADThreshold(int num);

/**
   *
   * @param opNum
   * @param x
   * @param xShapeInfo
   * @param extraParams
   */
ND4J_EXPORT void execIndexReduceScalar(Nd4jPointer *extraPointers,
                                       int opNum,
                                       OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                       void *extraParams,
                                       OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong  const* dZShapeInfo);

/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParams
 * @param result
 * @param resultShapeInfoBuffer
 * @param dimension
 * @param dimensionLength
 */
ND4J_EXPORT void   execIndexReduce(Nd4jPointer *extraPointers,
                                   int opNum,
                                   OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong  const* dXShapeInfo,
                                   void *extraParams,
                                   OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                   OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape);

/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param y
 * @param yShapeInfo
 * @param result
 * @param resultShapeInfo
 * @param dimension
 * @param dimensionLength
 */
ND4J_EXPORT void   execBroadcast(
        Nd4jPointer *extraPointers,
        int opNum,
        OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
        OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeInfo, Nd4jLong const* dYShapeInfo,
        OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
        OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape);


ND4J_EXPORT void   execBroadcastBool(
        Nd4jPointer *extraPointers,
        int opNum,
        OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
        OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeInfo, Nd4jLong const* dYShapeInfo,
        OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
        void *extraParams,
        OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape);

/**
 *
 * @param opNum
 * @param dx
 * @param xShapeInfo
 * @param y
 * @param yShapeInfo
 * @param result
 * @param resultShapeInfo
 * @param extraParams
 * @param n
 */
ND4J_EXPORT void execPairwiseTransform(
        Nd4jPointer *extraPointers,
        int opNum,
        OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
        OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeInfo, Nd4jLong const* dYShapeInfo,
        OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
        void *extraParams);

ND4J_EXPORT void execPairwiseTransformBool(
        Nd4jPointer *extraPointers,
        int opNum,
        OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
        OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeInfo, Nd4jLong const* dYShapeInfo,
        OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
        void *extraParams);

/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParams
 * @param result
 * @param resultShapeInfo
 */
ND4J_EXPORT void  execReduceFloat(Nd4jPointer *extraPointers,
                                  int opNum,
                                  OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                  void *extraParams,
                                  OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo);

ND4J_EXPORT void  execReduceSame(Nd4jPointer *extraPointers,
                                 int opNum,
                                 OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                 void *extraParams,
                                 OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo);

ND4J_EXPORT void  execReduceBool(Nd4jPointer *extraPointers,
                                 int opNum,
                                 OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                 void *extraParams,
                                 OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo);


ND4J_EXPORT void  execReduceLong(Nd4jPointer *extraPointers,
                                 int opNum,
                                 OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                 void *extraParams,
                                 OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo);

/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParams
 * @param result
 * @param resultShapeInfo
 */
ND4J_EXPORT void   execReduceFloat2(Nd4jPointer *extraPointers,
                                    int opNum,
                                    OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                    void *extraParams,
                                    OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                    OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape);


ND4J_EXPORT void   execReduceSame2(Nd4jPointer *extraPointers,
                                   int opNum,
                                   OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                   void *extraParams,
                                   OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                   OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape);


ND4J_EXPORT void   execReduceBool2(Nd4jPointer *extraPointers,
                                   int opNum,
                                   OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                   void *extraParams,
                                   OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                   OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape);


ND4J_EXPORT void   execReduceLong2(Nd4jPointer *extraPointers,
                                   int opNum,
                                   OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                   void *extraParams,
                                   OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                   OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape);

/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParamsVals
 * @param y
 * @param yShapeInfo
 * @param result
 * @param resultShapeInfo
 */
ND4J_EXPORT void  execReduce3(Nd4jPointer *extraPointers,
                              int opNum,
                              OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                              void *extraParamsVals,
                              OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeInfo, Nd4jLong const* dYShapeInfo,
                              OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo);

/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParamsVals
 * @param y
 * @param yShapeInfo
 */
ND4J_EXPORT void execReduce3Scalar(Nd4jPointer *extraPointers,
                                   int opNum,
                                   OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                   void *extraParamsVals,
                                   OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeInfo, Nd4jLong const* dYShapeInfo,
                                   OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo);
/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParamsVals
 * @param y
 * @param yShapeInfo
 * @param result
 * @param resultShapeInfoBuffer
 * @param dimension
 * @param dimensionLength
 */
ND4J_EXPORT void execReduce3Tad(Nd4jPointer *extraPointers,
                                int opNum,
                                OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                void *extraParamsVals,
                                OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeInfo, Nd4jLong const* dYShapeInfo,
                                OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape,
                                Nd4jLong const* tadOnlyShapeInfo, Nd4jLong const* tadOffsets,
                                Nd4jLong const* yTadOnlyShapeInfo, Nd4jLong const* yTadOffsets);


ND4J_EXPORT void execReduce3All(Nd4jPointer *extraPointers,
                                int opNum,
                                OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                void *extraParamsVals,
                                OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeInfo, Nd4jLong const* dYShapeInfo,
                                OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape,
                                Nd4jLong const* xTadShapeInfo, Nd4jLong const* xOffsets,
                                Nd4jLong const* yTadShapeInfo, Nd4jLong const* yOffsets);

/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param result
 * @param resultShapeInfo
 * @param scalar
 * @param extraParams
 * @param n
 */
ND4J_EXPORT void execScalar(Nd4jPointer *extraPointers,
                            int opNum,
                            OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                            OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                            OpaqueDataBuffer *dbScalar, Nd4jLong const* hSscalarShapeInfo, Nd4jLong const* dSscalarShapeInfo,
                            void *extraParams);

ND4J_EXPORT void execScalarBool(Nd4jPointer *extraPointers,
                                int opNum,
                                OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                OpaqueDataBuffer *dbScalar, Nd4jLong const* hSscalarShapeInfo, Nd4jLong const* dSscalarShapeInfo,
                                void *extraParams);

/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParams
 */
ND4J_EXPORT void execSummaryStatsScalar(Nd4jPointer *extraPointers,
                                        int opNum,
                                        OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                        void *extraParams,
                                        OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                        bool biasCorrected);
/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParams
 * @param result
 * @param resultShapeInfo
 */
ND4J_EXPORT void execSummaryStats(Nd4jPointer *extraPointers,
                                  int opNum,
                                  OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo,  Nd4jLong const* dXShapeInfo,
                                  void *extraParams,
                                  OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                  bool biasCorrected);
/**
 *
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param extraParams
 * @param result
 * @param resultShapeInfoBuffer
 * @param dimension
 * @param dimensionLength
 */
ND4J_EXPORT void execSummaryStatsTad(Nd4jPointer *extraPointers,
                                     int opNum,
                                     OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                     void *extraParams,
                                     OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                     OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape,
                                     bool biasCorrected,
                                     Nd4jLong const* tadShapeInfo, Nd4jLong const* tadOffsets);

/**
 *
 * @param opNum
 * @param dx
 * @param xShapeInfo
 * @param result
 * @param resultShapeInfo
 * @param extraParams
 * @param n
 */
ND4J_EXPORT void execTransformFloat(Nd4jPointer *extraPointers,
                                    int opNum,
                                    OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                    OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                    void *extraParams);

ND4J_EXPORT void execTransformSame(Nd4jPointer *extraPointers,
                                   int opNum,
                                   OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                   OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                   void *extraParams);

ND4J_EXPORT void execTransformBool(Nd4jPointer *extraPointers,
                                   int opNum,
                                   OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                   OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                   void *extraParams);

ND4J_EXPORT void execTransformAny(Nd4jPointer *extraPointers,
                                  int opNum,
                                  OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                  OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                  void *extraParams);

ND4J_EXPORT void execTransformStrict(Nd4jPointer *extraPointers,
                                     int opNum,
                                     OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                     OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                     void *extraParams);

/**
 *
 * @param extraPointers
 * @param opNum
 * @param x
 * @param xShapeInfo
 * @param z
 * @param zShapeInfo
 * @param scalars
 * @param extraParams
 * @param dimension
 * @param dimensionLength
 */
ND4J_EXPORT void execScalarTad(Nd4jPointer *extraPointers,
                               int opNum,
                               OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                               OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                               OpaqueDataBuffer *dbScalars, Nd4jLong const* hScalarShapeInfo, Nd4jLong const* dScalarShapeInfo,
                               void *extraParams,
                               OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape,
                               Nd4jLong const* tadShapeInfo, Nd4jLong const* tadOffsets,
                               Nd4jLong const* tadShapeInfoZ, Nd4jLong const* tadOffsetsZ);

ND4J_EXPORT void execScalarBoolTad(Nd4jPointer *extraPointers,
                                   int opNum,
                                   OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeInfo, Nd4jLong const* dXShapeInfo,
                                   OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeInfo, Nd4jLong const* dZShapeInfo,
                                   OpaqueDataBuffer *dbScalars, Nd4jLong const* hScalarShapeInfo, Nd4jLong const* dScalarShapeInfo,
                                   void *extraParams,
                                   OpaqueDataBuffer *dbDimension, Nd4jLong const* hDimensionShape, Nd4jLong const* dDimensionShape,
                                   Nd4jLong const* tadShapeInfo, Nd4jLong const* tadOffsets,
                                   Nd4jLong const* tadShapeInfoZ, Nd4jLong const* tadOffsetsZ);

ND4J_EXPORT void specialConcat (
        Nd4jPointer *extraPointers,
        int dimension,
        int numArrays,
        Nd4jPointer *data,
        Nd4jPointer *inputShapeInfo,
        void *result,
        Nd4jLong const* resultShapeInfo,
        Nd4jPointer *tadPointers,
        Nd4jPointer *offsetPointers);

/**
 * This method implementation exists only for cuda.
 * The other backends should have dummy method for JNI compatibility reasons.
 */
ND4J_EXPORT void initializeDevicesAndFunctions();

ND4J_EXPORT void initializeFunctions(Nd4jPointer *functions);

/**
 * This method acquires memory chunk of requested size on host side
 *
 * @param pointer pointer that'll be used for allocation
 * @param memorySize memory size, in bytes
 * @param flags optional parameter
 */
ND4J_EXPORT Nd4jPointer mallocHost(Nd4jLong memorySize, int flags);

/**
 * This method acquires memory chunk of requested size on specified device
 *
 * @param pointer pointer that'll be used for allocation
 * @param memorySize memory size, in bytes
 * @param ptrToDeviceId pointer to deviceId. For cuda that's just and int, for OpenCL that's pointer to device_id, etc
 * @param flags optional parameter
 */
ND4J_EXPORT Nd4jPointer mallocDevice(Nd4jLong memorySize, int deviceId, int flags);

/**
 * This method releases previously allocated host memory space
 *
 * @param pointer pointer that'll be freed
 */
ND4J_EXPORT int freeHost(Nd4jPointer pointer);

/**
 * This method releases previously allocated memory space on device
 *
 * @param pointer pointer that'll be freed
 * @param ptrToDeviceId pointer to deviceId.
 */
ND4J_EXPORT int freeDevice(Nd4jPointer pointer, int deviceId);

/**
 *
 * @return
 */
ND4J_EXPORT int ompGetMaxThreads();

/**
 *
 * @return
 */
ND4J_EXPORT int ompGetNumThreads();

/**
 *
 * @param threads
 */
ND4J_EXPORT void setOmpNumThreads(int threads);

/**
 *
 * @param threads
 */
ND4J_EXPORT void setOmpMinThreads(int threads);


ND4J_EXPORT bool isBlasVersionMatches(int major, int minor, int build);

/**
 *
 * @return
 */
ND4J_EXPORT Nd4jPointer createContext();

/**
 *
 * @return
 */
ND4J_EXPORT Nd4jPointer createStream();

/**
 *
 * @return
 */
ND4J_EXPORT Nd4jPointer createEvent();

/**
 *
 * @param event
 * @param stream
 * @return
 */
ND4J_EXPORT int registerEvent(Nd4jPointer event, Nd4jPointer stream);

/**
 *
 * @param event
 * @return
 */
ND4J_EXPORT int destroyEvent(Nd4jPointer event);

/**
 *
 * @param ptrToDeviceId
 * @return
 */
ND4J_EXPORT int setDevice(int deviceId);

/**
 *
 * @return
 */
ND4J_EXPORT int getDevice();

/**
 *
 * @param stream
 * @return
 */
ND4J_EXPORT int streamSynchronize(Nd4jPointer stream);

/**
 *
 * @param event
 * @return
 */
ND4J_EXPORT int eventSynchronize(Nd4jPointer event);

/**
 *
 * @param ptrToDeviceId
 * @return
 */
ND4J_EXPORT Nd4jLong getDeviceFreeMemory(int deviceId);

/**
 * Returns amount of free memory for current device
 * @return
 */
ND4J_EXPORT Nd4jLong getDeviceFreeMemoryDefault();

/**
 *
 * @param ptrToDeviceId
 * @return
 */
ND4J_EXPORT Nd4jLong getDeviceTotalMemory(int deviceId);

/**
 *
 * @param ptrToDeviceId
 * @return
 */
ND4J_EXPORT int getDeviceMajor(int deviceId);

/**
 * This method returns amount of cached memory
 * @param deviceId
 * @return
 */
ND4J_EXPORT Nd4jLong getCachedMemory(int deviceId);

/**
 *
 * @param ptrToDeviceId
 * @return
 */
ND4J_EXPORT int getDeviceMinor(int deviceId);

/**
 *
 * @param ptrToDeviceId
 * @return
 */
ND4J_EXPORT const char * getDeviceName(int deviceId);

/**
 *
 * @param dst
 * @param src
 * @param size
 * @param flags
 * @param reserved
 * @return
 */
ND4J_EXPORT int memcpySync(Nd4jPointer dst,
                           Nd4jPointer src,
                           Nd4jLong size,
                           int flags,
                           Nd4jPointer reserved);

/**
 *
 * @param dst
 * @param src
 * @param size
 * @param flags
 * @param reserved
 * @return
 */
ND4J_EXPORT int memcpyAsync(Nd4jPointer dst,
                            Nd4jPointer src,
                            Nd4jLong size,
                            int flags,
                            Nd4jPointer reserved);

/**
 *
 * @param dst
 * @param value
 * @param size
 * @param flags
 * @param reserved
 * @return
 */
ND4J_EXPORT int memsetSync(Nd4jPointer dst,
                           int value,
                           Nd4jLong size,
                           int flags,
                           Nd4jPointer reserved);

/**
 *
 * @param dst
 * @param value
 * @param size
 * @param flags
 * @param reserved
 * @return
 */
ND4J_EXPORT int memsetAsync(Nd4jPointer dst,
                            int value,
                            Nd4jLong size,
                            int flags,
                            Nd4jPointer reserved);

/**
 *
 * @param dst
 * @param src
 * @param size
 * @param flags
 * @param reserved
 * @return
 */
ND4J_EXPORT int memcpyConstantAsync(Nd4jLong dst,
                                    Nd4jPointer src,
                                    Nd4jLong size,
                                    int flags,
                                    Nd4jPointer reserved);

/**
 *
 * @return
 */
ND4J_EXPORT Nd4jPointer getConstantSpace();

/**
 *
 * @return
 */
ND4J_EXPORT int getAvailableDevices();

/**
 *
 * @param reallyEnable
 */
ND4J_EXPORT void enableDebugMode(bool reallyEnable);

/**
 *
 * @param reallyEnable
 */
ND4J_EXPORT void enableVerboseMode(bool reallyEnable);

/**
 *
 * @param gridSize
 */
ND4J_EXPORT void setGridLimit(int gridSize);

typedef sd::TadPack OpaqueTadPack;

/**
 *
 * @param xShapeInfo
 * @param dimension
 * @param dimensionLength
 * @param targetBuffer
 * @param offsetsBuffer
 */
ND4J_EXPORT OpaqueTadPack* tadOnlyShapeInfo(Nd4jLong const*xShapeInfo,
                                            int *dimension,
                                            int dimensionLength);

ND4J_EXPORT Nd4jLong const* getPrimaryShapeInfo(OpaqueTadPack* pack);
ND4J_EXPORT Nd4jLong const* getPrimaryOffsets(OpaqueTadPack* pack);
ND4J_EXPORT Nd4jLong const* getSpecialShapeInfo(OpaqueTadPack* pack);
ND4J_EXPORT Nd4jLong const* getSpecialOffsets(OpaqueTadPack* pack);
ND4J_EXPORT Nd4jLong getNumberOfTads(OpaqueTadPack* pack);
ND4J_EXPORT int getShapeInfoLength(OpaqueTadPack* pack);

ND4J_EXPORT void deleteTadPack(OpaqueTadPack* ptr);

/*
 * PullRow special op
 */

/**
 *
 * @param extraPointers
 * @param x
 * @param xShapeInfo
 * @param z
 * @param zShapeInfo
 * @param n
 * @param indexes
 * @param tadShapeInfo
 * @param tadOffsets
 * @param zTadShapeInfo
 * @param zTadOffsets
 */
ND4J_EXPORT void pullRows(Nd4jPointer *extraPointers,
                          OpaqueDataBuffer *dbX, Nd4jLong const* xShapeInfo, Nd4jLong const* dxShapeInfo,
                          OpaqueDataBuffer *dbZ, Nd4jLong const* zShapeInfo, Nd4jLong const* dzShapeInfo,
                          Nd4jLong n,
                          Nd4jLong *indexes,
                          Nd4jLong const* tadShapeInfo,
                          Nd4jLong const* tadOffsets,
                          Nd4jLong const* zTadShapeInfo,
                          Nd4jLong const* zTadOffsets);

/**
 *
 * @param extras
 * @param dx
 * @param dz
 * @param n
 * @param length
 * @param propagate
 */
ND4J_EXPORT void average(Nd4jPointer *extras,
                         Nd4jPointer *x, Nd4jLong const* xShapeInfo,
                         Nd4jPointer *dx, Nd4jLong const* dxShapeInfo,
                         void *z, Nd4jLong const* zShapeInfo,
                         void *dz, Nd4jLong const* dzShapeInfo,
                         int n,
                         Nd4jLong length,
                         bool propagate);


ND4J_EXPORT void accumulate(Nd4jPointer *extras,
                            Nd4jPointer *x, Nd4jLong const* xShapeInfo,
                            Nd4jPointer *dx, Nd4jLong const* dxShapeInfo,
                            void *z, Nd4jLong const* zShapeInfo,
                            void *dz, Nd4jLong const* dzShapeInfo,
                            int n,
                            Nd4jLong length);


/**
 * P2P enabler
 */
/**
 *
 * @param enable
 */
ND4J_EXPORT void enableP2P(bool enable);

/**
 *
 */
ND4J_EXPORT void checkP2P();

/**
 *
 * @return
 */
ND4J_EXPORT bool isP2PAvailable();

/**
 * Shuffle methods
 */

/**
 *
 * @param extras
 * @param dx
 * @param xShapeInfo
 * @param dz
 * @param zShapeInfo
 * @param N
 * @param shuffleMap
 * @param tadShapeInfo
 * @param tadOffsets
 */
ND4J_EXPORT void shuffle(Nd4jPointer *extras,
                         Nd4jPointer *x, Nd4jPointer *xShapeInfo,
                         Nd4jPointer *dx, Nd4jPointer *dxShapeInfo,
                         Nd4jPointer *z, Nd4jPointer *zShapeInfo,
                         Nd4jPointer *dz, Nd4jPointer *dzShapeInfo,
                         int N,
                         int *shuffleMap,
                         Nd4jPointer *tadShapeInfo,
                         Nd4jPointer *tadOffsets);


/**
 * Type Conversions
 */

/**
 *
 * @param extras
 * @param srcType
 * @param x
 * @param N
 * @param dstType
 * @param z
 */
ND4J_EXPORT void convertTypes(Nd4jPointer *extras, int srcType, Nd4jPointer x, Nd4jLong N, int dstType, Nd4jPointer z);


/**
 *
 * @return
 */
ND4J_EXPORT bool isExperimentalEnabled();

/**
 * Aggregate
 */

/**
 *
 * @param extraPointers
 * @param opNum
 * @param arguments
 * @param numArguments
 * @param shapeArguments
 * @param numShapeArguments
 * @param indexArguments
 * @param numIndexArguments
 * @param intArrays
 * @param numIntArrays
 * @param realArguments
 * @param numRealArguments
 */
ND4J_EXPORT void execAggregate(Nd4jPointer *extraPointers,
                               int opNum,
                               void **arguments,
                               int numArguments,
                               Nd4jLong **shapeArguments,
                               int numShapeArguments,
                               int *indexArguments,
                               int numIndexArguments,
                               int **intArrays,
                               int numIntArrays,
                               void *realArguments,
                               int numRealArguments,
                               sd::DataType dtype);


ND4J_EXPORT void batchExecutor(Nd4jPointer *extraPointers,
                               int numAggregates,
                               int opNum,
                               int maxArgs,
                               int maxShapes,
                               int maxIntArrays,
                               int maxIntArraySize,
                               int maxIdx,
                               int maxReals,
                               void *ptrToArguments,
                               sd::DataType dtype);

ND4J_EXPORT void execAggregateBatch(Nd4jPointer *extraPointers,
                                    int numAggregates,
                                    int opNum,
                                    int maxArgs,
                                    int maxShapes,
                                    int maxIntArrays,
                                    int maxIntArraySize,
                                    int maxIdx,
                                    int maxReals,
                                    void *ptrToArguments,
                                    sd::DataType dtype);

/**
 * Random operations
 */

/**
 *
 * @param extraPointers
 * @param opNum
 * @param state
 * @param z
 * @param zShapeBuffer
 * @param extraArguments
 */
ND4J_EXPORT void execRandom(Nd4jPointer *extraPointers,
                            int opNum,
                            Nd4jPointer state,
                            OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeBuffer, Nd4jLong const* dZShapeBuffer,
                            void *extraArguments);

/**
 *
 * @param extraPointers
 * @param opNum
 * @param state
 * @param x
 * @param xShapeBuffer
 * @param y
 * @param yShapeBuffer
 * @param z
 * @param zShapeBuffer
 * @param extraArguments
 */
ND4J_EXPORT void execRandom3(Nd4jPointer *extraPointers,
                             int opNum,
                             Nd4jPointer state,
                             OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeBuffer, Nd4jLong const* dXShapeBuffer,
                             OpaqueDataBuffer *dbY, Nd4jLong const* hYShapeBuffer, Nd4jLong const* dYShapeBuffer,
                             OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeBuffer, Nd4jLong const* dZShapeBuffer,
                             void *extraArguments);

/**
 *
 * @param extraPointers
 * @param opNum
 * @param state
 * @param x
 * @param xShapeBuffer
 * @param z
 * @param zShapeBuffer
 * @param extraArguments
 */
ND4J_EXPORT void execRandom2(Nd4jPointer *extraPointers,
                             int opNum,
                             Nd4jPointer state,
                             OpaqueDataBuffer *dbX, Nd4jLong const* hXShapeBuffer, Nd4jLong const* dXShapeBuffer,
                             OpaqueDataBuffer *dbZ, Nd4jLong const* hZShapeBuffer, Nd4jLong const* dZShapeBuffer,
                             void *extraArguments);


/**
 *
 * @param extraPointers
 * @param seed
 * @param bufferSize
 * @param ptrToBuffer
 * @return
 */
ND4J_EXPORT Nd4jPointer initRandom(Nd4jPointer *extraPointers,
                                   long seed,
                                   long bufferSize,
                                   Nd4jPointer ptrToBuffer);

/**
 *
 * @param extraPointers
 * @param seed
 * @param ptrRandom
 */
ND4J_EXPORT void refreshBuffer(Nd4jPointer *extraPointers,
                               long seed,
                               Nd4jPointer ptrRandom);

/**
 *
 * @param extraPointers
 * @param seed
 * @param ptrRandom
 */
ND4J_EXPORT void reSeedBuffer(Nd4jPointer *extraPointers,
                              long seed,
                              Nd4jPointer ptrRandom);

/**
 *
 * @param ptrRandom
 */
ND4J_EXPORT void destroyRandom(Nd4jPointer ptrRandom);

}

/**
*
* @param data
* @param shapeBuffer
* @param wordSize
* @param headerSize
* @return
*/

template <typename T>
static Nd4jPointer _numpyHeaderForNd4j(Nd4jPointer data,const Nd4jPointer shapeBuffer,Nd4jLong wordSize,Nd4jLong* headerSize) {
    Nd4jLong const* shapeBufferCast = reinterpret_cast<const Nd4jLong *>(shapeBuffer);
    int  rank = shape::rank(shapeBufferCast);
    const Nd4jLong* shape = shape::shapeOf(shapeBufferCast);
    unsigned int* npShape = new unsigned int[rank];
    for(int i = 0; i < rank; i++) {
        npShape[i] = shape[i];
    }

    Nd4jLong length = shape::prodLong(shape,rank);
    auto npHeader = cnpy::createNpyHeader<T>(data,npShape,rank,wordSize);
    char *ret = new char[npHeader.size() + 1];
    int count = 0;
    for(int i = 0; i < npHeader.size(); i++) {
        ret[count] = npHeader[i];
        count++;
    }

    ret[count] = '\0';
    count++;

    *headerSize = count;
    return reinterpret_cast<Nd4jPointer>(ret);
}

extern "C" {

static Nd4jPointer numpyHeaderForNd4j(Nd4jPointer data,Nd4jPointer shapeBuffer,Nd4jLong wordSize,Nd4jLong* headerSize) {
    auto shapeBufferCast = reinterpret_cast<Nd4jLong *>(shapeBuffer);
    auto type = sd::ArrayOptions::dataType(shapeBufferCast);
    BUILD_SINGLE_SELECTOR(type, return _numpyHeaderForNd4j, (data, shapeBuffer, wordSize, headerSize), LIBND4J_TYPES);
}

/**
* Load numpy from a header
* based on the cnpy parse from header method.
* @param data the header data to parse
* @return a pointer to a numpy cnpy:NpyArray struct
*/
static Nd4jPointer loadNpyFromHeader(Nd4jPointer data) {
    char *header = reinterpret_cast<char *>(data);

    cnpy::NpyArray arr = cnpy::loadNpyFromHeader(header);
    cnpy::NpyArray *ret = new cnpy::NpyArray();
    int totalLengthOfShape = 1;
    for(int i = 0; i < arr.shape.size(); i++) {
        totalLengthOfShape *= arr.shape[i];
    }

    ret->data = arr.data;
    ret->wordSize = arr.wordSize;
    ret->shape = arr.shape;
    return reinterpret_cast<Nd4jPointer>(ret);
}

}

/**
* Create a numpy array from an nd4j
* array
* @param data a pointer to the data
* @param shapeBuffer  the shapebuffer for the nd4j array
* @param wordSize  the word size (4 for float, 8 for doubles)
* @return a pointer to a numpy array
*/

template <typename T>
static Nd4jPointer _numpyFromNd4j(Nd4jPointer data,Nd4jPointer shapeBuffer,Nd4jLong wordSize) {
    Nd4jLong *shapeBufferCast = reinterpret_cast<Nd4jLong *>(shapeBuffer);
    int  rank = shape::rank(shapeBufferCast);
    Nd4jLong *shape = shape::shapeOf(shapeBufferCast);
    unsigned int *npShape = new unsigned int[rank];
    for(int i = 0; i < rank; i++) {
        npShape[i] = shape[i];
    }

    Nd4jLong length = shape::prodLong(shape,rank);
    auto npHeader = cnpy::createNpyHeader<T>(data,npShape,rank,wordSize);
    char *dataChar = reinterpret_cast<char *>(data);
    char *npHeaderData = npHeader.data();
    char *ret = new char[(wordSize * length) +  npHeader.size()];
    char *cursorStart = ret;
    std::memcpy(reinterpret_cast<void *>(ret), reinterpret_cast<void *>(npHeaderData), npHeader.size() * sizeof(Nd4jLong));
    //move to next
    cursorStart += npHeader.size();
    std::memcpy(reinterpret_cast<void *>(ret), reinterpret_cast<void *>(dataChar), length * wordSize * sizeof(Nd4jLong));
    Nd4jPointer  rettPointer = reinterpret_cast<Nd4jPointer>(ret);
    return rettPointer;
}

extern "C" {

static Nd4jPointer numpyFromNd4j(Nd4jPointer data,Nd4jPointer shapeBuffer,Nd4jLong wordSize) {
    auto shapeBufferCast = reinterpret_cast<Nd4jLong *>(shapeBuffer);
    auto type = sd::ArrayOptions::dataType(shapeBufferCast);
    BUILD_SINGLE_SELECTOR(type, return _numpyFromNd4j, (data, shapeBuffer, wordSize), LIBND4J_TYPES);
}


/**
*
* @param npyArray
* @return
*/
ND4J_EXPORT Nd4jPointer shapeBufferForNumpy(Nd4jPointer npyArray);


/**
* Get the shape buffer from a
* numpy array.
* **Warning** this allocates memory
* @param npyArray
* @return
*/
static Nd4jPointer shapeBufferForNumpyHeader(Nd4jPointer npyArray) {
    cnpy::NpyArray arr = cnpy::loadNpyFromHeader(reinterpret_cast<char *>(npyArray));
    auto shape = new unsigned int[arr.shape.size()];
    for(unsigned int i = 0; i < arr.shape.size(); i++) {
        shape[i] = arr.shape[i];
    }

    auto shapeBuffer = shape::shapeBufferOfNpy(arr.shape.size(), shape, arr.fortranOrder);
    delete[] shape;
    return reinterpret_cast<Nd4jPointer>(shapeBuffer);
}



/**
*
* @param npyArray
* @return
*/
static Nd4jPointer dataPointForNumpyHeader(Nd4jPointer npyArray) {
    cnpy::NpyArray arr = cnpy::loadNpyFromHeader(reinterpret_cast<char *>(npyArray));
    unsigned  char *dataToPrint = reinterpret_cast<unsigned  char *>(arr.data);
    return dataToPrint;
}

/**
*
* @param npyArray
* @return
*/
static Nd4jPointer dataPointForNumpyStruct(Nd4jPointer npyArrayStruct) {
    cnpy::NpyArray *arrPointer = reinterpret_cast<cnpy::NpyArray *>(npyArrayStruct);
    unsigned  char *dataToPrint = reinterpret_cast<unsigned  char *>(arrPointer->data);
    return reinterpret_cast<Nd4jPointer>(dataToPrint);
}

/**
*
* @param npyArray
* @param fromFile
* @return
*/
static Nd4jPointer dataPointForNumpy(Nd4jPointer npyArray) {
    char *npyArrayBuffer = reinterpret_cast<  char *>(npyArray);
    cnpy::NpyArray arr = cnpy::loadNpyFromPointer(npyArrayBuffer);
    return dataPointForNumpyStruct(reinterpret_cast<Nd4jPointer>(&arr));
}

/**
* Load a numpy array from a file
* and return it as an Nd4jPointer
* @param path
* @return
*/
static Nd4jPointer numpyFromFile(std::string path) {
    char *numpyBuffer = cnpy::loadFile(path.data());
    return reinterpret_cast<Nd4jPointer >(numpyBuffer);
}


////// NPZ //////

static void* mapFromNpzFile(std::string path){
    cnpy::npz_t* mapPtr = new cnpy::npz_t();
    cnpy::npz_t map = cnpy::npzLoad(path);
    mapPtr->insert(map.begin(), map.end());
    return reinterpret_cast<void*>(mapPtr);
}


static int getNumNpyArraysInMap(void *map){
    cnpy::npz_t* arrays = reinterpret_cast<cnpy::npz_t*>(map);
    int n = arrays->size();
    return n;
}

static const char* getNpyArrayNameFromMap(void *map, int index,char *nameBuffer) {
    cnpy::npz_t* arrays = reinterpret_cast<cnpy::npz_t*>(map);
    cnpy::npz_t::iterator it = arrays->begin();
    cnpy::npz_t::iterator end = arrays->end();
    int cnt = 0;
    for(; it != end; ++it, ++cnt){
        if (cnt == index) {
            size_t  len_of_str = strlen(it->first.c_str());
            memcpy(nameBuffer,it->first.c_str(),len_of_str);
        }
    }
    throw std::runtime_error("No array at index.");
}

static void* getNpyArrayFromMap(void *map, int index){
    cnpy::npz_t* arrays = reinterpret_cast<cnpy::npz_t*>(map);
    cnpy::npz_t::iterator it = arrays->begin();
    cnpy::npz_t::iterator end = arrays->end();
    cnpy::NpyArray *arr = new cnpy::NpyArray();
    int cnt = 0;
    for(; it != end; ++it, ++cnt){
        if (cnt == index){
            *arr = it->second;
            return arr;
        }
    }
    throw std::runtime_error("No array at index.");
}

ND4J_EXPORT int dataTypeFromNpyHeader(void *header);

static void* getNpyArrayData(void *npArray){
    cnpy::NpyArray* npyArray2 = reinterpret_cast<cnpy::NpyArray*>(npArray);
    return reinterpret_cast<void*>(npyArray2->data);
}

static int getNpyArrayRank(void *npArray){
    cnpy::NpyArray* arr = reinterpret_cast<cnpy::NpyArray*>(npArray);
    int rank = arr->shape.size();
    return rank;
}

static Nd4jLong* getNpyArrayShape(void *npArray){
    cnpy::NpyArray* arr = reinterpret_cast<cnpy::NpyArray*>(npArray);
    int ndim = arr->shape.size();
    Nd4jLong* shape = new Nd4jLong[ndim];
    for (int i=0; i<ndim; i++){
        shape[i] = arr->shape.at(i);
    }
    return shape;
}

static char getNpyArrayOrder(void *npArray){
    cnpy::NpyArray* arr = reinterpret_cast<cnpy::NpyArray*>(npArray);
    return (arr->fortranOrder)?'f':'c';
}

static int getNpyArrayElemSize(void *npArray){
    cnpy::NpyArray* arr = reinterpret_cast<cnpy::NpyArray*>(npArray);
    return arr->wordSize;
}

static void deleteNPArrayStruct(void *npArray){
    cnpy::NpyArray* arr = reinterpret_cast<cnpy::NpyArray*>(npArray);
    delete arr;
}

static void deleteNPArrayMap(void *map){
    cnpy::npz_t* arrays = reinterpret_cast<cnpy::npz_t*>(map);
    delete arrays;
}
//////

/**
* Get the element size for a numpy array
* @param npyArray  the numpy array's address
* to get the length for
* @return
*/
static int elementSizeForNpyArray(Nd4jPointer npyArray) {
    cnpy::NpyArray arr = cnpy::loadNpyFromPointer(reinterpret_cast<char *>(npyArray));
    cnpy::NpyArray *arrPointer = &arr;
    int size = arrPointer->wordSize;
    // arrPointer->destruct();
    return size;
}


/**
* Get the element size for a numpy array
* @param npyArray  the numpy array's address
* to get the length for
* @return
*/
static int elementSizeForNpyArrayHeader(Nd4jPointer npyArray) {
    cnpy::NpyArray arr = cnpy::loadNpyFromHeader(reinterpret_cast<char *>(npyArray));
    cnpy::NpyArray *arrPointer = &arr;
    int size = arrPointer->wordSize;
    return size;
}


static void releaseNumpy(Nd4jPointer npyArray) {
    free(reinterpret_cast<void *>(npyArray));
}


/**
 * Return the length of a shape buffer
 * based on the pointer
 * @param buffer  the buffer pointer to check
 * @return
 */
ND4J_EXPORT int lengthForShapeBufferPointer(Nd4jPointer buffer);


/**
* The pointer to get the address for
*
* @param address the address to get the pointer
* @return the pointer for the given address
*/

ND4J_EXPORT Nd4jPointer pointerForAddress(Nd4jLong address);

/**
 * This method takes single N-dimensional tensor, and copies its TADs to target arrays
 *
 * @param x
 * @param xShapeInfo
 * @param targets
 * @param zShapeInfo
 * @return
 */
ND4J_EXPORT void tear(Nd4jPointer *extraPointers,
                      OpaqueDataBuffer *dbX, Nd4jLong const* xShapeInfo, Nd4jLong const* dxShapeInfo,
                      Nd4jPointer *targets, Nd4jLong const* zShapeInfo,
                      Nd4jLong const* tadShapeInfo,
                      Nd4jLong const* tadOffsets);

ND4J_EXPORT void sort(Nd4jPointer *extraPointers,
                      void *x, Nd4jLong const* xShapeInfo,
                      void *dx, Nd4jLong const* dxShapeInfo,
                      bool descending);

ND4J_EXPORT void sortByKey(Nd4jPointer *extraPointers,
                           void *x, Nd4jLong const* xShapeInfo,
                           void *dx, Nd4jLong const* dxShapeInfo,
                           void *y, Nd4jLong const* yShapeInfo,
                           void *dy, Nd4jLong const* dyShapeInfo,
                           bool descending);

ND4J_EXPORT void sortByValue(Nd4jPointer *extraPointers,
                             void *x, Nd4jLong const* xShapeInfo,
                             void *dx, Nd4jLong const* dxShapeInfo,
                             void *y, Nd4jLong const* yShapeInfo,
                             void *dy, Nd4jLong const* dyShapeInfo,
                             bool descending);

ND4J_EXPORT void sortTad(Nd4jPointer *extraPointers,
                         void *x, Nd4jLong const* xShapeInfo,
                         void *dx, Nd4jLong const* dxShapeInfo,
                         int *dimension,
                         int dimensionLength,
                         Nd4jLong const* tadShapeInfo,
                         Nd4jLong const* tadOffsets,
                         bool descending);

ND4J_EXPORT void sortTadByKey(Nd4jPointer *extraPointers,
                              void *x, Nd4jLong const* xShapeInfo,
                              void *dx, Nd4jLong const* dxShapeInfo,
                              void *y, Nd4jLong const* yShapeInfo,
                              void *dy, Nd4jLong const* dyShapeInfo,
                              int *dimension,
                              int dimensionLength,
                              bool descending);

ND4J_EXPORT void sortTadByValue(Nd4jPointer *extraPointers,
                                void *x, Nd4jLong const* xShapeInfo,
                                void *dx, Nd4jLong const* dxShapeInfo,
                                void *y, Nd4jLong const* yShapeInfo,
                                void *dy, Nd4jLong const* dyShapeInfo,
                                int *dimension,
                                int dimensionLength,
                                bool descending);


// special sort impl for sorting out COO indices and values
ND4J_EXPORT void sortCooIndices(Nd4jPointer *extraPointers,
                                Nd4jLong *indices,
                                void *x,
                                Nd4jLong length,
                                const Nd4jLong *xShapeInfo);

/**
 *
 * @param extraPointers     not used
 * @param indices           DataBuffer containing COO indices for a sparse matrix that is to be raveled/flattened
 * @param flatIndices       DataBuffer where the raveled/flattened indices are to be written to
 * @param length            number of non-zero entries (length of flatIndices)
 * @param fullShapeBuffer   DataBuffer with ShapeInfo for the full matrix to be flattened
 * @param mode              clipMode determines the strategy to use if some of the the passed COO indices does
 *                          not fit into the shape determined by fullShapeBuffer
 *                              0   throw an exception (default)
 *                              1   wrap around shape
 *                              2   clip to shape
 */
ND4J_EXPORT void ravelMultiIndex(Nd4jPointer *extraPointers, Nd4jLong *indices, Nd4jLong *flatIndices, Nd4jLong length,  Nd4jLong *shapeInfo, int mode);

/**
 *
 * @param extraPointers     not used
 * @param indices           DataBuffer where the unraveled COO indices are to be written
 * @param flatIndices       DataBuffer containing the raveled/flattened indices to be unravel
 * @param length            number of non-zero entries (length of flatIndices)
 * @param fullShapeBuffer   DataBuffer with ShapeInfo for the full matrix to be unraveled
 */
ND4J_EXPORT void unravelIndex(Nd4jPointer *extraPointers, Nd4jLong *indices, Nd4jLong *flatIndices, Nd4jLong length,  Nd4jLong *shapeInfo);

ND4J_EXPORT Nd4jLong* mmapFile(Nd4jPointer *extraPointers, const char *fileName, Nd4jLong length);

ND4J_EXPORT void munmapFile(Nd4jPointer *extraPointers, Nd4jLong* ptrMap, Nd4jLong length);

typedef sd::graph::ResultWrapper OpaqueResultWrapper;

// flatbuffers execution
ND4J_EXPORT OpaqueResultWrapper* executeFlatGraph(Nd4jPointer *extraPointers, Nd4jPointer flatBufferPointer);

ND4J_EXPORT Nd4jLong getResultWrapperSize(OpaqueResultWrapper* ptr);
ND4J_EXPORT Nd4jPointer getResultWrapperPointer(OpaqueResultWrapper* ptr);

ND4J_EXPORT const char* getAllCustomOps();

ND4J_EXPORT const char* getAllOperations();

// customOp executioner
ND4J_EXPORT int execCustomOp(Nd4jPointer* extraPointers, Nd4jLong hash, Nd4jPointer* inputBuffers, Nd4jPointer* inputShapes, int numInputs, Nd4jPointer* outputBuffers, Nd4jPointer* outputShapes, int numOutputs, double* tArgs, int numTArgs, Nd4jLong *iArgs, int numIArgs, bool* bArgs, int numBArgs, bool isInplace);
ND4J_EXPORT int execCustomOp2(Nd4jPointer* extraPointers, Nd4jLong hash, Nd4jPointer opContext);

typedef sd::ShapeList OpaqueShapeList;

ND4J_EXPORT OpaqueShapeList* calculateOutputShapes(Nd4jPointer* extraPointers, Nd4jLong hash, Nd4jPointer* inputShapes, int numInputShapes, double* tArgs, int numTArgs, Nd4jLong *iArgs, int numIArgs);
ND4J_EXPORT OpaqueShapeList* calculateOutputShapes2(Nd4jPointer* extraPointers, Nd4jLong hash, Nd4jPointer* inputBuffers, Nd4jPointer* inputShapes, int numInputShapes, double* tArgs, int numTArgs, Nd4jLong *iArgs, int numIArgs, bool *bArgs, int numBArgs, int *dArgs, int numDArgs);

ND4J_EXPORT Nd4jLong getShapeListSize(OpaqueShapeList* list);
ND4J_EXPORT Nd4jLong const* getShape(OpaqueShapeList* list, Nd4jLong i);

ND4J_EXPORT void deleteShapeList(Nd4jPointer shapeList);

ND4J_EXPORT int registerGraph(Nd4jPointer *extraPointers, Nd4jLong graphId, Nd4jPointer flatBufferPointer);

typedef sd::graph::VariablesSet OpaqueVariablesSet;
typedef sd::graph::Variable OpaqueVariable;

ND4J_EXPORT OpaqueVariablesSet *executeStoredGraph(Nd4jPointer *extraPointers, Nd4jLong graphId, Nd4jPointer *inputBuffers, Nd4jPointer *inputShapes, int* inputIndices, int numInputs);

ND4J_EXPORT Nd4jLong getVariablesSetSize(OpaqueVariablesSet* set);
ND4J_EXPORT Nd4jStatus getVariablesSetStatus(OpaqueVariablesSet* set);
ND4J_EXPORT OpaqueVariable* getVariable(OpaqueVariablesSet* set, Nd4jLong i);
ND4J_EXPORT int getVariableId(OpaqueVariable* variable);
ND4J_EXPORT int getVariableIndex(OpaqueVariable* variable);
ND4J_EXPORT const char* getVariableName(OpaqueVariable* variable);
ND4J_EXPORT Nd4jLong const* getVariableShape(OpaqueVariable* variable);
ND4J_EXPORT void* getVariableBuffer(OpaqueVariable* variable);

ND4J_EXPORT int unregisterGraph(Nd4jPointer *extraPointers, Nd4jLong graphId);

ND4J_EXPORT void deleteCharArray(Nd4jPointer pointer);
ND4J_EXPORT void deleteIntArray(Nd4jPointer pointer);
ND4J_EXPORT void deleteLongArray(Nd4jPointer pointer);
ND4J_EXPORT void deletePointerArray(Nd4jPointer pointer);

ND4J_EXPORT void deleteVariablesSet(OpaqueVariablesSet* pointer);

// GraphState creation
ND4J_EXPORT Nd4jPointer getGraphState(Nd4jLong id);

ND4J_EXPORT void deleteGraphState(Nd4jPointer state);

ND4J_EXPORT void deleteResultWrapper(Nd4jPointer ptr);

ND4J_EXPORT int estimateThreshold(Nd4jPointer *extraPointers, Nd4jPointer x, Nd4jLong const* xShapeInfo, int N, float threshold);

// this method executes op that requires scope to be present: if/while/cond/whatever
ND4J_EXPORT Nd4jStatus execCustomOpWithScope(Nd4jPointer *extraPointers, Nd4jPointer state, Nd4jLong opHash, Nd4jLong *scopes, int numScopes, Nd4jPointer *inputBuffers, Nd4jPointer *inputShapes, int numInputs, Nd4jPointer *outputBuffers, Nd4jPointer *outputShapes, int numOutputs);

//void fillUtf8String(Nd4jPointer *extraPointers, const char **string, int numStrings, Nd4jPointer buffer);
ND4J_EXPORT Nd4jPointer createUtf8String(Nd4jPointer *extraPointers, const char *string, int length);
ND4J_EXPORT Nd4jLong getUtf8StringLength(Nd4jPointer *extraPointers, Nd4jPointer ptr);
ND4J_EXPORT char* getUtf8StringBuffer(Nd4jPointer *extraPointers, Nd4jPointer ptr);
ND4J_EXPORT void deleteUtf8String(Nd4jPointer *extraPointers, Nd4jPointer ptr);

ND4J_EXPORT void scatterUpdate(Nd4jPointer *extraPointers, int opCode, int numOfSubArrs,
                               void* hX, Nd4jLong const* hXShapeInfo, Nd4jLong const* hXOffsets,
                               void* dX, Nd4jLong const* dXShapeInfo, Nd4jLong const* dXOffsets,
                               void* hY, Nd4jLong const* hYShapeInfo, Nd4jLong const* hYOffsets,
                               void* dY, Nd4jLong const* dYShapeInfo, Nd4jLong const* dYOffsets,
                               void* hIindexes, Nd4jLong const* hIndicesShapeInfo, void* dIindexes, Nd4jLong const* dIndicesShapeInfo);

ND4J_EXPORT void inspectArray(Nd4jPointer *extraPointers, Nd4jPointer buffer, Nd4jLong *shapeInfo, Nd4jPointer specialBuffer, Nd4jLong *specialShapeInfo, Nd4jPointer debugInfo);


typedef sd::ConstantDataBuffer OpaqueConstantDataBuffer;
typedef sd::ConstantShapeBuffer OpaqueConstantShapeBuffer;

ND4J_EXPORT OpaqueConstantShapeBuffer* shapeBuffer(int rank, Nd4jLong *shape, Nd4jLong *strides, sd::DataType dtype, char order, Nd4jLong ews, bool empty);
ND4J_EXPORT OpaqueConstantShapeBuffer* shapeBufferEx(int rank, Nd4jLong *shape, Nd4jLong *strides, sd::DataType dtype, char order, Nd4jLong ews, Nd4jLong extras);

ND4J_EXPORT OpaqueConstantDataBuffer* constantBufferLong(sd::DataType dtype, Nd4jLong const* data, int length);
ND4J_EXPORT OpaqueConstantDataBuffer* constantBufferDouble(sd::DataType dtype, double *data, int length);
ND4J_EXPORT OpaqueConstantDataBuffer* constantBuffer(sd::DataType dtype, sd::ConstantDescriptor *descriptor);

ND4J_EXPORT Nd4jPointer getConstantDataBufferPrimary(OpaqueConstantDataBuffer* dbf);
ND4J_EXPORT Nd4jPointer getConstantDataBufferSpecial(OpaqueConstantDataBuffer* dbf);
ND4J_EXPORT Nd4jLong getConstantDataBufferLength(OpaqueConstantDataBuffer* dbf);

ND4J_EXPORT Nd4jPointer getConstantShapeBufferPrimary(OpaqueConstantShapeBuffer* dbf);
ND4J_EXPORT Nd4jPointer getConstantShapeBufferSpecial(OpaqueConstantShapeBuffer* dbf);

ND4J_EXPORT void deleteConstantShapeBuffer(OpaqueConstantShapeBuffer* ptr);
ND4J_EXPORT void deleteConstantDataBuffer(OpaqueConstantDataBuffer* ptr);

typedef sd::graph::Context OpaqueContext;
typedef sd::graph::RandomGenerator OpaqueRandomGenerator;

ND4J_EXPORT OpaqueContext* createGraphContext(int nodeId);
ND4J_EXPORT OpaqueRandomGenerator* getGraphContextRandomGenerator(OpaqueContext* ptr);
ND4J_EXPORT void ctxAllowHelpers(OpaqueContext* ptr, bool reallyAllow);
ND4J_EXPORT void ctxShapeFunctionOverride(OpaqueContext* ptr, bool reallyOverride);
ND4J_EXPORT void ctxSetExecutionMode(OpaqueContext* ptr, int execMode);
ND4J_EXPORT void ctxPurge(OpaqueContext* ptr);
ND4J_EXPORT void markGraphContextInplace(OpaqueContext* ptr, bool reallyInplace);
ND4J_EXPORT void setGraphContextCudaContext(OpaqueContext* ptr, void *stream, void *reductionPointer, void *allocationPointer);
ND4J_EXPORT void setGraphContextInputArray(OpaqueContext* ptr, int index, void *buffer, void *shapeInfo, void *specialBuffer, void *specialShapeInfo);
ND4J_EXPORT void setGraphContextOutputArray(OpaqueContext* ptr, int index, void *buffer, void *shapeInfo, void *specialBuffer, void *specialShapeInfo);
ND4J_EXPORT void setGraphContextInputBuffer(OpaqueContext* ptr, int index, OpaqueDataBuffer *buffer, void *shapeInfo, void *specialShapeInfo);
ND4J_EXPORT void setGraphContextOutputBuffer(OpaqueContext* ptr, int index, OpaqueDataBuffer *buffer, void *shapeInfo, void *specialShapeInfo);
ND4J_EXPORT void setGraphContextDArguments(OpaqueContext* ptr, int *arguments, int numberOfArguments);
ND4J_EXPORT void setGraphContextTArguments(OpaqueContext* ptr, double *arguments, int numberOfArguments);
ND4J_EXPORT void setGraphContextIArguments(OpaqueContext* ptr, Nd4jLong *arguments, int numberOfArguments);
ND4J_EXPORT void setGraphContextBArguments(OpaqueContext* ptr, bool *arguments, int numberOfArguments);
ND4J_EXPORT void deleteGraphContext(OpaqueContext* ptr);

ND4J_EXPORT OpaqueRandomGenerator* createRandomGenerator(Nd4jLong rootSeed = 0, Nd4jLong nodeSeed = 0);
ND4J_EXPORT Nd4jLong getRandomGeneratorRootState(OpaqueRandomGenerator* ptr);
ND4J_EXPORT Nd4jLong getRandomGeneratorNodeState(OpaqueRandomGenerator* ptr);
ND4J_EXPORT void setRandomGeneratorStates(OpaqueRandomGenerator* ptr, Nd4jLong rootSeed = 0, Nd4jLong nodeSeed = 0);
ND4J_EXPORT float getRandomGeneratorRelativeFloat(OpaqueRandomGenerator* ptr, Nd4jLong index);
ND4J_EXPORT double getRandomGeneratorRelativeDouble(OpaqueRandomGenerator* ptr, Nd4jLong index);
ND4J_EXPORT int getRandomGeneratorRelativeInt(OpaqueRandomGenerator* ptr, Nd4jLong index);
ND4J_EXPORT Nd4jLong getRandomGeneratorRelativeLong(OpaqueRandomGenerator* ptr, Nd4jLong index);
ND4J_EXPORT float getRandomGeneratorNextFloat(OpaqueRandomGenerator* ptr);
ND4J_EXPORT double getRandomGeneratorNextDouble(OpaqueRandomGenerator* ptr);
ND4J_EXPORT int getRandomGeneratorNextInt(OpaqueRandomGenerator* ptr);
ND4J_EXPORT Nd4jLong getRandomGeneratorNextLong(OpaqueRandomGenerator* ptr);
ND4J_EXPORT void deleteRandomGenerator(OpaqueRandomGenerator* ptr);



typedef sd::LaunchContext OpaqueLaunchContext;

ND4J_EXPORT OpaqueLaunchContext* defaultLaunchContext();
ND4J_EXPORT Nd4jPointer lcScalarPointer(OpaqueLaunchContext* lc);
ND4J_EXPORT Nd4jPointer lcReductionPointer(OpaqueLaunchContext* lc);
ND4J_EXPORT Nd4jPointer lcAllocationPointer(OpaqueLaunchContext* lc);
ND4J_EXPORT Nd4jPointer lcExecutionStream(OpaqueLaunchContext* lc);
ND4J_EXPORT Nd4jPointer lcCopyStream(OpaqueLaunchContext* lc);
ND4J_EXPORT Nd4jPointer lcBlasHandle(OpaqueLaunchContext* lc);
ND4J_EXPORT Nd4jPointer lcSolverHandle(OpaqueLaunchContext* lc);

ND4J_EXPORT OpaqueDataBuffer* allocateDataBuffer(Nd4jLong elements, int dataType, bool allocateBoth);
ND4J_EXPORT OpaqueDataBuffer* dbAllocateDataBuffer(Nd4jLong elements, int dataType, bool allocateBoth);
ND4J_EXPORT OpaqueDataBuffer* dbCreateExternalDataBuffer(Nd4jLong elements, int dataType, Nd4jPointer primary, Nd4jPointer special);
ND4J_EXPORT OpaqueDataBuffer* dbCreateView(OpaqueDataBuffer *dataBuffer, Nd4jLong length, Nd4jLong offset);
ND4J_EXPORT Nd4jPointer dbPrimaryBuffer(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT Nd4jPointer dbSpecialBuffer(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbExpandBuffer(OpaqueDataBuffer *dataBuffer, Nd4jLong elements);
ND4J_EXPORT void dbAllocatePrimaryBuffer(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbAllocateSpecialBuffer(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbSetPrimaryBuffer(OpaqueDataBuffer *dataBuffer, Nd4jPointer primaryBuffer, Nd4jLong numBytes);
ND4J_EXPORT void dbSetSpecialBuffer(OpaqueDataBuffer *dataBuffer, Nd4jPointer specialBuffer, Nd4jLong numBytes);
ND4J_EXPORT void dbSyncToSpecial(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbSyncToPrimary(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT int dbLocality(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT int dbDeviceId(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbSetDeviceId(OpaqueDataBuffer *dataBuffer, int deviceId);
ND4J_EXPORT void dbTickHostRead(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbTickHostWrite(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbTickDeviceRead(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbTickDeviceWrite(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbClose(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void deleteDataBuffer(OpaqueDataBuffer *dataBuffer);
ND4J_EXPORT void dbExpand(OpaqueDataBuffer *dataBuffer, Nd4jLong elements);


ND4J_EXPORT int  binaryLevel();
ND4J_EXPORT int optimalLevel();

ND4J_EXPORT bool isMinimalRequirementsMet();
ND4J_EXPORT bool isOptimalRequirementsMet();

}

#endif //NATIVEOPERATIONS_NATIVEOPS_H
