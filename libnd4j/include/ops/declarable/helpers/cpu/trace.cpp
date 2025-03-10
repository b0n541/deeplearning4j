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
// @author Yurii Shyrma (iuriish@yahoo.com), created on 20.04.2018
//


#include <ops/declarable/helpers/transforms.h>
#include <helpers/Loops.h>

namespace sd 	  {
namespace ops 	  {
namespace helpers {

//////////////////////////////////////////////////////////////////////////
template <typename T>
static void trace_(const NDArray& input, NDArray& output) {
    const int inRank = input.rankOf();
    auto setOfSubArrs = input.allTensorsAlongDimension({inRank-2, inRank-1});

    auto func = PRAGMA_THREADS_FOR {
        for (auto i = start; i < stop; i++)
            output.p(i, setOfSubArrs.at(i)->getTrace());
    };
    samediff::Threads::parallel_for(func, 0, setOfSubArrs.size());
}

    ND4J_LOCAL void trace(sd::LaunchContext * context, const NDArray& input, NDArray& output) {
        BUILD_SINGLE_SELECTOR(input.dataType(), trace_, (input, output), LIBND4J_TYPES);
    }
}
}
}
