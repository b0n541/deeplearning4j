// automatically generated by the FlatBuffers compiler, do not modify

package org.nd4j.graph;

import java.nio.*;
import java.lang.*;
import java.nio.ByteOrder;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class LongTriple extends Table {
  public static LongTriple getRootAsLongTriple(ByteBuffer _bb) { return getRootAsLongTriple(_bb, new LongTriple()); }
  public static LongTriple getRootAsLongTriple(ByteBuffer _bb, LongTriple obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public void __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; vtable_start = bb_pos - bb.getInt(bb_pos); vtable_size = bb.getShort(vtable_start); }
  public LongTriple __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public long first() { int o = __offset(4); return o != 0 ? bb.getLong(o + bb_pos) : 0L; }
  public long second() { int o = __offset(6); return o != 0 ? bb.getLong(o + bb_pos) : 0L; }
  public long third() { int o = __offset(8); return o != 0 ? bb.getLong(o + bb_pos) : 0L; }

  public static int createLongTriple(FlatBufferBuilder builder,
      long first,
      long second,
      long third) {
    builder.startObject(3);
    LongTriple.addThird(builder, third);
    LongTriple.addSecond(builder, second);
    LongTriple.addFirst(builder, first);
    return LongTriple.endLongTriple(builder);
  }

  public static void startLongTriple(FlatBufferBuilder builder) { builder.startObject(3); }
  public static void addFirst(FlatBufferBuilder builder, long first) { builder.addLong(0, first, 0L); }
  public static void addSecond(FlatBufferBuilder builder, long second) { builder.addLong(1, second, 0L); }
  public static void addThird(FlatBufferBuilder builder, long third) { builder.addLong(2, third, 0L); }
  public static int endLongTriple(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
}

