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

#include <ops/declarable/helpers/image_suppression.h>
#include <array/NDArrayFactory.h>
#include <algorithm>
#include <numeric>
#include <queue>

namespace sd {
namespace ops {
namespace helpers {

    template <typename T>
    static void nonMaxSuppressionV2_(NDArray* boxes, NDArray* scales, int maxSize, double overlapThreshold,
            double scoreThreshold, NDArray* output) {
        std::vector<int> indices(scales->lengthOf());
        std::iota(indices.begin(), indices.end(), 0);
        auto actualIndicesCount = indices.size();
        for (auto e = 0; e < scales->lengthOf(); e++) {
            if (scales->e<float>(e) < (float)scoreThreshold) {
                indices[e] = -1;
                actualIndicesCount--;
            }
        }
        std::sort(indices.begin(), indices.end(), [scales](int i, int j) {return i >= 0 && j >=0?scales->e<T>(i) > scales->e<T>(j):(i > j);});

//        std::vector<int> selected(output->lengthOf());
        std::vector<int> selectedIndices(output->lengthOf(), 0);
        auto needToSuppressWithThreshold = [] (NDArray& boxes, int previousIndex, int nextIndex, T threshold) -> bool {
            if (previousIndex < 0 || nextIndex < 0) return true;
            T minYPrev = sd::math::nd4j_min(boxes.t<T>(previousIndex, 0), boxes.t<T>(previousIndex, 2));
            T minXPrev = sd::math::nd4j_min(boxes.t<T>(previousIndex, 1), boxes.t<T>(previousIndex, 3));
            T maxYPrev = sd::math::nd4j_max(boxes.t<T>(previousIndex, 0), boxes.t<T>(previousIndex, 2));
            T maxXPrev = sd::math::nd4j_max(boxes.t<T>(previousIndex, 1), boxes.t<T>(previousIndex, 3));
            T minYNext = sd::math::nd4j_min(boxes.t<T>(nextIndex, 0), boxes.t<T>(nextIndex, 2));
            T minXNext = sd::math::nd4j_min(boxes.t<T>(nextIndex, 1), boxes.t<T>(nextIndex, 3));
            T maxYNext = sd::math::nd4j_max(boxes.t<T>(nextIndex, 0), boxes.t<T>(nextIndex, 2));
            T maxXNext = sd::math::nd4j_max(boxes.t<T>(nextIndex, 1), boxes.t<T>(nextIndex, 3));
            T areaPrev = (maxYPrev - minYPrev) * (maxXPrev - minXPrev);
            T areaNext = (maxYNext - minYNext) * (maxXNext - minXNext);

            if (areaNext <= T(0.f) || areaPrev <= T(0.f)) return false;

            T minIntersectionY = sd::math::nd4j_max(minYPrev, minYNext);
            T minIntersectionX = sd::math::nd4j_max(minXPrev, minXNext);
            T maxIntersectionY = sd::math::nd4j_min(maxYPrev, maxYNext);
            T maxIntersectionX = sd::math::nd4j_min(maxXPrev, maxXNext);
            T intersectionArea =
                    sd::math::nd4j_max(T(maxIntersectionY - minIntersectionY), T(0.0f)) *
                            sd::math::nd4j_max(T(maxIntersectionX - minIntersectionX), T(0.0f));
            T intersectionValue = intersectionArea / (areaPrev + areaNext - intersectionArea);
            return intersectionValue > threshold;

        };
//        int numSelected = 0;
        int numBoxes = actualIndicesCount; //boxes->sizeAt(0);
        int numSelected = 0;

        for (int i = 0; i < numBoxes; ++i) {
            bool shouldSelect = numSelected < output->lengthOf();

            // FIXME: add parallelism here
            for (int j = numSelected - 1; j >= 0; --j) {
                if (shouldSelect)
                if (needToSuppressWithThreshold(*boxes, indices[i], indices[selectedIndices[j]], T(overlapThreshold))) {
                    shouldSelect = false;
                }
            }
            if (shouldSelect) {
                output->p(numSelected, indices[i]);
                selectedIndices[numSelected++] = i;
            }
        }
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Return intersection-over-union overlap between boxes i and j
    template <typename T>
    static inline T similirityV3_(NDArray const& boxes, Nd4jLong i, Nd4jLong j) {
        const T zero = static_cast<T>(0.f);
        const T yminI = math::nd4j_min(boxes.t<T>(i, 0), boxes.t<T>(i, 2));
        const T xminI = math::nd4j_min(boxes.t<T>(i, 1), boxes.t<T>(i, 3));
        const T ymaxI = math::nd4j_max(boxes.t<T>(i, 0), boxes.t<T>(i, 2));
        const T xmaxI = math::nd4j_max(boxes.t<T>(i, 1), boxes.t<T>(i, 3));
        const T yminJ = math::nd4j_min(boxes.t<T>(j, 0), boxes.t<T>(j, 2));
        const T xminJ = math::nd4j_min(boxes.t<T>(j, 1), boxes.t<T>(j, 3));
        const T ymaxJ = math::nd4j_max(boxes.t<T>(j, 0), boxes.t<T>(j, 2));
        const T xmaxJ = math::nd4j_max(boxes.t<T>(j, 1), boxes.t<T>(j, 3));
        const T areaI = (ymaxI - yminI) * (xmaxI - xminI);
        const T areaJ = (ymaxJ - yminJ) * (xmaxJ - xminJ);
        if (areaI <= zero || areaJ <= zero) {
            return zero;
        }
        const T intersectionYmin = math::nd4j_max(yminI, yminJ);
        const T intersectionXmin = math::nd4j_max(xminI, xminJ);
        const T intersectionYmax = math::nd4j_min(ymaxI, ymaxJ);
        const T intersectionXmax = math::nd4j_min(xmaxI, xmaxJ);
        const T intersectionY = intersectionYmax - intersectionYmin;
        const T intersectionX = intersectionXmax - intersectionXmin;
        const T intersectionArea = math::nd4j_max(intersectionY, zero) *  math::nd4j_max(intersectionX, zero);
        return intersectionArea / (areaI + areaJ - intersectionArea);
    }

    template <typename T>
    static inline T similiratyOverlaps_(NDArray const& boxes, Nd4jLong i, Nd4jLong j) {
        return boxes.t<T>(i, j);
    }

    typedef NDArray (*SimiliratyFunc)(NDArray const& boxes, Nd4jLong i, Nd4jLong j);

    static NDArray similiratyOverlaps(NDArray const& boxes, Nd4jLong i, Nd4jLong j) {
        NDArray res(boxes.dataType(), boxes.getContext()); // = NDArrayFactory::create(0.);
        BUILD_SINGLE_SELECTOR(boxes.dataType(), res = similiratyOverlaps_, (boxes, i, j) , FLOAT_TYPES);
        return res;
    }

    static NDArray similiratyV3(NDArray const& boxes, Nd4jLong i, Nd4jLong j) {
        NDArray res(boxes.dataType(), boxes.getContext()); // = NDArrayFactory::create(0.);
        BUILD_SINGLE_SELECTOR(boxes.dataType(), res = similirityV3_, (boxes, i, j) , FLOAT_TYPES);
        return res;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T, typename I>
    static Nd4jLong
    nonMaxSuppressionGeneric_(sd::LaunchContext* context, NDArray* boxes, NDArray* scores, int outputSize,
            float overlapThreshold, float scoreThreshold, NDArray* output,  SimiliratyFunc f) {

        auto numBoxes = boxes->sizeAt(0);
        T* scoresData = scores->dataBuffer()->primaryAsT<T>();

        // Data structure for a selection candidate in NMS.
        struct Candidate {
            int _boxIndex;
            T _score;
            int _suppressBeginIndex;
        };

        auto cmp = [](const Candidate& bsI, const Candidate& bsJ) -> bool{
            return ((bsI._score == bsJ._score) && (bsI._boxIndex > bsJ._boxIndex)) ||
                    (bsI._score < bsJ._score);
        };

        std::priority_queue<Candidate, std::deque<Candidate>, decltype(cmp)> candidatePriorityQueue(cmp);
        for (auto i = 0; i < scores->lengthOf(); ++i) {
            if ((float)scoresData[i] > (float)scoreThreshold) {
                candidatePriorityQueue.emplace(Candidate({i, scoresData[i], 0}));
            }
        }

        std::vector<I> selected;
        T similarity, originalScore;
        Candidate nextCandidate;

        while (selected.size() < outputSize && !candidatePriorityQueue.empty()) {
            nextCandidate = candidatePriorityQueue.top();
            originalScore = nextCandidate._score;
            candidatePriorityQueue.pop();

            // Overlapping boxes are likely to have similar scores, therefore we
            // iterate through the previously selected boxes backwards in order to
            // see if `nextCandidate` should be suppressed. We also enforce a property
            // that a candidate can be suppressed by another candidate no more than
            // once via `suppress_begin_index` which tracks which previously selected
            // boxes have already been compared against next_candidate prior to a given
            // iteration.  These previous selected boxes are then skipped over in the
            // following loop.
            bool shouldHardSuppress = false;
            for (int j = static_cast<int>(selected.size()) - 1; j >= nextCandidate._suppressBeginIndex; --j) {
                auto similarityA = f(*boxes, nextCandidate._boxIndex, selected[j]); //boxes->t<T>(nextCandidate._boxIndex, selected[j]);
                similarity = similarityA.template t<T>(0);
                nextCandidate._score *= T(similarity <= overlapThreshold?1.0:0.); //suppressWeightFunc(similarity);

                // First decide whether to perform hard suppression
                if ((float)similarity >= static_cast<float>(overlapThreshold)) {
                    shouldHardSuppress = true;
                    break;
                }

                // If next_candidate survives hard suppression, apply soft suppression
                if ((float)nextCandidate._score <= (float)scoreThreshold) break;
            }
            // If `nextCandidate._score` has not dropped below `scoreThreshold`
            // by this point, then we know that we went through all of the previous
            // selections and can safely update `suppress_begin_index` to
            // `selected.size()`. If on the other hand `next_candidate.score`
            // *has* dropped below the score threshold, then since `suppressWeight`
            // always returns values in [0, 1], further suppression by items that were
            // not covered in the above for loop would not have caused the algorithm
            // to select this item. We thus do the same update to
            // `suppressBeginIndex`, but really, this element will not be added back
            // into the priority queue in the following.
            nextCandidate._suppressBeginIndex = selected.size();

            if (!shouldHardSuppress) {
                if (nextCandidate._score == originalScore) {
                    // Suppression has not occurred, so select next_candidate
                    selected.push_back(nextCandidate._boxIndex);
//                    selected_scores.push_back(nextCandidate._score);
                }
                if ((float)nextCandidate._score > (float)scoreThreshold) {
                    // Soft suppression has occurred and current score is still greater than
                    // score_threshold; add next_candidate back onto priority queue.
                    candidatePriorityQueue.push(nextCandidate);
                }
            }
        }

        if (output) {
            DataBuffer buf(selected.data(), selected.size() * sizeof(I), DataTypeUtils::fromT<I>());
            output->dataBuffer()->copyBufferFrom(buf, buf.getLenInBytes());
        }

        return (Nd4jLong)selected.size();
    }

    ND4J_LOCAL Nd4jLong
    nonMaxSuppressionGeneric(sd::LaunchContext* context, NDArray* boxes, NDArray* scores, int maxSize,
                              double overlapThreshold, double scoreThreshold, NDArray* output) {
        BUILD_DOUBLE_SELECTOR(boxes->dataType(), output == nullptr?DataType::INT32:output->dataType(), return nonMaxSuppressionGeneric_, (context, boxes, scores, maxSize, overlapThreshold, scoreThreshold, output, similiratyOverlaps), FLOAT_TYPES, INTEGER_TYPES);
        return 0;
    }

    ND4J_LOCAL Nd4jLong
    nonMaxSuppressionV3(sd::LaunchContext* context, NDArray* boxes, NDArray* scores, int maxSize,
                             double overlapThreshold, double scoreThreshold, NDArray* output) {
        BUILD_DOUBLE_SELECTOR(boxes->dataType(), output == nullptr?DataType::INT32:output->dataType(), return nonMaxSuppressionGeneric_, (context, boxes, scores, maxSize, overlapThreshold, scoreThreshold, output, similiratyV3), FLOAT_TYPES, INTEGER_TYPES);
        return 0;
    }

    BUILD_DOUBLE_TEMPLATE(template ND4J_LOCAL Nd4jLong nonMaxSuppressionGeneric_, (sd::LaunchContext* context, NDArray* boxes, NDArray* scores, int maxSize,
            float overlapThreshold, float scoreThreshold, NDArray* output, SimiliratyFunc similiratyFunc), FLOAT_TYPES, INTEGER_TYPES);

    ND4J_LOCAL void
    nonMaxSuppression(sd::LaunchContext * context, NDArray* boxes, NDArray* scales, int maxSize,
            double overlapThreshold, double scoreThreshold, NDArray* output) {
        BUILD_SINGLE_SELECTOR(boxes->dataType(), nonMaxSuppressionV2_, (boxes, scales, maxSize,
                overlapThreshold, scoreThreshold, output), NUMERIC_TYPES);
    }
    BUILD_SINGLE_TEMPLATE(template ND4J_LOCAL void nonMaxSuppressionV2_, (NDArray* boxes, NDArray* scales, int maxSize,
            double overlapThreshold, double scoreThreshold, NDArray* output), NUMERIC_TYPES);

}
}
}