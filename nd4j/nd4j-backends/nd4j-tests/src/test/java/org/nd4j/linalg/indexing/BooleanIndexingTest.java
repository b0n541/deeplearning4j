/*
 *  ******************************************************************************
 *  *
 *  *
 *  * This program and the accompanying materials are made available under the
 *  * terms of the Apache License, Version 2.0 which is available at
 *  * https://www.apache.org/licenses/LICENSE-2.0.
 *  *
 *  *  See the NOTICE file distributed with this work for additional
 *  *  information regarding copyright ownership.
 *  * Unless required by applicable law or agreed to in writing, software
 *  * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *  * License for the specific language governing permissions and limitations
 *  * under the License.
 *  *
 *  * SPDX-License-Identifier: Apache-2.0
 *  *****************************************************************************
 */

package org.nd4j.linalg.indexing;

import lombok.val;
import org.junit.jupiter.api.Tag;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

import org.nd4j.common.tests.tags.NativeTag;
import org.nd4j.common.tests.tags.TagNames;
import org.nd4j.linalg.BaseNd4jTestWithBackends;
import org.nd4j.linalg.api.buffer.DataType;
import org.nd4j.linalg.api.ndarray.INDArray;
import org.nd4j.linalg.api.ops.executioner.OpExecutioner;
import org.nd4j.linalg.api.ops.impl.controlflow.WhereNumpy;
import org.nd4j.linalg.api.ops.impl.reduce.longer.MatchCondition;
import org.nd4j.linalg.api.ops.impl.transforms.comparison.CompareAndReplace;
import org.nd4j.linalg.api.ops.impl.transforms.comparison.CompareAndSet;
import org.nd4j.linalg.factory.Nd4j;
import org.nd4j.linalg.factory.Nd4jBackend;
import org.nd4j.linalg.indexing.conditions.Conditions;
import org.nd4j.linalg.indexing.conditions.GreaterThan;
import org.nd4j.nativeblas.NativeOpsHolder;

import java.util.Arrays;
import java.util.Collections;

