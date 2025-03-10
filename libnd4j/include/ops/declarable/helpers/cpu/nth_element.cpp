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
//  @author sgazeos@gmail.com
//

#include <ops/declarable/helpers/nth_element.h>
#include <helpers/TAD.h>
#include <helpers/ShapeUtils.h>
#include <helpers/ConstantTadHelper.h>
#include <execution/Threads.h>

namespace sd {
namespace ops {
namespace helpers {

    template <typename T>
    ND4J_LOCAL void nthElementFunctor_(NDArray* input, Nd4jLong n, NDArray* output, bool reverse) {

        NDArray sortedVals(*input);
        if (input->isVector()) {
            //std::vector<float> data(input->lengthOf());
            //memcpy(&data[0], input->buffer(), sizeof(T) * data.size());
            //size_t l = 0;
            //for (size_t l = 0; l < data.size(); ++l)
            //    data[l] = input->e<float>(l);
            //auto nthPos = data.begin();
            //nthPos += n;
            //std::nth_element(data.begin(), nthPos, data.end());
            SpecialMethods<T>::sortGeneric(sortedVals.buffer(), sortedVals.shapeInfo(), reverse);
            output->p(0, sortedVals.e<T>(n));
        }
        else { // rank greater than 1
            std::vector<int> lastDims({input->rankOf() - 1});// = ShapeUtils::evalDimsToExclude(input->rankOf(), {input->rankOf() - 1});

            auto pack = sd::ConstantTadHelper::getInstance().tadForDimensions(sortedVals.shapeInfo(), lastDims);

            SpecialMethods<T>::sortTadGeneric(sortedVals.buffer(), sortedVals.shapeInfo(), lastDims.data(), lastDims.size(), pack.primaryShapeInfo(), pack.primaryOffsets(), reverse);

            ResultSet rows = sortedVals.allTensorsAlongDimension(lastDims);
            Nd4jLong oL = output->lengthOf();

            auto func = PRAGMA_THREADS_FOR {
                for (auto e = start; e < stop; e++) {
                    auto row = rows.at(e);
                    output->p(e, row->e<T>(n));
                }
            };

            samediff::Threads::parallel_for(func, 0, oL);
        }
    }

    ND4J_LOCAL void nthElementFunctor(sd::LaunchContext  *launchContext, NDArray* input, Nd4jLong n, NDArray* output, bool reverse) {
    BUILD_SINGLE_SELECTOR(input->dataType(), nthElementFunctor_, (input, n, output, reverse), LIBND4J_TYPES);

    }
    BUILD_SINGLE_TEMPLATE(template ND4J_LOCAL void nthElementFunctor_, (NDArray* input, Nd4jLong n, NDArray* output, bool reverse), LIBND4J_TYPES);

}
}
}
