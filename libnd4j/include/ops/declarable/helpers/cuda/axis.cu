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

#include <ops/declarable/helpers/axis.h>


namespace sd {
namespace ops {
namespace helpers {

    ND4J_LOCAL void adjustAxis(Nd4jLong rank, NDArray* axisVector, std::vector<int>& output) {
        output.resize(axisVector->lengthOf());
        axisVector->tickReadDevice(); // mark input as read on device
        axisVector->syncToHost(); // sync to host
        for (int e = 0; e < axisVector->lengthOf(); e++) {
                auto ca = axisVector->e<int>(e);
                if (ca < 0) // shift values on rank for negative vals
                    ca += rank;

                output[e] = ca;
        }
    }

    ND4J_LOCAL void adjustAxis(Nd4jLong rank, std::vector<int> &axisVector) {
        for (int e = 0; e < axisVector.size(); e++) {
            auto a = axisVector[e];
            if (a < 0) // shift vals on rank for negative vals
                axisVector[e] = a + rank;
        }
    }

}
}
}