import static org.junit.jupiter.api.Assertions.*;
@Tag(TagNames.NDARRAY_INDEXING)
@NativeTag
public class BooleanIndexingTest extends BaseNd4jTestWithBackends {
    /*
        1D array checks
     */

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testAnd1(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

        assertTrue(BooleanIndexing.and(array, Conditions.greaterThan(0.5f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testAnd2(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

        assertTrue(BooleanIndexing.and(array, Conditions.lessThan(6.0f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testAnd3(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

        assertFalse(BooleanIndexing.and(array, Conditions.lessThan(5.0f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testAnd4(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

        assertFalse(BooleanIndexing.and(array, Conditions.greaterThan(4.0f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testAnd5(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1e-5f, 1e-5f, 1e-5f, 1e-5f, 1e-5f});

        assertTrue(BooleanIndexing.and(array, Conditions.greaterThanOrEqual(1e-5f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testAnd6(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1e-5f, 1e-5f, 1e-5f, 1e-5f, 1e-5f});

        assertFalse(BooleanIndexing.and(array, Conditions.lessThan(1e-5f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testAnd7(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1e-5f, 1e-5f, 1e-5f, 1e-5f, 1e-5f});

        assertTrue(BooleanIndexing.and(array, Conditions.equals(1e-5f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testOr1(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

        assertTrue(BooleanIndexing.or(array, Conditions.greaterThan(3.0f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testOr2(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

        assertTrue(BooleanIndexing.or(array, Conditions.lessThan(3.0f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testOr3(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new float[] {1.0f, 2.0f, 3.0f, 4.0f, 5.0f});

        assertFalse(BooleanIndexing.or(array, Conditions.greaterThan(6.0f)));
    }

    /*
        2D array checks
     */

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void test2dAnd1(Nd4jBackend backend) {
        INDArray array = Nd4j.zeros(10, 10);

        assertTrue(BooleanIndexing.and(array, Conditions.equals(0f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void test2dAnd2(Nd4jBackend backend) {
        INDArray array = Nd4j.zeros(DataType.DOUBLE,10, 10);
        array.slice(4).putScalar(2, 1e-4);
//        System.out.println(array);
        boolean and = BooleanIndexing.and(array, Conditions.epsEquals(0f));
        assertFalse(and);


    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void test2dAnd3(Nd4jBackend backend) {
        INDArray array = Nd4j.zeros(10, 10);

        array.slice(4).putScalar(2, 1e-5f);

        assertFalse(BooleanIndexing.and(array, Conditions.greaterThan(0f)));
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void test2dAnd4(Nd4jBackend backend) {
        INDArray array = Nd4j.zeros(10, 10);

        array.slice(4).putScalar(2, 1e-5f);

        assertTrue(BooleanIndexing.or(array, Conditions.greaterThan(1e-6f)));
    }


    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testConditionalAssign1(Nd4jBackend backend) {
        INDArray array1 = Nd4j.create(new double[] {1, 2, 3, 4, 5, 6, 7});
        INDArray array2 = Nd4j.create(new double[] {7, 6, 5, 4, 3, 2, 1});
        INDArray comp = Nd4j.create(new double[] {1, 2, 3, 4, 3, 2, 1});

        BooleanIndexing.replaceWhere(array1, array2, Conditions.greaterThan(4));

        assertEquals(comp, array1);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testCaSTransform1(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new double[] {1, 2, 0, 4, 5});
        INDArray comp = Nd4j.create(new double[] {1, 2, 3, 4, 5});

        Nd4j.getExecutioner().exec(new CompareAndSet(array, 3, Conditions.equals(0)));

        assertEquals(comp, array);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testCaSTransform2(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new double[] {1, 2, 0, 4, 5});
        INDArray comp = Nd4j.create(new double[] {3, 2, 3, 4, 5});

        Nd4j.getExecutioner().exec(new CompareAndSet(array, 3.0, Conditions.lessThan(2)));

        assertEquals(comp, array);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testCaSPairwiseTransform1(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new double[] {1, 2, 0, 4, 5});
        INDArray comp = Nd4j.create(new double[] {1, 2, 3, 4, 5});

        Nd4j.getExecutioner().exec(new CompareAndSet(array, comp, Conditions.lessThan(5)));

        assertEquals(comp, array);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testCaRPairwiseTransform1(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new double[] {1, 2, 0, 4, 5});
        INDArray comp = Nd4j.create(new double[] {1, 2, 3, 4, 5});

        INDArray z = Nd4j.exec(new CompareAndReplace(array, comp, Conditions.lessThan(1)));

        assertEquals(comp, z);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testCaSPairwiseTransform2(Nd4jBackend backend) {
        INDArray x = Nd4j.create(new double[] {1, 2, 0, 4, 5});
        INDArray y = Nd4j.create(new double[] {2, 4, 3, 0, 5});
        INDArray comp = Nd4j.create(new double[] {2, 4, 3, 4, 5});

        Nd4j.getExecutioner().exec(new CompareAndSet(x, y, Conditions.epsNotEquals(0.0)));

        assertEquals(comp, x);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testCaRPairwiseTransform2(Nd4jBackend backend) {
        INDArray x = Nd4j.create(new double[] {1, 2, 0, 4, 5});
        INDArray y = Nd4j.create(new double[] {2, 4, 3, 4, 5});
        INDArray comp = Nd4j.create(new double[] {2, 4, 0, 4, 5});

        INDArray z = Nd4j.exec(new CompareAndReplace(x, y, Conditions.epsNotEquals(0.0)));

        assertEquals(comp, z);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testCaSPairwiseTransform3(Nd4jBackend backend) {
        INDArray x = Nd4j.create(new double[] {1, 2, 0, 4, 5});
        INDArray y = Nd4j.create(new double[] {2, 4, 3, 4, 5});
        INDArray comp = Nd4j.create(new double[] {2, 4, 3, 4, 5});

        INDArray z = Nd4j.exec(new CompareAndReplace(x, y, Conditions.lessThan(4)));

        assertEquals(comp, z);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testCaRPairwiseTransform3(Nd4jBackend backend) {
        INDArray x = Nd4j.create(new double[] {1, 2, 0, 4, 5});
        INDArray y = Nd4j.create(new double[] {2, 4, 3, 4, 5});
        INDArray comp = Nd4j.create(new double[] {2, 2, 3, 4, 5});

        INDArray z = Nd4j.exec(new CompareAndReplace(x, y, Conditions.lessThan(2)));

        assertEquals(comp, z);
    }


    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testMatchConditionAllDimensions1(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new double[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

        int val = (int) Nd4j.getExecutioner().exec(new MatchCondition(array, Conditions.lessThan(5)))
                .getDouble(0);

        assertEquals(5, val);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testMatchConditionAllDimensions2(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new double[] {0, 1, 2, 3, Double.NaN, 5, 6, 7, 8, 9});

        int val = (int) Nd4j.getExecutioner().exec(new MatchCondition(array, Conditions.isNan()))
                .getDouble(0);

        assertEquals(1, val);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testMatchConditionAllDimensions3(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new double[] {0, 1, 2, 3, Double.NEGATIVE_INFINITY, 5, 6, 7, 8, 9});

        int val = (int) Nd4j.getExecutioner()
                .exec(new MatchCondition(array, Conditions.isInfinite())).getDouble(0);

        assertEquals(1, val);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testMatchConditionAlongDimension1(Nd4jBackend backend) {
        INDArray array = Nd4j.ones(3, 10);
        array.getRow(2).assign(0.0);

        boolean result[] = BooleanIndexing.and(array, Conditions.equals(0.0), 1);
        boolean comp[] = new boolean[] {false, false, true};

//        System.out.println("Result: " + Arrays.toString(result));
        assertArrayEquals(comp, result);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testMatchConditionAlongDimension2(Nd4jBackend backend) {
        INDArray array = Nd4j.ones(3, 10);
        array.getRow(2).assign(0.0).putScalar(0, 1.0);

//        System.out.println("Array: " + array);

        boolean result[] = BooleanIndexing.or(array, Conditions.lessThan(0.9), 1);
        boolean comp[] = new boolean[] {false, false, true};

//        System.out.println("Result: " + Arrays.toString(result));
        assertArrayEquals(comp, result);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testMatchConditionAlongDimension3(Nd4jBackend backend) {
        INDArray array = Nd4j.ones(3, 10);
        array.getRow(2).assign(0.0).putScalar(0, 1.0);

        boolean result[] = BooleanIndexing.and(array, Conditions.lessThan(0.0), 1);
        boolean comp[] = new boolean[] {false, false, false};

//        System.out.println("Result: " + Arrays.toString(result));
        assertArrayEquals(comp, result);
    }


    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testConditionalUpdate(Nd4jBackend backend) {
        INDArray arr = Nd4j.linspace(-2, 2, 5, DataType.DOUBLE);
        INDArray ones = Nd4j.ones(DataType.DOUBLE, 5);
        INDArray exp = Nd4j.create(new double[] {1, 1, 0, 1, 1});


        Nd4j.getExecutioner().exec(new CompareAndSet(ones, arr, ones, Conditions.equals(0.0)));

        assertEquals(exp, ones);
    }


    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testFirstIndex1(Nd4jBackend backend) {
        INDArray arr = Nd4j.create(new double[] {1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
        INDArray result = BooleanIndexing.firstIndex(arr, Conditions.greaterThanOrEqual(3));

        assertEquals(2, result.getDouble(0), 0.0);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testFirstIndex2(Nd4jBackend backend) {
        INDArray arr = Nd4j.create(new double[] {1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
        INDArray result = BooleanIndexing.firstIndex(arr, Conditions.lessThan(3));

        assertEquals(0, result.getDouble(0), 0.0);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testLastIndex1(Nd4jBackend backend) {
        INDArray arr = Nd4j.create(new double[] {1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
        INDArray result = BooleanIndexing.lastIndex(arr, Conditions.greaterThanOrEqual(3));

        assertEquals(8, result.getDouble(0), 0.0);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testFirstIndex2D(Nd4jBackend backend) {
        INDArray arr = Nd4j.create(new double[] {1, 2, 3, 0, 1, 3, 7, 8, 9}).reshape('c', 3, 3);
        INDArray result = BooleanIndexing.firstIndex(arr, Conditions.greaterThanOrEqual(2), 1);
        INDArray exp = Nd4j.create(new long[] {1, 2, 0}, new long[]{3}, DataType.LONG);

        assertEquals(exp, result);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testLastIndex2D(Nd4jBackend backend) {
        INDArray arr = Nd4j.create(new double[] {1, 2, 3, 0, 1, 3, 7, 8, 0}).reshape('c', 3, 3);
        INDArray result = BooleanIndexing.lastIndex(arr, Conditions.greaterThanOrEqual(2), 1);
        INDArray exp = Nd4j.create(new long[] {2, 2, 1}, new long[]{3}, DataType.LONG);

        assertEquals(exp, result);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testEpsEquals1(Nd4jBackend backend) {
        INDArray array = Nd4j.create(new double[] {-1, -1, -1e-8, 1e-8, 1, 1});
        MatchCondition condition = new MatchCondition(array, Conditions.epsEquals(0.0));
        int numZeroes = Nd4j.getExecutioner().exec(condition).getInt(0);

        assertEquals(2, numZeroes);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testChooseNonZero(Nd4jBackend backend) {
        INDArray testArr = Nd4j.create(new double[] {
                0.00,  0.51,  0.68,  0.69,  0.86,  0.91,  0.96,  0.97,  0.97,  1.03,  1.13,  1.16,  1.16,  1.17,  1.19,  1.25,  1.25,  1.26,  1.27,  1.28,  1.29,  1.29,  1.29,  1.30,  1.31,  1.32,  1.33,  1.33,  1.35,  1.35,  1.36,  1.37,  1.38,  1.40,  1.41,  1.42,  1.43,  1.44,  1.44,  1.45,  1.45,  1.47,  1.47,  1.51,  1.51,  1.51,  1.52,  1.53,  1.56,  1.57,  1.58,  1.59,  1.61,  1.62,  1.63,  1.63,  1.64,  1.64,  1.66,  1.66,  1.67,  1.67,  1.70,  1.70,  1.70,  1.72,  1.72,  1.72,  1.72,  1.73,  1.74,  1.74,  1.76,  1.76,  1.77,  1.77,  1.80,  1.80,  1.81,  1.82,  1.83,  1.83,  1.84,  1.84,  1.84,  1.85,  1.85,  1.85,  1.86,  1.86,  1.87,  1.88,  1.89,  1.89,  1.89,  1.89,  1.89,  1.91,  1.91,  1.91,  1.92,  1.94,  1.95,  1.97,  1.98,  1.98,  1.98,  1.98,  1.98,  1.99,  2.00,  2.00,  2.01,  2.01,  2.02,  2.03,  2.03,  2.03,  2.04,  2.04,  2.05,  2.06,  2.07,  2.08,  2.08,  2.08,  2.08,  2.09,  2.09,  2.10,  2.10,  2.11,  2.11,  2.11,  2.12,  2.12,  2.13,  2.13,  2.14,  2.14,  2.14,  2.14,  2.15,  2.15,  2.16,  2.16,  2.16,  2.16,  2.16,  2.17
        });

        INDArray filtered = BooleanIndexing.chooseFrom(new INDArray[]{testArr},Arrays.asList(0.0), Collections.emptyList(),new GreaterThan());
        assertFalse(filtered.getDouble(0) == 0);
        assertEquals(testArr.length() - 1,filtered.length());

    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testChooseBasic(Nd4jBackend backend) {
        Nd4j.getExecutioner().setProfilingMode(OpExecutioner.ProfilingMode.ANY_PANIC);
        NativeOpsHolder.getInstance().getDeviceNativeOps().enableDebugMode(true);
        INDArray arr = Nd4j.linspace(1,4,4, Nd4j.dataType()).reshape(2,2);
        INDArray filtered = BooleanIndexing.chooseFrom(new INDArray[]{arr}, Arrays.asList(2.0), Collections.emptyList(),new GreaterThan());
        assertEquals(2, filtered.length());
    }


    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testChooseGreaterThanZero(Nd4jBackend backend) {
        INDArray zero = Nd4j.linspace(0,4,4, Nd4j.dataType());
        INDArray filtered = BooleanIndexing.chooseFrom(new INDArray[]{zero},Arrays.asList(0.0), Collections.emptyList(),new GreaterThan());
        assertEquals(3, filtered.length());
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testChooseNone(Nd4jBackend backend) {
        Nd4j.getExecutioner().setProfilingMode(OpExecutioner.ProfilingMode.ANY_PANIC);
        NativeOpsHolder.getInstance().getDeviceNativeOps().enableDebugMode(true);
        INDArray arr = Nd4j.linspace(1,4,4, Nd4j.dataType()).reshape(2,2);
        INDArray filtered = BooleanIndexing.chooseFrom(new INDArray[]{arr},Arrays.asList(5.0), Collections.emptyList(),new GreaterThan());
        assertNull(filtered);
    }


    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testWhere(Nd4jBackend backend) {
        INDArray data = Nd4j.create(4);
        INDArray mask = Nd4j.create(DataType.BOOL, 4);
        INDArray put = Nd4j.create(4);
        INDArray resultData = Nd4j.create(4);
        INDArray assertion = Nd4j.create(4);
        for (int i = 0; i < 4; i++) {
            data.putScalar(i,i);
            if (i > 1) {
                assertion.putScalar(i, 5.0);
                mask.putScalar(i, 1);
            } else {
                assertion.putScalar(i, i);
                mask.putScalar(i, 0);
            }

            put.putScalar(i, 5.0);
            resultData.putScalar(i, 0.0);
        }


        Nd4j.getExecutioner().exec(new WhereNumpy(new INDArray[]{mask,data,put},new INDArray[]{resultData}));
        assertEquals(assertion,resultData);
    }

    @ParameterizedTest
    @MethodSource("org.nd4j.linalg.BaseNd4jTestWithBackends#configs")
    public void testEpsStuff_1(Nd4jBackend backend) {
        val dtype = Nd4j.dataType();
        val array = Nd4j.create(new float[]{0.001f, 5e-6f, 5e-6f, 5e-6f, 5e-6f});
        val exp = Nd4j.create(new float[]{0.001f, 1.0f, 1.0f, 1.0f, 1.0f});
        BooleanIndexing.replaceWhere(array, 1.0f, Conditions.epsEquals(0));

        assertEquals(exp, array);
    }

    @Override
    public char ordering() {
        return 'c';
    }
}
