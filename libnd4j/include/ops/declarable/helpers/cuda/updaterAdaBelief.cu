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
// @author Oleh Semeniv (oleg.semeniv@gmail.com)
// @author Abdelrauf (rauf@konduit.ai)

// https://arxiv.org/pdf/2010.07468.pdf

#include <system/op_boilerplate.h>
#include <ops/declarable/helpers/updatersHelpers.h>
#include <helpers/PointersManager.h>
#include <math/platformmath.h>
#include <math/templatemath.h>

namespace sd    {
namespace ops     {
namespace helpers {

///////////////////////////////////////////////////////////////////
template<typename T>
__global__ void adaBeliefUpdaterCuda(const void* vx, const Nd4jLong* xShapeInfo, const void* vinv, const Nd4jLong* invShapeInfo, const void* vinm,
                                const Nd4jLong* inmShapeInfo, void* vz, const Nd4jLong* zShapeInfo, void* vstV, 
                                const Nd4jLong* stvShapeInfo, void* vstM, const Nd4jLong* stmShapeInfo,
                                const T lr, const T beta1, const T beta2, const T epsilon, const T iteration) {

    const auto grad = reinterpret_cast<const T*>(vx);
    const auto initU = reinterpret_cast<const T*>(vinv);
    const auto initM = reinterpret_cast<const T*>(vinm);

    auto up = reinterpret_cast<T*>(vz);
    auto stU = reinterpret_cast<T*>(vstV);
    auto stM = reinterpret_cast<T*>(vstM);

    __shared__ Nd4jLong xLen;
    __shared__ T epsilonT;
    __shared__ bool bEWS, bOrdering, bXZsame, bXInUSame, bXStUSame, bXInMSame, bXStMSame;

    if (threadIdx.x == 0) {
        xLen = shape::length(xShapeInfo);

        T beta1T = sd::math::nd4j_pow<T, T, T>(beta1, (iteration + 1));
        T beta2T = sd::math::nd4j_pow<T, T, T>(beta2, (iteration + 1));

        epsilonT =  lr * sd::math::nd4j_sqrt<T, T>(1. - beta2T) / (1.0 - beta1T);
        if (sd::math::nd4j_isnan(epsilonT) || 0 == epsilonT || sd::math::nd4j_isinf(epsilonT))
            epsilonT = epsilon;
        
        bEWS = 1 == shape::elementWiseStride(xShapeInfo) && 1 == shape::elementWiseStride(zShapeInfo) &&
                1 == shape::elementWiseStride(stmShapeInfo) && 1 == shape::elementWiseStride(inmShapeInfo) &&
                1 == shape::elementWiseStride(stvShapeInfo) && 1 == shape::elementWiseStride(invShapeInfo);
        bOrdering = shape::order(xShapeInfo) == shape::order(zShapeInfo) && shape::order(zShapeInfo) == shape::order(stmShapeInfo) &&
                    shape::order(stmShapeInfo) == shape::order(inmShapeInfo) && shape::order(inmShapeInfo) == shape::order(stvShapeInfo) &&
                    shape::order(stvShapeInfo) == shape::order(invShapeInfo);
        
        bXZsame = shape::haveSameShapeAndStrides(xShapeInfo, zShapeInfo);
        bXInUSame = shape::haveSameShapeAndStrides(xShapeInfo, invShapeInfo);
        bXStUSame = shape::haveSameShapeAndStrides(xShapeInfo, stvShapeInfo);
        bXInMSame = shape::haveSameShapeAndStrides(xShapeInfo, inmShapeInfo);
        bXStMSame = shape::haveSameShapeAndStrides(xShapeInfo, stmShapeInfo);
    }
    __syncthreads();

    int coords[MAX_RANK];

    for (Nd4jLong i = blockIdx.x * blockDim.x + threadIdx.x; i < xLen; i += gridDim.x * blockDim.x) {

        auto xOffset = i, zOffset = i, initMOffset = i, initUOffset = i, stMOffset = i, stUOffset = i;

        if (!bEWS || !bOrdering){

            shape::index2coords(i, xShapeInfo, coords);
            xOffset  = shape::getOffset(xShapeInfo, coords);
            zOffset  = bXZsame ? xOffset : shape::getOffset(zShapeInfo, coords);
            initUOffset = bXInUSame ? xOffset : shape::getOffset(invShapeInfo, coords);
            stUOffset = bXStUSame ? xOffset : shape::getOffset(stvShapeInfo, coords);
            initMOffset = bXInMSame ? xOffset : shape::getOffset(inmShapeInfo, coords);
            stMOffset = bXStMSame ? xOffset : shape::getOffset(stmShapeInfo, coords);
        }

        stM[stMOffset] = beta1 * initM[initMOffset] + grad[xOffset] * (1 - beta1);
        stU[stUOffset] = beta2 * initU[initUOffset] +  (grad[xOffset] - stM[stMOffset]) * (grad[xOffset] - stM[stMOffset]) * (1 - beta2) + epsilon;

        up[zOffset] = (stM[stMOffset] * epsilonT) / ( sd::math::nd4j_sqrt<T, T>(stU[stUOffset]) + epsilon);
    }
}

///////////////////////////////////////////////////////////////////
template<typename T>
ND4J_LOCAL linkage void adaBeliefUpdaterCudaLauncher(const int blocksPerGrid, const int threadsPerBlock, const cudaStream_t* stream, const void* vx, const Nd4jLong* xShapeInfo,
    const void* vinv, const Nd4jLong* invShapeInfo, const void* vinm, const Nd4jLong* inmShapeInfo,
    void* vz, const Nd4jLong* zShapeInfo, void* vstV, const Nd4jLong* stvShapeInfo, 
    void* vstM, const Nd4jLong* stmShapeInfo, const double dLr, const double dBeta1, const double dBeta2, const double dEpsilon, const int nIteration) {
    
    const T lr = static_cast<T>(dLr);
    const T beta1 = static_cast<T>(dBeta1);
    const T beta2 = static_cast<T>(dBeta2);
    const T epsilon = static_cast<T>(dEpsilon);
    const T iteration = static_cast<T>(nIteration);
    adaBeliefUpdaterCuda<T><<<blocksPerGrid, threadsPerBlock, 256, * stream>>>(vx, xShapeInfo, vinv, invShapeInfo, vinm, inmShapeInfo,
        vz, zShapeInfo, vstV, stvShapeInfo, vstM, stmShapeInfo, lr, beta1, beta2, epsilon, iteration);
}

///////////////////////////////////////////////////////////////////
ND4J_LOCAL void updaterAdaBelief(sd::LaunchContext* context, const NDArray& gradient, const NDArray& initStateU, const NDArray& initStateM, 
                 NDArray& update, NDArray& stateU, NDArray& stateM,  const double dLr, const double dBeta1, const double dBeta2, 
                 const double dEpsilon, const int nIteration) {

    PointersManager manager(context, "adamUpdater");

    const int threadsPerBlock = MAX_NUM_THREADS / 4;
    const int blocksPerGrid = (gradient.lengthOf() + threadsPerBlock - 1) / threadsPerBlock;

    NDArray::prepareSpecialUse({ &update, &stateU, &stateM }, { &gradient, &initStateU, &initStateM });

    BUILD_SINGLE_SELECTOR(gradient.dataType(), adaBeliefUpdaterCudaLauncher, (blocksPerGrid, threadsPerBlock, context->getCudaStream(), gradient.specialBuffer(), gradient.specialShapeInfo(),
        initStateU.specialBuffer(), initStateU.specialShapeInfo(), initStateM.specialBuffer(), initStateM.specialShapeInfo(),
        update.specialBuffer(), update.specialShapeInfo(), stateU.specialBuffer(), stateU.specialShapeInfo(),
        stateM.specialBuffer(), stateM.specialShapeInfo(),  dLr, dBeta1, dBeta2, dEpsilon, nIteration), FLOAT_TYPES);

    NDArray::registerSpecialUse({ &update, &stateU, &stateM }, { &gradient, &initStateU, &initStateM });

    manager.synchronize();
}

}
}
}
