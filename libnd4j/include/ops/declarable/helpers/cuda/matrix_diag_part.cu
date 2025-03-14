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
// Created by GS <sgazeos@gmail.com> on 3/21/2018.
//

#include <array/ResultSet.h>
#include <ops/declarable/helpers/matrix_diag_part.h>
#include <graph/Status.h>
#include <helpers/ShapeUtils.h>
#include <helpers/ShapeUtils.h>
#include <helpers/TAD.h>
#include <exceptions/cuda_exception.h>
#include <helpers/ConstantTadHelper.h>

namespace sd {
namespace ops {
namespace helpers {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// put diagonals from input batched matricies to output batched vectors
    template <typename T>
    static __global__ void matrixDiagPartKernel(void const* inputBuffer, void* outputBuffer, Nd4jLong numTads, Nd4jLong inputLength,
                                                const Nd4jLong* tadOnlyInputShapeInfo,  const Nd4jLong *tadInputOffsets,
                                                const Nd4jLong* tadOnlyOutputShapeInfo, const Nd4jLong *tadOutputOffsets) {
        int totalThreads = blockDim.x;
        for (Nd4jLong i = blockIdx.x; i < numTads; i += gridDim.x) {
            auto yOffset = tadInputOffsets[i];
            auto xOffset = tadOutputOffsets[i];
            for (Nd4jLong j = threadIdx.x; j < inputLength; j += totalThreads) {
                Nd4jLong coords[2] = {j, j};
                Nd4jLong tadOffset = shape::getOffset(tadOnlyInputShapeInfo, coords);
                *(reinterpret_cast<T*>(outputBuffer) + xOffset + shape::getIndexOffset(j, tadOnlyOutputShapeInfo)) = *(reinterpret_cast<T const*>(inputBuffer) + yOffset + tadOffset);
            }
        }
    }

//////////////////////////////////////////////////////////////////////////
// Returns a batched matrix tensor with new batched diagonal values.
// for detailed explanations please take a look on web page: https://www.tensorflow.org/api_docs/python/tf/matrix_set_diag
//
    template <typename T>
    static int _matrixDiagPart(sd::LaunchContext * context, const NDArray* input, NDArray* output) {
        auto stream = context->getCudaStream();
        auto listOut  = output->allTensorsAlongDimension({output->rankOf() - 1});
        auto listDiag = input->allTensorsAlongDimension({input->rankOf() - 2, input->rankOf() - 1});

        if (listOut.size() != listDiag.size()) {
            nd4j_printf("matrix_diag_part: Input matrix has wrong shape.", "");
            return ND4J_STATUS_VALIDATION;
        }
        Nd4jLong lastDimension = sd::math::nd4j_min(input->sizeAt(-2), input->sizeAt(-1));

        std::vector<int> dimsToExclude = ShapeUtils::evalDimsToExclude(output->rankOf(), {output->rankOf() - 1});
        const Nd4jLong numTads = ShapeUtils::getNumOfSubArrs(input->shapeInfo(), dimsToExclude); //this->tensorsAlongDimension({dimension});
        //printf("Repeat delta %lld, numTads %lld\n", repeatDelta, numTads);
        //tadOnlyInputShapeInfo, tadInputOffsets, tadOnlyOutputShapeInfo, tadOutputOffsets;
        std::vector<int> outputDims({output->rankOf() - 1});
        std::vector<int> inputDims({input->rankOf() - 2, input->rankOf() - 1});
        auto packX = sd::ConstantTadHelper::getInstance().tadForDimensions(input->shapeInfo(), inputDims);
        auto packZ = sd::ConstantTadHelper::getInstance().tadForDimensions(output->shapeInfo(), outputDims);


        if (!output->isActualOnDeviceSide())
            input->syncToDevice();

        if (!input->isActualOnDeviceSide())
            input->syncToDevice();


        dim3 launchDims(256, 512, 8192);
        matrixDiagPartKernel<T><<<launchDims.x, launchDims.y, launchDims.z, *stream>>>(input->specialBuffer(), output->specialBuffer(), numTads, lastDimension, packX.specialShapeInfo(), packX.specialOffsets(), packZ.specialShapeInfo(), packZ.specialOffsets());

        return Status::OK();
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// caller for _matrixDiagPart
//
    ND4J_LOCAL int matrixDiagPart(sd::LaunchContext * context, const NDArray* input, NDArray* output) {
        BUILD_SINGLE_SELECTOR(input->dataType(), return _matrixDiagPart, (context, input, output), LIBND4J_TYPES);
    }

    BUILD_SINGLE_TEMPLATE(template ND4J_LOCAL int _matrixDiagPart, (sd::LaunchContext * context, const NDArray* input, NDArray* output), LIBND4J_TYPES);

}
}
}